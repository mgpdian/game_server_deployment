cmake_minimum_required(VERSION 3.0)

project("Sample2")

aux_source_directory(. ROOT_SRC)

aux_source_directory(./modules MODULES_SRC)

aux_source_directory(./utils UTILS_SRC)

add_executable(main ${ROOT_SRC} ${MODULES_SRC} ${UTILS_SRC})
