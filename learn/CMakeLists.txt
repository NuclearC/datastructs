set(CUR_DIR ${CMAKE_CURRENT_LIST_DIR})

set(APP_SRC_FILES ${CUR_DIR}/main.cpp)

set(APP_OUTPUT_FILE learn)

add_executable(${APP_OUTPUT_FILE} ${APP_SRC_FILES})
target_link_libraries(${APP_OUTPUT_FILE} SDL2::SDL2)

set_property(TARGET ${APP_OUTPUT_FILE} PROPERTY CXX_STANDARD 23)