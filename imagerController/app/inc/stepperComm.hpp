#include <string>
#include <boost/asio.hpp>

class stepperCommunicator{
public:
    stepperCommunicator(): m_serial_port(boost::asio::io_service){};
    void connectSerial(const std::string serial_port);
private:
    bool m_is_connected = false;
    boost::asio::serial_port m_serial_port;
};