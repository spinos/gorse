set (SOURC_FILES main.cpp)

ADD_EXECUTABLE (bvht ${SOURC_FILES}
${GORS_MATH_SRC}
${GORS_GEOM_SRC}
${GORS_MESH_SRC} 
${GORS_RNG_SRC}
${GORS_GJK_SRC}
${GORS_SDB_SRC}
${GORS_BVH_SRC}
${GORS_CULL_SRC}
${GORS_SSDF_SRC}
${GORS_GRD_SRC})

set_target_properties( bvht PROPERTIES CXX_STANDARD 11 )

target_link_libraries (bvht ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries (bvht ${Boost_SYSTEM_LIBRARY})
target_link_libraries (bvht ${Boost_DATE_TIME_LIBRARY})
target_link_libraries (bvht ${Boost_CHRONO_LIBRARY})
target_link_libraries (bvht ${Boost_THREAD_LIBRARY})
