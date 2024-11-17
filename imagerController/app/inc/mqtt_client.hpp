#pragma once

#include "MQTTClient.h"
#include <cstdlib>
#include <string>

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "test/topic"
#define PAYLOAD     "Hello World!"
#define QOS         0
#define TIMEOUT     10000L

void testMQTT();

struct MqttMessage{
    std::string topic;
    std::string payload;
};

class MqttClientWrapper{
public:
    MqttClientWrapper();
    ~MqttClientWrapper();
    int publishMessageNumber(std::string topic, float value);
    int publishMessageString(std::string topic, std::string mess);
    int publishMessageImageRaw(std::string topic, char* image, int image_size);
    int checkForMessage(MqttMessage& mqtt_message);
private:
    MQTTClient m_client;
    int m_connecton_status = -1;
};
