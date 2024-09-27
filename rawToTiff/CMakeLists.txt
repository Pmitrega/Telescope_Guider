project(RawToTiff)
cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)

set(SOURCES
    main.cpp
)



set(LIB_INCLUDES
    PUBLIC
    "/usr/include/opencv4/"
)

set(INCLUDES
    ${LIB_INCLUDES}
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} ${INCLUDES})
target_link_libraries(${PROJECT_NAME} opencv_core opencv_imgcodecs)