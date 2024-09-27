#include <cstdlib>
#include <thread>
#include <fstream>

#include "ASICamera2.h"
#include <iostream>
#include "cameraController.hpp"
#include "starLocator.hpp"
#include "logger.hpp"

int main(){
	CameraController cam_controller;
	StarLocator star_locator;
	cam_controller.scanForCameras();
	auto camera_list = cam_controller.getCameraList();
	std::cout<< camera_list.size() << std::endl;
	for(auto it = camera_list.cbegin(); it < camera_list.cend(); it++ ){
		std::cout<< (*it) << std::endl;
	}
	cam_controller.openCameraByProducerAndID(ASI, 0U);
	cam_controller.setCameraExposure_us(1500'000);
	cam_controller.setCameraGain(80);
	cam_controller.setImageType(IMG_RAW16);
	uint8_t buffer[1280*960*2];
	while(true){
		if(CAM_CTRL_FAIL == cam_controller.takeAnImage()){
			continue;
		}
		std::cout<< "Adding to buffer" << std::endl;
		star_locator.addToBuffer(cam_controller.getBuffer().get(), 1280, 960, CV_16U, false);
		std::cout<< "finding stars" << std::endl;
		star_locator.findStars();
	}
    return EXIT_SUCCESS;
}