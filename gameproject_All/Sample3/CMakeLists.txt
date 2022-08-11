cmake_minimum_required(VERSION 3.0)

project("Sample3")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


aux_source_directory(. ROOT_SRC)

aux_source_directory(./modules MODULES_SRC)

add_subdirectory(utils)

add_executable(main ${ROOT_SRC} ${MODULES_SRC})

target_link_libraries(main sample_utils)
