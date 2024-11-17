#include "mqtt_client.hpp"
#include "string.h"
#include "logger.hpp"
#include <format>
#include <fstream>
#include <thread>
#include <stdio.h>
#include <functional>
#include <iostream>
#include <queue>

static std::queue<MqttMessage> mqtt_message_queue;


void readFile(std::string filename, char* buffer, long buff_len){
        std::ifstream fin;
        fin.open(filename.c_str(), std::ios::binary | std::ios::in);
        fin.read(buffer, buff_len);
        if(!fin.is_open()){
            LOG_ERROR("Can't open file!!")
        }
        fin.close();
    }

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    // deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    std::string topic(topicName);
    char payload_buffer[50];
    sprintf(payload_buffer,"%.*s", message->payloadlen, (char*)message->payload);
    std::string payload(payload_buffer);
    LOG_INFO("Recieved message \r\n topic:%s \r\n payload: %s \r\n", topic.c_str(), payload.c_str());
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    mqtt_message_queue.push({topic, payload});
    return 1;
}


int MqttClientWrapper::checkForMessage(MqttMessage& mqtt_message){
    if(!mqtt_message_queue.empty()){
        mqtt_message = mqtt_message_queue.front();
        mqtt_message_queue.pop();
        return 0;
    }
    else{
        return 1;
    }
}
void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    if (cause)
    	printf("     cause: %s\n", cause);
}


MqttClientWrapper::MqttClientWrapper(){
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_create(&m_client, ADDRESS, CLIENTID,
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.password = "stepper";
    conn_opts.username = "stepper";
    int rc;
    if((rc= MQTTClient_setCallbacks(m_client, NULL, connlost, msgarrvd , delivered))!= MQTTCLIENT_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        MQTTClient_destroy(&m_client);
    }

    rc = MQTTClient_connect(m_client, &conn_opts);
    if( rc != MQTTCLIENT_SUCCESS){
        LOG_ERROR("Failed to connect MQTT client with error code: %d \r\n", rc);
        m_connecton_status = rc;
    }
    MQTTClient_subscribe(m_client, "camera/#", 0);
    MQTTClient_subscribe(m_client, "stepper/#", 0);
    MQTTClient_subscribe(m_client, "motors/#", 0);
}

int MqttClientWrapper::publishMessageImageRaw(std::string topic, char* image, int image_size){
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = (void*)image;
    pubmsg.payloadlen = image_size;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(m_client, topic.c_str(), &pubmsg, &token);
    int rc = 0;
    rc = MQTTClient_waitForCompletion(m_client, token, TIMEOUT);
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
    int rc = 0;
    rc = MQTTClient_waitForCompletion(m_client, token, TIMEOUT);
    if(rc != MQTTCLIENT_SUCCESS){
        LOG_ERROR("Failed to publish image with return code: %d \r\n", rc);
    }
    return rc;
}


int MqttClientWrapper::publishMessageString(std::string topic, std::string mess){
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = (void*)mess.c_str();
    pubmsg.payloadlen = mess.length();
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(m_client, topic.c_str(), &pubmsg, &token);
    int rc = 0;
    rc = MQTTClient_waitForCompletion(m_client, token, TIMEOUT);
    if(rc != MQTTCLIENT_SUCCESS){
        LOG_ERROR("Failed to publish image with return code: %d \r\n", rc);
    }
    return rc;
}
void testMQTT(){
    /*TO SUBSCRIBE FOR TOPIC*/
    /*mosquitto_sub -v -h localhost -p 1883 -t myTopic -u stepper -P stepper*/
    /*TO SUBSCRIBE FOR ALL TOPICS*/
    /*mosquitto_sub -v -h localhost -p 1883 -t '#' -u stepper -P stepper*/
    /*TO PUBLISH*/
    /*mosquitto_pub -h localhost -t test -m "hello world" -u "stepper" -P "stepper"*/
    MqttClientWrapper mqtt_client;
    mqtt_client.publishMessageNumber("sensors/battV", 3.14);
    mqtt_client.publishMessageNumber("sensors/buck1V", 4.14);
    mqtt_client.publishMessageNumber("sensors/buck2V", 5.14);
    mqtt_client.publishMessageNumber("sensors/M1C1curr", 6.14);
    mqtt_client.publishMessageNumber("sensors/M1C2curr", 7.14);
    mqtt_client.publishMessageNumber("sensors/M2C1curr", 8.14);
    mqtt_client.publishMessageNumber("sensors/M2C2curr", 9.14);
    mqtt_client.publishMessageNumber("sensors/battcurr", 10.14);
    mqtt_client.publishMessageNumber("hello", 0.0);
    for(int i =0; i< 1000; i++){
        const int width = 1280;
        const int heigth = 960;
        const int bytes_per_pixel = 2;
        const int buffer_len = width*heigth*bytes_per_pixel;
        char buffer[buffer_len];
        std::string filename;
        for(int j =0; j <=30; j++){
            if(j < 10){
                    filename = std::string("image0") + std::to_string(j) + ".raw";
            }
            else{
                    filename = std::string("image") + std::to_string(j) + ".raw";
            }
            readFile("../tests/starLocator_test/series_2000ms/" + filename, buffer, buffer_len);
            mqtt_client.publishMessageNumber("images/raw/height", heigth);
            mqtt_client.publishMessageNumber("images/raw/width", width);
            mqtt_client.publishMessageNumber("images/raw/bytes_per_pixel", bytes_per_pixel);
            mqtt_client.publishMessageString("images/raw/title", filename);
            mqtt_client.publishMessageImageRaw("images/raw", buffer, buffer_len);
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }
    }
}