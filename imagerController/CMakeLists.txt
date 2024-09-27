
set(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(APP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/app/src)
set(APP_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/app/inc)


enable_testing()

add_subdirectory(tests)
add_subdirectory(app)