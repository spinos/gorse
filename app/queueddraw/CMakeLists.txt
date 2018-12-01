set (QUEUED_SOURCE_FILES main.cpp
glwidget.cpp
cylinderthread.cpp
mainwindow.cpp)

add_executable (queueddraw ${QUEUED_SOURCE_FILES}
${GORS_MATH_SRC} 
${GORS_GEOM_SRC} 
${GORS_RNG_SRC}
${GORS_BVH_SRC}
${GORS_SDB_SOURCE}
${GORS_QT_OGL_SRC}
)

target_link_libraries (queueddraw Qt5::OpenGL)
