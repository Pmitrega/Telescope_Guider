# Prepating environment

<ol>
    <li>Add to /etc/apt/sources.list new server by appending <strong>deb http://ftp.de.debian.org/debian sid main</strong> to the end of file </li>
    <li>run <strong>apt-get update</strong></li>
    <li>Install following packages</li>
    <code>

        sudo apt-get install libpaho-mqtt1.3    
        sudo apt-get install libpaho-mqtt-dev
        sudo apt-get install build-essential
        sudo apt-get install libopencv-dev
        sudo apt-get install libboost-dev
        sudo apt-get install libboost-system-dev
        sudo apt-get install libboost-thread-dev
        
</code>
    
</ol>

# Setup mosquito broker

Change mosquitto config
<code>
    sudo nano /etc/mosquitto/mosquitto.conf
</code>
to following:
<code>

    #
    # A full description of the configuration file is at
    # /usr/share/doc/mosquitto/examples/mosquitto.conf.example

    #pid_file /run/mosquitto/mosquitto.pid

    persistence true
    persistence_location /var/lib/mosquitto/

    allow_anonymous true
    listener 1883 0.0.0.0
    log_dest file /var/log/mosquitto/mosquitto.log

    include_dir /etc/mosquitto/conf.d
</code>
Now restart service:
<code>
sudo systemctl restart mosquitto    
</code>
