project(ImagerController)
cmake_minimum_required(VERSION 3.12)


set(SOURCES
    main.cpp
    src/cameraController.cpp
)

set(STATIC_LIBS
    lib/ASICamera/x64
)

set(LIB_INCLUDES
    PUBLIC
    lib/ASICamera/includes
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} ${LIB_INCLUDES})
target_link_libraries(${PROJECT_NAME} ASICamera2)