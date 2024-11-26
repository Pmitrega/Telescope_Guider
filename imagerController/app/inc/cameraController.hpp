#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <atomic>

#include "ASICamera2.h"


#define CAM_CTRL_SUCCESS false
#define CAM_CTRL_FAIL true

enum CameraControlTypes{
    EXPOSURE = 0,
    GAIN,
    TEMPERATURE,
	GAMMA,
    WB_R,
	WB_B,
	OFFSET,
	FLIP,
    NUMBER_OF_CONTROLL_TYPES
};

typedef enum IMG_TYPE{
	IMG_RAW8 = 0,
	IMG_RGB24,
	IMG_RAW16,
	IMG_Y8,
	IMG_END = -1
}IMG_TYPE;

typedef enum CameraProducer{
    ASI = 0,
    SVBONY,
    NUMBER_OF_CAMERA_PRODUCERS
}CameraProducer;

struct CameraInfo{
    uint16_t cameraID;
    CameraProducer cameraProducer;
    uint16_t cameraWidth;
    uint16_t cameraHeigth;
    bool isColor;
    double PixelSize; //the pixel size of the camera, unit is um. such like 5.6um
    bool controlTypeAviable[CameraControlTypes::NUMBER_OF_CONTROLL_TYPES];
    bool operator == (const CameraInfo &Ref) const 
    {
        return  (this->cameraProducer == Ref.cameraProducer) && (this->cameraWidth == Ref.cameraWidth) &&
                (this->cameraHeigth == Ref.cameraHeigth) && (this->PixelSize == Ref.PixelSize) && (this->isColor == Ref.isColor);
    }
};

struct CameraROIInfo{
    uint16_t startX;
    uint16_t startY;
    uint16_t width;
    uint16_t height;
};

class CameraController{
    public:
        ~CameraController();

        /**
        * @brief Updated internal list of connected cameras,
        * @return number of found cameras
        */
        int scanForCameras();
        /**
         *
         */
        bool openCameraByProducerAndID(CameraProducer producer, uint16_t camID);
        const std::vector<CameraInfo>& getCameraList(){return m_camera_list;};
        /**
         * @brief Set exposure time in microseconds.
         * @return it returns exposure that was set (in case of too high value, max value is set)
         *         when setting exposure failed it returns -1
         */
        int setCameraExposure_us(int exposure);
        /**
         * @brief Set camera gain
         * @return it returns gain that was set (in case of too high value, max value is set)
         *         when setting gain failed it returns -1
         */
        int setCameraGain(int gain);
        /**
         * @brief set image tape
         */
        bool setImageType(IMG_TYPE img_type);
        /**
         * @brief set image tape
         */
        std::shared_ptr<uint8_t[]> getBuffer(){
            return m_image_buffer;
        }

        /**
         * @brief Close currently opened camera
         */
        bool closeCamera();

        /**
         * @brief Takes and image
         */
        void runContionousCapture(uint16_t ms_betw_frames);
        /**
         * @brief Takes and image
         */
        bool takeAnImage();
        /**
         * @brief saves data to specified file
         */
        bool saveBufferToFile(std::string filename);
        /**
         * @brief Starts exposure
         */
        bool startExposure();
        /**
         * @brief Stops exposure
         */
        bool stopExposure();

        /**
         * @brief Stops exposure
         */
        int getGain();
        /**
         * @brief Stops exposure
         */
        int getExposure_us();
        /**
         * @brief Waits for camera to be present
         */
        void tryReconnectCamera(const CameraInfo& cam_info);
        /**
         * @brief starts Video capture, need to wait for scan for exposure end
         * to recieve data
         */ 
        bool startVideoCapture();
        /**
         * @brief stops Video capture
         */ 
        bool stopVideoCapture();
        /**
         * @brief try getting video data;
         */ 
        bool tryGetVideoData();
    protected:
        /**
         * Helper function scanning for each camera producer.
         */
        int scanForASI();
        int scanForSVBONY();
        bool prepareImageBuffer();
        IMG_TYPE getImageTypeASI();
        std::pair<uint16_t, uint16_t> getImageSizeASI();
        IMG_TYPE getImageTypeSVBONY();
    private:
        std::atomic<bool> m_run_countinious_capture = false;
        std::shared_ptr<uint8_t[]> m_image_buffer;
        long m_image_buffer_size;
        std::vector<CameraInfo> m_camera_list;
        CameraROIInfo m_roi_info;
        uint16_t m_selected_camera_id;
        IMG_TYPE m_img_type = IMG_END;
        CameraProducer m_camera_producer_api;
        std::unique_ptr<CameraInfo> m_connected_camera = nullptr;
};

std::ostream &operator<<(std::ostream &os, CameraInfo const &cam_info);