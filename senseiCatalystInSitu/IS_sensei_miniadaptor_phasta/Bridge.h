/*=========================================================================

  Program:   ParaView
  Module:    PhastaAdaptor.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef PhastaBridge_h
#define PhastaBridge_h

extern "C"
{

void sensei_adaptors_init();
void createpointsandallocatecells(int* nsd, int* numPoints, double* coordsArray, int* numCells);
void insertblockofcells( int* numCellsInBlock, int* numPointsPerCell, int* cellConnectivity);
void addfields(int* nshg, int* ndof, double* dofArray, int* compressibleFlow, double* vmeanLow, double* vampLow, double* vmeanUp, double* vampUp, double* upperBlowerPosition, double* ybarWeight, double* dwalArray, double* ybarArray);
void sensei_coprocess(int* timestep);
void sensei_adaptors_fini();

} /* extern "C" */

#endif /* PhastaBridge_h */
