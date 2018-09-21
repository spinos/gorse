set (INSTD_SOURCE_FILES instdwidget.cpp
window.cpp
main.cpp)

add_executable (instd ${GORS_MATH_SRC}  
${GORS_OGL_SRC}
${GORS_QT_OGL_SRC}
${GORS_RNG_SRC}
${INSTD_SOURCE_FILES})

target_link_libraries (instd Qt4::QtGui Qt4::QtOpenGL)
target_link_libraries (instd ${OPENGL_LIBRARIES})
target_link_libraries (instd ${Boost_SYSTEM_LIBRARY})
target_link_libraries (instd ${Boost_DATE_TIME_LIBRARY})
target_link_libraries (instd ${Boost_REGEX_LIBRARY})
target_link_libraries (instd ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries (instd ${Boost_THREAD_LIBRARY})
target_link_libraries (instd ${Boost_CHRONO_LIBRARY})