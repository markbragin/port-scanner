cmake_minimum_required(VERSION 3.10)

project(scanner)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
	src/main.cpp
	src/argparser.cpp
	src/scanner.cpp
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")
