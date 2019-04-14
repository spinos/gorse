file (GLOB VCHL_OPS_SRC
ops/*.h
ops/*.cpp)

file (GLOB VCHL_WIDGETS_SRC
widgets/*.h
widgets/*.cpp)

set (VCHL_SOURCE_FILES main.cpp 
${VCHL_WIDGETS_SRC}
VachellScene.cpp 
MainWindow.cpp
${ALOE_BRDF_SRC}
${ALOE_INTERFACE_SRC}
${ALOE_QT_INTERFACE_SRC}
${ALOE_IMG_SRC}
${GORS_QT_BASE_SRC}
${GORS_QT_TREE_SRC}
${GORS_QT_GRAPH_SRC}
${GORS_MATH_SRC}
${GORS_GEOM_SRC}
${GORS_MESH_SRC} 
${GORS_RNG_SRC}
${GORS_GJK_SOURCE}
${GORS_SDB_SOURCE}
${GORS_BVH_SRC}
${GORS_CULL_SRC}
${GORS_H5_SOURCE}
${GORS_H5_MESH_SOURCE}
${GORS_JSN_SOURCE}
${GORS_IPC_SOURCE})

QT5_ADD_RESOURCES(VCHL_RES_SOURCES vachell.qrc)

add_executable (vachellia ${VCHL_SOURCE_FILES}
${VCHL_RES_SOURCES})

set_target_properties( vachellia PROPERTIES CXX_STANDARD 11 )

target_link_libraries (vachellia Qt5::Widgets)
target_link_libraries (vachellia ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries (vachellia ${Boost_SYSTEM_LIBRARY})
target_link_libraries (vachellia ${Boost_DATE_TIME_LIBRARY})
target_link_libraries (vachellia ${Boost_THREAD_LIBRARY})
target_link_libraries (vachellia ${Boost_CHRONO_LIBRARY})
target_link_libraries (vachellia ${HDF5_LIBRARIES})
target_link_libraries (vachellia ${SZIP_LIBRARY})
target_link_libraries (vachellia ${ZLIB_LIBRARY})
target_link_libraries (vachellia ${OpenEXR_Library})

IF(WIN32)
## set_target_properties(acacia PROPERTIES LINK_FLAGS "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
ENDIF()