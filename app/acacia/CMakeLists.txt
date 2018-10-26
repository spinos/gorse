file (GLOB ACAC_OPS_SRC
ops/*.h
ops/*.cpp)

set (ACAC_SOURCE_FILES main.cpp 
mainwindow.cpp
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
${GORS_MESH_SRC} 
${GORS_RNG_SRC}
${GORS_GEOM_SRC})

QT5_ADD_RESOURCES(ACAC_RES_SOURCES acacia.qrc)

add_executable (acacia ${ACAC_SOURCE_FILES}
${ACAC_RES_SOURCES})

target_link_libraries (acacia Qt5::OpenGL)