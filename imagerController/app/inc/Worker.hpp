#
#pragma once
#include <thread>
#include <fstream>
#include <chrono>

#include "cameraController.hpp"
#include "starLocator.hpp"
#include "logger.hpp"
#include "stepperComm.hpp"
#include "mqtt_client.hpp"
#include <mutex>
#include <utility>

struct stepperSensorInfo{
    float value;
    bool has_updated;
};


struct cameraSettings{
    std::pair<std::string, bool> producer = {"", false};
    std::pair<std::string, bool> exposure = {"500", false};
    std::pair<std::string, bool> gain =     {"70", false};
    std::pair<std::string, bool> interval = {"2000", false};
    std::pair<std::string, bool> ROI_width = {"1280", false};
    std::pair<std::string, bool> ROI_heigth = {"960", false};
    std::pair<std::string, bool> ROI_xstart = {"0", false};
    std::pair<std::string, bool> ROI_ystart = {"0", false};
};

struct imageInfo{
    std::pair<uint8_t*, bool> image_buffer =    {nullptr, false};
    std::pair<std::string, bool> image_title = {"", false};
    std::pair<std::string, bool> image_width =  {"", false};
    std::pair<std::string, bool> image_heigth = {"", false};
    std::pair<std::string, bool> image_xstart = {"", false};
    std::pair<std::string, bool> image_ystart = {"", false};
    std::pair<std::string, bool> image_exposure = {"", false};
    std::pair<std::string, bool> image_capture_time = {"", false};
    std::pair<std::string, bool> image_gain = {"", false};
};


struct stepperControllerSensors{
    stepperSensorInfo BattVolt = {0, true};
    stepperSensorInfo Buck1Volt = {0, true};
    stepperSensorInfo Buck2Volt = {0, true};
    stepperSensorInfo M1C1Current = {0, true};
    stepperSensorInfo M1C2Current = {0, true};
    stepperSensorInfo M2C1Current = {0, true};
    stepperSensorInfo M2C2Current = {0, true};
    stepperSensorInfo BattCurren = {0, true};
};


struct stepperControllerMotorsInfo{
    std::pair<std::string, bool> mode =        {"AUTO", true};
    std::pair<std::string, bool> dec_speed =   {"0", true};
    std::pair<std::string, bool> ra_speed =    {"0", true};
    std::pair<std::string, bool> coil1_ctrl =  {"0", false};
    std::pair<std::string, bool> coil2_ctrl =  {"0", false};
    std::pair<std::string, bool> coil3_ctrl =  {"0", false};
    std::pair<std::string, bool> coil4_ctrl =  {"0", false};
};

class GuiderWorker{
public:
    GuiderWorker();
    void run();
    void handleMQTTTransmission();
    void handleMQTTRecieve();
    void handleUARTRequests();
    void handleCamera();
private:
    bool checkForUARTUpdate();
    bool waitForUART();
    void handleCameraSettChangeReq();
    void requestImageTransimssion();
    void requestCameraInfoTransmision();
    CameraController m_cam_controller;
	MqttClientWrapper m_mqtt_client;
    stepperCommunicator m_step_com;
    stepperControllerSensors m_stepper_sensors;
    stepperControllerMotorsInfo m_motor_info;
    imageInfo m_image_info;
    cameraSettings m_camera_settings;
    bool m_send_jpg = false;
    bool m_jpg_ready = false;
};