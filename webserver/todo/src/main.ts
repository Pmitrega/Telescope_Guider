import { bootstrapApplication } from '@angular/platform-browser';
import { appConfig } from './app/app.config';
import { AppComponent } from './app/app.component';
import mqtt from "mqtt"; // import namespace "mqtt"


bootstrapApplication(AppComponent, appConfig)
  .catch((err) => console.error(err));


  
  const url = 'ws://10.147.18.204:8080/'
  /***
   * Node.js
   * This document explains how to use MQTT over TCP with both mqtt and mqtts protocols.
   * EMQX's default port for mqtt connections is 1883, while for mqtts it is 8883.
   */
  // const url = 'mqtt://broker.emqx.io:1883'
  
  // Create an MQTT client instance
  const options = {
    // Clean session
    clean: true,
    connectTimeout: 4000,
    // Authentication
    clientId: 'emqx_test',
    username: 'emqx_test',
    password: 'emqx_test',
  }
  const client  = mqtt.connect(url, options)
  client.on('connect', function () {
    console.log('Connected')
    // Subscribe to a topic
    client.subscribe('guider/status/#', function (err) {
      if (!err) {
        // Publish a message to a topic
        client.publish('test', 'Hello mqtt')
      }
    })
  })
  
  // Receive messages
  client.on('message', function (topic, message) {
    // message is Buffer
    console.log(message.toString())
    client.end()
  })