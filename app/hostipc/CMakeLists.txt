set (SOURC_FILES main.cpp)

ADD_EXECUTABLE (hostipc ${SOURC_FILES}
${GORS_IPC_SOURCE})

target_link_libraries (hostipc ${Boost_SYSTEM_LIBRARY})
target_link_libraries (hostipc ${Boost_DATE_TIME_LIBRARY})
target_link_libraries (hostipc ${Boost_THREAD_LIBRARY})
target_link_libraries (hostipc ${Boost_CHRONO_LIBRARY})

