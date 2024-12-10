
#include <string>
#include <Worker.hpp>


void saveImage(uint8_t* buffer, uint32_t width, uint32_t heigth);

void GuiderWorker::handleMQTTTransmission(){
    int not_updated_count = 0;
    const int not_updated_timeout = 200;
    const int hearthbeat_timeout = 200;
    int hearthbeat_timer = 0;
    while(1){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if(hearthbeat_timer%hearthbeat_timeout == 0){
            hearthbeat_timer = 0;
            m_mqtt_client.publishMessageString("guider/status", "alive");
        }
        hearthbeat_timer +=1;
        if(checkForUARTUpdate() == false){
            not_updated_count +=1;
        }
        if(not_updated_count > not_updated_timeout){
            m_mqtt_client.publishMessageString("sensors/status", "NOT OK");
        }

        if(m_stepper_sensors.BattVolt.has_updated == true){
            m_stepper_sensors.BattVolt.has_updated = false;
            m_mqtt_client.publishMessageNumber("sensors/battV", m_stepper_sensors.BattVolt.value);
        }
        if(m_stepper_sensors.Buck1Volt.has_updated == true){
            m_stepper_sensors.Buck1Volt.has_updated = false;
            m_mqtt_client.publishMessageNumber("sensors/buck1V", m_stepper_sensors.Buck1Volt.value);
        }
        if(m_stepper_sensors.Buck2Volt.has_updated == true){
            m_stepper_sensors.Buck2Volt.has_updated = false;
            m_mqtt_client.publishMessageNumber("sensors/buck2V", m_stepper_sensors.Buck2Volt.value);
        }
        if(m_stepper_sensors.M1C1Current.has_updated == true){
            m_stepper_sensors.M1C1Current.has_updated = false;
            m_mqtt_client.publishMessageNumber("sensors/M1C1curr", m_stepper_sensors.M1C1Current.value);
        }
        if(m_stepper_sensors.M1C2Current.has_updated == true){
            m_stepper_sensors.M1C2Current.has_updated = false;
            m_mqtt_client.publishMessageNumber("sensors/M1C2curr", m_stepper_sensors.M1C2Current.value);
        }
        if(m_stepper_sensors.M2C1Current.has_updated == true){
            m_stepper_sensors.M2C1Current.has_updated = false;
            m_mqtt_client.publishMessageNumber("sensors/M2C1curr", m_stepper_sensors.M2C1Current.value);
        }
        if(m_stepper_sensors.M2C2Current.has_updated == true){
            m_stepper_sensors.M2C2Current.has_updated = false;
            m_mqtt_client.publishMessageNumber("sensors/M2C2curr", m_stepper_sensors.M2C2Current.value);
        }
        if(m_stepper_sensors.BattCurren.has_updated == true){
            m_stepper_sensors.BattCurren.has_updated = false;
            m_mqtt_client.publishMessageNumber("sensors/battcurr", m_stepper_sensors.BattCurren.value);
        }

        // mqtt_client.publishMessageString("images/raw/title", filename);
		// mqtt_client.publishMessageString("images/raw/capture_time", oss.str());
		// mqtt_client.publishMessageNumber("images/raw/exposure", 500);
		// mqtt_client.publishMessageNumber("images/raw/gain", 70);
		// mqtt_client.publishMessageString("images/raw/cameraType", "ASI");
		// mqtt_client.publishMessageString("images/raw/ROIwidth", "1280");
		// mqtt_client.publishMessageString("images/raw/ROIheight", "960");
		// mqtt_client.publishMessageString("images/raw/ROIstart_x", "0");
		// mqtt_client.publishMessageString("images/raw/ROIstart_y", "0");
        

        if(m_image_info.image_width.second == true){
            m_image_info.image_width.second = false;
		    m_mqtt_client.publishMessageString("images/raw/ROIwidth", m_image_info.image_width.first);
        }
        if(m_image_info.image_heigth.second == true){
            m_image_info.image_heigth.second = false;
		    m_mqtt_client.publishMessageString("images/raw/ROIheight", m_image_info.image_heigth.first);
        }
        if(m_image_info.image_xstart.second == true){
            m_image_info.image_xstart.second = false;
		    m_mqtt_client.publishMessageString("images/raw/ROIstart_x", m_image_info.image_xstart.first);
        }
        if(m_image_info.image_ystart.second == true){
            m_image_info.image_ystart.second = false;
		    m_mqtt_client.publishMessageString("images/raw/ROIstart_y", m_image_info.image_ystart.first);
        }
        if(m_image_info.image_gain.second == true){
            m_image_info.image_gain.second = false;
		    m_mqtt_client.publishMessageString("images/raw/gain", m_image_info.image_gain.first);
        }
        if(m_image_info.image_exposure.second == true){
            m_image_info.image_exposure.second = false;
		    m_mqtt_client.publishMessageString("images/raw/exposure", m_image_info.image_exposure.first);
        }
        if(m_image_info.image_title.second == true){
            m_image_info.image_title.second = false;
		    m_mqtt_client.publishMessageString("images/raw/title", m_image_info.image_title.first);
        }
        if(m_image_info.image_capture_time.second == true){
            m_image_info.image_capture_time.second = false;
		    m_mqtt_client.publishMessageString("images/raw/capture_time", m_image_info.image_capture_time.first);
        }
        if(m_image_info.image_buffer.second == true && m_image_info.image_buffer.first != nullptr && !m_send_jpg){
            LOG_INFO("transmiting %s\r\n", m_image_info.image_title.first.c_str());
            m_image_info.image_buffer.second = false;
		    m_mqtt_client.publishMessageImageRaw("images/raw", (char*)m_image_info.image_buffer.first, 1280*960*2);
        }
        if(m_send_jpg && m_jpg_ready){
            LOG_INFO("transmiting %s\r\n", m_image_info.image_title.first.c_str());
            m_jpg_ready = false;
            std::ifstream file("/tmp/compressed.jpg", std::ios::binary | std::ios::ate);
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            std::vector<char> buffer(size);
            if (file.read(buffer.data(), size))
            {
                m_mqtt_client.publishMessageImageRaw("images/jpg", buffer.data(), size);
            }
            else{
                LOG_ERROR("Couldn't read compressed.jpg\r\n");
            }
            
        }

    }
}


void GuiderWorker::handleMQTTRecieve(){
    MqttMessage mqtt_message;
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if(m_mqtt_client.checkForMessage(mqtt_message) == 0){
            LOG_INFO("Recieved message %s %s \r\n", mqtt_message.topic.c_str(), mqtt_message.payload.c_str());
            if(mqtt_message.topic == "camera/exposure"){
                if(m_camera_settings.exposure.first != mqtt_message.payload){
                    m_camera_settings.exposure.first = mqtt_message.payload;
                    m_camera_settings.exposure.second = true;
                }
            }
           if(mqtt_message.topic == "camera/gain"){
                if(m_camera_settings.gain.first != mqtt_message.payload){
                    m_camera_settings.gain.first = mqtt_message.payload;
                    m_camera_settings.gain.second = true;
                }
            }
           if(mqtt_message.topic == "camera/interval"){
                if(m_camera_settings.interval.first != mqtt_message.payload){
                    m_camera_settings.interval.first = mqtt_message.payload;
                    m_camera_settings.interval.second = true;
                }
            }
           if(mqtt_message.topic == "camera/interval"){
                if(m_camera_settings.interval.first != mqtt_message.payload){
                    m_camera_settings.interval.first = mqtt_message.payload;
                    m_camera_settings.interval.second = true;
                }
            }
           if(mqtt_message.topic == "motors/ra"){
                if(m_motor_info.ra_speed.first != mqtt_message.payload){
                    m_motor_info.ra_speed.first = mqtt_message.payload;
                    m_step_com.setRaSpeed(std::stoi(m_motor_info.ra_speed.first));
                    m_motor_info.ra_speed.second = true;
                }
            }
           if(mqtt_message.topic == "motors/dec"){
                if(m_motor_info.dec_speed.first != mqtt_message.payload){
                    m_motor_info.dec_speed.first = mqtt_message.payload;
                    m_step_com.setDecSpeed(std::stoi(m_motor_info.dec_speed.first));
                    m_motor_info.dec_speed.second = true;
                }
            }
           if(mqtt_message.topic == "motors/mode"){
                if(mqtt_message.payload == "AUTO"){
                    m_motor_info.mode.first = "AUTO";
                    m_motor_info.mode.second = true;
                    m_step_com.setAutoMode();
                }
                else if(mqtt_message.payload == "MANUAL"){
                    m_motor_info.mode.first = "MANUAL";
                    m_motor_info.mode.second = true;
                    m_step_com.setManualMode();
                }
            }
           if(mqtt_message.topic == "images/jpg/enable"){
                if(mqtt_message.payload[0] == '1')
                    m_send_jpg = true;
                else
                    m_send_jpg = false;
            }
        }
    }
}

// void GuiderWorker::run(){

// }

void GuiderWorker::handleCamera(){
    m_cam_controller.scanForCameras();

    auto camera_list = m_cam_controller.getCameraList();
	while(camera_list.size() < 1){
        LOG_INFO("Waiting for camera ... \r\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        camera_list = m_cam_controller.getCameraList();
        m_cam_controller.scanForCameras();
    }
    auto current_camera = camera_list[0];
	m_cam_controller.openCameraByProducerAndID(current_camera.cameraProducer, 0U);
    m_cam_controller.setCameraExposure_us(std::stoi(m_camera_settings.exposure.first) * 1000);
	m_cam_controller.setCameraGain(std::stoi(m_camera_settings.gain.first));
	m_cam_controller.setImageType(IMG_RAW16);
    m_image_info.image_width.first = "1280";
    m_image_info.image_heigth.first = "960";
    m_image_info.image_xstart.first = "0";
    m_image_info.image_ystart.first = "0";
    m_image_info.image_exposure.first = m_camera_settings.exposure.first;
    m_image_info.image_gain.first = m_camera_settings.gain.first;
    m_camera_settings.interval.first = m_camera_settings.interval.first;
    /*TO BE UPDATED WITH PROPER API */
	uint8_t buffer[1280*960*2];
	int  i = 0;
    m_cam_controller.startVideoCapture();
    while(true){
        int ms_betw_frames = std::stoi(m_camera_settings.interval.first);
        if(ms_betw_frames < 100){
            ms_betw_frames = 100;
        }
		auto t = std::time(nullptr);
		if(CAM_CTRL_FAIL == m_cam_controller.tryGetVideoData()){
            m_cam_controller.tryReconnectCamera(current_camera);
            m_cam_controller.setCameraExposure_us(std::stoi(m_camera_settings.exposure.first) * 1000);
	        m_cam_controller.setCameraGain(std::stoi(m_camera_settings.gain.first));
            m_cam_controller.startVideoCapture();
			continue;
		}
        handleCameraSettChangeReq();
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
        m_image_info.image_capture_time.first = oss.str();
        m_image_info.image_title.first = filename;
        m_image_info.image_buffer.first = m_cam_controller.getBuffer().get();
        requestImageTransimssion();
        auto capture_start_time =  std::chrono::system_clock::now();
        if(m_send_jpg){
        saveImage(m_image_info.image_buffer.first, 1280, 960);
        m_jpg_ready = true;
        }
		auto capture_end_time =  std::chrono::system_clock::now();
        std::chrono::duration<float> elapesed_time = capture_end_time - capture_start_time;
        int execution_time = (ms_betw_frames) - static_cast<int>(1000*elapesed_time.count());
        if(ms_betw_frames > std::stoi(m_camera_settings.exposure.first)){
            int sleep_time = (ms_betw_frames) - static_cast<int>(1000*elapesed_time.count());
            // LOG_INFO("sleep time: %d ms\r\n", sleep_time)
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        }
		i++;
	}
}


GuiderWorker::GuiderWorker(){
}

bool GuiderWorker::checkForUARTUpdate(){
    bool has_updated = false;
    if(m_step_com.readBattVolt() !=  m_stepper_sensors.BattVolt.value){
        m_stepper_sensors.BattVolt.value = m_step_com.readBattVolt();
        m_stepper_sensors.BattVolt.has_updated = true;
        has_updated = true;
    }
    if(m_step_com.readBuck1Volt() !=  m_stepper_sensors.BattVolt.value){
        m_stepper_sensors.Buck1Volt.value = m_step_com.readBuck1Volt();
        m_stepper_sensors.Buck1Volt.has_updated = true;
        has_updated = true;
    }
    if(m_step_com.readBuck2Volt() !=  m_stepper_sensors.Buck2Volt.value){
        m_stepper_sensors.Buck2Volt.value = m_step_com.readBuck2Volt();
        m_stepper_sensors.Buck2Volt.has_updated = true;
        has_updated = true;
    }
    if(m_step_com.readM1C1Current() !=  m_stepper_sensors.M1C1Current.value){
        m_stepper_sensors.M1C1Current.value = m_step_com.readM1C1Current();
        m_stepper_sensors.M1C1Current.has_updated = true;
        has_updated = true;
    }
    if(m_step_com.readM1C2Current() !=  m_stepper_sensors.M1C2Current.value){
        m_stepper_sensors.M1C2Current.value = m_step_com.readM1C2Current();
        m_stepper_sensors.M1C2Current.has_updated = true;
        has_updated = true;
    }
    if(m_step_com.readM2C1Current() !=  m_stepper_sensors.M2C1Current.value){
        m_stepper_sensors.M2C1Current.value = m_step_com.readM2C1Current();
        m_stepper_sensors.M2C1Current.has_updated = true;
        has_updated = true;
    }
    if(m_step_com.readM2C2Current() !=  m_stepper_sensors.M2C2Current.value){
        m_stepper_sensors.M2C2Current.value = m_step_com.readM2C2Current();
        m_stepper_sensors.M2C2Current.has_updated = true;
        has_updated = true;
    }
    if(m_step_com.readBattCurrent() !=  m_stepper_sensors.BattCurren.value){
        m_stepper_sensors.BattCurren.value = m_step_com.readBattCurrent();
        m_stepper_sensors.BattCurren.has_updated = true;
        has_updated = true;
    }

    
    return has_updated;
}


bool GuiderWorker::waitForUART(){
    bool uart_ok = false;
    while(!uart_ok){
        try{
            m_step_com.connectSerial("/dev/ttyUSB0");
            uart_ok = true;
        }
        catch(...)
        {
            LOG_INFO("Can't connect to /dev/ttyUSB0, waiting ...\r\n");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
    return uart_ok;
}

void GuiderWorker::handleUARTRequests(){
    waitForUART();
    stepperCommunicator::Status stat = stepperCommunicator::SERIAL_OK;
    while(true){
		stat = m_step_com.reqBattVolt();
        if(stat == stepperCommunicator::SERIAL_ERROR){
            waitForUART();
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	stat = m_step_com.reqBuck1Volt();
        if(stat == stepperCommunicator::SERIAL_ERROR){
            waitForUART();
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	stat = m_step_com.reqBuck2Volt();
        if(stat == stepperCommunicator::SERIAL_ERROR){
            waitForUART();
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	stat = m_step_com.reqM1C1Current();
        if(stat == stepperCommunicator::SERIAL_ERROR){
            waitForUART();
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	stat = m_step_com.reqM1C2Current();
        if(stat == stepperCommunicator::SERIAL_ERROR){
            waitForUART();
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	stat = m_step_com.reqM2C1Current();
        if(stat == stepperCommunicator::SERIAL_ERROR){
            waitForUART();
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	stat = m_step_com.reqM2C2Current();
        if(stat == stepperCommunicator::SERIAL_ERROR){
            waitForUART();
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	stat = m_step_com.reqBattCurrent();
        if(stat == stepperCommunicator::SERIAL_ERROR){
            waitForUART();
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}


void GuiderWorker::handleCameraSettChangeReq(){
    if(m_camera_settings.exposure.second == true){
        LOG_INFO("Updating exposure to %s \r\n", m_camera_settings.interval.first.c_str());
        m_camera_settings.exposure.second = false;
        m_cam_controller.setCameraExposure_us(std::stoi(m_camera_settings.exposure.first) * 1000);
        m_image_info.image_exposure.first = m_camera_settings.exposure.first;
        }
    if(m_camera_settings.gain.second == true){
        m_camera_settings.gain.second = false;
        LOG_INFO("Updating gain to %s \r\n", m_camera_settings.gain.first.c_str());
        m_image_info.image_gain.first = m_camera_settings.gain.first;
        m_cam_controller.setCameraGain(std::stoi(m_camera_settings.gain.first));
    }
}


void GuiderWorker::requestCameraInfoTransmision(){

}

void GuiderWorker::requestImageTransimssion(){
        m_image_info.image_capture_time.second = true;
        m_image_info.image_gain.second = true;
        m_image_info.image_exposure.second = true;
        m_image_info.image_width.second = true;
        m_image_info.image_heigth.second = true;
        m_image_info.image_xstart.second = true;
        m_image_info.image_ystart.second = true;
        m_image_info.image_title.second = true;
        m_image_info.image_buffer.second = true;
}


void saveImage(uint8_t* buffer, uint32_t width, uint32_t heigth)
{
    cv::Mat image(heigth, width, CV_16U, reinterpret_cast<uint16_t*>(buffer));
    image = image/256;
    cv::imwrite ("/tmp/compressed.jpg", image);

}
