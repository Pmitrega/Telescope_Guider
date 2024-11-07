project(ImagerController)
cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)

set(SOURCES
    main.cpp
    src/cameraController.cpp
    src/starLocator.cpp
    src/telescopeIdentifier.cpp
    src/stepperComm.cpp
)

set(STATIC_LIBS
    ${BASE_DIR}/lib/ASICamera/x64
)
set(LIB_INCLUDES
    PUBLIC
    ${BASE_DIR}/lib/ASICamera/includes
    /usr/include/opencv4/
)
set(INCLUDES
    ${LIB_INCLUDES}
    inc
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} ${INCLUDES})
target_link_libraries(${PROJECT_NAME} ASICamera2 opencv_core opencv_imgcodecs opencv_imgproc opencv_highgui boost_system)