aux_source_directory(. SRC_LIST)

add_executable(test_run ${SRC_LIST})

target_link_libraries(test_run PRIVATE ${GTEST_LIBRARIES} PRIVATE ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(test_run PRIVATE ../include)

gtest_discover_tests(test_run)