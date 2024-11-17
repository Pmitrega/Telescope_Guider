#!/bin/bash
sudo echo "deb http://ftp.de.debian.org/debian sid main" >> /etc/apt/sources.list
sudo apt-get update -y
sudo apt-get upgrade -y
sudo apt-get install mosquitto mosquitto-clients -y
sudo systemctl start mosquitto
sudo systemctl enable mosquitto
sudo apt-get install libpaho-mqtt1.3
sudo apt-get install build-essential
sudo apt-get install libopencv-dev
sudo apt-get install libboost-dev
sudo apt-get install libboost-system-dev
sudo apt-get install libboost-thread-dev
sudo cp ./lib/ASICamera/armv8/libASICamera2.so.1.36 /usr/lib
sudo ln /usr/lib/libASICamera2.so.1.36 /usr/lib/libASICamera2.so