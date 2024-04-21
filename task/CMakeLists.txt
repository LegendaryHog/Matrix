add_executable(determinant matrix.cpp)

target_link_libraries(determinant PRIVATE ${PROJECT_NAME} Vector)
