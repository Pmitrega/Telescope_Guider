#include <cstdlib>
#include <thread>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>


#include "ASICamera2.h"
#include <iostream>
#include "cameraController.hpp"
#include "starLocator.hpp"
#include "logger.hpp"
#include "stepperComm.hpp"
#include "mqtt_client.hpp"


void takeTestImages(){
	CameraController cam_controller;
	StarLocator star_locator;
	MqttClientWrapper mqtt_client;
	cam_controller.scanForCameras();
	auto camera_list = cam_controller.getCameraList();
	std::cout<< camera_list.size() << std::endl;
	for(auto it = camera_list.cbegin(); it < camera_list.cend(); it++ ){
		std::cout<< (*it) << std::endl;
	}
	cam_controller.openCameraByProducerAndID(ASI, 0U);
	cam_controller.setCameraExposure_us(500'000);
	cam_controller.setCameraGain(70);
	cam_controller.setImageType(IMG_RAW16);
	uint8_t buffer[1280*960*2];
	int  i = 0;
	int ms_betw_frames = 2000;
	while(true){
        auto capture_start_time =  std::chrono::system_clock::now();
		if(CAM_CTRL_FAIL == cam_controller.takeAnImage()){
			continue;
		}
		auto capture_end_time =  std::chrono::system_clock::now();
        std::chrono::duration<float> elapesed_time = capture_end_time - capture_start_time;
        int sleep_time = (ms_betw_frames) - static_cast<int>(1000*elapesed_time.count());
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
		std::string filename;
		if(i < 10){
			filename = std::string("image0") + std::to_string(i) + ".raw";
		}
		else{
			filename = std::string("image") + std::to_string(i) + ".raw";
		}
		i++;
		LOG_INFO("Saving to file ... \r\n");
		cam_controller.saveBufferToFile(filename);
		// std::cout<< "Adding to buffer" << std::endl;
		// star_locator.addToBuffer(cam_controller.getBuffer().get(), 1280, 960, CV_16U, false);
		// std::cout<< "finding stars" << std::endl;
		// star_locator.findStars();
	}
}

void streamCameraMQTT(){

	CameraController cam_controller;
	MqttClientWrapper mqtt_client;
	cam_controller.scanForCameras();
	auto camera_list = cam_controller.getCameraList();
	std::cout<< camera_list.size() << std::endl;
	for(auto it = camera_list.cbegin(); it < camera_list.cend(); it++ ){
		std::cout<< (*it) << std::endl;
	}
	cam_controller.openCameraByProducerAndID(ASI, 0U);
	cam_controller.setCameraExposure_us(500'000);
	cam_controller.setCameraGain(70);
	cam_controller.setImageType(IMG_RAW16);
	uint8_t buffer[1280*960*2];
	int  i = 0;
	int ms_betw_frames = 3000;
	while(true){
        auto capture_start_time =  std::chrono::system_clock::now();
		auto t = std::time(nullptr);
		if(CAM_CTRL_FAIL == cam_controller.takeAnImage()){
			continue;
		}
		std::string filename;
		if(i < 10){
			filename = std::string("image0") + std::to_string(i) + ".raw";
		}
		else{
			filename = std::string("image") + std::to_string(i) + ".raw";
		}
		auto tm = *std::localtime(&t);
		std::ostringstream oss;
		oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
		mqtt_client.publishMessageString("images/raw/title", filename);
		mqtt_client.publishMessageString("images/raw/capture_time", oss.str());
		mqtt_client.publishMessageNumber("images/raw/exposure", 500);
		mqtt_client.publishMessageNumber("images/raw/gain", 70);
		mqtt_client.publishMessageString("images/raw/cameraType", "ASI");
		mqtt_client.publishMessageString("images/raw/ROIwidth", "1280");
		mqtt_client.publishMessageString("images/raw/ROIheight", "960");
		mqtt_client.publishMessageString("images/raw/ROIstart_x", "0");
		mqtt_client.publishMessageString("images/raw/ROIstart_y", "0");
		mqtt_client.publishMessageImageRaw("images/raw", (char*)cam_controller.getBuffer().get(), 1280*960*2);
		auto capture_end_time =  std::chrono::system_clock::now();
        std::chrono::duration<float> elapesed_time = capture_end_time - capture_start_time;
        int sleep_time = (ms_betw_frames) - static_cast<int>(1000*elapesed_time.count());
		LOG_INFO("sleep time: %d ms", sleep_time)
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

		i++;
	}

}
		

void testStepperControllerComm(){
	stepperCommunicator step_com;
	step_com.connectSerial("/dev/ttyUSB0");
	step_com.setManualMode();
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	step_com.setAutoMode();
	step_com.setRaSpeed(1200);
	// std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	// step_com.setRaSpeed(5000 - i*100 + 50);
	for(int i = 0; i < 100;i++){
		// step_com.setRaSpeed(5000 - i*100 + 50);
		step_com.reqBattVolt();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	step_com.reqBuck1Volt();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	step_com.reqBuck2Volt();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	step_com.reqM1C1Current();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	step_com.reqM1C2Current();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	step_com.reqM2C1Current();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	step_com.reqM2C2Current();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	step_com.reqBattCurrent();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	step_com.setManualMode();
}


int main(){
	streamCameraMQTT();
	//testStepperControllerComm();
	//testMQTT();
    return EXIT_SUCCESS;
}