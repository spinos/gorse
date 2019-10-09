set (PARTT_SOURCE_FILES main.cpp
SimulationThread.cpp
ParticleRenderer.cpp
glwidget.cpp
mainwindow.cpp)

add_executable (particle ${PARTT_SOURCE_FILES}
${GORS_MATH_SRC} 
${GORS_GEOM_SRC} 
${GORS_QT_OGL_SRC}
${GORS_RNG_SRC}
${GORS_PBD_SRC}
${GORS_SDB_SRC}
${GORS_SDS_SRC})

target_link_libraries (particle Qt5::OpenGL)
target_link_libraries (particle ${BLAS_LIBRARY})
target_link_libraries (particle ${LAPACK_LIBRARY})
target_link_libraries (particle ${F2C_LIBRARY})
target_link_libraries (particle ${Boost_SYSTEM_LIBRARY})
target_link_libraries (particle ${Boost_DATE_TIME_LIBRARY})
target_link_libraries (particle ${Boost_CHRONO_LIBRARY})
target_link_libraries (particle ${Boost_THREAD_LIBRARY})
