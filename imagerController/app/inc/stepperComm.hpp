
#include <string>

class stepperCommunicator{
public:
    void connectSerial(std::string serial_port);
private:
    bool m_is_connected = false;
};