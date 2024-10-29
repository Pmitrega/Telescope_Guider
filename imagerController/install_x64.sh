#!/bin/bash
sudo apt-get update 
sudo apt-get install libopencv-dev
sudo cp ./lib/ASICamera/x64/libASICamera2.so.1.36 /usr/lib
sudo ln /usr/lib/libASICamera2.so.1.36 /usr/lib/libASICamera2.so
