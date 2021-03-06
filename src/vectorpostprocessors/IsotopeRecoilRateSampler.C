/**********************************************************************/
/*                     DO NOT MODIFY THIS HEADER                      */
/* MAGPIE - Mesoscale Atomistic Glue Program for Integrated Execution */
/*                                                                    */
/*            Copyright 2017 Battelle Energy Alliance, LLC            */
/*                        ALL RIGHTS RESERVED                         */
/**********************************************************************/

#include "IsotopeRecoilRateSampler.h"
#include "NeutronicsSpectrumSamplerBase.h"

// MOOSE includes
#include "MooseMesh.h"
#include "MooseVariable.h"

#include "libmesh/mesh_tools.h"

registerMooseObject("MagpieApp", IsotopeRecoilRateSampler);

template <>
InputParameters
validParams<IsotopeRecoilRateSampler>()
{
  InputParameters params = validParams<GeneralVectorPostprocessor>();
  params.addRequiredParam<std::string>(
      "target_isotope", "The isotope name that you want to get the total recoil rate for");
  params.addRequiredParam<std::vector<unsigned int>>(
      "point_ids", "The indices of the points in neutronics_sampler");
  params.addRequiredParam<UserObjectName>(
      "neutronics_sampler", "The neutronics sampler object that the data is retrieved from");
  params.addParam<PostprocessorName>(
      "scaling_factor", 1, "A scaling factor multiplying the isotope recoil rate");
  params.addClassDescription("Gets the total recoil rate from target_isotope at points provided in "
                             "point_id contained in the neutronics_sampler");
  return params;
}

IsotopeRecoilRateSampler::IsotopeRecoilRateSampler(const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _target_isotope(getParam<std::string>("target_isotope")),
    _point_ids(getParam<std::vector<unsigned int>>("point_ids")),
    _neutronics_sampler(getUserObject<NeutronicsSpectrumSamplerBase>("neutronics_sampler")),
    _scaling_factor(getPostprocessorValue("scaling_factor")),
    _recoil_rates(declareVector("recoil_rates"))
{
  _recoil_rates.assign(_point_ids.size(), 0);
  for (auto & p : _point_ids)
    if (_neutronics_sampler.getNumberOfPoints() < p)
      mooseError("The provided neutronics sampler object only has",
                 _neutronics_sampler.getNumberOfPoints(),
                 " points but point id ",
                 p,
                 " is requested");

  if (!_neutronics_sampler.hasIsotope(_target_isotope))
    mooseError("Target isotope ", _target_isotope, " not preset in neutronics sampler object");
}

void
IsotopeRecoilRateSampler::execute()
{
  for (unsigned int j = 0; j < _point_ids.size(); ++j)
    _recoil_rates[j] =
        _scaling_factor * _neutronics_sampler.totalRecoilRate(_point_ids[j], _target_isotope);
}
