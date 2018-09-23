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
target_link_libraries (instd ${Boost_SYSTEM_LIBRARY})
target_link_libraries (instd ${Boost_DATE_TIME_LIBRARY})
target_link_libraries (instd ${Boost_REGEX_LIBRARY})
target_link_libraries (instd ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries (instd ${Boost_THREAD_LIBRARY})
target_link_libraries (instd ${Boost_CHRONO_LIBRARY})