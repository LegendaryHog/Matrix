
aux_source_directory(. SRC_LIST)

add_executable(matrix ${SRC_LIST})

target_include_directories(matrix PRIVATE .)