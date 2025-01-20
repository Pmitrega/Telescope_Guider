#include "stepperComm.hpp"
#include <utility>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <logger.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <chrono>
#include <string>

#define IS_LETTER(symbol) ((symbol >='a' && symbol <='z') || (symbol >='A' && symbol <='Z')?true:false)
#define IS_NUMBER(symbol) ((symbol >='0' && symbol <='9') ? true:false)

static unsigned char rec_buffer[1];
std::string rec_buffer_str = "Hello it's  empty recieve buffer!!!!!!!!!!!!!!!!!!!!!!!";
std::string acc_message = "";



float stepperCommunicator::readBattVolt(){return m_batt_volt;};
float stepperCommunicator::readBuck1Volt(){return m_buck1_volt;};
float stepperCommunicator::readBuck2Volt(){return m_buck2_volt;};
float stepperCommunicator::readM1C1Current(){return m_M1C1_curr;};
float stepperCommunicator::readM1C2Current(){return m_M1C2_curr;};
float stepperCommunicator::readM2C1Current(){return m_M2C1_curr;};
float stepperCommunicator::readM2C2Current(){return m_M2C2_curr;};
float stepperCommunicator::readBattCurrent(){return m_batt_curr;};

float stepperCommunicator::readM1C1Res(){return m_M1C1_R;};
float stepperCommunicator::readM1C2Res(){return m_M1C2_R;};
float stepperCommunicator::readM2C1Res(){return m_M2C1_R;};
float stepperCommunicator::readM2C2Res(){return m_M2C2_R;};

void dupaHandler(const boost::system::error_code& error, std::size_t bytes_transferred);
void stepperCommunicator::connectSerial(const std::string serial_port){
    m_serial_port.open(serial_port);
    m_serial_port.set_option(boost::asio::serial_port_base::baud_rate(115200));
    m_serial_port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

    m_serial_port.async_read_some(boost::asio::buffer(rec_buffer_str),
	                              boost::bind(&stepperCommunicator::asyncMessageHandler, 
		                                      this,
		                                      boost::asio::placeholders::error, 
                                              boost::asio::placeholders::bytes_transferred
                                                ));

    boost::thread t(boost::bind(&boost::asio::io_service::run, &m_io_service));
    if(m_serial_port.is_open() == true){
        LOG_INFO("Opened port %s\r\n", serial_port.c_str());
    }
    else{
        LOG_ERROR("Failed to open port %s\r\n", serial_port.c_str());
    }
}


stepperCommunicator::Status stepperCommunicator::setManualMode(){
    if(m_serial_port.is_open() == true){
        LOG_INFO("Setting manual\r\n");
        com_transmission_mutex.lock();
        m_serial_port.write_some(boost::asio::buffer("-M\r\n"));
        com_transmission_mutex.unlock();
        return SERIAL_OK;
    }
    else{
        LOG_ERROR("Port is not opened, can't write\r\n");
        return SERIAL_ERROR;
    }
}


stepperCommunicator::Status stepperCommunicator::setAutoMode(){
    if(m_serial_port.is_open() == true){
        LOG_INFO("Setting auto\r\n");
        com_transmission_mutex.lock();
        m_serial_port.write_some(boost::asio::buffer("-A0\r\n"));
        com_transmission_mutex.unlock();
        return SERIAL_OK;
    }
    else{
        LOG_ERROR("Port is not opened, can't write\r\n");
        return SERIAL_ERROR;
    }
}

stepperCommunicator::Status stepperCommunicator::setAutoStepsMode(){
    if(m_serial_port.is_open() == true){
        LOG_INFO("Setting auto stepps\r\n");
        com_transmission_mutex.lock();
        m_serial_port.write_some(boost::asio::buffer("-A1\r\n"));
        com_transmission_mutex.unlock();
        return SERIAL_OK;
    }
    else{
        LOG_ERROR("Port is not opened, can't write\r\n");
        return SERIAL_ERROR;
    }
}

stepperCommunicator::Status stepperCommunicator::setRaSpeed(int msecPerSec){
    if(m_serial_port.is_open() == true){
        LOG_INFO("Setting Ra\r\n");
        char buff[100];
        sprintf(buff, "-R%d\r\n", msecPerSec);
        std::string message = std::string(buff);
        std::cout <<message << std::endl;
        com_transmission_mutex.lock();
        m_serial_port.write_some(boost::asio::buffer(message));
        com_transmission_mutex.unlock();
        return SERIAL_OK;
    }
    else{
        LOG_ERROR("Port is not opened, can't write\r\n");
        return SERIAL_ERROR;
    }
}
stepperCommunicator::Status stepperCommunicator::setDecSpeed(int msecPerSec){
    if(m_serial_port.is_open() == true){
        char buff[100];
        sprintf(buff, "-D%d\r\n", msecPerSec);
        std::string message = std::string(buff);
        std::cout <<message << std::endl;
        com_transmission_mutex.lock();
        m_serial_port.write_some(boost::asio::buffer(message));
        com_transmission_mutex.unlock();
        return SERIAL_OK;
    }
    else{
        LOG_ERROR("Port is not opened, can't write\r\n");
        return SERIAL_ERROR;
    }
}

void stepperCommunicator::asyncMessageHandler(const boost::system::error_code& error, std::size_t bytes_transferred){

    messageParser(rec_buffer_str);
    m_serial_port.async_read_some(boost::asio::buffer(rec_buffer_str),
	                              boost::bind(  &stepperCommunicator::asyncMessageHandler, 
		                                        this,
		                                        boost::asio::placeholders::error, 
                                                boost::asio::placeholders::bytes_transferred
                                                ));
}


stepperCommunicator::Status stepperCommunicator::reqBattVolt(){
    auto ret = SERIAL_OK;
    com_transmission_mutex.lock();
    try{
        m_serial_port.write_some(boost::asio::buffer("-V0\r\n"));
    }
    catch(...){
        ret = SERIAL_ERROR;
    }
    com_transmission_mutex.unlock();
    return ret;
}

stepperCommunicator::Status stepperCommunicator::reqBuck1Volt(){
    auto ret = SERIAL_OK;
    com_transmission_mutex.lock();
    try{
        m_serial_port.write_some(boost::asio::buffer("-V1\r\n"));
    }
    catch(...){
        ret = SERIAL_ERROR;
    }
    com_transmission_mutex.unlock();
    return ret;
}
stepperCommunicator::Status stepperCommunicator::reqBuck2Volt(){
    auto ret = SERIAL_OK;
    com_transmission_mutex.lock();
    try{
        m_serial_port.write_some(boost::asio::buffer("-V2\r\n"));
    }
    catch(...){
        ret = SERIAL_ERROR;
    }
    com_transmission_mutex.unlock();
    return ret;
}
stepperCommunicator::Status stepperCommunicator::reqM1C1Current(){
    auto ret = SERIAL_OK;
    com_transmission_mutex.lock();
    try{
        m_serial_port.write_some(boost::asio::buffer("-C0\r\n"));
    }
    catch(...){
        ret = SERIAL_ERROR;
    }
    com_transmission_mutex.unlock();
    return ret;
}
stepperCommunicator::Status stepperCommunicator::reqM1C2Current(){
    auto ret = SERIAL_OK;
    com_transmission_mutex.lock();
    try{
        m_serial_port.write_some(boost::asio::buffer("-C1\r\n"));
    }
    catch(...){
        ret = SERIAL_ERROR;
    }
    com_transmission_mutex.unlock();
    return ret;
}
stepperCommunicator::Status stepperCommunicator::reqM2C1Current(){
    auto ret = SERIAL_OK;
    com_transmission_mutex.lock();
    try{
        m_serial_port.write_some(boost::asio::buffer("-C2\r\n"));
    }
    catch(...){
        ret = SERIAL_ERROR;
    }
    com_transmission_mutex.unlock();
    return ret;
}
stepperCommunicator::Status stepperCommunicator::reqM2C2Current(){
    auto ret = SERIAL_OK;
    com_transmission_mutex.lock();
    try{
        m_serial_port.write_some(boost::asio::buffer("-C3\r\n"));
    }
    catch(...){
        ret = SERIAL_ERROR;
    }
    com_transmission_mutex.unlock();
    return ret;
}
stepperCommunicator::Status stepperCommunicator::reqBattCurrent(){
    auto ret = SERIAL_OK;
    com_transmission_mutex.lock();
    try{
        m_serial_port.write_some(boost::asio::buffer("-C4\r\n"));
    }
    catch(...){
        ret = SERIAL_ERROR;
    }
    com_transmission_mutex.unlock();
    return ret;
}

stepperCommunicator::Status stepperCommunicator::reqM1C1Res(){
    auto ret = SERIAL_OK;
    com_transmission_mutex.lock();
    try{
        m_serial_port.write_some(boost::asio::buffer("-Z0\r\n"));
    }
    catch(...){
        ret = SERIAL_ERROR;
    }
    com_transmission_mutex.unlock();
    return ret;
}

stepperCommunicator::Status stepperCommunicator::reqM1C2Res(){
    auto ret = SERIAL_OK;
    com_transmission_mutex.lock();
    try{
        m_serial_port.write_some(boost::asio::buffer("-Z1\r\n"));
    }
    catch(...){
        ret = SERIAL_ERROR;
    }
    com_transmission_mutex.unlock();
    return ret;
}

stepperCommunicator::Status stepperCommunicator::reqM2C1Res(){
    auto ret = SERIAL_OK;
    com_transmission_mutex.lock();
    try{
        m_serial_port.write_some(boost::asio::buffer("-Z2\r\n"));
    }
    catch(...){
        ret = SERIAL_ERROR;
    }
    com_transmission_mutex.unlock();
    return ret;
}

stepperCommunicator::Status stepperCommunicator::reqM2C2Res(){
    auto ret = SERIAL_OK;
    com_transmission_mutex.lock();
    try{
        m_serial_port.write_some(boost::asio::buffer("-Z3\r\n"));
    }
    catch(...){
        ret = SERIAL_ERROR;
    }
    com_transmission_mutex.unlock();
    return ret;
}

void stepperCommunicator::enableMotors(bool enabled){
    if(enabled == true){
        m_serial_port.write_some(boost::asio::buffer("-S1\r\n"));
    }
    else{
        m_serial_port.write_some(boost::asio::buffer("-S0\r\n"));
    }
}

void stepperCommunicator::messageParser(std::string message){
    std::string pre_parsed = "";
    for(int i = 0; i < rec_buffer_str.size(); i++){
        if( i >= 1){
            if((rec_buffer_str[i] == '\r' || rec_buffer_str[i] == '\n') && IS_NUMBER(rec_buffer_str[i-1]) && IS_LETTER(rec_buffer_str[0]) && !IS_LETTER(rec_buffer_str[1])){
                pre_parsed = rec_buffer_str.substr(0, i);
                break;
            }
        }
    }
    if(pre_parsed.size() != 0 && pre_parsed.size() > 3){
        if(pre_parsed.substr(0, 2) == "B0"){
            std::string number = "";
            bool is_fine = true;
            for(int i = 3; i < pre_parsed.size(); i++){
                if(!IS_NUMBER(pre_parsed[i])){
                    is_fine = false;
                    break;
                }
            }
            if(is_fine){
                m_batt_volt = (float)std::stoi(pre_parsed.substr(3, pre_parsed.size()));
                // std::cout << "recieved batt v: " << m_batt_volt<<std::endl;
            }
        }
        else if(pre_parsed.substr(0, 2) == "B1"){
            std::string number = "";
            bool is_fine = true;
            for(int i = 3; i < pre_parsed.size(); i++){
                if(!IS_NUMBER(pre_parsed[i])){
                    is_fine = false;
                    break;
                }
            }
            if(is_fine){
                m_buck1_volt = (float)std::stoi(pre_parsed.substr(3, pre_parsed.size()));
                // std::cout << "recieved buck1 v: " << m_batt_volt<<std::endl;
            }
        }
        else if(pre_parsed.substr(0, 2) == "B2"){
            std::string number = "";
            bool is_fine = true;
            for(int i = 3; i < pre_parsed.size(); i++){
                if(!IS_NUMBER(pre_parsed[i])){
                    is_fine = false;
                    break;
                }
            }
            if(is_fine){
                m_buck2_volt = (float)std::stoi(pre_parsed.substr(3, pre_parsed.size()));
                // std::cout << "recieved buck2 v: " << m_batt_volt<<std::endl;
            }
        }
        else if(pre_parsed.substr(0, 2) == "C0"){
            std::string number = "";
            bool is_fine = true;
            for(int i = 3; i < pre_parsed.size(); i++){
                if(!IS_NUMBER(pre_parsed[i])){
                    is_fine = false;
                    break;
                }
            }
            if(is_fine){
                m_M1C1_curr = (float)std::stoi(pre_parsed.substr(3, pre_parsed.size()));
                // std::cout << "recieved M1C1 curr: " << m_M1C1_curr<<std::endl;
            }
        }
        else if(pre_parsed.substr(0, 2) == "C1"){
            std::string number = "";
            bool is_fine = true;
            for(int i = 3; i < pre_parsed.size(); i++){
                if(!IS_NUMBER(pre_parsed[i])){
                    is_fine = false;
                    break;
                }
            }
            if(is_fine){
                m_M1C2_curr = (float)std::stoi(pre_parsed.substr(3, pre_parsed.size()));
                // std::cout << "recieved M1C2 curr: " << m_M1C2_curr<<std::endl;
            }
        }
        else if(pre_parsed.substr(0, 2) == "C2"){
            std::string number = "";
            bool is_fine = true;
            for(int i = 3; i < pre_parsed.size(); i++){
                if(!IS_NUMBER(pre_parsed[i])){
                    is_fine = false;
                    break;
                }
            }
            if(is_fine){
                m_M2C1_curr = (float)std::stoi(pre_parsed.substr(3, pre_parsed.size()));
                // std::cout << "recieved M2C1 curr: " << m_M2C1_curr<<std::endl;
            }
        }
        else if(pre_parsed.substr(0, 2) == "C3"){
            std::string number = "";
            bool is_fine = true;
            for(int i = 3; i < pre_parsed.size(); i++){
                if(!IS_NUMBER(pre_parsed[i])){
                    is_fine = false;
                    break;
                }
            }
            if(is_fine){
                m_M2C2_curr = (float)std::stoi(pre_parsed.substr(3, pre_parsed.size()));
                // std::cout << "recieved M2C2 curr: " << m_M2C2_curr<<std::endl;
            }
        }
        else if(pre_parsed.substr(0, 2) == "C4"){
            std::string number = "";
            bool is_fine = true;
            for(int i = 3; i < pre_parsed.size(); i++){
                if(!IS_NUMBER(pre_parsed[i])){
                    is_fine = false;
                    break;
                }
            }
            if(is_fine){
                m_batt_curr = (float)std::stoi(pre_parsed.substr(3, pre_parsed.size()));
                // std::cout << "recieved batt curr v: " << m_batt_curr<<std::endl;
            }
        }
        else if(pre_parsed.substr(0, 2) == "R0"){
            std::string number = "";
            bool is_fine = true;
            for(int i = 3; i < pre_parsed.size(); i++){
                if(!IS_NUMBER(pre_parsed[i])){
                    is_fine = false;
                    break;
                }
            }
            if(is_fine){
                m_M1C1_R = (float)std::stoi(pre_parsed.substr(3, pre_parsed.size()));
                // std::cout << "recieved batt curr v: " << m_batt_curr<<std::endl;
            }
        }
        else if(pre_parsed.substr(0, 2) == "R1"){
            std::string number = "";
            bool is_fine = true;
            for(int i = 3; i < pre_parsed.size(); i++){
                if(!IS_NUMBER(pre_parsed[i])){
                    is_fine = false;
                    break;
                }
            }
            if(is_fine){
                m_M1C2_R = (float)std::stoi(pre_parsed.substr(3, pre_parsed.size()));
                // std::cout << "recieved batt curr v: " << m_batt_curr<<std::endl;
            }
        }
        else if(pre_parsed.substr(0, 2) == "R2"){
            std::string number = "";
            bool is_fine = true;
            for(int i = 3; i < pre_parsed.size(); i++){
                if(!IS_NUMBER(pre_parsed[i])){
                    is_fine = false;
                    break;
                }
            }
            if(is_fine){
                m_M2C1_R = (float)std::stoi(pre_parsed.substr(3, pre_parsed.size()));
                // std::cout << "recieved batt curr v: " << m_batt_curr<<std::endl;
            }
        }
        else if(pre_parsed.substr(0, 2) == "R3"){
            
            std::string number = "";
            bool is_fine = true;
            for(int i = 3; i < pre_parsed.size(); i++){
                if(!IS_NUMBER(pre_parsed[i])){
                    is_fine = false;
                    break;
                }
            }
            if(is_fine){
                m_M2C2_R = (float)std::stoi(pre_parsed.substr(3, pre_parsed.size()));
                LOG_WARNING("got R3 %f!\r\n", m_M2C2_R);
                // std::cout << "recieved batt curr v: " << m_batt_curr<<std::endl;
            }
        }
        else{
            LOG_WARNING("Can't parse message \r\n");
        }
    }
}


