# This Python file uses the following encoding: utf-8
import sys
import numpy as np
import random
import pyqtgraph
from src.mqtt_handler import MqttHandler

from PySide6.QtWidgets import QApplication, QMainWindow
from PySide6.QtCore import QTimer
from PySide6.QtGui import QPixmap
# Important:
# You need to run the following command to generate the ui_form.py file
#     pyside6-uic form.ui -o ui_form.py, or
#     pyside2-uic form.ui -o ui_form.py
from ui_form import Ui_MainWindow

class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.initializePlots()
        self.timer_image_update=QTimer()
        self.timer_mqtt_status=QTimer()
        self.mqtt_handler = MqttHandler(self.ui)
        self.timer_image_update.timeout.connect(self.updateImage)
        self.timer_image_update.start(50)
        self.timer_mqtt_status.timeout.connect(self.checkMqttStatus)
        self.timer_mqtt_status.start(1000)

        self.errors_x = []
        self.errors_y = []
        self.attachImageMouseClick()

    def setDisplayedImage(self, img:np.ndarray):
        self.ui.imageWidget.setImage(img, levels=(0,65536))
        hist = np.histogram(img, 256, range=(0, 65536))
        self.ui.histogramWidget.clear()
        self.ui.histogramWidget.plot(hist[0],hist[1][0:-1])

    def addErrorX(self, err_x):
        self.errors_x.append(err_x)
        if len(self.errors_x) > 20:
            t = np.linspace(-20, 0, 20)
            self.ui.ErrorX_plot.clear()
            self.ui.ErrorX_plot.plot(t, self.errors_x[-20:], pen = None,symbol = "x")
        else:
            t = np.linspace(-len(self.errors_x) + 1, 0, len(self.errors_x))
            self.ui.ErrorX_plot.clear()
            self.ui.ErrorX_plot.plot(t, self.errors_x, pen = None,symbol = "x")

    def resetErrorX(self):
        self.errors_x = []
        self.ui.ErrorX_plot.clear()
        self.ui.ErrorX_plot.plot(self.errors_x, pen = None,symbol = "x")

    def addErrorY(self, err_y):
        self.errors_y.append(err_y)
        if len(self.errors_y) > 20:
            t = np.linspace(-20, 0, 20)
            self.ui.ErrorY_plot.clear()
            self.ui.ErrorY_plot.plot(t, self.errors_y[-20:], pen = None,symbol = "x")
        else:
            t = np.linspace(-len(self.errors_y) + 1, 0, len(self.errors_y))
            self.ui.ErrorY_plot.clear()
            self.ui.ErrorY_plot.plot(t, self.errors_y, pen = None,symbol = "x")

    def resetErrorY(self):
        self.errors_y = []
        self.ui.ErrorY_plot.clear()
        self.ui.ErrorY_plot.plot(self.errors_y, pen = None,symbol = "x")

    def updateImage(self):
        if self.mqtt_handler.image_ready:
            self.setDisplayedImage(self.mqtt_handler.image)

    def initializePlots(self):
        image_data = np.random.randn(1280,960) * 2000 + 20000
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
            x_rescale = 1280/self.ui.imageWidget.width()
            y_rescale = 960/self.ui.imageWidget.height()
            print("clicked", event.position().toPoint().x() * x_rescale," ", event.position().toPoint().y() * y_rescale)

        self.ui.imageWidget.mousePressEvent = callback

    def checkMqttStatus(self):
        if self.mqtt_handler.mqtt_client.is_connected():
            self.ui.label_broker_status.setPixmap(QPixmap(u"images/led_green.png"))
        else:
            self.ui.label_broker_status.setPixmap(QPixmap(u"images/led_red.png"))


    def connectMqtt(self):
        self.mqtt_handler.run_client(self.ui.comboBox_address.currentText())
        # def calback(event):
        #     print(event[0])
        # pyqtgraph.SignalProxy(self.ui.imageWidget., rateLimit=20, slot=callback)



if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = MainWindow()
    widget.show()
    sys.exit(app.exec())
