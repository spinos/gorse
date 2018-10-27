set (SOURC_FILES main.cpp)

ADD_EXECUTABLE (clientipc ${SOURC_FILES})

target_link_libraries (clientipc ${Boost_SYSTEM_LIBRARY})
target_link_libraries (clientipc ${Boost_DATE_TIME_LIBRARY})
target_link_libraries (clientipc ${Boost_THREAD_LIBRARY})

