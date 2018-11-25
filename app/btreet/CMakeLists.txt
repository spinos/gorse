set (SOURC_FILES main.cpp)

ADD_EXECUTABLE (btreet ${SOURC_FILES}
${GORS_SDB_SOURCE})

set (CMAKE_CXX_STANDARD 11)

target_link_libraries (btreet ${Boost_SYSTEM_LIBRARY})
target_link_libraries (btreet ${Boost_DATE_TIME_LIBRARY})
target_link_libraries (btreet ${Boost_THREAD_LIBRARY})
target_link_libraries (btreet ${Boost_CHRONO_LIBRARY})

