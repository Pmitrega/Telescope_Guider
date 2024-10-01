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
        std::cout << "et:"<< elapesed_time<< std::endl;
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
		std::cout << filename << std::endl;
		cam_controller.saveBufferToFile(filename);
		// std::cout<< "Adding to buffer" << std::endl;
		// star_locator.addToBuffer(cam_controller.getBuffer().get(), 1280, 960, CV_16U, false);
		// std::cout<< "finding stars" << std::endl;
		// star_locator.findStars();
	}
    return EXIT_SUCCESS;
}