#include <string>
#include <vector>
#include <memory>
#include <map>

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

enum CameraProducer{
    ASI = 0,
    SVBONY,
    NUMBER_OF_CAMERA_PRODUCERS
};

struct CameraInfo{
    uint16_t cameraID;
    CameraProducer cameraProducer;
    uint16_t cameraWidth;
    uint16_t cameraHeigth;
    bool isColor;
    double PixelSize; //the pixel size of the camera, unit is um. such like 5.6um
    bool controlTypeAviable[CameraControlTypes::NUMBER_OF_CONTROLL_TYPES];
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
         * @brief Close currently opened camera
         */
        bool closeCamera();
    protected:
        /**
         * Helper function scanning for each camera producer.
         */
        int scanForASI();
        int scanForSVBONY();
    private:
        std::vector<CameraInfo> m_camera_list;
        uint16_t m_selected_camera_id;
        CameraProducer m_camera_producer_api;
        std::unique_ptr<CameraInfo> m_connected_camera = nullptr;
};

std::ostream &operator<<(std::ostream &os, CameraInfo const &cam_info);