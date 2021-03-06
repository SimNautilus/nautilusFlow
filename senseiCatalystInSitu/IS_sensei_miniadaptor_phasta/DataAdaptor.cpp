#include "DataAdaptor.h"

#include "sensei/Error.h"

#include "vtkCellData.h"
#include "vtkCellType.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkUnstructuredGrid.h"

namespace phasta
{

struct CellBlock
{
  vtkIdType NumberOfCells;
  vtkIdType NumberOfPointsPerCell;
  int* Connectivity;
};

class DataAdaptor::Internal
{
public:
  bool Compressible;
  bool StructureOnly;
  int Dimension;
  double* Coords;
  vtkIdType NumberOfPoints;
  vtkIdType NumberOfCells;
  std::vector<CellBlock> CellBlocks;
  int NumberOfDOF;
  int PointDataOffset;
  double* PointData;
  double* Dwall;
  double* Ybar;
  double* MeanLowerBlowerVelocity;
  double* AmplitudeLowerBlowerVelocity;
  double* MeanUpperBlowerVelocity;
  double* AmplitudeUpperBlowerVelocity;
  double* UpperBlowerPosition;
  double* YbarWeight;
  int MeanLowerBlowerVelocityId;
  int AmplitudeLowerBlowerVelocityId;
  int MeanUpperBlowerVelocityId;
  int AmplitudeUpperBlowerVelocityId;
  int UpperBlowerPositionId;
  int YbarWeightId;
  std::function<void()> LiveUpdater;

  Internal()
    : Compressible(false)
    , Dimension(0)
    , Coords(nullptr)
    , NumberOfPoints(0)
    , NumberOfCells(0)
    , NumberOfDOF(0)
    , PointDataOffset(0)
    , PointData(nullptr)
    , Dwall(nullptr)
    , Ybar(nullptr)
    , MeanLowerBlowerVelocity(nullptr)
    , AmplitudeLowerBlowerVelocity(nullptr)
    , MeanUpperBlowerVelocity(nullptr)
    , AmplitudeUpperBlowerVelocity(nullptr)
    , UpperBlowerPosition(nullptr)
    , YbarWeight(nullptr)
    , MeanLowerBlowerVelocityId(-1)
    , AmplitudeLowerBlowerVelocityId(-1)
    , MeanUpperBlowerVelocityId(-1)
    , AmplitudeUpperBlowerVelocityId(-1)
    , UpperBlowerPositionId(-1)
    , YbarWeightId(-1)
    , LiveUpdater(nullptr)
  {
  }
};

senseiNewMacro(DataAdaptor);

DataAdaptor::DataAdaptor()
{
  m_p = new Internal;
}

DataAdaptor::~DataAdaptor()
{
  m_grid = nullptr;
  m_steering = nullptr;
  delete m_p;
}

void DataAdaptor::InitializeGrid(int* dim, int* numPoints, double* coordsArray, int* numCells)
{
  m_p->Dimension = *dim;
  m_p->Coords = coordsArray;
  m_p->NumberOfPoints = static_cast<vtkIdType>(*numPoints);
  m_p->NumberOfCells = static_cast<vtkIdType>(*numCells);
}

void DataAdaptor::InitializeCellBlock(int* numCellsInBlock, int* numPointsPerCell, int* cellConnectivity)
{
  CellBlock block;
  block.NumberOfCells = static_cast<vtkIdType>(*numCellsInBlock);
  block.NumberOfPointsPerCell = static_cast<vtkIdType>(*numPointsPerCell);
  block.Connectivity = cellConnectivity;
  m_p->CellBlocks.push_back(block);
}

void DataAdaptor::InitializeFields(
  int* globalNumberOfShapeFunctions, int* ndof, double* dofArray, int* compressibleFlow, double* vmeanLow, double* vampLow, double* vmeanUp, double* vampUp, double* upperBlowerPosition, double* ybarWeight, double* dwalArray, double* ybarArray)
{
  m_p->PointDataOffset = *globalNumberOfShapeFunctions;
  m_p->NumberOfDOF = *ndof;
  m_p->PointData = dofArray;
  m_p->Compressible = (*compressibleFlow == 0 ? false : true);
  m_p->MeanLowerBlowerVelocity = vmeanLow;
  m_p->AmplitudeLowerBlowerVelocity = vampLow;
  m_p->MeanUpperBlowerVelocity = vmeanUp;
  m_p->AmplitudeUpperBlowerVelocity = vampUp;
  m_p->UpperBlowerPosition = upperBlowerPosition;
  m_p->YbarWeight = ybarWeight;
  m_p->Dwall = dwalArray;
  m_p->Ybar = ybarArray;
}

int DataAdaptor::GetNumberOfMeshes(unsigned int& numMeshes)
{
  numMeshes = m_p->MeanLowerBlowerVelocity ? 2 : 1;
  numMeshes = m_p->AmplitudeLowerBlowerVelocity ? 2 : 1;
  numMeshes = m_p->MeanUpperBlowerVelocity ? 2 : 1;
  numMeshes = m_p->AmplitudeUpperBlowerVelocity ? 2 : 1;
  numMeshes = m_p->UpperBlowerPosition ? 2 : 1;
  numMeshes = m_p->YbarWeight ? 2 : 1;
  return 0;
}

int DataAdaptor::GetMeshMetadata(unsigned int id, sensei::MeshMetadataPtr& metadata)
{
  if (id > (m_p->MeanLowerBlowerVelocity ? 1 : 0))
  {
    SENSEI_ERROR("Invalid mesh ID " << id << ".");
    return 1;
  }
  if (id > (m_p->AmplitudeLowerBlowerVelocity ? 1 : 0))
  {
    SENSEI_ERROR("Invalid mesh ID " << id << ".");
    return 1;
  }
  if (id > (m_p->MeanUpperBlowerVelocity ? 1 : 0))
  {
    SENSEI_ERROR("Invalid mesh ID " << id << ".");
    return 1;
  }
  if (id > (m_p->AmplitudeUpperBlowerVelocity ? 1 : 0))
  {
    SENSEI_ERROR("Invalid mesh ID " << id << ".");
    return 1;
  }
  if (id > (m_p->UpperBlowerPosition ? 1 : 0))
  {
    SENSEI_ERROR("Invalid mesh ID " << id << ".");
    return 1;
  }
  if (id > (m_p->YbarWeight ? 1 : 0))
  {
    SENSEI_ERROR("Invalid mesh ID " << id << ".");
    return 1;
  }

  switch (id)
  {
  default:
  case 0:
    {
      metadata->MeshName = "data";
      metadata->MeshType = VTK_UNSTRUCTURED_GRID;
      metadata->CoordinateType = VTK_DOUBLE;
      // metadata->NumGhostCells = this->Internals->NumGhostCells;
      metadata->MeshAccessType = sensei::MeshMetadata::READ_ONLY;
      metadata->NumArrays = 5;
      metadata->ArrayName = {"velocity", "pressure", "temperature", "dwall", "ybar"};
      metadata->ArrayCentering = {vtkDataObject::POINT, vtkDataObject::POINT, vtkDataObject::POINT, vtkDataObject::POINT, vtkDataObject::POINT};
      metadata->ArrayComponents = {3, 1, 1, 1, 3};
      metadata->ArrayType = {VTK_DOUBLE, VTK_DOUBLE, VTK_DOUBLE, VTK_DOUBLE, VTK_DOUBLE};
      // FIXME: The mesh is static in the example I have... is this always the case?
      metadata->StaticMesh = 1;
    }
    break;
  case 1:
    {
      metadata->MeshName = "steering";
      metadata->MeshType = VTK_UNSTRUCTURED_GRID;
      metadata->CoordinateType = VTK_DOUBLE;
      metadata->MeshAccessType = sensei::MeshMetadata::READ_WRITE;
      metadata->NumArrays = 6;
      metadata->ArrayName = {"Mean Lower Blower Vel", "Amplitude Lower Blower Vel", "Mean Upper Blower Vel", "Amplitude Upper Blower Vel", "Upper Blower Position", "ybar Weight" };
      metadata->ArrayCentering = {vtkDataObject::FIELD, vtkDataObject::FIELD, vtkDataObject::FIELD, vtkDataObject::FIELD, vtkDataObject::FIELD, vtkDataObject::FIELD};
      metadata->ArrayComponents = {1, 1, 1, 1, 1, 1};
      metadata->ArrayType = {VTK_DOUBLE, VTK_DOUBLE, VTK_DOUBLE, VTK_DOUBLE, VTK_DOUBLE, VTK_DOUBLE};
      metadata->StaticMesh = 1;
    }
    break;
  }

  return 0;
}

int DataAdaptor::GetMesh(const std::string& meshName, bool structureOnly, vtkDataObject*& mesh)
{
  if (meshName == "data")
  {
    mesh = this->CreateGrid(structureOnly);
    mesh->Register(nullptr); // The returned mesh is also owned by the caller.
    return 0;
  }
  else if (meshName == "steering")
  {
    if (m_p->MeanLowerBlowerVelocity)
    {
      mesh = this->CreateSteering(structureOnly);
      this->InitializeBidirectionalInfo(mesh, meshName);
      mesh->Register(nullptr); // The returned mesh is also owned by the caller.
      return 0;
    }
    else if (m_p->AmplitudeLowerBlowerVelocity)
    {
      mesh = this->CreateSteering(structureOnly);
      this->InitializeBidirectionalInfo(mesh, meshName);
      mesh->Register(nullptr); // The returned mesh is also owned by the caller.
      return 0;
    }
    else if (m_p->MeanUpperBlowerVelocity)
    {
      mesh = this->CreateSteering(structureOnly);
      this->InitializeBidirectionalInfo(mesh, meshName);
      mesh->Register(nullptr); // The returned mesh is also owned by the caller.
      return 0;
    }
    else if (m_p->AmplitudeUpperBlowerVelocity)
    {
      mesh = this->CreateSteering(structureOnly);
      this->InitializeBidirectionalInfo(mesh, meshName);
      mesh->Register(nullptr); // The returned mesh is also owned by the caller.
      return 0;
    }
    else if (m_p->UpperBlowerPosition)
    {
      mesh = this->CreateSteering(structureOnly);
      this->InitializeBidirectionalInfo(mesh, meshName);
      mesh->Register(nullptr); // The returned mesh is also owned by the caller.
      return 0;
    }
    else if (m_p->YbarWeight)
    {
      mesh = this->CreateSteering(structureOnly);
      this->InitializeBidirectionalInfo(mesh, meshName);
      mesh->Register(nullptr); // The returned mesh is also owned by the caller.
      return 0;
    }
  }

  mesh = nullptr;
  SENSEI_ERROR("Failed to get mesh.");
  return 1;
}

int DataAdaptor::AddArray(
  vtkDataObject* mesh, const std::string& meshName, int association, const std::string& arrayName)
{
  int status = 1;
  auto grid = vtkUnstructuredGrid::SafeDownCast(mesh);
  if (!grid)
  {
    SENSEI_ERROR("No mesh to add array to");
    return status;
  }

  /* If we have a steerable parameter, add a steering adaptor.
  if (
    meshName == "steering" &&
    association == vtkDataObject::FIELD &&
    arrayName == "mean_blower_speed" &&
    m_p->MeanBlowerVelocity != nullptr)
  {
    m_p->MeanBlowerVelocityId = this->AddBidirectionalData(mesh,
      vtkDataObject::FIELD, m_p->MeanBlowerVelocity, 1, 1, "mean_blower_speed");
    if (m_p->MeanBlowerVelocityId >= 0)
    {
      this->ObserveArray(mesh, -1, [&](const ArrayKey& key, vtkAbstractArray* arr)
        {
          std::cout << "Change in " << key.ArrayName << " (" << arr << ")\n";
          if (m_grid && m_p->LiveUpdater)
          {
            std::cout << "  Updating due to change.\n";
            m_p->LiveUpdater();
            m_grid->Modified();
          }
        }
      );
      status = 0;
    }
  } */
  // If we have a steerable parameter, add a steering adaptor.
  if (
    meshName == "steering" &&
    association == vtkDataObject::FIELD) 
  {
    if (arrayName == "Mean Lower Blower Vel" && m_p->MeanLowerBlowerVelocity != nullptr)
    {
      m_p->MeanLowerBlowerVelocityId = this->AddBidirectionalData(mesh,
        vtkDataObject::FIELD, m_p->MeanLowerBlowerVelocity, 1, 1, "Mean Lower Blower Vel");
      if (m_p->MeanLowerBlowerVelocityId >= 0)
      {
        this->ObserveArray(mesh, -1, [&](const ArrayKey& key, vtkAbstractArray* arr)
          {
            std::cout << "Change in " << key.ArrayName << " (" << arr << ")\n";
            if (m_grid && m_p->LiveUpdater)
            {
              std::cout << "  Updating due to change.\n";
              m_p->LiveUpdater();
              m_grid->Modified();
            }
          }
        );
        status = 0;
      }
    }
    else if (arrayName == "Amplitude Lower Blower Vel" && m_p->AmplitudeLowerBlowerVelocity != nullptr)
    {
      m_p->AmplitudeLowerBlowerVelocityId = this->AddBidirectionalData(mesh,
        vtkDataObject::FIELD, m_p->AmplitudeLowerBlowerVelocity, 1, 1, "Amplitude Lower Blower Vel");
      if (m_p->AmplitudeLowerBlowerVelocityId >= 0)
      {
        this->ObserveArray(mesh, -1, [&](const ArrayKey& key, vtkAbstractArray* arr)
          {
            std::cout << "Change in " << key.ArrayName << " (" << arr << ")\n";
            if (m_grid && m_p->LiveUpdater)
            {
              std::cout << "  Updating due to change.\n";
              m_p->LiveUpdater();
              m_grid->Modified();
            }
          }
        );
        status = 0;
      }
    }
    else if (arrayName == "Mean Upper Blower Vel" && m_p->MeanUpperBlowerVelocity != nullptr)
    {
      m_p->MeanUpperBlowerVelocityId = this->AddBidirectionalData(mesh,
        vtkDataObject::FIELD, m_p->MeanUpperBlowerVelocity, 1, 1, "Mean Upper Blower Vel");
      if (m_p->MeanUpperBlowerVelocityId >= 0)
      {
        this->ObserveArray(mesh, -1, [&](const ArrayKey& key, vtkAbstractArray* arr)
          {
            std::cout << "Change in " << key.ArrayName << " (" << arr << ")\n";
            if (m_grid && m_p->LiveUpdater)
            {
              std::cout << "  Updating due to change.\n";
              m_p->LiveUpdater();
              m_grid->Modified();
            }
          }
        );
        status = 0;
      }
    }
    else if (arrayName == "Amplitude Upper Blower Vel" && m_p->AmplitudeUpperBlowerVelocity != nullptr)
    {
      m_p->AmplitudeUpperBlowerVelocityId = this->AddBidirectionalData(mesh,
        vtkDataObject::FIELD, m_p->AmplitudeUpperBlowerVelocity, 1, 1, "Amplitude Upper Blower Vel");
      if (m_p->AmplitudeUpperBlowerVelocityId >= 0)
      {
        this->ObserveArray(mesh, -1, [&](const ArrayKey& key, vtkAbstractArray* arr)
          {
            std::cout << "Change in " << key.ArrayName << " (" << arr << ")\n";
            if (m_grid && m_p->LiveUpdater)
            {
              std::cout << "  Updating due to change.\n";
              m_p->LiveUpdater();
              m_grid->Modified();
            }
          }
        );
        status = 0;
      }
    }
    else if (arrayName == "Upper Blower Position" && m_p->UpperBlowerPosition != nullptr)
    {
      m_p->UpperBlowerPositionId = this->AddBidirectionalData(mesh,
        vtkDataObject::FIELD, m_p->UpperBlowerPosition, 1, 1, "Upper Blower Position");
      if (m_p->UpperBlowerPositionId >= 0)
      {
        this->ObserveArray(mesh, -1, [&](const ArrayKey& key, vtkAbstractArray* arr)
          {
            std::cout << "Change in " << key.ArrayName << " (" << arr << ")\n";
            if (m_grid && m_p->LiveUpdater)
            {
              std::cout << "  Updating due to change.\n";
              m_p->LiveUpdater();
              m_grid->Modified();
            }
          }
        );
        status = 0;
      }
    }
    else if (arrayName == "ybar Weight" && m_p->YbarWeight != nullptr)
    {
      m_p->YbarWeightId = this->AddBidirectionalData(mesh,
        vtkDataObject::FIELD, m_p->YbarWeight, 1, 1, "ybar Weight");
      if (m_p->YbarWeightId >= 0)
      {
        this->ObserveArray(mesh, -1, [&](const ArrayKey& key, vtkAbstractArray* arr)
          {
            std::cout << "Change in " << key.ArrayName << " (" << arr << ")\n";
            if (m_grid && m_p->LiveUpdater)
            {
              std::cout << "  Updating due to change.\n";
              m_p->LiveUpdater();
              m_grid->Modified();
            }
          }
        );
        status = 0;
      }
    }
  }
  //else if (meshName == "data" && m_p->PointData)
  else if (meshName == "data")
  {
    switch (association)
    {
    case vtkDataObject::FIELD_ASSOCIATION_POINTS:
      if (arrayName == "velocity")
      {
        vtkNew<vtkDoubleArray> velocity;
        velocity->SetName("velocity");
        velocity->SetNumberOfComponents(3);
        velocity->SetNumberOfTuples(m_p->NumberOfPoints);
        for (vtkIdType ii = 0; ii < m_p->NumberOfPoints; ++ii)
        {
          double vx = m_p->PointData[ii];
          double vy = m_p->PointData[ii + m_p->PointDataOffset];
          double vz = m_p->PointData[ii + m_p->PointDataOffset * 2];
          velocity->SetTuple3(ii, vx, vy, vz);
        }
        grid->GetPointData()->AddArray(velocity);
        status = 0;
      }
      else if (arrayName == "pressure")
      {
        vtkNew<vtkDoubleArray> pressure;
        pressure->SetName("pressure");
        pressure->SetArray(m_p->PointData + m_p->PointDataOffset * 3, m_p->NumberOfPoints, 1);
        grid->GetPointData()->AddArray(pressure);
        status = 0;
      }
      else if (arrayName == "temperature")
      {
        vtkNew<vtkDoubleArray> temperature;
        temperature->SetName("temperature");
        temperature->SetArray(m_p->PointData + m_p->PointDataOffset * 4, m_p->NumberOfPoints, 1);
        grid->GetPointData()->AddArray(temperature);
        status = 0;
      }
      else if (arrayName == "dwall")
      {
        vtkNew<vtkDoubleArray> dwall;
        dwall->SetName("dwall");
        dwall->SetArray(m_p->Dwall, m_p->NumberOfPoints, 1);
        grid->GetPointData()->AddArray(dwall);
        status = 0;
      }
      else if (arrayName == "ybar")
      {
        vtkNew<vtkDoubleArray> ybar;
        ybar->SetName("ybar");
        ybar->SetNumberOfComponents(3);
        ybar->SetNumberOfTuples(m_p->NumberOfPoints);
        for (vtkIdType ii = 0; ii < m_p->NumberOfPoints; ++ii)
        {
          double yx = m_p->Ybar[ii];
          double yy = m_p->Ybar[ii + m_p->PointDataOffset];
          double yz = m_p->Ybar[ii + m_p->PointDataOffset * 2];
          ybar->SetTuple3(ii, yx, yy, yz);
        }
        grid->GetPointData()->AddArray(ybar);
        status = 0;
      }
      else
      {
        SENSEI_ERROR("Improper array name.");
      }
      break;
    default:
      break;
    }
  }
  else
  {
    SENSEI_ERROR("Improper mesh name or uninitialized adaptor.");
  }

  return status;
}

int DataAdaptor::AddGhostCellsArray(
  vtkDataObject*, const std::string& )
{
  SENSEI_ERROR("PHASTA does not provide ghost cells");
  return 1;
}

int DataAdaptor::ReleaseData()
{
  m_grid = nullptr;
  m_steering = nullptr;
  return 0;
}

void DataAdaptor::ObserveSteeringParameters(const std::function<void()>& updater)
{
  m_p->LiveUpdater = updater;
}

void DataAdaptor::UpdatePointCoords()
{
  auto np = m_p->NumberOfPoints;
  auto dd = m_p->Dimension;
  if (np > 0 && !m_p->StructureOnly)
  {
    auto coords = vtkDoubleArray::SafeDownCast(m_grid->GetPoints()->GetData());
#if 0
    if (dd == 3)
    {
      coords->SetArray(m_p->Coords, np * dd, /* save */ 1);
    }
    else
#endif // 0
    {
      coords->SetNumberOfTuples(np);
      for (auto ii = 0; ii < np; ++ii)
      {
        coords->SetTuple3(
          ii,
          m_p->Coords[ii],
          dd > 1 ? m_p->Coords[ii + np] : 0.0,
          dd > 2 ? m_p->Coords[ii + 2 * np] : 0.0);
      }
    }
  }
}

vtkUnstructuredGrid* DataAdaptor::CreateGrid(bool structureOnly)
{
  m_p->StructureOnly = structureOnly;
  if (!m_grid)
  {
    vtkNew<vtkPoints> pts;
    vtkNew<vtkDoubleArray> coords;
    m_grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    m_grid->SetPoints(pts);
    coords->SetNumberOfComponents(3);
    pts->SetData(coords);
  }

  // Always recompute points since the mesh may move.
  this->UpdatePointCoords();

  // Add cell connectivity if "structure" is requested
  if (!structureOnly)
  {
    // TODO: This assumes the topology does not change (no adaptation)
    if (!m_p->CellBlocks.empty() && m_grid->GetNumberOfCells() == 0)
    {
      m_grid->Allocate(m_p->NumberOfCells * 5);
      for (auto& cellBlock : m_p->CellBlocks)
      {
        int type = -1;
        switch (cellBlock.NumberOfPointsPerCell)
        {
        case 4: { type = VTK_TETRA; break; }
        case 5: { type = VTK_PYRAMID; break; }
        case 6: { type = VTK_WEDGE; break; }
        case 8: { type = VTK_HEXAHEDRON; break; }
        default:
          {
            vtkGenericWarningMacro(
              "Incorrect per-element vertex count: " << cellBlock.NumberOfPointsPerCell);
            continue;
          }
        }
        vtkIdType pts[8]; // assume for now we only have linear elements
        vtkIdType numPoints = m_grid->GetNumberOfPoints();
        for (int iCell = 0; iCell < cellBlock.NumberOfCells; iCell++)
        {
          for (int i = 0; i < cellBlock.NumberOfPointsPerCell; i++)
          {
            pts[i] = cellBlock.Connectivity[iCell + i * (cellBlock.NumberOfCells)] - 1; //-1 to get from f to c++

            if (pts[i] < 0 || pts[i] >= numPoints)
            {
              vtkGenericWarningMacro(<< pts[i] << " is not a valid node id.");
            }
          }
          if (type == VTK_TETRA)
          { // change the canonical ordering of the tet to match VTK style
            vtkIdType temp = pts[0];
            pts[0] = pts[1];
            pts[1] = temp;
          }
          m_grid->InsertNextCell(type, cellBlock.NumberOfPointsPerCell, pts);
        }
      }
    }
  }

  return m_grid;
}

vtkUnstructuredGrid* DataAdaptor::CreateSteering(bool /* structureOnly */)
{
  if (!m_steering)
  {
    m_steering = vtkSmartPointer<vtkUnstructuredGrid>::New();
  }

  return m_steering;
}


}
