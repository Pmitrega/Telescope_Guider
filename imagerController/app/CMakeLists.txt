project(ImagerController)
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    src/cameraController.cpp
    src/starLocator.cpp
    src/telescopeIdentifier.cpp
    src/stepperComm.cpp
    src/mqtt_client.cpp
    src/Worker.cpp
    src/motorMonitor.cpp
)

set(STATIC_LIBS
    ${BASE_DIR}/lib/ASICamera/x64
)
set(LIB_INCLUDES
    PUBLIC
    ${BASE_DIR}/lib/ASICamera/includes
    ${BASE_DIR}/lib/include
    /usr/include/opencv4/
)
set(INCLUDES
    ${LIB_INCLUDES}
    inc
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} ${INCLUDES})
target_link_libraries(${PROJECT_NAME} ASICamera2 opencv_core opencv_imgcodecs opencv_imgproc opencv_highgui paho-mqtt3c boost_thread)