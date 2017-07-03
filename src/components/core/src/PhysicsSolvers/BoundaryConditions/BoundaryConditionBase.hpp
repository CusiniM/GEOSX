

#ifndef BOUNDARYCONDITIONBASE_H
#define BOUNDARYCONDITIONBASE_H

#include "common/DataTypes.hpp"
#include "dataRepository/ManagedGroup.hpp"
#include "managers/TableManager.hpp"

namespace geosx
{
class Function;

namespace dataRepository
{
namespace keys
{
string const setNames = "setNames";
string const fieldName = "fieldName";
string const component("component");
string const direction("direction");
string const timeTableName("timeTableName");
string const bcApplicationTableName("bcApplicationTableName");
string const scale("scale");
string const functionName("functionName");
}
}

class BoundaryConditionBase : public dataRepository::ManagedGroup
{
public:

  using CatalogInterface = cxx_utilities::CatalogInterface< BoundaryConditionBase, string const &, dataRepository::ManagedGroup * const >;
  static CatalogInterface::CatalogType& GetCatalog();

  BoundaryConditionBase( string const & name, dataRepository::ManagedGroup *const parent );

  virtual ~BoundaryConditionBase();

  void FillDocumentationNode( dataRepository::ManagedGroup * const ) override;

  real64 GetValue( realT time );


  virtual const string& GetFieldName(realT time)
  {
    return m_fieldName;
  }

  virtual int GetComponent(realT time)
  {
    return m_component;
  }

  virtual const R1Tensor& GetDirection(realT time)
  {
    return m_direction;
  }


protected:

  string_array m_setNames; // sets the boundary condition is applied to

  string m_fieldName;    // the name of the field the boundary condition is applied to or a description of the boundary condition.

  // TODO get rid of components. Replace with direction only.
  int m_component;       // the component the boundary condition acts on (-ve indicates that direction should be used).
  R1Tensor m_direction;  // the direction the boundary condition acts in.



  string m_timeTableName;
  string m_bcApplicationTableName;
  real64 m_scale;



  std::string m_functionName;
//  localIndex m_nVars; // number of variables
//
//  bool m_isConstantInSpace;
//  bool m_isConstantInTime;
//
//
//  std::vector<FieldTypeMultiPtr> m_fieldPtrs;
//  Array1dT<FieldType> m_variableTypes;
//  sArray1d m_variableNames;
//  std::vector<realT> m_x;  // function input vector
//
  Function* m_function;


};





}
#endif