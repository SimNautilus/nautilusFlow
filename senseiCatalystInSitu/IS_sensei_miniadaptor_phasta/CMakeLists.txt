enable_language(Fortran)
include(FortranCInterface)

FortranCInterface_HEADER(PhastaAdaptorAPIMangling.h SYMBOLS
  createpointsandallocatecells insertblockofcells addfields
  sensei_adaptors_init sensei_coprocess sensei_adaptors_fini)

set(PhastaDataAdaptor_SRCS
  Bridge.h
  Bridge.cpp
  DataAdaptor.h
  DataAdaptor.cpp
)
add_library(PhastaDataAdaptor
  ${PhastaDataAdaptor_SRCS}
)
if (ParaView_VERSION VERSION_LESS "5.7.0")
  message(FATAL_ERROR "Bidirectional PHASTA adaptor only works with ParaView 5.7.x or newer.")
  set(SENSEI_CATALYST_COMPONENTS vtkPVCatalyst)
else()
  set(SENSEI_CATALYST_COMPONENTS ParaView::Catalyst)
endif()
target_link_libraries(PhastaDataAdaptor
  ${SENSEI_CATALYST_COMPONENTS}
  sensei
)
target_include_directories(PhastaDataAdaptor
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include/sensei/phasta>
)
install(
  TARGETS PhastaDataAdaptor
  DESTINATION lib
  COMPONENT Development
)
install(
  FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Bridge.h
    ${CMAKE_CURRENT_BINARY_DIR}/PhastaAdaptorAPIMangling.h
  DESTINATION include/sensei/phasta
  COMPONENT Development
)
