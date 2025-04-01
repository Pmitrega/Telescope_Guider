import paho.mqtt.client as mqtt
import cv2
import numpy as np
import time

# MQTT Configuration
MQTT_BROKER = "127.0.0.1"  # Change this to your MQTT broker
MQTT_PORT = 1883
MQTT_TOPIC = "images/raw"



# Initialize MQTT Client
client = mqtt.Client()
client.password = "stepper"
client.username = "stepper"

# Connect to the broker
client.connect(MQTT_BROKER, MQTT_PORT, 60)
i = 0
while True:
# Publish the raw image dat
    image = cv2.imread("image" + str(i + 1579) + ".raw.png", cv2.IMREAD_GRAYSCALE)
    # Convert 8-bit grayscale to 16-bit
    image_16bit = np.uint16(image) * 256  # Scale 8-bit values to 16-bit range
    # Flatten the image into a 1D byte array
    raw_data = image_16bit.tobytes()

    client.publish(MQTT_TOPIC, raw_data)
    
    
    i +=1
    i = i%60
    time.sleep(1)
print(f"Sent {len(raw_data)} bytes of raw 16-bit grayscale image data to {MQTT_TOPIC}")

# Disconnect MQTT client
client.disconnect()