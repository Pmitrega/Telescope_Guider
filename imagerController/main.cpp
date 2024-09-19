#include <cstdlib>
#include "ASICamera2.h"
#include <iostream>
#include "cameraController.hpp"
#include "logger.hpp"

int main(){
	
	CameraController cam_controller;
	cam_controller.scanForCameras();
	auto camera_list = cam_controller.getCameraList();
	std::cout<< camera_list.size() << std::endl;
	for(auto it = camera_list.cbegin(); it < camera_list.cend(); it++ ){
		std::cout<< (*it) << std::endl;
	}
	cam_controller.openCameraByProducerAndID(ASI, 0U);
	cam_controller.setCameraExposure_us(10);
	cam_controller.setCameraGain(110);
    return EXIT_SUCCESS;
}