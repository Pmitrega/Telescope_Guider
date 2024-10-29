
set(TEST_NAME telescopeIdentifier_test)

add_executable(${TEST_NAME}
    telescopeIdentifier_test.cpp
    ${APP_SRC_DIR}/telescopeIdentifier.cpp
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

