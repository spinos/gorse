file (GLOB ACAC_OPS_SRC
ops/*.h
ops/*.cpp)

set (ACAC_SOURCE_FILES main.cpp 
MainWindow.cpp
AssetPalette.cpp
PiecesList.cpp
AcaciaScene.cpp 
AttribEditor.cpp
GLWidget.cpp
${ACAC_OPS_SRC}
${GORS_QT_BASE_SRC}
${GORS_QT_TREE_SRC}
${GORS_QT_GRAPH_SRC}
${GORS_QT_OGL_SRC}
${GORS_MATH_SRC}
${GORS_GEOM_SRC}
${GORS_MESH_SRC} 
${GORS_RNG_SRC}
${GORS_GJK_SRC}
${GORS_SDB_SRC}
${GORS_BVH_SRC}
${GORS_CULL_SRC}
${GORS_SDS_SRC}
${GORS_GRAPH_SRC}
${GORS_SMP_SRC}
${GORS_SSDF_SRC}
${GORS_SVF_SRC}
${GORS_GRD_SRC}
${GORS_GDF_SRC}
${GORS_H5_SRC}
${GORS_H5_MESH_SRC}
${GORS_H5_SSDF_SRC}
${GORS_H5_GRD_SRC}
${GORS_H5_SMP_SRC}
${GORS_JSN_SOURCE}
${GORS_IPC_SOURCE})

QT5_ADD_RESOURCES(ACAC_RES_SOURCES acacia.qrc)

add_executable (acacia ${ACAC_SOURCE_FILES}
${ACAC_RES_SOURCES})

set_target_properties( acacia PROPERTIES CXX_STANDARD 11 )

target_link_libraries (acacia Qt5::OpenGL)
target_link_libraries (acacia ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries (acacia ${Boost_SYSTEM_LIBRARY})
target_link_libraries (acacia ${Boost_DATE_TIME_LIBRARY})
target_link_libraries (acacia ${Boost_THREAD_LIBRARY})
target_link_libraries (acacia ${Boost_CHRONO_LIBRARY})
target_link_libraries (acacia ${HDF5_LIBRARIES})
target_link_libraries (acacia ${SZIP_LIBRARY})
target_link_libraries (acacia ${ZLIB_LIBRARY})

IF(WIN32)
## set_target_properties(acacia PROPERTIES LINK_FLAGS "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
ENDIF()