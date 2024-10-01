
set(TEST_NAME starLocator_test)

add_executable(${TEST_NAME}
    starLocator_test.cpp
    ${APP_SRC_DIR}/starLocator.cpp
)

target_include_directories(${TEST_NAME}
    PRIVATE
    ${APP_INC_DIR}
    /usr/include/opencv4/
)

target_link_libraries(
    ${TEST_NAME}
    GTest::gtest_main
)
target_link_libraries(${TEST_NAME} opencv_core opencv_imgcodecs opencv_imgproc opencv_highgui)
include(GoogleTest)
gtest_discover_tests(${TEST_NAME})

file(GLOB TEST_IMAGES
  "*.raw"
)
file(COPY ${TEST_IMAGES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB SERIES_1
    "series_2000ms/*.raw"
)
file(COPY ${SERIES_1} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/series_2000ms)

file(GLOB SERIES_2
    "series_250ms/*.png"
)
file(COPY ${SERIES_2} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/series_250ms)
