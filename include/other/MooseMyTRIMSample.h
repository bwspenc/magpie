#ifndef MOOSEMYTRIMSAMPLE_H
#define MOOSEMYTRIMSAMPLE_H

#include "MyTRIMRasterizer.h"
#include "MooseMyTRIMMaterial.h"

#include "mytrim/sample.h"
#include "mytrim/ion.h"
#include "mytrim/element.h"

// Forward declarations
class MooseMesh;

/**
 * MyTRIM sample class that uses PointLocator lookups on a MooseMesh to
 * fetch and dynamically prepare material data from a MyTRIMRasterizer.
 * A new MooseMyTRIMSample class has to be constructed for every timestep the
 * TRIM simulation is run if the mesh has changed.
 */
class MooseMyTRIMSample : public MyTRIM_NS::sampleBase
{
public:
  MooseMyTRIMSample(const MyTRIMRasterizer &, const MooseMesh &, MyTRIM_NS::simconfType *);

  /// average crossections for current ion
  virtual void averages(const MyTRIM_NS::ionBase  * pka);

  /// interface called by MyTRIM to look up material data
  virtual MyTRIM_NS::materialBase* lookupMaterial(double * pos);

protected:

  /// the rasterizer provides average concentrations for each element
  const MyTRIMRasterizer & _rasterizer;

  /// number of elements used in the problem
  unsigned int _nvars;

  ///@{ Element data
  const std::vector<Real> & _trim_mass;
  const std::vector<Real> & _trim_charge;
  ///@}

  /// mesh of the simulation domain
  const MooseMesh & _mesh;

  /// point locator to determine element pointers form locations
  UniquePtr<PointLocatorBase> _pl;

  /// material cache map
  typedef std::map<const Elem *, MooseMyTRIMMaterial> MaterialsCache;
  MaterialsCache _materials_cache;

  /// current ion (for on-the fly averaging)
  const MyTRIM_NS::ionBase * _current_ion;

  /// internal state object tor teh TRIM simulation
  MyTRIM_NS::simconfType * _simconf;
};

#endif //MOOSEMYTRIMSAMPLE_H
