#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

#include "ASICamera2.h"
#include "cameraController.hpp"
#include "logger.hpp"



std::map<CameraProducer, std::string> producerToString = { { ASI, "ASI" }, 
                                                        { SVBONY, "SVBONY" }, 
                                                        { NUMBER_OF_CAMERA_PRODUCERS, "INVALID PRODUCER" } }; 


std::ostream &operator<<(std::ostream &os, CameraInfo const &cam_info) { 

    return os<<std::setprecision(4)<<
               "------Camera "<<cam_info.cameraID<<" Info------\r\n"<<
               "Producer:"   << cam_info.cameraProducer      <<"\r\n"
               "Width:"      << cam_info.cameraWidth         <<"\r\n"
               "Height:"     << cam_info.cameraHeigth        <<"\r\n"
               "Pixel_Size:" << cam_info.PixelSize           <<"\r\n"
               "Color:"      << (cam_info.isColor?"yes":"no")<<"\r\n";
}

int CameraController::scanForASI(){
    ASI_CAMERA_INFO ASICameraInfo;
    int ASI_numDevices = ASIGetNumOfConnectedCameras();
	for(int i = 0; i < ASI_numDevices; i++)
	{
        CameraInfo camera_info;
		ASIGetCameraProperty(&ASICameraInfo, i);
        camera_info.cameraID = i;
		camera_info.cameraProducer = ASI;
        camera_info.cameraHeigth = ASICameraInfo.MaxHeight;
        camera_info.cameraWidth = ASICameraInfo.MaxWidth;
        camera_info.isColor = ASICameraInfo.IsColorCam;
        camera_info.PixelSize = ASICameraInfo.PixelSize;
        m_camera_list.push_back(std::move(camera_info));
	}
    return ASI_numDevices;
}

int CameraController::scanForSVBONY(){
    LOG_WARNING("UNIMPLEMENTED_FUNCTION_CALL: %s () ",__func__);
    return 0;
}

CameraController::~CameraController(){
    if(m_connected_camera != nullptr)
        closeCamera();
}


bool CameraController::closeCamera(){
    bool ret = CAM_CTRL_FAIL;
    if(m_connected_camera != nullptr){
        switch (m_connected_camera->cameraProducer)
        {
        case ASI:
            if(ASICloseCamera(m_connected_camera->cameraID) == ASI_SUCCESS){
                LOG_INFO("Closed %s %d camera \r\n", producerToString[m_connected_camera->cameraProducer].c_str(),m_connected_camera->cameraID ) ;
                ret = CAM_CTRL_SUCCESS;
                m_connected_camera = nullptr;
            };
            break;
        case SVBONY:
            /* code */
            break;
        
        default:
            break;
        }
    }
    else{
        LOG_WARNING("Can't close, there is no camera connected \r\n");
    }
    return ret;
}

bool CameraController::openCameraByProducerAndID(CameraProducer producer, uint16_t camID){
    bool ret = CAM_CTRL_FAIL;
    for(auto it = m_camera_list.cbegin(); it < m_camera_list.cend(); it++ ){
        if(it->cameraID == camID && it->cameraProducer == producer){
            m_selected_camera_id = camID;
            switch (producer)
            {
                case CameraProducer::ASI:
                        if(ASIOpenCamera(camID) != ASI_SUCCESS){
                            ret = CAM_CTRL_FAIL;
                            break;
                        }
                        ret = CAM_CTRL_SUCCESS;
                        if(ASIInitCamera(camID) != ASI_SUCCESS){
                            ret = CAM_CTRL_FAIL;
                            break;
                        }
                        m_connected_camera = std::make_unique<CameraInfo>(*it);                    
                        LOG_INFO("Succesfully opened camera  %s %d \r\n", producerToString[m_connected_camera->cameraProducer].c_str(),
                                                                     m_connected_camera->cameraID);
                    break;
                
                case CameraProducer::SVBONY:
                    /* code */
                    break;
                default:
                    break;
            }
            ret = CAM_CTRL_SUCCESS;
        }
	}
    if(ret == CAM_CTRL_FAIL){
        LOG_ERROR("Couldn't open camera %s %d\r\n", producerToString[producer].c_str(), camID);
    }
    return ret;
}

int CameraController::scanForCameras(){
    int detected_camera_numb = 0;
    detected_camera_numb += scanForASI();
    detected_camera_numb += scanForSVBONY();
    return detected_camera_numb;
}


int CameraController::setCameraExposure_us(int exposure){
    int ret = -1;
    if(m_connected_camera != nullptr){
        switch (m_connected_camera->cameraProducer)
        {
        case ASI:
	        if(ASISetControlValue(m_connected_camera->cameraID, ASI_EXPOSURE, exposure, ASI_FALSE) == ASI_SUCCESS){
                long read_exposure; 
                ASI_BOOL is_auto; 
                ASIGetControlValue(m_connected_camera->cameraID, ASI_EXPOSURE, &read_exposure, &is_auto);
                if(read_exposure != exposure){
                    LOG_WARNING("Requested exposure is out of range. Value %dus has been set instead of %dus. \r\n", read_exposure, exposure);
                }else{
                    LOG_INFO("Exposure of camera has been set to %dus \r\n", exposure);
                }
                ret = read_exposure;
            };
            break;
        case SVBONY:

            break;
        default:
            break;
        }
    }
    return ret;
}

int CameraController::setCameraGain(int gain){
        int ret = -1;
    if(m_connected_camera != nullptr){
        switch (m_connected_camera->cameraProducer)
        {
        case ASI:
	        if(ASISetControlValue(m_connected_camera->cameraID, ASI_GAIN, gain, ASI_FALSE) == ASI_SUCCESS){
                long read_gain; 
                ASI_BOOL is_auto; 
                ASIGetControlValue(m_connected_camera->cameraID, ASI_GAIN, &read_gain, &is_auto);
                if(read_gain != gain){
                    LOG_WARNING("Requested gain is out of range. Value %d has been set instead of %d. \r\n", read_gain, gain);
                }else{
                    LOG_INFO("Gain of camera has been set to %d \r\n", gain);
                }
                ret = gain;
            };
            break;
        case SVBONY:

            break;
        default:
            break;
        }
    }
    return ret;
}



