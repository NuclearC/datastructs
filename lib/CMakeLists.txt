set(CUR_DIR ${CMAKE_CURRENT_LIST_DIR})

file(GLOB LIB_SRC_FILES ${CUR_DIR}/*.cpp ${CUR_DIR}/*.h)

set(LIB_OUTPUT_FILE datastructs)


add_library(${LIB_OUTPUT_FILE} ${LIB_SRC_FILES})

set_property(TARGET ${LIB_OUTPUT_FILE} PROPERTY CXX_STANDARD 23)