#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <memory>
#include <fstream>
#include <chrono>
#include <ctime>
#include <thread>

#include "ASICamera2.h"
#include "cameraController.hpp"
#include "logger.hpp"




std::map<CameraProducer, std::string> producerToString = { { ASI, "ASI" },
                                                        { SVBONY, "SVBONY" },
                                                        { NUMBER_OF_CAMERA_PRODUCERS, "INVALID PRODUCER" } };



std::map<IMG_TYPE, std::string> ImageTypeCommonToString = {
                                                        {IMG_RAW8,  "IMG_RAW8"},
                                                        {IMG_RGB24, "IMG_RGB24"},
                                                        {IMG_RAW16, "IMG_RAW16"},
                                                        {IMG_Y8,    "IMG_Y8"},
                                                        };

std::map<IMG_TYPE, ASI_IMG_TYPE> ImageTypeCommonToASI = {
                                                        {IMG_RAW8,  ASI_IMG_RAW8},
                                                        {IMG_RGB24, ASI_IMG_RGB24},
                                                        {IMG_RAW16, ASI_IMG_RAW16},
                                                        {IMG_Y8,    ASI_IMG_Y8},
                                                        };

std::map<IMG_TYPE, uint8_t> ImageTypeCommonToBytes = {
                                                        {IMG_RAW8,  1},
                                                        {IMG_RGB24, 3},
                                                        {IMG_RAW16, 2},
                                                        {IMG_Y8,    1},
                                                        };


std::map<ASI_IMG_TYPE, IMG_TYPE> ImageTypeASIToCommon = {
                                                        {ASI_IMG_RAW8,  IMG_RAW8},
                                                        {ASI_IMG_RGB24, IMG_RGB24},
                                                        {ASI_IMG_RAW16, IMG_RAW16},
                                                        {ASI_IMG_Y8,    IMG_Y8},
                                                        };




std::ostream &operator<<(std::ostream &os, CameraInfo const &cam_info) { 

    return os<<std::setprecision(4)<<
               "------Camera "<<cam_info.cameraID<<" Info------\r\n"        <<
               "Producer:"   << producerToString[cam_info.cameraProducer]   <<"\r\n"
               "Width:"      << cam_info.cameraWidth                        <<"\r\n"
               "Height:"     << cam_info.cameraHeigth                       <<"\r\n"
               "Pixel_Size:" << cam_info.PixelSize                          <<"\r\n"
               "Color:"      << (cam_info.isColor?"yes":"no")               <<"\r\n";
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
                LOG_INFO("Closed %s %d camera \r\n" , producerToString[m_connected_camera->cameraProducer].c_str()
                                                    , m_connected_camera->cameraID );
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


bool CameraController::setImageType(IMG_TYPE img_type){
    bool ret = CAM_CTRL_FAIL;
    if(m_connected_camera != nullptr){
        switch (m_connected_camera->cameraProducer)
            {
                case CameraProducer::ASI:
                    if(ASI_SUCCESS == ASISetROIFormat(  m_connected_camera->cameraID,
                                                        m_roi_info.width,
                                                        m_roi_info.height,
                                                        1,
                                                        ImageTypeCommonToASI[img_type]
                                    ))
                    {
                        ret = CAM_CTRL_SUCCESS;
                        prepareImageBuffer();
                    }

                    break;
                case CameraProducer::SVBONY:
                    /* code */
                    break;
                default:
                    break;
            }
        if(ret == CAM_CTRL_FAIL){
            LOG_ERROR("Couldn't set image type for connected camera\r\n");
        }
        else{

        }
    }
    else{
        LOG_ERROR("Couldn't set image type, no camera connected! \r\n");
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
                        if(ASIInitCamera(camID) != ASI_SUCCESS){
                            ret = CAM_CTRL_FAIL;
                            break;
                        }
                        ret = CAM_CTRL_SUCCESS;
                        m_connected_camera = std::make_unique<CameraInfo>(*it);
                    break;
                case CameraProducer::SVBONY:
                    /* code */
                    break;
                default:
                    break;
            }
            if(ret == CAM_CTRL_SUCCESS){
                m_roi_info.startX = 0;
                m_roi_info.startY = 0;
                m_roi_info.width = m_connected_camera->cameraWidth;
                m_roi_info.height = m_connected_camera->cameraHeigth;
                LOG_INFO("Succesfully opened camera  %s %d \r\n", producerToString[m_connected_camera->cameraProducer].c_str(),
                                                                        m_connected_camera->cameraID);
                prepareImageBuffer();
            }
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
                    LOG_INFO("Exposure of camera has been set to %0.1lfms \r\n", static_cast<double>(exposure/1000));
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
    else{
        LOG_ERROR("Couldn't set exposure, no camera connected! \r\n")
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
    else{
        LOG_ERROR("Couldn't set gain, no camera connected! \r\n");
    }
    return ret;
}


IMG_TYPE CameraController::getImageTypeASI(){
    IMG_TYPE ret = IMG_END;
    if(m_connected_camera != nullptr){
        int width;
        int height;
        int bin;
        ASI_IMG_TYPE img_type;
        if(ASIGetROIFormat(m_connected_camera->cameraID, &width, &height,  &bin, &img_type) == ASI_SUCCESS){
            ret = ImageTypeASIToCommon[img_type];
        };
    }
    return ret;
}

std::pair<uint16_t, uint16_t> CameraController::getImageSizeASI(){
    std::pair<uint16_t, uint16_t> ret = {0,0};
    if(m_connected_camera !=nullptr){
        int width;
        int height;
        int bin;
        ASI_IMG_TYPE img_type;
        if(ASIGetROIFormat(m_connected_camera->cameraID, &width, &height,  &bin, &img_type) == ASI_SUCCESS){
            ret.first = static_cast<uint16_t>(width);
            ret.second = static_cast<uint16_t>(height);
        }
    }
    return ret;
}

void CameraController::runContionousCapture(uint16_t ms_betw_frames){
    int read_exposure = getExposuretTime_us();
    if(read_exposure == -1){
        LOG_ERROR("Couldn't start countinous capture! \r\n");
        return;
    }

    m_run_countinious_capture.store(true);
    while(m_run_countinious_capture.load()){
        auto capture_start_time =  std::chrono::system_clock::now();
        if(takeAnImage() != CAM_CTRL_FAIL){
            saveBufferToFile("test_dd.raw");
            auto capture_end_time =  std::chrono::system_clock::now();
            std::chrono::duration<float> elapesed_time = capture_end_time - capture_start_time;
            // std::cout << "et:"<< elapesed_time<< std::endl;
            int sleep_time = (ms_betw_frames) - static_cast<int>(1000*elapesed_time.count());
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            capture_end_time =  std::chrono::system_clock::now();
            elapesed_time = capture_end_time - capture_start_time;
            // std::cout << "Image captured et:"<< elapesed_time<< std::endl;
        }
        else{
            LOG_ERROR("Couldn't take an image! \r\n");
            m_run_countinious_capture.store(false);
        }

    }
    return;
}

bool CameraController::prepareImageBuffer(){
    bool ret = CAM_CTRL_FAIL;
    if(m_connected_camera == nullptr){
        LOG_ERROR("No camera connected, can't prepate image buffer \r\n")
    }
    else{
        uint8_t size_multipier;
        std::pair<uint16_t, uint16_t> img_size;
        size_t image_bytes;
        switch (m_connected_camera->cameraProducer)
        {
        case ASI:
            m_img_type = getImageTypeASI();
            img_size =  getImageSizeASI();
            m_roi_info.width = img_size.first;
            m_roi_info.height = img_size.second;
            size_multipier  = ImageTypeCommonToBytes[m_img_type];
            image_bytes = size_multipier* m_roi_info.width* m_roi_info.height;
            LOG_INFO("Camera is set to %s w:%d h:%d, buffer size: %d \r\n",  ImageTypeCommonToString[m_img_type].c_str(),
                                                                        m_roi_info.width,
                                                                        m_roi_info.height,
                                                                        image_bytes)
            if(m_img_type != IMG_END && img_size.first != 0 && img_size.second !=0){
                uint8_t* buffer = new uint8_t[image_bytes];
                m_image_buffer = std::make_shared<uint8_t*>(buffer);
                m_image_buffer_size = image_bytes;
            }
            break;
        case SVBONY:
            break;
        default:
            break;
        }
    }
    return ret;
}



bool CameraController::takeAnImage(){
    bool ret = CAM_CTRL_FAIL;
    switch (m_connected_camera->cameraProducer)
    {
    case ASI:
        {
        if(startExposure() == CAM_CTRL_FAIL){
            return CAM_CTRL_FAIL;
        }
        ASI_EXPOSURE_STATUS exp_status = ASI_EXP_WORKING;
        while(exp_status == ASI_EXP_WORKING){
            if(ASIGetExpStatus(m_connected_camera->cameraID, &exp_status) != ASI_SUCCESS){
                LOG_ERROR("Couldn't get exposure status \r\n");
                return CAM_CTRL_FAIL;
            }
        }
        if(exp_status == ASI_EXP_SUCCESS){
            auto asi_ret = ASIGetDataAfterExp(m_connected_camera->cameraID, (uint8_t*)m_image_buffer.get(), m_image_buffer_size);
            if(asi_ret == ASI_SUCCESS){
                ret = CAM_CTRL_SUCCESS;
            }
        }
        break;
        }
    case SVBONY:
        break;
    default:
        break;
    }
    return ret;
}

bool CameraController::saveBufferToFile(std::string filename){
    if(m_image_buffer != nullptr){
        std::ofstream fout;
        fout.open(filename.c_str(), std::ios::binary | std::ios::out);
        fout.write((const char *)m_image_buffer.get(), m_image_buffer_size);
        fout.close();
    }else{
        return CAM_CTRL_FAIL;
    }
    return CAM_CTRL_SUCCESS;
}

bool CameraController::startExposure(){
    bool ret = CAM_CTRL_FAIL;
    if(m_connected_camera == nullptr){
        LOG_WARNING("There is no camera connected, can't start exposure \r\n")
    }
    else{
        switch (m_connected_camera->cameraProducer)
        {
        case ASI:
            if(ASIStartExposure(m_connected_camera->cameraID, static_cast<ASI_BOOL>(false)) != ASI_SUCCESS){
                LOG_ERROR("Fail to start exposure of selected camera! \r\n")
            }
            else{
                ret = CAM_CTRL_SUCCESS;
            }
            break;
        case SVBONY:
            break;
        default:
            break;
        }
    }
    return ret;
}
bool CameraController::stopExposure(){
    bool ret = CAM_CTRL_FAIL;
    if(m_connected_camera != nullptr){
        LOG_WARNING("There is no camera connected, can't stop exposure \r\n")
    }
    else{
        switch (m_connected_camera->cameraProducer)
        {
        case ASI:
            if(ASIStopExposure(m_connected_camera->cameraID) != ASI_SUCCESS){
                LOG_ERROR("Fail to stop exposure of selected camera! \r\n")
            }
            else{
                ret = CAM_CTRL_SUCCESS;
            }
            break;
        case SVBONY:
            break;
        default:
            break;
        }
    }

    return ret;
}


int CameraController::getExposuretTime_us(){
    int ret = -1;
    switch (m_connected_camera->cameraProducer)
    {
    case ASI:
        {
        long read_exposure;
        ASI_BOOL is_auto;
        if(ASIGetControlValue(m_connected_camera->cameraID, ASI_EXPOSURE, &read_exposure, &is_auto) == ASI_SUCCESS){
            ret = read_exposure;
        }
        else{
            LOG_ERROR("Couldn't read camera exposuer \r\n");
        }
        }
        break;
    case SVBONY:
        {
        }
        break;
    default:
        break;
    }
    return ret;
}