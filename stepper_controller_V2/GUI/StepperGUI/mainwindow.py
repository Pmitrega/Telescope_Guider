# This Python file uses the following encoding: utf-8
import sys
import os
import serial as ser
import asyncio
import time
import json
import cv2
import paho.mqtt.client as mqtt
import numpy as np
import threading
from PySide6.QtWidgets import QApplication, QMainWindow
from PySide6.QtCore import QTimer
from PySide6.QtCharts import QChart
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)

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
        self.conn_handler = None
        self.ws_handler = None
        self.rx_timer = QTimer()
        self.iter = 0
        self.rx_timer.timeout.connect(self.txTimerTimout)
        self.update_image_timer = QTimer()
        self.update_image_timer.timeout.connect(self.updateImageTimeout)
        self.update_image_timer.start(100)
        self.data_rec_thread = None
        self.title = "UNKNOWN"
        self.last_update_time = 0
        self.new_image = True

    def connectTarget(self):
        if self.ui.checkBox_ws.isChecked():
            self.connectWS()
        elif self.ui.checkBox_serial.isChecked():
            self.connectSerial()
    def connectMQTT(self):
        address = self.ui.ws_address.toPlainText()
        def on_connect(client, userdata, flags, reason_code, properties):
            print(f"Connected with result code {reason_code}")
            # Subscribing in on_connect() means that if we lose the connection and
            # reconnect then subscriptions will be renewed.
            client.subscribe("#")

        # The callback for when a PUBLISH message is received from the server.
        def on_message(client, userdata, msg):
            print(msg.topic)
            if len(msg.topic) > 8  and msg.topic[0:8] == "sensors/":
                print(msg.topic+" "+str(msg.payload))
            elif msg.topic == "images/raw":
                deserialized_bytes = np.frombuffer(msg.payload, dtype=np.uint16)
                deserialized_bytes =  np.reshape(deserialized_bytes, newshape=(960, 1280))
                cv2.imwrite('output.png', deserialized_bytes)
                self.new_image = True
            elif msg.topic == "images/raw/title":
                self.title = str(msg.payload)

        mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        mqttc.on_connect = on_connect
        mqttc.on_message = on_message
        mqttc.password = "stepper"
        mqttc.username = "stepper"
        mqttc.connect("localhost", 1883, 60)
        # Blocking call that processes network traffic, dispatches callbacks and
        # handles reconnecting.
        # Other loop*() functions are available that give a threaded interface and a
        # manual interface.
        # mqttc.loop_forever()

        self.data_rec_thread = threading.Thread(target=mqttc.loop_forever)
        self.data_rec_thread.start()



    def connectSerial(self):
        print("connecting Serial!")
        if self.conn_handler is None:
            COM_PORT = self.ui.textEdit.toPlainText()
            try:
                self.conn_handler = ser.Serial(port=COM_PORT,baudrate=115200)
                self.conn_handler.timeout = 0.03
            except:
                print("Can't open " + COM_PORT)
        elif not self.conn_handler.is_open:
            print("opening")
            self.conn_handler.open()
        elif self.conn_handler.is_open:
            print("closing")
            self.conn_handler.close()
        if self.conn_handler is not None and self.conn_handler.is_open:
            self.ui.radioButton.setChecked(True)
            self.rx_timer.start(100)
        else:
            self.ui.radioButton.setChecked(False)
            self.rx_timer.stop()

    def connectWS(self):
        print("connecting MQTT!")
        self.connectMQTT()

    def WSRequestBatteryVoltage(self):
        if self.ws_handler is None:
            return
        else:
            m_send = False
            match self.iter%2:
                case 0:
                    request = {"request_type": "batt_volt"}
                    self.ws_handler.send(json.dumps(request))
                    m_send = True
                case 1:
                    request = {"request_type": "coils_curr"}
                    self.ws_handler.send(json.dumps(request))
                    m_send = True
            self.iter = self.iter + 1
            if m_send:
                message = self.ws_handler.recv()
                data_received = json.loads(message)
                if data_received["response_type"] == "batt_volt":
                        self.ui.lcdNumber_7.display(float(data_received["content"]))
                elif data_received["response_type"] == "coils_curr":
                    self.ui.lcdNumber.display(data_received["content"][0]/1000)
                    self.ui.lcdNumber_2.display(data_received["content"][1]/1000)
                    self.ui.lcdNumber_3.display(data_received["content"][2]/1000)
                    self.ui.lcdNumber_4.display(data_received["content"][3]/1000)


    def txTimerTimout(self):
        match self.iter%10:
            case 0:
                self.conn_handler.write(b'-V0\r\n')
            case 1:
                self.conn_handler.write(b'-C0\r\n')
            case 2:
                self.conn_handler.write(b'-C1\r\n')
            case 3:
                self.conn_handler.write(b'-C2\r\n')
            case 4:
                self.conn_handler.write(b'-C3\r\n')
            case 5:
                self.conn_handler.write(b'-C4\r\n')
            case 6:
                self.conn_handler.write(b'-P\r\n')

        rec_line = self.conn_handler.read_until().decode("utf-8")
        arg = ""
        if len(rec_line) > 3:
            arg = rec_line[0:2]
            print(rec_line)
        match arg:
            case "B0":
                batt_volt = int(rec_line[2:-1])
                self.ui.lcdNumber_7.display(batt_volt/1000)
            case "C0":
                curr_c0 = int(rec_line[2:-1])
                self.ui.lcdNumber.display(curr_c0 / 1000)
            case "C1":
                curr_c1 = int(rec_line[2:-1])
                self.ui.lcdNumber_2.display(curr_c1 / 1000)
            case "C2":
                curr_c2 = int(rec_line[2:-1])
                self.ui.lcdNumber_3.display(curr_c2 / 1000)
            case "C3":
                curr_c3 = int(rec_line[2:-1])
                self.ui.lcdNumber_4.display(curr_c3 / 1000)
            case "BC":
                curr_c4 = int(rec_line[2:-1])
                self.ui.lcdNumber_8.display(curr_c4 / 1000)
            case "PP":
                perc = int(rec_line[2:-1])
                self.ui.progressBar.setValue(perc)

        self.iter = self.iter + 1

    def updateImageTimeout(self):
        if self.new_image:
            self.ui.label_10.setPixmap(QPixmap(u"output.png"))
            self.ui.textBrowser.setText(str(self.title))
            self.new_image = False

    def goManualCoils(self):
        self.conn_handler.write(b'-M\r\n')
    def setCoil0(self, val):
        cmd = "-u" + str(val) + "\r\n"
        print(cmd)
        self.conn_handler.write(cmd.encode())
    def setCoil1(self, val):
        cmd = "-i" + str(val) + "\r\n"
        self.conn_handler.write(cmd.encode())

    def setCoil2(self, val):
        cmd = "-o" + str(val) + "\r\n"
        self.conn_handler.write(cmd.encode())

    def setCoil3(self, val):
        cmd = "-p" + str(val) + "\r\n"
        self.conn_handler.write(cmd.encode())

    def setDecSpeed(self, val):
        if(val >10 or val < -10):
            cmd = "-D" + str(val) + "\r\n"
            self.conn_handler.write(cmd.encode())
    def setRaSpeed(self, val):
        if (val > 10 or val < -10):
            cmd = "-R" + str(val) + "\r\n"
            self.conn_handler.write(cmd.encode())

    def goManualSpeed(self):
        self.conn_handler.write(b'-A\r\n')




if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = MainWindow()
    widget.show()
    sys.exit(app.exec())




