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
	cam_controller.setCameraExposure_us(10000);
	cam_controller.setCameraGain(50);
	cam_controller.setImageType(IMG_RAW16);
	cam_controller.takeAnImage();
	cam_controller.saveBufferToFile("abc.raw");
    return EXIT_SUCCESS;
}