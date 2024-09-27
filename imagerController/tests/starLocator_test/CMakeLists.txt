
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
target_link_libraries(${TEST_NAME} opencv_core opencv_imgcodecs opencv_imgproc)
include(GoogleTest)
gtest_discover_tests(${TEST_NAME})

file(GLOB TEST_IMAGES
  "*.raw"
)
file(COPY ${TEST_IMAGES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
