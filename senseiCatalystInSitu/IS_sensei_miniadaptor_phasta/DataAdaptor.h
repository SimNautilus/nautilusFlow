#ifndef PHASTA_DataAdaptor_h
#define PHASTA_DataAdaptor_h

#include <sensei/DataAdaptor.h>
#include <sensei/BidirectionalDataAdaptor.h>

#include "vtkSmartPointer.h"

class vtkUnstructuredGrid;

namespace phasta
{

class DataAdaptor : public sensei::BidirectionalDataAdaptor
{
public:
  static DataAdaptor* New();
  senseiTypeMacro(DataAdaptor, sensei::BidirectionalDataAdaptor);

  void InitializeGrid(int* dim, int* numPoints, double* coordsArray, int* numCells);
  void InitializeCellBlock(int* numCellsInBlock, int* numPointsPerCell, int* cellConnectivity);
  void InitializeFields(int* nshg, int* ndof, double* dofArray, int* compressibleFlow, double* vmeanLow, double* vampLow, double* vmeanUp, double* vampUp, double* upperBlowerPosition, double* ybarWeight, double* dwalArray, double* ybarArray);

  // SENSEI-mandated API
  int GetNumberOfMeshes(unsigned int &numMeshes) override;
  int GetMeshMetadata(unsigned int id, sensei::MeshMetadataPtr &md) override;
  int GetMesh(const std::string &meshName, bool structureOnly, vtkDataObject*& mesh) override;
  int AddArray(vtkDataObject* mesh, const std::string &meshName,
    int association, const std::string &arrayName) override;
  int AddGhostCellsArray(vtkDataObject* mesh, const std::string &meshName) override;
  int ReleaseData() override;

  /// The bridge may call this method with an \a updater to be invoked
  /// whenever the mean blower speed is changed. Currently, this can
  /// only happen inside a call to AnalysisAdaptor::Execute(). In the
  /// future, asynchronous behavior may be allowed in some circumstances.
  void ObserveSteeringParameters(const std::function<void()>& updater);

protected:
  DataAdaptor();
  ~DataAdaptor();

  void UpdatePointCoords();
  vtkUnstructuredGrid* CreateGrid(bool structureOnly);
  vtkUnstructuredGrid* CreateSteering(bool);

  class Internal;
  Internal* m_p;

  vtkSmartPointer<vtkUnstructuredGrid> m_grid;
  vtkSmartPointer<vtkUnstructuredGrid> m_steering;

private:
  DataAdaptor(const DataAdaptor&); // not implemented.
  void operator=(const DataAdaptor&); // not implemented.
};

} // namespace phasta

#endif // PHASTA_DataAdaptor_h
