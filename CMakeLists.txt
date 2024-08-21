cmake_minimum_required(VERSION 3.10)
project(DataStructures)

include(cmake/CPM.cmake)

CPMAddPackage("gh:libsdl-org/SDL#release-2.30.6")

add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/test)
add_subdirectory(${PROJECT_SOURCE_DIR}/learn)