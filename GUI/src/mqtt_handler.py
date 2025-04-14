import paho.mqtt.client as mqtt
import numpy as np
import threading
from ui_form import Ui_MainWindow
import cv2
import json
import math
from src.WCS import WCS, deg2dms, deg2hms
from src.telescope_controller import TelescopeController
from src.logger import Logger
from PySide6.QtGui import QPixmap


mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)


class MqttHandler:
    def __init__(self, ui: Ui_MainWindow, logger: Logger, wcs: WCS, ref_star_plot):
        self.image = np.zeros((1280, 96), dtype=np.uint16)
        self.image_ready = False
        self.wcs = wcs
        self.ref_star_plot = ref_star_plot
        self.setupClient()
        self.ui = ui
        self.logger = logger
        self.sky_rot = 0
        self.sky_dec_vect = [1,0]
        self.sky_ra_vect = [0,1]
        self.localization_dec_ra_rot = [None, None, None]
        self.star_loc_center_x = 0
        self.star_loc_center_y = 0


    def on_message(self, client, userdata, msg):
        if msg.topic == "images/raw":
            deserialized_bytes = np.frombuffer(msg.payload, dtype=np.uint16)
            if deserialized_bytes.size != 0:
                self.image = np.reshape(deserialized_bytes, newshape=(960, 1280))
                if self.ui.checkBox_save_raw.isChecked():
                    path = "./saved/" + self.ui.lineEdit_rec_title.text()+".png"
                    cv2.imwrite(path, self.image)
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
            self.localization_dec_ra_rot[1] = (loc[0] + loc[1]/60 + loc[2]/3600)*15
            if self.localization_dec_ra_rot[0] is not None and self.localization_dec_ra_rot[2] is not None and self.localization_dec_ra_rot[2] is not None:
                self.logger.LogFacingLoc(self.localization_dec_ra_rot[0], self.localization_dec_ra_rot[1], self.localization_dec_ra_rot[2], self.logger.loc_capt_time)
        elif msg.topic == "solver/dec_dms":
            loc = json.loads(msg.payload.decode("utf-8"))
            self.ui.lineEdit_sky_dec.setText(str(loc[0]) + u'\N{DEGREE SIGN} ' + str(loc[1]) + "' " + str(loc[2]) + "\"")
            self.localization_dec_ra_rot[0] = (loc[0] + loc[1]/60 + loc[2]/3600)
            if self.localization_dec_ra_rot[1] is not None and self.localization_dec_ra_rot[2] is not None and self.localization_dec_ra_rot[2] is not None:
                self.logger.LogFacingLoc(self.localization_dec_ra_rot[0], self.localization_dec_ra_rot[1], self.localization_dec_ra_rot[2], self.logger.loc_capt_time)
        elif msg.topic == "solver/rotation":
            rot = float(msg.payload.decode("utf-8")) - 90
            self.ui.lineEdit_sky_rot.setText(str(round(rot, 2)) + u'\N{DEGREE SIGN} ')
            self.sky_rot = rot
            self.sky_dec_vect = (
                math.cos(float(rot) * math.pi / 180), math.sin(float(rot) * math.pi / 180))
            self.sky_ra_vect = (
                -math.sin(float(rot) * math.pi / 180), math.cos(float(rot) * math.pi / 180))
            self.localization_dec_ra_rot[2] = self.sky_rot
            if self.localization_dec_ra_rot[0] is not None and self.localization_dec_ra_rot[1] is not None and self.localization_dec_ra_rot[2] is not None:
                ang_rad = self.localization_dec_ra_rot[2]/180*np.pi
                R = np.array([[np.cos(ang_rad), -np.sin(ang_rad)],
                              [np.sin(ang_rad), np.cos(ang_rad)]])
                diff = np.array([[960 - self.star_loc_center_x],
                                 [ 1280 - self.star_loc_center_y]])

                shift = R@diff * 3.22/3600 # scale shift to degress
                self.logger.LogFacingLoc(self.localization_dec_ra_rot[0], self.localization_dec_ra_rot[1], self.localization_dec_ra_rot[2], self.logger.loc_capt_time)
        elif msg.topic == "sensors/battV":
            self.ui.lcdNumber_batt_volt.display(float(msg.payload.decode("utf-8")))
        elif msg.topic == "sensors/buck1V":
            self.ui.lcdNumber_buck1.display(float(msg.payload.decode("utf-8")))
        elif msg.topic == "sensors/buck2V":
            self.ui.lcdNumber_buck2.display(float(msg.payload.decode("utf-8")))
        elif msg.topic == "sensors/M1C1curr":
            self.ui.lcdNumber_M1C1.display(float(msg.payload.decode("utf-8")))
        elif msg.topic == "sensors/M1C2curr":
            self.ui.lcdNumber_M1C2.display(float(msg.payload.decode("utf-8")))
        elif msg.topic == "sensors/M2C1curr":
            self.ui.lcdNumber_M2C1.display(float(msg.payload.decode("utf-8")))
        elif msg.topic == "sensors/M2C2curr":
            self.ui.lcdNumber_M2C2.display(float(msg.payload.decode("utf-8")))
        elif msg.topic == "sensors/battcurr":
            self.ui.lcdNumber_batt_curr.display(float(msg.payload.decode("utf-8")))
        elif msg.topic == "sensors/M1C1_R":
            self.ui.lcdNumber_M1C1_R.display(float(msg.payload.decode("utf-8"))/1000)
        elif msg.topic == "sensors/M1C2_R":
            self.ui.lcdNumber_M1C2_R.display(float(msg.payload.decode("utf-8"))/1000)
        elif msg.topic == "sensors/M2C1_R":
            self.ui.lcdNumber_M2C1_R.display(float(msg.payload.decode("utf-8"))/1000)
        elif msg.topic == "sensors/M2C2_R":
            self.ui.lcdNumber_M2C2_R.display(float(msg.payload.decode("utf-8"))/1000)
        elif msg.topic == "solver/wcs":
            wcs_vals = json.loads(msg.payload.decode("utf-8"))
            print( wcs_vals )
            self.wcs = WCS(wcs_vals[0], wcs_vals[1], wcs_vals[2], wcs_vals[3], wcs_vals[4], wcs_vals[5], wcs_vals[6], wcs_vals[7])
            print(self.ref_star_plot.x_cent, self.ref_star_plot.y_cent)
            ra, dec = self.wcs.pixel_to_world(self.ref_star_plot.y_cent, self.ref_star_plot.x_cent)
            self.ui.grid_mer = self.wcs.get_meridian_grid_lines(step_deg=1)
            self.ui.grid_lat = self.wcs.get_latitude_grid_lines()
            self.ui.lineEdit_ref_ra.setText(deg2hms(ra))
            self.ui.lineEdit_ref_dec.setText(deg2dms(dec))
        elif msg.topic == "solver/status":
            status = str(msg.payload.decode("utf-8"))
            if status=="ok":
                self.ui.label_solver_status.setPixmap(QPixmap(u"images/led_green.png"))
            elif status == "n_ok":
                self.ui.label_solver_status.setPixmap(QPixmap(u"images/led_red.png"))

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
        self.setImageMode(self.ui.comboBox_address_2.currentText())

    def setRaSpeed(self, speed: int):
        self.logger.logRaCtrl(speed)
        self.mqtt_client.publish("motors/ra", str(speed))

    def setDecSpeed(self, speed: int):
        self.logger.logDecCtrl(speed)
        self.mqtt_client.publish("motors/dec", str(speed))

    def setImageMode(self, mode: str):
        self.mqtt_client.publish("image_mode", mode)

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
