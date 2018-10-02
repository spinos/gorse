set (INSTD_SOURCE_FILES main.cpp
glprog.cpp
glwidget.cpp
mainwindow.cpp)

add_executable (instd ${INSTD_SOURCE_FILES}
${GORS_MATH_SRC} 
${GORS_GEOM_SRC} 
${GORS_QT_OGL_SRC}
${GORS_RNG_SRC}
)

target_link_libraries (instd Qt5::OpenGL)
