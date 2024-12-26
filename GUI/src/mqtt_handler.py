import paho.mqtt.client as mqtt
import numpy as np
import threading
from ui_form import Ui_MainWindow
import cv2
import json
import math
from src.telescope_controller import TelescopeController

mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)


class MqttHandler:
    def __init__(self, ui: Ui_MainWindow):
        self.image = np.zeros((1280, 96), dtype=np.uint16)
        self.image_ready = False
        self.setupClient()
        self.ui = ui
        self.sky_rot = 0
        self.sky_dec_vect = [1,0]
        self.sky_ra_vect = [0,1]


    def on_message(self, client, userdata, msg):
        if msg.topic == "images/raw":
            deserialized_bytes = np.frombuffer(msg.payload, dtype=np.uint16)
            if deserialized_bytes.size != 0:
                self.image = np.reshape(deserialized_bytes, newshape=(960, 1280))
                self.image_ready = True
        elif msg.topic == "images/jpg":
            deserialized_bytes = np.frombuffer(msg.payload, dtype=np.uint8)
            if deserialized_bytes.size != 0:
                self.image = cv2.imdecode(deserialized_bytes, cv2.IMREAD_GRAYSCALE).astype(np.uint16) * 256
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
        elif msg.topic == "solver/ra_hms":
            loc = json.loads(msg.payload.decode("utf-8"))
            self.ui.lineEdit_sky_ra.setText(str(loc[0]) + 'h ' + str(loc[1]) + "m " + str(loc[2]) + "s")
        elif msg.topic == "solver/dec_dms":
            loc = json.loads(msg.payload.decode("utf-8"))
            self.ui.lineEdit_sky_dec.setText(str(loc[0]) + u'\N{DEGREE SIGN} ' + str(loc[1]) + "' " + str(loc[2]) + "\"")
        elif msg.topic == "solver/rotation":
            rot = float(msg.payload.decode("utf-8")) - 90
            self.ui.lineEdit_sky_rot.setText(str(round(rot, 2)) + u'\N{DEGREE SIGN} ')
            self.sky_rot = rot
            self.sky_dec_vect = (
                math.cos(float(rot) * math.pi / 180), math.sin(float(rot) * math.pi / 180))
            self.sky_ra_vect = (
                -math.sin(float(rot) * math.pi / 180), math.cos(float(rot) * math.pi / 180))
        elif msg.topic == "sensors/battV":
            self.ui.lcdNumber_batt_volt.display(float(msg.payload.decode("utf-8")))
    def setupCamera(self, exposure: int, gain: int, interval: int):
        self.mqtt_client.publish("camera/exposure", exposure)
        self.mqtt_client.publish("camera/interval", interval)
        self.mqtt_client.publish("camera/gain", gain)

    def requestCompressed(self, compressed: bool):
        if compressed:
            self.mqtt_client.publish("images/jpg/enable", "1")
        else:
            self.mqtt_client.publish("images/jpg/enable", "0")

    def on_connect(self, client, userdata, flags, reason_code, properties):
        print(f"Connected with result code {reason_code}")
        client.subscribe("#")

    def setRaSpeed(self, speed: int):
        self.mqtt_client.publish("motors/ra", str(speed))

    def setDecSpeed(self, speed: int):
        self.mqtt_client.publish("motors/dec", str(speed))

    def setupClient(self):
        self.mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.mqtt_client.on_connect = self.on_connect
        self.mqtt_client.on_message = self.on_message
        self.mqtt_client.password = "stepper"
        self.mqtt_client.username = "stepper"

    def run_client(self, address: str):
        if self.mqtt_client.is_connected():
            self.disconnect()
        self.mqtt_client.connect(address, 1883, 60)
        self.subscribe_thread = threading.Thread(target=self.mqtt_client.loop_forever)
        self.subscribe_thread.start()

    def disconnect(self):
        if self.mqtt_client is not None and self.mqtt_client.is_connected():
            self.mqtt_client.disconnect()
