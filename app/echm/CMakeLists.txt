set (ECHM_SOURCE_FILES main.cpp
glwidget.cpp
mainwindow.cpp)

add_executable (echm ${ECHM_SOURCE_FILES}
${GORS_MATH_SRC} 
${GORS_GEOM_SRC} 
${GORS_MESH_SRC}
${GORS_RNG_SRC}
${GORS_QT_OGL_SRC}
)

target_link_libraries (echm Qt5::OpenGL)
