# This Python file uses the following encoding: utf-8
import sys
import numpy as np
import random
import pyqtgraph
import faulthandler
import cv2
from src.mqtt_handler import MqttHandler
import src.star_detection as star_detec

from PySide6.QtWidgets import QApplication, QMainWindow
from PySide6.QtCore import QTimer
from PySide6.QtGui import QPixmap
# Important:
# You need to run the following command to generate the ui_form.py file
#     pyside6-uic form.ui -o ui_form.py, or
#     pyside2-uic form.ui -o ui_form.py
from ui_form import Ui_MainWindow
from src.telescope_controller import TelescopeController

from src.preview import transformImage
from src.logger import Logger

from datetime import datetime

class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.circ_center = [0, 0]
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.logger = Logger()
        self.mqtt_handler = MqttHandler(self.ui, self.logger)
        self.telescope_controller = TelescopeController(self.mqtt_handler.setRaSpeed, self.mqtt_handler.setDecSpeed, self.logger)
        self.errors_x = []
        self.errors_y = []
        self.ctrl_ra = []
        self.ctrl_dec = []
        self.attachImageMouseClick()
        self.initializePlots()
        self.timer_image_update = QTimer()
        self.timer_mqtt_status = QTimer()
        self.timer_image_update.timeout.connect(self.updateImage)
        self.timer_image_update.start(50)
        self.timer_mqtt_status.timeout.connect(self.checkMqttStatus)
        self.timer_mqtt_status.start(1000)
        self.control_mode = "MANUAL_SPEED"
        self.ui.spinBox_setExposure.setRange(1, 10000)
        self.ui.pushButton_3.clicked.connect(self.keepCurrentLoc)
        self.localize_request_time = None

    def setDisplayedImage(self, img: np.ndarray):
        transformed = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)
        transformed = transformImage(self.ui, transformed, self.telescope_controller)
        cv2.circle(transformed, self.circ_center, 20, color=(255, 255, 0))
        transformed = cv2.resize(transformed, (self.ui.imageWidget.height(), self.ui.imageWidget.width()))

        self.ui.imageWidget.setImage(transformed, levels=(0, 256))
        hist = np.histogram(img, 256, range=(0, 65536))
        self.ui.histogramWidget.clear()
        self.ui.histogramWidget.plot(hist[0], hist[1][0:-1])

    def addErrorX(self, err_x):
        self.errors_x.append(err_x)
        if len(self.errors_x) > 20:
            t = np.linspace(-20, 0, 20)
            self.ui.ErrorX_plot.clear()
            self.ui.ErrorX_plot.plot(t, self.errors_x[-20:], pen=None, symbol="x")
        else:
            t = np.linspace(-len(self.errors_x) + 1, 0, len(self.errors_x))
            self.ui.ErrorX_plot.clear()
            self.ui.ErrorX_plot.plot(t, self.errors_x, pen=None, symbol="x")

    def resetErrorX(self):
        self.errors_x = []
        self.ui.ErrorX_plot.clear()
        self.ui.ErrorX_plot.plot(self.errors_x, pen=None, symbol="x")

    def addErrorY(self, err_y):
        self.errors_y.append(err_y)
        if len(self.errors_y) > 20:
            t = np.linspace(-20, 0, 20)
            self.ui.ErrorY_plot.clear()
            self.ui.ErrorY_plot.plot(t, self.errors_y[-20:], pen=None, symbol="x")
        else:
            t = np.linspace(-len(self.errors_y) + 1, 0, len(self.errors_y))
            self.ui.ErrorY_plot.clear()
            self.ui.ErrorY_plot.plot(t, self.errors_y, pen=None, symbol="x")

    def addCtrlRa(self, ctrl_ra):
        self.ctrl_ra.append(ctrl_ra)
        if len(self.ctrl_ra) > 20:
            t = np.linspace(-20, 0, 20)
            self.ui.ControlRa_plot_2.clear()
            self.ui.ControlRa_plot_2.plot(t, self.ctrl_ra[-20:], pen=None, symbol="x")
        else:
            t = np.linspace(-len(self.ctrl_ra) + 1, 0, len(self.ctrl_ra))
            self.ui.ControlRa_plot_2.clear()
            self.ui.ControlRa_plot_2.plot(t, self.ctrl_ra, pen=None, symbol="x")

    def addCtrlDec(self, ctrl_dec):
        self.ctrl_dec.append(ctrl_dec)
        if len(self.ctrl_dec) > 20:
            t = np.linspace(-20, 0, 20)
            self.ui.ControlDec_plot_2.clear()
            self.ui.ControlDec_plot_2.plot(t, self.ctrl_dec[-20:], pen=None, symbol="x")
        else:
            t = np.linspace(-len(self.ctrl_dec) + 1, 0, len(self.ctrl_dec))
            self.ui.ControlDec_plot_2.clear()
            self.ui.ControlDec_plot_2.plot(t, self.ctrl_dec, pen=None, symbol="x")

    def resetErrorY(self):
        self.errors_y = []
        self.ui.ErrorY_plot.clear()
        self.ui.ErrorY_plot.plot(self.errors_y, pen=None, symbol="x")

    def updateImage(self):
        if self.mqtt_handler.image_ready:
            self.telescope_controller.sky_dec_vect = self.mqtt_handler.sky_dec_vect
            self.telescope_controller.sky_ra_vect = self.mqtt_handler.sky_ra_vect
            self.mqtt_handler.image_ready = False
            transformed = cv2.rotate(self.mqtt_handler.image, cv2.ROTATE_90_CLOCKWISE)
            run_auto = self.ui.radioButton_auto_speed.isChecked() and self.ui.checkBox_startGuiding.isChecked()
            self.telescope_controller.genNewImage(transformed, 1000, run_auto)
            if self.telescope_controller.reference_star_current is not None:
                err = self.telescope_controller.getErrorToRefStar(self.telescope_controller.go_to_loc)
                print("error: ", err)
                self.addErrorX(err[0])
                self.addErrorY(err[1])
            if run_auto:
                self.addCtrlRa(self.telescope_controller.curr_ctrl[0])
                self.addCtrlDec(self.telescope_controller.curr_ctrl[1])
            elif self.ui.radioButton_auto_speed.isChecked() and not self.ui.checkBox_startGuiding.isChecked():
                self.mqtt_handler.setDecSpeed(0)
                self.mqtt_handler.setDecSpeed(0)
                self.addCtrlRa(0)
                self.addCtrlDec(0)
            self.setDisplayedImage(self.mqtt_handler.image)

    def initializePlots(self):
        image_data = np.random.randn(1280, 960) * 2000 + 20000
        self.setDisplayedImage(image_data.astype(np.uint16))

    def closeEvent(self, event):
        self.mqtt_handler.disconnect()

    def requestCompressed(self, compressed: bool):
        self.mqtt_handler.requestCompressed(compressed)

    def setupCamera(self):
        gain = self.ui.spinBox_setGain.value()
        inter = self.ui.spinBox_setInterval.value()
        expo = self.ui.spinBox_setExposure.value()
        self.mqtt_handler.setupCamera(expo, gain, inter)

    def attachImageMouseClick(self):
        def callback(event):
            x_rescale = 1280 / self.ui.imageWidget.width()
            y_rescale = 960 / self.ui.imageWidget.height()
            # print("clicked", event.position().toPoint().x() * x_rescale, " ",
            #       event.position().toPoint().y() * y_rescale)
            x = int(event.position().toPoint().x() * x_rescale)
            y = int(event.position().toPoint().y() * y_rescale)
            self.circ_center = [y, x]
            self.telescope_controller.setSetPoint(y, x)
            self.setDisplayedImage(self.mqtt_handler.image)
        self.ui.imageWidget.mousePressEvent = callback

    def checkMqttStatus(self):
        if self.mqtt_handler.mqtt_client.is_connected():
            self.ui.label_broker_status.setPixmap(QPixmap(u"images/led_green.png"))
        else:
            self.ui.label_broker_status.setPixmap(QPixmap(u"images/led_red.png"))

    def connectMqtt(self):
        self.mqtt_handler.run_client(self.ui.comboBox_address.currentText())

    def localizeField(self):
        localize_capture_time = datetime.now()
        self.logger.loc_capt_time = localize_capture_time
        self.mqtt_handler.localization_dec_ra_rot = [None, None, None]
        transformed = cv2.rotate(self.mqtt_handler.image, cv2.ROTATE_90_CLOCKWISE)
        transformed = transformed / 2 ** 8
        bin_im, star_locs = star_detec.segmentation(transformed.astype(np.uint8))
        self.mqtt_handler.star_loc_center_x = 0
        self.mqtt_handler.star_loc_center_y = 0
        for st_loc in star_locs:
            self.mqtt_handler.star_loc_center_x += st_loc[0]
            self.mqtt_handler.star_loc_center_y += st_loc[1]
        self.mqtt_handler.star_loc_center_x = self.mqtt_handler.star_loc_center_x / len(star_locs)
        self.mqtt_handler.star_loc_center_y = self.mqtt_handler.star_loc_center_y / len(star_locs)
        if len(star_locs) >= 0:
            star_detec.requestStarsLocation(star_locs, self.mqtt_handler.mqtt_client)

    def enterManualMode(self):
        self.control_mode = "MANUAL_SPEED"
        self.mqtt_handler.mqtt_client.publish("motors/mode", "AUTO")

    def enterAutoMode(self):
        self.control_mode = "AUTO_SPEED"
        self.mqtt_handler.mqtt_client.publish("motors/mode", "AUTO")

    def setRaSpeed(self, value):
        if self.ui.radioButton_manual_speed.isChecked():
            self.mqtt_handler.setRaSpeed(value)

    def setDecSpeed(self, value):
        if self.ui.radioButton_manual_speed.isChecked():
            self.mqtt_handler.setDecSpeed(value)

    def runIdentification(self):
        self.telescope_controller.run_ident = True
        self.telescope_controller.run_dec_ident = True
        self.telescope_controller.run_dec_ident_iter = 0
        self.telescope_controller.run_ra_ident_iter = 0
        self.telescope_controller.run_ra_ident = True
    def requestImageTransmissionMode(self, value):
        if self.mqtt_handler.mqtt_client.is_connected():
            self.mqtt_handler.setImageMode(str(value))
    def keepCurrentLoc(self):
        self.telescope_controller.go_to_loc = self.telescope_controller.reference_star_current




if __name__ == "__main__":
    faulthandler.enable()  # start @ the beginning
    app = QApplication(sys.argv)
    widget = MainWindow()
    widget.show()
    sys.exit(app.exec())
