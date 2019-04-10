set (SOURC_FILES main.cpp)

ADD_EXECUTABLE (testh5 ${SOURC_FILES}
${GORS_H5_SOURCE})

target_link_libraries (testh5 ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries (testh5 ${Boost_SYSTEM_LIBRARY})
target_link_libraries (testh5 ${Boost_DATE_TIME_LIBRARY})
target_link_libraries (testh5 ${Boost_THREAD_LIBRARY})
target_link_libraries (testh5 ${HDF5_LIBRARIES})
target_link_libraries (testh5 ${SZIP_LIBRARY})
target_link_libraries (testh5 ${ZLIB_LIBRARY})

