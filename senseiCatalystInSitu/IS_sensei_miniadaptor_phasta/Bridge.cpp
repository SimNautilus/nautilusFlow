/*=========================================================================

  Program:   ParaView
  Module:    PhastaAdaptor.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "Bridge.h"

#include "DataAdaptor.h"
#include "PhastaAdaptorAPIMangling.h"
#include "sensei/ConfigurableAnalysis.h"

#include "FortranAdaptorAPI.h"
#include "vtkCPAdaptorAPI.h"
#include "vtkCPDataDescription.h"
#include "vtkCPInputDataDescription.h"
#include "vtkCPProcessor.h"
#include "vtkCellData.h"
#include "vtkCellType.h"
#include "vtkDoubleArray.h"
#include "vtkFieldData.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkUnstructuredGrid.h"

#include <stdlib.h>

vtkSmartPointer<phasta::DataAdaptor> adaptor;
vtkSmartPointer<sensei::ConfigurableAnalysis> analysis;

extern "C" void sensei_adaptors_init()
{
  const char* cfgDir = getenv("PHASTA_CONFIG");
  std::string configFile;
  if (cfgDir && cfgDir[0])
  {
    configFile = cfgDir;
    configFile += '/';
  }
  adaptor = vtkSmartPointer<phasta::DataAdaptor>::New();
  analysis = vtkSmartPointer<sensei::ConfigurableAnalysis>::New();
  configFile += "senseiPhasta.xml";
  analysis->Initialize(configFile);
}

extern "C" void createpointsandallocatecells(int* nsd, int* numPoints, double* coordsArray, int* numCells)
{
  adaptor->InitializeGrid(nsd, numPoints, coordsArray, numCells);
}

extern "C" void insertblockofcells(
  int* numCellsInBlock, int* numPointsPerCell, int* cellConnectivity)
{
  adaptor->InitializeCellBlock(numCellsInBlock, numPointsPerCell, cellConnectivity);
}

extern "C" void addfields(int* nshg, int* ndof, double* dofArray, int* compressibleFlow, double* vmeanLow, double* vampLow, double* vmeanUp, double* vampUp, double* upperBlowerPosition, double* ybarWeight, double* dwalArray, double* ybarArray)
{
  adaptor->InitializeFields(nshg, ndof, dofArray, compressibleFlow, vmeanLow, vampLow, vmeanUp, vampUp, upperBlowerPosition, ybarWeight, dwalArray, ybarArray);
}

extern "C" void sensei_coprocess(int* timestep)
{
  double time = static_cast<double>(*timestep);
  adaptor->SetDataTime(time);
  adaptor->SetDataTimeStep(*timestep);

  analysis->Execute(adaptor);
  adaptor->ReleaseData();
}

extern "C" void sensei_adaptors_fini()
{
  analysis->Finalize();
  adaptor  = nullptr;
  analysis = nullptr;
}
