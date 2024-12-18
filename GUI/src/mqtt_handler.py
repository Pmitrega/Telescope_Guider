import paho.mqtt.client as mqtt
import numpy as np
import threading
from ui_form import Ui_MainWindow
import cv2

mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)






class MqttHandler:
    def __init__(self, ui:Ui_MainWindow):
        self.image = np.zeros((1280,96), dtype=np.uint16)
        self.image_ready = False
        self.setupClient()
        self.ui = ui


    def on_message(self,client, userdata, msg):
        if msg.topic == "images/raw":
            deserialized_bytes = np.frombuffer(msg.payload, dtype=np.uint16)
            if deserialized_bytes.size != 0:
                self.image = np.reshape(deserialized_bytes, newshape=(960, 1280))
                self.image_ready = True
        elif msg.topic == "images/jpg":
            deserialized_bytes = np.frombuffer(msg.payload, dtype=np.uint8)
            if deserialized_bytes.size != 0:
                self.image = cv2.imdecode(deserialized_bytes,  cv2.IMREAD_GRAYSCALE).astype(np.uint16)*256
                self.image_ready = True
        elif msg.topic == "images/raw/title":
            self.ui.lineEdit_rec_title.setText(msg.payload.decode("utf-8"))
        elif msg.topic == "images/raw/capture_time":
            self.ui.lineEdit_rec_capt_time.setText(msg.payload.decode("utf-8"))
        elif msg.topic == "images/raw/exposure":
            self.ui.lineEdit_rec_exp.setText(msg.payload.decode("utf-8"))
        elif msg.topic == "images/raw/gain":
            self.ui.lineEdit_rec_gain.setText(msg.payload.decode("utf-8"))
        elif msg.topic == "images/raw/interval":
            self.ui.lineEdit_rec_interval.setText(msg.payload.decode("utf-8"))

    def setupCamera(self, exposure:int, gain:int, interval:int):
        self.mqtt_client.publish("camera/exposure", exposure)
        self.mqtt_client.publish("camera/interval", interval)
        self.mqtt_client.publish("camera/gain", gain)

    def requestCompressed(self,compressed:bool):
        if compressed:
            self.mqtt_client.publish("images/jpg/enable", "1")
        else :
            self.mqtt_client.publish("images/jpg/enable", "0")

    def on_connect(self, client, userdata, flags, reason_code, properties):
        print(f"Connected with result code {reason_code}")
        client.subscribe("#")


    def setupClient(self):
        self.mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.mqtt_client.on_connect = self.on_connect
        self.mqtt_client.on_message = self.on_message
        self.mqtt_client.password = "stepper"
        self.mqtt_client.username = "stepper"

    def run_client(self, address:str):
        if self.mqtt_client.is_connected():
            self.disconnect()
        self.mqtt_client.connect(address, 1883, 60)
        self.subscribe_thread = threading.Thread(target=self.mqtt_client.loop_forever)
        self.subscribe_thread.start()

    def disconnect(self):
        if self.mqtt_client is not None and self.mqtt_client.is_connected():
            self.mqtt_client.disconnect()
