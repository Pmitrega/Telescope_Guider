
#include <string>
#include <Worker.hpp>


void saveImage(uint8_t* buffer, uint32_t width, uint32_t heigth);

static void readFile(std::string filename, char* buffer, long buff_len) {
    // Check if the file extension is .png
    if (filename.substr(filename.find_last_of(".") + 1) == "png") {
        // Read the image using OpenCV
        cv::Mat image = cv::imread(filename, cv::IMREAD_UNCHANGED);  // Use IMREAD_UNCHANGED to read the raw data

        if (image.empty()) {
            LOG_ERROR("Failed to load image: %s\r\n", filename.c_str());
            return;
        }

        // Check if the buffer is large enough
        if (image.total() * image.elemSize() > buff_len) {
            LOG_ERROR("Buffer size is too small to hold the image data for: %s\r\n", filename.c_str());
            return;
        }

        // Copy the image data to the provided buffer
        std::memcpy(buffer, image.data, image.total() * image.elemSize());
    } else {
        // If it's not a PNG file, fallback to the original file reading method
        std::ifstream fin;
        fin.open(filename.c_str(), std::ios::binary | std::ios::in);
        if (fin.is_open()) {
            fin.read(buffer, buff_len);
            fin.close();
        } else {
            LOG_ERROR("Failed to open file: %s\r\n", filename.c_str());
        }
    }
}


// Function to read CSV file
struct ImageInfo {
    std::string name;
    int interval;
    int exposure;
    int gain;
    std::string capture_time;
};

// Function to read the CSV file and return a vector of ImageInfo structs
static std::vector<ImageInfo> readCSV(const std::string &filename) {
    std::ifstream file(filename);  // Open the file
    std::string line;
    std::vector<ImageInfo> imageInfoList;  // Vector to store ImageInfo structs

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return imageInfoList;
    }

    // Read the first line (header) and discard it
    std::getline(file, line);

    // Read the rest of the lines
    while (std::getline(file, line)) {
        std::stringstream ss(line);  // Stringstream to split the line into fields
        ImageInfo imageInfo;

        // Read the fields from the CSV line
        std::getline(ss, imageInfo.name, ',');            // Read the name
        ss >> imageInfo.interval; ss.ignore();            // Read the interval
        ss >> imageInfo.exposure; ss.ignore();            // Read the exposure
        ss >> imageInfo.gain; ss.ignore();                // Read the gain
        std::getline(ss, imageInfo.capture_time, ',');    // Read the capture time

        // Add the filled struct to the vector
        imageInfoList.push_back(imageInfo);
    }

    file.close();  // Close the file
    return imageInfoList;  // Return the vector of ImageInfo structs
}

void GuiderWorker::handleMQTTTransmission(){
    int not_updated_count = 0;
    const int not_updated_timeout = 200;
    const int hearthbeat_timeout = 200;
    int hearthbeat_timer = 0;

    long unsigned int send_motor_info_it = 0

    std::string test1_path = "/home/guider/Project/Telescope_Guider/imagerController/test_images/series_1/";
    std::string test2_path = "/home/guider/Project/Telescope_Guider/imagerController/test_images/series_2/";
    auto test1_ImagesInfo = readCSV(test1_path + "info.csv");
    auto test2_ImagesInfo = readCSV(test2_path + "info.csv");
    int test_it = 0;

    auto start_time = std::chrono::steady_clock::now(); 

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

        if(m_stepper_sensors.M1C1Res.has_updated == true){
            m_stepper_sensors.M1C1Res.has_updated = false;
            m_mqtt_client.publishMessageNumber("sensors/M1C1_R", m_stepper_sensors.M1C1Res.value);
        }
        if(m_stepper_sensors.M1C2Res.has_updated == true){
            m_stepper_sensors.M1C2Res.has_updated = false;
            m_mqtt_client.publishMessageNumber("sensors/M1C2_R", m_stepper_sensors.M1C2Res.value);
        }
        if(m_stepper_sensors.M2C1Res.has_updated == true){
            m_stepper_sensors.M2C1Res.has_updated = false;
            m_mqtt_client.publishMessageNumber("sensors/M2C1_R", m_stepper_sensors.M2C1Res.value);
        }
        if(m_stepper_sensors.M2C2Res.has_updated == true){
            LOG_INFO("Sending motor res m2c2 %f \r\n", m_stepper_sensors.M2C2Res.value)
            m_stepper_sensors.M2C2Res.has_updated = false;
            m_mqtt_client.publishMessageNumber("sensors/M2C2_R", m_stepper_sensors.M2C2Res.value);
        }

        if(m_stepper_sensors.BattCurren.has_updated == true){
            m_stepper_sensors.BattCurren.has_updated = false;
            m_mqtt_client.publishMessageNumber("sensors/battcurr", m_stepper_sensors.BattCurren.value);
        }

        if(send_motor_info_it % 10 == 0){
            double ra  =  m_motor_monitor.getPositionRa();
            double dec = m_motor_monitor.getPositionDec();
            std::ostringstream json;
            json << std::fixed << std::setprecision(6);
            json << "{"
                 << "\"ra\": " << ra << ", "
                 << "\"dec\": " << dec
                 << "}";

            m_mqtt_client.publishMessageString("motors/position", json.str());
        }
        send_motor_info_it +=1;
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
        if(m_image_info.image_gain.second == true && m_image_mode == "camera"){
            m_image_info.image_gain.second = false;
		    m_mqtt_client.publishMessageString("images/raw/gain", m_image_info.image_gain.first);
        }
        if(m_image_info.image_exposure.second == true && m_image_mode == "camera"){
            m_image_info.image_exposure.second = false;
		    m_mqtt_client.publishMessageString("images/raw/exposure", m_image_info.image_exposure.first);
        }
        if(m_image_info.image_title.second == true && m_image_mode == "camera"){
            m_image_info.image_title.second = false;
		    m_mqtt_client.publishMessageString("images/raw/title", m_image_info.image_title.first);
        }
        if(m_image_info.capture_interval.second == true && m_image_mode == "camera"){
            m_image_info.capture_interval.second = false;
		    m_mqtt_client.publishMessageString("images/raw/interval", m_image_info.capture_interval.first);
        }
        if(m_image_info.image_capture_time.second == true && m_image_mode == "camera"){
            m_image_info.image_capture_time.second = false;
		    m_mqtt_client.publishMessageString("images/raw/capture_time", m_image_info.image_capture_time.first);
        }
        if(m_image_mode == "camera"){
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
        if (m_image_mode == "Test_data_1" || m_image_mode == "Test_data_2") {
            // Calculate elapsed time since the last send
            auto current_time = std::chrono::steady_clock::now();
            std::chrono::milliseconds elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);
            std::vector<ImageInfo>* test_ImagesInfo = nullptr;
            bool is_ok = true;
            std::string test_path = "";
            if(m_image_mode == "Test_data_1"){
                test_ImagesInfo = &test1_ImagesInfo;
                test_path = test1_path;
            }
            else if(m_image_mode == "Test_data_2"){
                test_ImagesInfo = &test2_ImagesInfo;
                test_path = test2_path;
            }
            else{
                LOG_ERROR("WRONG test name\r\n")
                is_ok = false;
            }
            // If the required interval has passed, do the operation
            test_it = test_it % test_ImagesInfo->size(); // Dereference the pointer to get the size
            if (is_ok && elapsed_time.count() >= (*test_ImagesInfo)[test_it].interval) { // Dereference the pointer to access elements
                test_it += 1;
                test_it = test_it % test_ImagesInfo->size(); // Ensure it wraps around
                char buffer[1280 * 960 * 2];
                std::string filename = test_path;
                filename = filename + (*test_ImagesInfo)[test_it].name;  // Dereference pointer to access name
                readFile(filename, buffer, sizeof(buffer));
                m_mqtt_client.publishMessageImageRaw("images/raw", (char*)buffer, sizeof(buffer));
                m_mqtt_client.publishMessageString("images/raw/gain", std::to_string((*test_ImagesInfo)[test_it].gain)); // Dereference to get gain
                m_mqtt_client.publishMessageString("images/raw/exposure", std::to_string((*test_ImagesInfo)[test_it].exposure)); // Dereference to get exposure
                m_mqtt_client.publishMessageString("images/raw/title", (*test_ImagesInfo)[test_it].name); // Dereference to get name
                m_mqtt_client.publishMessageString("images/raw/interval", std::to_string((*test_ImagesInfo)[test_it].interval)); // Dereference to get interval
                m_mqtt_client.publishMessageString("images/raw/capture_time", (*test_ImagesInfo)[test_it].capture_time); // Dereference to get capture_time
                start_time = current_time;  // Update the start_time to the current time
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
           if(mqtt_message.topic == "motors/ra"){
                if(m_motor_info.ra_speed.first != mqtt_message.payload){
                    m_motor_info.ra_speed.first = mqtt_message.payload;
                    m_step_com.setRaSpeed(std::stoi(m_motor_info.ra_speed.first));
                    m_motor_monitor.updateRaSpeed((double)std::stoi(m_motor_info.ra_speed.first));
                    m_motor_info.ra_speed.second = true;
                }
            }
           if(mqtt_message.topic == "motors/dec"){
                if(m_motor_info.dec_speed.first != mqtt_message.payload){
                    m_motor_info.dec_speed.first = mqtt_message.payload;
                    m_step_com.setDecSpeed(std::stoi(m_motor_info.dec_speed.first));
                    m_motor_monitor.updateDecSpeed((double)std::stoi(m_motor_info.dec_speed.first));
                    m_motor_info.dec_speed.second = true;
                }
            }
            if(mqtt_message.topic == "motors/enable"){
                if(mqtt_message.payload[0] == '1'){
                    m_step_com.enableMotors(true);
                }
                else if(mqtt_message.payload[0] == '0'){
                    m_step_com.enableMotors(false);
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
            if(mqtt_message.topic == "image_mode"){
                if(mqtt_message.payload == "camera" || mqtt_message.payload == "Test_data_1" || mqtt_message.payload == "Test_data_2"){
                    m_image_mode = mqtt_message.payload;
                    LOG_INFO("Sending image data from: %s\r\n", mqtt_message.payload.c_str());
                }
                else
                    LOG_WARNING("Can't hande image mode, requested image mode: %s\r\n", mqtt_message.payload.c_str());
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
    m_image_info.capture_interval.first = m_camera_settings.interval.first;
    /*TO BE UPDATED WITH PROPER API */
	uint8_t buffer[1280*960*2];
    auto last_send_time = std::chrono::system_clock::now();
	int  i = 0;
    m_cam_controller.startVideoCapture();
    while(true){
        int ms_betw_frames = std::stoi(m_camera_settings.interval.first);
        if(ms_betw_frames < 100){
            ms_betw_frames = 100;
        }
		auto t = std::time(nullptr);
        auto capture_start_time =  std::chrono::system_clock::now();
		if(CAM_CTRL_FAIL == m_cam_controller.tryGetVideoData()){
            m_cam_controller.tryReconnectCamera(current_camera);
            m_cam_controller.setCameraExposure_us(std::stoi(m_camera_settings.exposure.first) * 1000);
	        m_cam_controller.setCameraGain(std::stoi(m_camera_settings.gain.first));
            m_cam_controller.startVideoCapture();
			continue;
		}
        handleCameraSettChangeReq();
		
        

		auto capture_end_time =  std::chrono::system_clock::now();
        // std::chrono::duration<float> elapesed_time = capture_end_time - capture_start_time;
        // int execution_time = (ms_betw_frames) - static_cast<int>(1000*elapesed_time.count());
        
        int wait_time = ms_betw_frames;
        std::chrono::duration<float> time_elapsed =std::chrono::system_clock::now()  - last_send_time;
        if(std::stoi(m_camera_settings.exposure.first) > 100){
            wait_time -= 10;   
        }
        LOG_INFO("elapsed %d, wait time: %d\r\n", static_cast<int>(1000*time_elapsed.count()), wait_time);
        if(static_cast<int>(1000*time_elapsed.count()) >= wait_time){
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
            if(m_send_jpg){
                saveImage(m_image_info.image_buffer.first, 1280, 960);
                m_jpg_ready = true;
            }
            last_send_time = std::chrono::system_clock::now();
            // int sleep_time = (ms_betw_frames) - static_cast<int>(1000*elapesed_time.count());
            // // LOG_INFO("sleep time: %d ms\r\n", sleep_time)
            // std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            i++;
        }
		
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

    if(m_step_com.readM1C1Res() !=  m_stepper_sensors.M1C1Res.value){
        m_stepper_sensors.M1C1Res.value = m_step_com.readM1C1Res();
        m_stepper_sensors.M1C1Res.has_updated = true;
        has_updated = true;
    }
    if(m_step_com.readM1C2Res() !=  m_stepper_sensors.M1C2Res.value){
        m_stepper_sensors.M1C2Res.value = m_step_com.readM1C2Res();
        m_stepper_sensors.M1C2Res.has_updated = true;
        has_updated = true;
    }
    if(m_step_com.readM2C1Res() !=  m_stepper_sensors.M2C1Res.value){
        m_stepper_sensors.M2C1Res.value = m_step_com.readM2C1Res();
        m_stepper_sensors.M2C1Res.has_updated = true;
        has_updated = true;
    }
    if(m_step_com.readM2C2Res() !=  m_stepper_sensors.M2C2Res.value){
        LOG_INFO("Reading M2C2Res %f \r\n", m_stepper_sensors.M2C2Res.value);
        m_stepper_sensors.M2C2Res.value = m_step_com.readM2C2Res();
        m_stepper_sensors.M2C2Res.has_updated = true;
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
    	stat = m_step_com.reqM1C1Res();
        if(stat == stepperCommunicator::SERIAL_ERROR){
            waitForUART();
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	stat = m_step_com.reqM1C2Res();
        if(stat == stepperCommunicator::SERIAL_ERROR){
            waitForUART();
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	stat = m_step_com.reqM2C1Res();
        if(stat == stepperCommunicator::SERIAL_ERROR){
            waitForUART();
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	stat = m_step_com.reqM2C2Res();
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
    if(m_camera_settings.interval.second == true){
        m_camera_settings.interval.second = false;
        LOG_INFO("Updating interval to %s \r\n", m_camera_settings.interval.first.c_str());
        m_image_info.capture_interval.first = m_camera_settings.interval.first;
    }
}

void GuiderWorker::handleMotorMonitor(){
    while(true){
        auto start = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = end - start;
        double microseconds = duration.count() * 1.0;
        m_motor_monitor.updateRaPosition(microseconds/1.0e6);
        m_motor_monitor.updateDecPosition(microseconds/1.0e6);
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
        m_image_info.capture_interval.second = true;
        m_image_info.image_buffer.second = true;
}


void saveImage(uint8_t* buffer, uint32_t width, uint32_t heigth)
{
    cv::Mat image(heigth, width, CV_16U, reinterpret_cast<uint16_t*>(buffer));
    image = image/256;
    cv::imwrite ("/tmp/compressed.jpg", image);

}
