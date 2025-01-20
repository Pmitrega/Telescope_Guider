#pragma once

#include <string>
#include <boost/asio.hpp>
#include <mutex>

class stepperCommunicator{
public:
    enum Status{
        SERIAL_OK,
        SERIAL_ERROR
    };
    stepperCommunicator(): m_serial_port(m_io_service){};
    void connectSerial(const std::string serial_port);
    Status setManualMode();
    Status setAutoMode();
    Status setAutoStepsMode();
    Status makeRaSteps(int steps);
    Status makeDecSteps(int steps);
    Status setRaSpeed(int msecPerSec);
    Status setDecSpeed(int msecPerSec);
    Status reqBattVolt();
    Status reqBuck1Volt();
    Status reqBuck2Volt();
    Status reqM1C1Current();
    Status reqM1C2Current();
    Status reqM2C1Current();
    Status reqM2C2Current();
    Status reqBattCurrent();
    Status reqM1C1Res();
    Status reqM1C2Res();
    Status reqM2C1Res();
    Status reqM2C2Res();
    float readBattVolt();
    float readBuck1Volt();
    float readBuck2Volt();
    float readM1C1Current();
    float readM1C2Current();
    float readM2C1Current();
    float readM2C2Current();
    float readBattCurrent();
    float readM1C1Res();
    float readM1C2Res();
    float readM2C1Res();
    float readM2C2Res();
    void messageParser(std::string message);
    void asyncMessageHandler(const boost::system::error_code& error, std::size_t bytes_transferred);
    void enableMotors(bool enabled);
private:
    float m_batt_volt = 0;
    float m_buck1_volt = 0;
    float m_buck2_volt = 0;
    float m_M1C1_curr = 0;
    float m_M1C2_curr = 0;
    float m_M2C1_curr = 0;
    float m_M2C2_curr = 0;
    float m_batt_curr = 0;
    float m_M1C1_R = 0;
    float m_M1C2_R = 0;
    float m_M2C1_R = 0;
    float m_M2C2_R = 0;
    bool m_is_connected = false;
    std::mutex com_transmission_mutex;
    boost::asio::io_service m_io_service;
    boost::asio::serial_port m_serial_port;
};