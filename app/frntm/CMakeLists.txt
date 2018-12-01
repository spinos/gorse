set (FRNTM_SOURCE_FILES tube.cpp 
main.cpp
glprog.cpp
glwidget.cpp
mainwindow.cpp)

add_executable (frntm ${FRNTM_SOURCE_FILES}
${GORS_MATH_SRC} 
${GORS_GEOM_SRC} 
${GORS_MESH_SRC} 
${GORS_RNG_SRC}
${GORS_QT_OGL_SRC}
${GORS_SDB_SOURCE})

target_link_libraries (frntm Qt5::OpenGL)
