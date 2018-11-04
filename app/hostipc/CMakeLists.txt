set (SOURC_FILES main.cpp)

ADD_EXECUTABLE (hostipc ${SOURC_FILES}
${GORS_JSN_SOURCE}
${GORS_IPC_SOURCE})

set (CMAKE_CXX_STANDARD 11)

target_link_libraries (hostipc ${Boost_SYSTEM_LIBRARY})
target_link_libraries (hostipc ${Boost_DATE_TIME_LIBRARY})
target_link_libraries (hostipc ${Boost_THREAD_LIBRARY})
target_link_libraries (hostipc ${Boost_CHRONO_LIBRARY})

