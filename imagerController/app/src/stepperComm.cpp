#include "stepperComm.hpp"
#include <utility>
#include <boost/asio.hpp>
#include <logger.hpp>

void stepperCommunicator::connectSerial(const std::string serial_port){
    m_serial_port.open(serial_port);
    m_serial_port.set_option(boost::asio::serial_port_base::baud_rate(115200));
    if(m_serial_port.is_open() == true){
        LOG_INFO("Opened port %s\r\n", serial_port.c_str());
    }
    else{
        LOG_ERROR("Failed to pened port %s\r\n", serial_port.c_str());
    }
}