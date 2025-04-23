# This Python file uses the following encoding: utf-8
import sys
import numpy as np
import random
import pyqtgraph
import faulthandler
import cv2
from src.mqtt_handler import MqttHandler
import src.star_detection as star_detec
import src.WCS as WCS
from PySide6.QtWidgets import QApplication, QMainWindow, QMenu
from PySide6.QtCore import QTimer, Qt
from PySide6.QtGui import QPixmap, QAction, QImage
# Important:
# You need to run the following command to generate the ui_form.py file
#     pyside6-uic form.ui -o ui_form.py, or
#     pyside2-uic form.ui -o ui_form.py
from ui_form import Ui_MainWindow
from src.telescope_controller import TelescopeController, starCentroid

from src.preview import transformImage
from src.logger import Logger

from datetime import datetime

class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.circ_center = [0, 0]
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.ui.solver_is_ready = True
        self.logger = Logger()
        self.ui.grid_mer = [[[0, 0]]]
        self.ui.grid_lat = [[[0, 0]]]
        self.ui.grid_shift = [0, 0]
        self.wcs = WCS.WCS(0.00074019, 0.00085509, 0.0008611016, - 0.000761324044, 989.3, 664.5, 308.25474896 , 83.6631)
        self.ref_star_plot = starCentroid(0, 1280/2, 960/2)
        self.mqtt_handler = MqttHandler(self.ui, self.logger, self.wcs, self.ref_star_plot)
        self.telescope_controller = TelescopeController(self.mqtt_handler.setRaSpeed, self.mqtt_handler.setDecSpeed, self.logger, self.mqtt_handler, self.ui)
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
        self.test_it = 0

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
            test_run = False
            if test_run:
                run_auto = False
            self.telescope_controller.genNewImage(transformed, 1000, run_auto)
            self.shiftGrid()
            self.ui.grid_shift[0] = self.ui.grid_shift[0] + self.telescope_controller.delta_errx/self.telescope_controller.sec_per_pixel
            self.ui.grid_shift[1] = self.ui.grid_shift[1] + self.telescope_controller.delta_erry/self.telescope_controller.sec_per_pixel
            print(self.ui.grid_shift)
            if self.telescope_controller.reference_star_current is not None:
                err = self.telescope_controller.getErrorToRefStar(self.telescope_controller.go_to_loc)
                # print("error: ", err)
                self.addErrorX(err[0])
                self.addErrorY(err[1])
            if run_auto:
                self.addCtrlRa(self.telescope_controller.curr_ctrl[0])
                self.addCtrlDec(self.telescope_controller.curr_ctrl[1])
            elif test_run:
                if self.ui.solver_is_ready is True:
                    self.localizeField()
                if(self.test_it == 0):
                    print("Leaving deadzone ...")
                    self.telescope_controller.setDecSpeed(100)
                    self.telescope_controller.setRaSpeed(100)
                elif self.test_it == 5:
                    print("Motors stoped ...")
                    self.telescope_controller.setDecSpeed(0)
                    self.telescope_controller.setRaSpeed(0)
                elif self.test_it == 6:
                    print("Localizing, start moving Ra")
                    
                    self.telescope_controller.setRaSpeed(500)
                    self.telescope_controller.setDecSpeed(0)
                elif self.test_it == 40:
                    print("Motors stoped ...")
                    self.telescope_controller.setDecSpeed(0)
                    self.telescope_controller.setRaSpeed(0)
                elif self.test_it == 41:
                    print("Localizing, start moving Dec")
                    self.localizeField()
                    self.telescope_controller.setDecSpeed(500)
                    self.telescope_controller.setRaSpeed(0)
                elif self.test_it == 75:
                    print("Motors stoped ...")
                    self.telescope_controller.setDecSpeed(0)
                    self.telescope_controller.setRaSpeed(0)
                elif self.test_it == 76:
                    print("Localizing ...")
                    self.localizeField()
                
                self.test_it = self.test_it + 1

            elif self.ui.radioButton_auto_speed.isChecked() and not self.ui.checkBox_startGuiding.isChecked():
                self.mqtt_handler.setDecSpeed(0)
                self.mqtt_handler.setRaSpeed(0)
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

    def shiftGrid(self):
        for j in range(len(self.ui.grid_mer)):
            for i in range(len(self.ui.grid_mer[0])):
                self.ui.grid_mer[j][i][0] = self.ui.grid_mer[j][i][0] + self.telescope_controller.delta_errx/self.telescope_controller.sec_per_pixel
                self.ui.grid_mer[j][i][1] = self.ui.grid_mer[j][i][1] + self.telescope_controller.delta_erry/self.telescope_controller.sec_per_pixel
        for j in range(len(self.ui.grid_lat)):
            for i in range(len(self.ui.grid_lat[0])):
                self.ui.grid_lat[j][i][0] = self.ui.grid_lat[j][i][0] + self.telescope_controller.delta_errx/self.telescope_controller.sec_per_pixel
                self.ui.grid_lat[j][i][1] = self.ui.grid_lat[j][i][1] + self.telescope_controller.delta_erry/self.telescope_controller.sec_per_pixel

    def attachImageMouseClick(self):
        def callback(event):
            if event.button() == Qt.MouseButton.LeftButton:
                x_rescale = 1280 / self.ui.imageWidget.width()
                y_rescale = 960 / self.ui.imageWidget.height()
                # print("clicked", event.position().toPoint().x() * x_rescale, " ",
                #       event.position().toPoint().y() * y_rescale)
                x = int(event.position().toPoint().x() * x_rescale)
                y = int(event.position().toPoint().y() * y_rescale)
                self.circ_center = [y, x]
                self.telescope_controller.setSetPoint(y, x)
                self.setDisplayedImage(self.mqtt_handler.image)
            else:
                x_rescale = 1280 / self.ui.imageWidget.width()
                y_rescale = 960 / self.ui.imageWidget.height()
                # print("clicked", event.position().toPoint().x() * x_rescale, " ",
                #       event.position().toPoint().y() * y_rescale)
                x = int(event.position().toPoint().x() * x_rescale)
                y = int(event.position().toPoint().y() * y_rescale)
                show_context_menu(event, x, y)

        def show_context_menu(event, x_pos, y_pos):
            # Tworzymy menu kontekstowe
            context_menu = QMenu(self)

            # Dodajemy akcję zapisu do schowka
            save_action = QAction("Save to clipboard", self)
            save_action.triggered.connect(save_image_to_clipboard)
            context_menu.addAction(save_action)

            # Dodajemy akcję zapisu do schowka
            save_action = QAction("Copy click location", self)
            save_action.triggered.connect(lambda: get_sky_loc(x_pos, y_pos))
            context_menu.addAction(save_action)

            # Wyświetlamy menu w miejscu kliknięcia
            context_menu.exec(event.globalPosition().toPoint())  # Convert to QPoint

        def save_image_to_clipboard():
            # Zapisz obraz do schowka
            clipboard = QApplication.clipboard()
            image_data = cv2.rotate(self.mqtt_handler.image, cv2.ROTATE_180)
            image_data = cv2.flip(image_data, 1)
            height, width = image_data.shape

            qimage = QImage(image_data.data, width, height, QImage.Format_Grayscale16)
            pixmap = QPixmap.fromImage(qimage)  # Convert to QPixmap
            clipboard.setPixmap(pixmap)
        def get_sky_loc(x_pos, y_pos):
            # Zapisz obraz do schowka
            ra, dec = self.mqtt_handler.wcs.pixel_to_world(x_pos - self.ui.grid_shift[1], y_pos - self.ui.grid_shift[0])
            clipboard = QApplication.clipboard()
            clipboard.setText(WCS.deg2hms(ra) + " " + WCS.deg2dms(dec))

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
        bin_im, star_locs = star_detec.segmentation(transformed.astype(np.uint8), thr = int(self.ui.horizontalSlider_sens.value()))
        self.mqtt_handler.star_loc_center_x = 0
        self.mqtt_handler.star_loc_center_y = 0
        for st_loc in star_locs:
            self.mqtt_handler.star_loc_center_x += st_loc[0]
            self.mqtt_handler.star_loc_center_y += st_loc[1]
        self.mqtt_handler.star_loc_center_x = self.mqtt_handler.star_loc_center_x / len(star_locs)
        self.mqtt_handler.star_loc_center_y = self.mqtt_handler.star_loc_center_y / len(star_locs)
        if len(star_locs) >= 0:
            star_detec.requestStarsLocation(star_locs, self.mqtt_handler.mqtt_client, self.ui)

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
