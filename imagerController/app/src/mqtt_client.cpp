#include "mqtt_client.hpp"
#include "string.h"
#include "logger.hpp"
#include <format>
#include <fstream>
#include <thread>

void readFile(std::string filename, char* buffer, long buff_len){
        std::ifstream fin;
        fin.open(filename.c_str(), std::ios::binary | std::ios::in);
        fin.read(buffer, buff_len);
        fin.close();
    }

MqttClientWrapper::MqttClientWrapper(){
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_create(&m_client, ADDRESS, CLIENTID,
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.password = "stepper";
    conn_opts.username = "stepper";
    int rc = MQTTClient_connect(m_client, &conn_opts);
    if( rc != MQTTCLIENT_SUCCESS){
        LOG_ERROR("Failed to connect MQTT client with error code: %d \r\n", rc);
        m_connecton_status = rc;
    }

}

int MqttClientWrapper::publishMessageImageRaw(std::string topic, char* image, int image_size){
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = (void*)image;
    pubmsg.payloadlen = image_size;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(m_client, topic.c_str(), &pubmsg, &token);
    int rc = MQTTClient_waitForCompletion(m_client, token, TIMEOUT);
    if(rc != MQTTCLIENT_SUCCESS){
        LOG_ERROR("Failed to publish image with return code: %d \r\n", rc);
    }
    return rc;
}

MqttClientWrapper::~MqttClientWrapper(){
    MQTTClient_disconnect(m_client, 10000);
    MQTTClient_destroy(&m_client);
}
int MqttClientWrapper::publishMessageNumber(std::string topic, float value){
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    std::string payload = std::format("{:.6f}", value).c_str();
    pubmsg.payload = (void*)payload.c_str();
    pubmsg.payloadlen = payload.length();
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(m_client, topic.c_str(), &pubmsg, &token);
    int rc = MQTTClient_waitForCompletion(m_client, token, TIMEOUT);
    if(rc != MQTTCLIENT_SUCCESS){
        LOG_ERROR("Failed to publish message with return code: %d \r\n", rc);
    }
    return rc;
}

void testMQTT(){
    /*TO SUBSCRIBE FOR TOPIC*/
    /*mosquitto_sub -v -h localhost -p 1883 -t myTopic -u stepper -P stepper*/
    /*TO SUBSCRIBE FOR ALL TOPICS*/
    /*mosquitto_sub -v -h localhost -p 1883 -t '#' -u stepper -P stepper*/
    MqttClientWrapper mqtt_client;
    // mqtt_client.publishMessageNumber("sensors/battV", 3.14);
    // mqtt_client.publishMessageNumber("sensors/buck1V", 4.14);
    // mqtt_client.publishMessageNumber("sensors/buck2V", 5.14);
    // mqtt_client.publishMessageNumber("sensors/M1C1curr", 6.14);
    // mqtt_client.publishMessageNumber("sensors/M1C2curr", 7.14);
    // mqtt_client.publishMessageNumber("sensors/M2C1curr", 8.14);
    // mqtt_client.publishMessageNumber("sensors/M2C2curr", 9.14);
    // mqtt_client.publishMessageNumber("sensors/battcurr", 10.14);
    for(int i =0; i< 1000; i++){
        const int width = 1280;
        const int heigth = 960;
        const int bytes_per_pixel = 2;
        const int buffer_len = width*heigth*bytes_per_pixel;
        char buffer[buffer_len];
        readFile("test_im1.raw", buffer, buffer_len);
        mqtt_client.publishMessageImageRaw("images/raw", buffer, buffer_len);
        std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    }
}