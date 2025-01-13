# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'form.ui'
##
## Created by: Qt User Interface Compiler version 6.8.1
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QCheckBox, QComboBox, QHBoxLayout,
    QLCDNumber, QLabel, QLayout, QLineEdit,
    QMainWindow, QMenuBar, QPushButton, QRadioButton,
    QSizePolicy, QSlider, QSpinBox, QStatusBar,
    QTabWidget, QVBoxLayout, QWidget)

from pyqtgraph import (PlotWidget, RawImageWidget)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(1368, 776)
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Maximum, QSizePolicy.Policy.Maximum)
        sizePolicy.setHorizontalStretch(60)
        sizePolicy.setVerticalStretch(60)
        sizePolicy.setHeightForWidth(MainWindow.sizePolicy().hasHeightForWidth())
        MainWindow.setSizePolicy(sizePolicy)
        MainWindow.setMouseTracking(False)
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.verticalLayoutWidget = QWidget(self.centralwidget)
        self.verticalLayoutWidget.setObjectName(u"verticalLayoutWidget")
        self.verticalLayoutWidget.setGeometry(QRect(0, 0, 211, 230))
        self.verticalLayout = QVBoxLayout(self.verticalLayoutWidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(5, 5, 5, 5)
        self.verticalLayout_21 = QVBoxLayout()
        self.verticalLayout_21.setObjectName(u"verticalLayout_21")
        self.verticalLayout_20 = QVBoxLayout()
        self.verticalLayout_20.setObjectName(u"verticalLayout_20")
        self.verticalLayout_19 = QVBoxLayout()
        self.verticalLayout_19.setObjectName(u"verticalLayout_19")
        self.comboBox_address = QComboBox(self.verticalLayoutWidget)
        self.comboBox_address.addItem("")
        self.comboBox_address.addItem("")
        self.comboBox_address.setObjectName(u"comboBox_address")
        self.comboBox_address.setEditable(True)

        self.verticalLayout_19.addWidget(self.comboBox_address)


        self.verticalLayout_20.addLayout(self.verticalLayout_19)

        self.pushButton = QPushButton(self.verticalLayoutWidget)
        self.pushButton.setObjectName(u"pushButton")

        self.verticalLayout_20.addWidget(self.pushButton)

        self.horizontalLayout_17 = QHBoxLayout()
        self.horizontalLayout_17.setObjectName(u"horizontalLayout_17")
        self.label_20 = QLabel(self.verticalLayoutWidget)
        self.label_20.setObjectName(u"label_20")
        self.label_20.setAlignment(Qt.AlignmentFlag.AlignRight|Qt.AlignmentFlag.AlignTrailing|Qt.AlignmentFlag.AlignVCenter)

        self.horizontalLayout_17.addWidget(self.label_20)

        self.label_19 = QLabel(self.verticalLayoutWidget)
        self.label_19.setObjectName(u"label_19")
        self.label_19.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_17.addWidget(self.label_19)

        self.label_21 = QLabel(self.verticalLayoutWidget)
        self.label_21.setObjectName(u"label_21")
        self.label_21.setAlignment(Qt.AlignmentFlag.AlignLeading|Qt.AlignmentFlag.AlignLeft|Qt.AlignmentFlag.AlignVCenter)

        self.horizontalLayout_17.addWidget(self.label_21)


        self.verticalLayout_20.addLayout(self.horizontalLayout_17)

        self.horizontalLayout_18 = QHBoxLayout()
        self.horizontalLayout_18.setObjectName(u"horizontalLayout_18")
        self.label_broker_status = QLabel(self.verticalLayoutWidget)
        self.label_broker_status.setObjectName(u"label_broker_status")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.label_broker_status.sizePolicy().hasHeightForWidth())
        self.label_broker_status.setSizePolicy(sizePolicy1)
        self.label_broker_status.setMaximumSize(QSize(16, 16))
        self.label_broker_status.setPixmap(QPixmap(u"../stepper_controller_V2/GUI/StepperGUI/graphics/led_red.png"))
        self.label_broker_status.setScaledContents(True)
        self.label_broker_status.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_18.addWidget(self.label_broker_status)

        self.label_guider_status = QLabel(self.verticalLayoutWidget)
        self.label_guider_status.setObjectName(u"label_guider_status")
        sizePolicy1.setHeightForWidth(self.label_guider_status.sizePolicy().hasHeightForWidth())
        self.label_guider_status.setSizePolicy(sizePolicy1)
        self.label_guider_status.setMaximumSize(QSize(16, 16))
        self.label_guider_status.setPixmap(QPixmap(u"../stepper_controller_V2/GUI/StepperGUI/graphics/led_red.png"))
        self.label_guider_status.setScaledContents(True)
        self.label_guider_status.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_18.addWidget(self.label_guider_status)

        self.label_stepper_status = QLabel(self.verticalLayoutWidget)
        self.label_stepper_status.setObjectName(u"label_stepper_status")
        sizePolicy1.setHeightForWidth(self.label_stepper_status.sizePolicy().hasHeightForWidth())
        self.label_stepper_status.setSizePolicy(sizePolicy1)
        self.label_stepper_status.setMaximumSize(QSize(16, 16))
        self.label_stepper_status.setPixmap(QPixmap(u"../stepper_controller_V2/GUI/StepperGUI/graphics/led_red.png"))
        self.label_stepper_status.setScaledContents(True)
        self.label_stepper_status.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_18.addWidget(self.label_stepper_status)


        self.verticalLayout_20.addLayout(self.horizontalLayout_18)


        self.verticalLayout_21.addLayout(self.verticalLayout_20)

        self.radioButton_manual_speed = QRadioButton(self.verticalLayoutWidget)
        self.radioButton_manual_speed.setObjectName(u"radioButton_manual_speed")
        self.radioButton_manual_speed.setChecked(True)

        self.verticalLayout_21.addWidget(self.radioButton_manual_speed)

        self.radioButton_auto_speed = QRadioButton(self.verticalLayoutWidget)
        self.radioButton_auto_speed.setObjectName(u"radioButton_auto_speed")

        self.verticalLayout_21.addWidget(self.radioButton_auto_speed)

        self.radioButton_manual_steps = QRadioButton(self.verticalLayoutWidget)
        self.radioButton_manual_steps.setObjectName(u"radioButton_manual_steps")

        self.verticalLayout_21.addWidget(self.radioButton_manual_steps)

        self.radioButton_manual_coil = QRadioButton(self.verticalLayoutWidget)
        self.radioButton_manual_coil.setObjectName(u"radioButton_manual_coil")

        self.verticalLayout_21.addWidget(self.radioButton_manual_coil)


        self.verticalLayout.addLayout(self.verticalLayout_21)

        self.verticalLayoutWidget_2 = QWidget(self.centralwidget)
        self.verticalLayoutWidget_2.setObjectName(u"verticalLayoutWidget_2")
        self.verticalLayoutWidget_2.setGeometry(QRect(220, 0, 911, 151))
        self.verticalLayout_2 = QVBoxLayout(self.verticalLayoutWidget_2)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.verticalLayout_2.setContentsMargins(0, 5, 0, 5)
        self.tabWidget = QTabWidget(self.verticalLayoutWidget_2)
        self.tabWidget.setObjectName(u"tabWidget")
        self.tab = QWidget()
        self.tab.setObjectName(u"tab")
        self.verticalLayoutWidget_4 = QWidget(self.tab)
        self.verticalLayoutWidget_4.setObjectName(u"verticalLayoutWidget_4")
        self.verticalLayoutWidget_4.setGeometry(QRect(10, 0, 891, 112))
        self.verticalLayout_6 = QVBoxLayout(self.verticalLayoutWidget_4)
        self.verticalLayout_6.setObjectName(u"verticalLayout_6")
        self.verticalLayout_6.setContentsMargins(0, 3, 0, 3)
        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.horizontalLayout_3.setSizeConstraint(QLayout.SizeConstraint.SetDefaultConstraint)
        self.horizontalLayout_3.setContentsMargins(20, 5, 20, 5)
        self.label = QLabel(self.verticalLayoutWidget_4)
        self.label.setObjectName(u"label")
        font = QFont()
        font.setFamilies([u"Palatino Linotype"])
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setUnderline(False)
        font.setStrikeOut(False)
        font.setKerning(True)
        self.label.setFont(font)
        self.label.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_3.addWidget(self.label)

        self.spinBox_RaSpeed = QSpinBox(self.verticalLayoutWidget_4)
        self.spinBox_RaSpeed.setObjectName(u"spinBox_RaSpeed")
        sizePolicy2 = QSizePolicy(QSizePolicy.Policy.Maximum, QSizePolicy.Policy.Maximum)
        sizePolicy2.setHorizontalStretch(50)
        sizePolicy2.setVerticalStretch(50)
        sizePolicy2.setHeightForWidth(self.spinBox_RaSpeed.sizePolicy().hasHeightForWidth())
        self.spinBox_RaSpeed.setSizePolicy(sizePolicy2)
        self.spinBox_RaSpeed.setMinimumSize(QSize(50, 40))
        self.spinBox_RaSpeed.setMinimum(-4000)
        self.spinBox_RaSpeed.setMaximum(4000)

        self.horizontalLayout_3.addWidget(self.spinBox_RaSpeed)

        self.horizontalSlider_RaSpeed = QSlider(self.verticalLayoutWidget_4)
        self.horizontalSlider_RaSpeed.setObjectName(u"horizontalSlider_RaSpeed")
        self.horizontalSlider_RaSpeed.setMinimum(-4000)
        self.horizontalSlider_RaSpeed.setMaximum(4000)
        self.horizontalSlider_RaSpeed.setSingleStep(10)
        self.horizontalSlider_RaSpeed.setOrientation(Qt.Orientation.Horizontal)

        self.horizontalLayout_3.addWidget(self.horizontalSlider_RaSpeed)


        self.verticalLayout_6.addLayout(self.horizontalLayout_3)

        self.horizontalLayout_5 = QHBoxLayout()
        self.horizontalLayout_5.setObjectName(u"horizontalLayout_5")
        self.horizontalLayout_5.setSizeConstraint(QLayout.SizeConstraint.SetDefaultConstraint)
        self.horizontalLayout_5.setContentsMargins(20, 5, 20, 5)
        self.label_3 = QLabel(self.verticalLayoutWidget_4)
        self.label_3.setObjectName(u"label_3")
        self.label_3.setFont(font)
        self.label_3.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_5.addWidget(self.label_3)

        self.spinBox_DecSpeed = QSpinBox(self.verticalLayoutWidget_4)
        self.spinBox_DecSpeed.setObjectName(u"spinBox_DecSpeed")
        sizePolicy2.setHeightForWidth(self.spinBox_DecSpeed.sizePolicy().hasHeightForWidth())
        self.spinBox_DecSpeed.setSizePolicy(sizePolicy2)
        self.spinBox_DecSpeed.setMinimumSize(QSize(50, 40))
        self.spinBox_DecSpeed.setMinimum(-4000)
        self.spinBox_DecSpeed.setMaximum(4000)

        self.horizontalLayout_5.addWidget(self.spinBox_DecSpeed)

        self.horizontalSlider_DecSpeed = QSlider(self.verticalLayoutWidget_4)
        self.horizontalSlider_DecSpeed.setObjectName(u"horizontalSlider_DecSpeed")
        self.horizontalSlider_DecSpeed.setMinimum(-4000)
        self.horizontalSlider_DecSpeed.setMaximum(4000)
        self.horizontalSlider_DecSpeed.setSingleStep(10)
        self.horizontalSlider_DecSpeed.setOrientation(Qt.Orientation.Horizontal)

        self.horizontalLayout_5.addWidget(self.horizontalSlider_DecSpeed)


        self.verticalLayout_6.addLayout(self.horizontalLayout_5)

        self.tabWidget.addTab(self.tab, "")
        self.tab_2 = QWidget()
        self.tab_2.setObjectName(u"tab_2")
        self.horizontalLayoutWidget_2 = QWidget(self.tab_2)
        self.horizontalLayoutWidget_2.setObjectName(u"horizontalLayoutWidget_2")
        self.horizontalLayoutWidget_2.setGeometry(QRect(9, 9, 881, 101))
        self.horizontalLayout_4 = QHBoxLayout(self.horizontalLayoutWidget_2)
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")
        self.horizontalLayout_4.setContentsMargins(5, 5, 5, 5)
        self.horizontalLayout_8 = QHBoxLayout()
        self.horizontalLayout_8.setObjectName(u"horizontalLayout_8")
        self.horizontalLayout_8.setContentsMargins(3, -1, 3, -1)
        self.verticalLayout_7 = QVBoxLayout()
        self.verticalLayout_7.setSpacing(3)
        self.verticalLayout_7.setObjectName(u"verticalLayout_7")
        self.verticalLayout_7.setContentsMargins(-1, 3, -1, 3)
        self.horizontalLayout_7 = QHBoxLayout()
        self.horizontalLayout_7.setObjectName(u"horizontalLayout_7")
        self.label_4 = QLabel(self.horizontalLayoutWidget_2)
        self.label_4.setObjectName(u"label_4")
        self.label_4.setSizeIncrement(QSize(0, 0))

        self.horizontalLayout_7.addWidget(self.label_4)

        self.spinBox_5 = QSpinBox(self.horizontalLayoutWidget_2)
        self.spinBox_5.setObjectName(u"spinBox_5")

        self.horizontalLayout_7.addWidget(self.spinBox_5)


        self.verticalLayout_7.addLayout(self.horizontalLayout_7)

        self.horizontalLayout_6 = QHBoxLayout()
        self.horizontalLayout_6.setObjectName(u"horizontalLayout_6")
        self.label_2 = QLabel(self.horizontalLayoutWidget_2)
        self.label_2.setObjectName(u"label_2")

        self.horizontalLayout_6.addWidget(self.label_2)

        self.spinBox_4 = QSpinBox(self.horizontalLayoutWidget_2)
        self.spinBox_4.setObjectName(u"spinBox_4")

        self.horizontalLayout_6.addWidget(self.spinBox_4)


        self.verticalLayout_7.addLayout(self.horizontalLayout_6)


        self.horizontalLayout_8.addLayout(self.verticalLayout_7)

        self.verticalLayout_8 = QVBoxLayout()
        self.verticalLayout_8.setSpacing(3)
        self.verticalLayout_8.setObjectName(u"verticalLayout_8")
        self.verticalLayout_8.setContentsMargins(-1, 3, -1, 3)
        self.checkBox_startGuiding = QCheckBox(self.horizontalLayoutWidget_2)
        self.checkBox_startGuiding.setObjectName(u"checkBox_startGuiding")

        self.verticalLayout_8.addWidget(self.checkBox_startGuiding)

        self.pushButton_3 = QPushButton(self.horizontalLayoutWidget_2)
        self.pushButton_3.setObjectName(u"pushButton_3")

        self.verticalLayout_8.addWidget(self.pushButton_3)

        self.pushButton_2 = QPushButton(self.horizontalLayoutWidget_2)
        self.pushButton_2.setObjectName(u"pushButton_2")

        self.verticalLayout_8.addWidget(self.pushButton_2)


        self.horizontalLayout_8.addLayout(self.verticalLayout_8)


        self.horizontalLayout_4.addLayout(self.horizontalLayout_8)

        self.tabWidget.addTab(self.tab_2, "")
        self.tab_3 = QWidget()
        self.tab_3.setObjectName(u"tab_3")
        self.tabWidget.addTab(self.tab_3, "")
        self.tab_6 = QWidget()
        self.tab_6.setObjectName(u"tab_6")
        self.horizontalLayoutWidget_3 = QWidget(self.tab_6)
        self.horizontalLayoutWidget_3.setObjectName(u"horizontalLayoutWidget_3")
        self.horizontalLayoutWidget_3.setGeometry(QRect(10, 0, 891, 111))
        self.horizontalLayout_23 = QHBoxLayout(self.horizontalLayoutWidget_3)
        self.horizontalLayout_23.setObjectName(u"horizontalLayout_23")
        self.horizontalLayout_23.setContentsMargins(0, 0, 0, 0)
        self.ControlRa_plot_2 = PlotWidget(self.horizontalLayoutWidget_3)
        self.ControlRa_plot_2.setObjectName(u"ControlRa_plot_2")

        self.horizontalLayout_23.addWidget(self.ControlRa_plot_2)

        self.ControlDec_plot_2 = PlotWidget(self.horizontalLayoutWidget_3)
        self.ControlDec_plot_2.setObjectName(u"ControlDec_plot_2")

        self.horizontalLayout_23.addWidget(self.ControlDec_plot_2)

        self.tabWidget.addTab(self.tab_6, "")
        self.tab_7 = QWidget()
        self.tab_7.setObjectName(u"tab_7")
        self.horizontalLayoutWidget = QWidget(self.tab_7)
        self.horizontalLayoutWidget.setObjectName(u"horizontalLayoutWidget")
        self.horizontalLayoutWidget.setGeometry(QRect(10, 0, 891, 111))
        self.horizontalLayout = QHBoxLayout(self.horizontalLayoutWidget)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.ErrorX_plot = PlotWidget(self.horizontalLayoutWidget)
        self.ErrorX_plot.setObjectName(u"ErrorX_plot")

        self.horizontalLayout.addWidget(self.ErrorX_plot)

        self.ErrorY_plot = PlotWidget(self.horizontalLayoutWidget)
        self.ErrorY_plot.setObjectName(u"ErrorY_plot")

        self.horizontalLayout.addWidget(self.ErrorY_plot)

        self.tabWidget.addTab(self.tab_7, "")

        self.verticalLayout_2.addWidget(self.tabWidget)

        self.verticalLayoutWidget_3 = QWidget(self.centralwidget)
        self.verticalLayoutWidget_3.setObjectName(u"verticalLayoutWidget_3")
        self.verticalLayoutWidget_3.setGeometry(QRect(0, 240, 211, 461))
        self.verticalLayout_5 = QVBoxLayout(self.verticalLayoutWidget_3)
        self.verticalLayout_5.setObjectName(u"verticalLayout_5")
        self.verticalLayout_5.setContentsMargins(5, 0, 5, 0)
        self.tabWidget_2 = QTabWidget(self.verticalLayoutWidget_3)
        self.tabWidget_2.setObjectName(u"tabWidget_2")
        self.tabWidget_2.setUsesScrollButtons(True)
        self.Mott = QWidget()
        self.Mott.setObjectName(u"Mott")
        self.verticalLayoutWidget_8 = QWidget(self.Mott)
        self.verticalLayoutWidget_8.setObjectName(u"verticalLayoutWidget_8")
        self.verticalLayoutWidget_8.setGeometry(QRect(10, 60, 181, 55))
        self.verticalLayout_10 = QVBoxLayout(self.verticalLayoutWidget_8)
        self.verticalLayout_10.setObjectName(u"verticalLayout_10")
        self.verticalLayout_10.setContentsMargins(0, 3, 0, 3)
        self.label_8 = QLabel(self.verticalLayoutWidget_8)
        self.label_8.setObjectName(u"label_8")
        font1 = QFont()
        font1.setFamilies([u"Segoe MDL2 Assets"])
        font1.setPointSize(12)
        self.label_8.setFont(font1)
        self.label_8.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_10.addWidget(self.label_8)

        self.comboBox_preview = QComboBox(self.verticalLayoutWidget_8)
        self.comboBox_preview.addItem("")
        self.comboBox_preview.addItem("")
        self.comboBox_preview.addItem("")
        self.comboBox_preview.setObjectName(u"comboBox_preview")

        self.verticalLayout_10.addWidget(self.comboBox_preview)

        self.verticalLayoutWidget_11 = QWidget(self.Mott)
        self.verticalLayoutWidget_11.setObjectName(u"verticalLayoutWidget_11")
        self.verticalLayoutWidget_11.setGeometry(QRect(10, 10, 181, 41))
        self.verticalLayout_13 = QVBoxLayout(self.verticalLayoutWidget_11)
        self.verticalLayout_13.setObjectName(u"verticalLayout_13")
        self.verticalLayout_13.setContentsMargins(0, 0, 0, 0)
        self.checkBox = QCheckBox(self.verticalLayoutWidget_11)
        self.checkBox.setObjectName(u"checkBox")
        font2 = QFont()
        font2.setPointSize(12)
        self.checkBox.setFont(font2)

        self.verticalLayout_13.addWidget(self.checkBox)

        self.verticalLayoutWidget_14 = QWidget(self.Mott)
        self.verticalLayoutWidget_14.setObjectName(u"verticalLayoutWidget_14")
        self.verticalLayoutWidget_14.setGeometry(QRect(10, 120, 181, 140))
        self.verticalLayout_16 = QVBoxLayout(self.verticalLayoutWidget_14)
        self.verticalLayout_16.setObjectName(u"verticalLayout_16")
        self.verticalLayout_16.setContentsMargins(0, 3, 0, 3)
        self.checkBox_show_sky_coo = QCheckBox(self.verticalLayoutWidget_14)
        self.checkBox_show_sky_coo.setObjectName(u"checkBox_show_sky_coo")

        self.verticalLayout_16.addWidget(self.checkBox_show_sky_coo)

        self.checkBox_show_motor_coo = QCheckBox(self.verticalLayoutWidget_14)
        self.checkBox_show_motor_coo.setObjectName(u"checkBox_show_motor_coo")

        self.verticalLayout_16.addWidget(self.checkBox_show_motor_coo)

        self.checkBox_show_all_detected = QCheckBox(self.verticalLayoutWidget_14)
        self.checkBox_show_all_detected.setObjectName(u"checkBox_show_all_detected")

        self.verticalLayout_16.addWidget(self.checkBox_show_all_detected)

        self.checkBox_show_sky_grid = QCheckBox(self.verticalLayoutWidget_14)
        self.checkBox_show_sky_grid.setObjectName(u"checkBox_show_sky_grid")

        self.verticalLayout_16.addWidget(self.checkBox_show_sky_grid)

        self.checkBox_show_guiding_star = QCheckBox(self.verticalLayoutWidget_14)
        self.checkBox_show_guiding_star.setObjectName(u"checkBox_show_guiding_star")

        self.verticalLayout_16.addWidget(self.checkBox_show_guiding_star)

        self.tabWidget_2.addTab(self.Mott, "")
        self.tab_5 = QWidget()
        self.tab_5.setObjectName(u"tab_5")
        self.verticalLayoutWidget_16 = QWidget(self.tab_5)
        self.verticalLayoutWidget_16.setObjectName(u"verticalLayoutWidget_16")
        self.verticalLayoutWidget_16.setGeometry(QRect(0, 0, 201, 81))
        self.verticalLayout_18 = QVBoxLayout(self.verticalLayoutWidget_16)
        self.verticalLayout_18.setObjectName(u"verticalLayout_18")
        self.verticalLayout_18.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout_10 = QHBoxLayout()
        self.horizontalLayout_10.setObjectName(u"horizontalLayout_10")
        self.horizontalLayout_10.setContentsMargins(-1, 3, -1, 3)
        self.label_5 = QLabel(self.verticalLayoutWidget_16)
        self.label_5.setObjectName(u"label_5")
        self.label_5.setFont(font2)
        self.label_5.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_10.addWidget(self.label_5)

        self.label_13 = QLabel(self.verticalLayoutWidget_16)
        self.label_13.setObjectName(u"label_13")
        self.label_13.setFont(font2)
        self.label_13.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_10.addWidget(self.label_13)


        self.verticalLayout_18.addLayout(self.horizontalLayout_10)

        self.horizontalLayout_9 = QHBoxLayout()
        self.horizontalLayout_9.setObjectName(u"horizontalLayout_9")
        self.lcdNumber_buck1 = QLCDNumber(self.verticalLayoutWidget_16)
        self.lcdNumber_buck1.setObjectName(u"lcdNumber_buck1")

        self.horizontalLayout_9.addWidget(self.lcdNumber_buck1)

        self.lcdNumber_buck2 = QLCDNumber(self.verticalLayoutWidget_16)
        self.lcdNumber_buck2.setObjectName(u"lcdNumber_buck2")

        self.horizontalLayout_9.addWidget(self.lcdNumber_buck2)


        self.verticalLayout_18.addLayout(self.horizontalLayout_9)

        self.verticalLayoutWidget_15 = QWidget(self.tab_5)
        self.verticalLayoutWidget_15.setObjectName(u"verticalLayoutWidget_15")
        self.verticalLayoutWidget_15.setGeometry(QRect(0, 80, 203, 201))
        self.verticalLayout_17 = QVBoxLayout(self.verticalLayoutWidget_15)
        self.verticalLayout_17.setObjectName(u"verticalLayout_17")
        self.verticalLayout_17.setContentsMargins(0, 0, 0, 0)
        self.label_18 = QLabel(self.verticalLayoutWidget_15)
        self.label_18.setObjectName(u"label_18")
        font3 = QFont()
        font3.setPointSize(16)
        self.label_18.setFont(font3)

        self.verticalLayout_17.addWidget(self.label_18)

        self.horizontalLayout_13 = QHBoxLayout()
        self.horizontalLayout_13.setObjectName(u"horizontalLayout_13")
        self.horizontalLayout_13.setContentsMargins(5, 3, 10, 3)
        self.label_14 = QLabel(self.verticalLayoutWidget_15)
        self.label_14.setObjectName(u"label_14")
        self.label_14.setFont(font2)

        self.horizontalLayout_13.addWidget(self.label_14)

        self.label_31 = QLabel(self.verticalLayoutWidget_15)
        self.label_31.setObjectName(u"label_31")
        sizePolicy1.setHeightForWidth(self.label_31.sizePolicy().hasHeightForWidth())
        self.label_31.setSizePolicy(sizePolicy1)
        self.label_31.setMaximumSize(QSize(16, 16))
        self.label_31.setPixmap(QPixmap(u"../stepper_controller_V2/GUI/StepperGUI/graphics/led_red.png"))
        self.label_31.setScaledContents(True)
        self.label_31.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_13.addWidget(self.label_31)

        self.lcdNumber_M1C1 = QLCDNumber(self.verticalLayoutWidget_15)
        self.lcdNumber_M1C1.setObjectName(u"lcdNumber_M1C1")

        self.horizontalLayout_13.addWidget(self.lcdNumber_M1C1)


        self.verticalLayout_17.addLayout(self.horizontalLayout_13)

        self.horizontalLayout_14 = QHBoxLayout()
        self.horizontalLayout_14.setObjectName(u"horizontalLayout_14")
        self.horizontalLayout_14.setContentsMargins(5, 3, 10, 3)
        self.label_15 = QLabel(self.verticalLayoutWidget_15)
        self.label_15.setObjectName(u"label_15")
        self.label_15.setFont(font2)

        self.horizontalLayout_14.addWidget(self.label_15)

        self.label_34 = QLabel(self.verticalLayoutWidget_15)
        self.label_34.setObjectName(u"label_34")
        sizePolicy1.setHeightForWidth(self.label_34.sizePolicy().hasHeightForWidth())
        self.label_34.setSizePolicy(sizePolicy1)
        self.label_34.setMaximumSize(QSize(16, 16))
        self.label_34.setPixmap(QPixmap(u"../stepper_controller_V2/GUI/StepperGUI/graphics/led_red.png"))
        self.label_34.setScaledContents(True)
        self.label_34.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_14.addWidget(self.label_34)

        self.lcdNumber_M1C2 = QLCDNumber(self.verticalLayoutWidget_15)
        self.lcdNumber_M1C2.setObjectName(u"lcdNumber_M1C2")

        self.horizontalLayout_14.addWidget(self.lcdNumber_M1C2)


        self.verticalLayout_17.addLayout(self.horizontalLayout_14)

        self.horizontalLayout_16 = QHBoxLayout()
        self.horizontalLayout_16.setObjectName(u"horizontalLayout_16")
        self.horizontalLayout_16.setContentsMargins(5, 3, 10, 3)
        self.label_17 = QLabel(self.verticalLayoutWidget_15)
        self.label_17.setObjectName(u"label_17")
        self.label_17.setFont(font2)

        self.horizontalLayout_16.addWidget(self.label_17)

        self.label_32 = QLabel(self.verticalLayoutWidget_15)
        self.label_32.setObjectName(u"label_32")
        sizePolicy1.setHeightForWidth(self.label_32.sizePolicy().hasHeightForWidth())
        self.label_32.setSizePolicy(sizePolicy1)
        self.label_32.setMaximumSize(QSize(16, 16))
        self.label_32.setPixmap(QPixmap(u"../stepper_controller_V2/GUI/StepperGUI/graphics/led_red.png"))
        self.label_32.setScaledContents(True)
        self.label_32.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_16.addWidget(self.label_32)

        self.lcdNumber_M2C1 = QLCDNumber(self.verticalLayoutWidget_15)
        self.lcdNumber_M2C1.setObjectName(u"lcdNumber_M2C1")

        self.horizontalLayout_16.addWidget(self.lcdNumber_M2C1)


        self.verticalLayout_17.addLayout(self.horizontalLayout_16)

        self.horizontalLayout_15 = QHBoxLayout()
        self.horizontalLayout_15.setObjectName(u"horizontalLayout_15")
        self.horizontalLayout_15.setContentsMargins(5, 3, 10, 3)
        self.label_16 = QLabel(self.verticalLayoutWidget_15)
        self.label_16.setObjectName(u"label_16")
        self.label_16.setFont(font2)

        self.horizontalLayout_15.addWidget(self.label_16)

        self.label_38 = QLabel(self.verticalLayoutWidget_15)
        self.label_38.setObjectName(u"label_38")
        sizePolicy1.setHeightForWidth(self.label_38.sizePolicy().hasHeightForWidth())
        self.label_38.setSizePolicy(sizePolicy1)
        self.label_38.setMaximumSize(QSize(16, 16))
        self.label_38.setPixmap(QPixmap(u"../stepper_controller_V2/GUI/StepperGUI/graphics/led_red.png"))
        self.label_38.setScaledContents(True)
        self.label_38.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_15.addWidget(self.label_38)

        self.lcdNumber_M2C2 = QLCDNumber(self.verticalLayoutWidget_15)
        self.lcdNumber_M2C2.setObjectName(u"lcdNumber_M2C2")

        self.horizontalLayout_15.addWidget(self.lcdNumber_M2C2)


        self.verticalLayout_17.addLayout(self.horizontalLayout_15)

        self.verticalLayoutWidget_20 = QWidget(self.tab_5)
        self.verticalLayoutWidget_20.setObjectName(u"verticalLayoutWidget_20")
        self.verticalLayoutWidget_20.setGeometry(QRect(0, 280, 201, 100))
        self.verticalLayout_22 = QVBoxLayout(self.verticalLayoutWidget_20)
        self.verticalLayout_22.setObjectName(u"verticalLayout_22")
        self.verticalLayout_22.setContentsMargins(0, 0, 0, 0)
        self.label_29 = QLabel(self.verticalLayoutWidget_20)
        self.label_29.setObjectName(u"label_29")
        self.label_29.setFont(font3)

        self.verticalLayout_22.addWidget(self.label_29)

        self.horizontalLayout_20 = QHBoxLayout()
        self.horizontalLayout_20.setObjectName(u"horizontalLayout_20")
        self.horizontalLayout_20.setContentsMargins(-1, 3, -1, 3)
        self.label_24 = QLabel(self.verticalLayoutWidget_20)
        self.label_24.setObjectName(u"label_24")
        self.label_24.setFont(font2)

        self.horizontalLayout_20.addWidget(self.label_24)

        self.lcdNumber_M1C1_R = QLCDNumber(self.verticalLayoutWidget_20)
        self.lcdNumber_M1C1_R.setObjectName(u"lcdNumber_M1C1_R")
        self.lcdNumber_M1C1_R.setSmallDecimalPoint(False)
        self.lcdNumber_M1C1_R.setDigitCount(3)

        self.horizontalLayout_20.addWidget(self.lcdNumber_M1C1_R)

        self.label_28 = QLabel(self.verticalLayoutWidget_20)
        self.label_28.setObjectName(u"label_28")
        self.label_28.setFont(font2)

        self.horizontalLayout_20.addWidget(self.label_28)

        self.lcdNumber_M1C2_R = QLCDNumber(self.verticalLayoutWidget_20)
        self.lcdNumber_M1C2_R.setObjectName(u"lcdNumber_M1C2_R")
        self.lcdNumber_M1C2_R.setSmallDecimalPoint(False)
        self.lcdNumber_M1C2_R.setDigitCount(3)

        self.horizontalLayout_20.addWidget(self.lcdNumber_M1C2_R)


        self.verticalLayout_22.addLayout(self.horizontalLayout_20)

        self.horizontalLayout_19 = QHBoxLayout()
        self.horizontalLayout_19.setObjectName(u"horizontalLayout_19")
        self.horizontalLayout_19.setContentsMargins(-1, 3, -1, 3)
        self.label_22 = QLabel(self.verticalLayoutWidget_20)
        self.label_22.setObjectName(u"label_22")
        self.label_22.setFont(font2)

        self.horizontalLayout_19.addWidget(self.label_22)

        self.lcdNumber_M2C1_R = QLCDNumber(self.verticalLayoutWidget_20)
        self.lcdNumber_M2C1_R.setObjectName(u"lcdNumber_M2C1_R")
        self.lcdNumber_M2C1_R.setSmallDecimalPoint(False)
        self.lcdNumber_M2C1_R.setDigitCount(3)

        self.horizontalLayout_19.addWidget(self.lcdNumber_M2C1_R)

        self.label_23 = QLabel(self.verticalLayoutWidget_20)
        self.label_23.setObjectName(u"label_23")
        self.label_23.setFont(font2)

        self.horizontalLayout_19.addWidget(self.label_23)

        self.lcdNumber_M2C2_R = QLCDNumber(self.verticalLayoutWidget_20)
        self.lcdNumber_M2C2_R.setObjectName(u"lcdNumber_M2C2_R")
        self.lcdNumber_M2C2_R.setSmallDecimalPoint(False)
        self.lcdNumber_M2C2_R.setDigitCount(3)

        self.horizontalLayout_19.addWidget(self.lcdNumber_M2C2_R)


        self.verticalLayout_22.addLayout(self.horizontalLayout_19)

        self.tabWidget_2.addTab(self.tab_5, "")
        self.tab_4 = QWidget()
        self.tab_4.setObjectName(u"tab_4")
        self.verticalLayoutWidget_19 = QWidget(self.tab_4)
        self.verticalLayoutWidget_19.setObjectName(u"verticalLayoutWidget_19")
        self.verticalLayoutWidget_19.setGeometry(QRect(9, 10, 181, 415))
        self.verticalLayout_26 = QVBoxLayout(self.verticalLayoutWidget_19)
        self.verticalLayout_26.setObjectName(u"verticalLayout_26")
        self.verticalLayout_26.setContentsMargins(0, 0, 0, 6)
        self.verticalLayout_29 = QVBoxLayout()
        self.verticalLayout_29.setObjectName(u"verticalLayout_29")
        self.pushButton_find_location = QPushButton(self.verticalLayoutWidget_19)
        self.pushButton_find_location.setObjectName(u"pushButton_find_location")

        self.verticalLayout_29.addWidget(self.pushButton_find_location)

        self.label_42 = QLabel(self.verticalLayoutWidget_19)
        self.label_42.setObjectName(u"label_42")
        self.label_42.setMaximumSize(QSize(16777215, 20))
        font4 = QFont()
        font4.setPointSize(11)
        self.label_42.setFont(font4)
        self.label_42.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_29.addWidget(self.label_42)

        self.verticalLayout_27 = QVBoxLayout()
        self.verticalLayout_27.setObjectName(u"verticalLayout_27")
        self.label_40 = QLabel(self.verticalLayoutWidget_19)
        self.label_40.setObjectName(u"label_40")
        self.label_40.setMaximumSize(QSize(16777215, 20))
        self.label_40.setFont(font4)
        self.label_40.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_27.addWidget(self.label_40)

        self.lineEdit_sky_ra = QLineEdit(self.verticalLayoutWidget_19)
        self.lineEdit_sky_ra.setObjectName(u"lineEdit_sky_ra")
        self.lineEdit_sky_ra.setMinimumSize(QSize(0, 20))
        self.lineEdit_sky_ra.setMaximumSize(QSize(16777215, 25))
        self.lineEdit_sky_ra.setReadOnly(True)

        self.verticalLayout_27.addWidget(self.lineEdit_sky_ra)


        self.verticalLayout_29.addLayout(self.verticalLayout_27)

        self.verticalLayout_28 = QVBoxLayout()
        self.verticalLayout_28.setObjectName(u"verticalLayout_28")
        self.label_41 = QLabel(self.verticalLayoutWidget_19)
        self.label_41.setObjectName(u"label_41")
        self.label_41.setMaximumSize(QSize(16777215, 20))
        self.label_41.setFont(font4)
        self.label_41.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_28.addWidget(self.label_41)

        self.lineEdit_sky_dec = QLineEdit(self.verticalLayoutWidget_19)
        self.lineEdit_sky_dec.setObjectName(u"lineEdit_sky_dec")
        self.lineEdit_sky_dec.setMinimumSize(QSize(0, 20))
        self.lineEdit_sky_dec.setMaximumSize(QSize(16777215, 25))
        self.lineEdit_sky_dec.setReadOnly(True)

        self.verticalLayout_28.addWidget(self.lineEdit_sky_dec)


        self.verticalLayout_29.addLayout(self.verticalLayout_28)

        self.verticalLayout_33 = QVBoxLayout()
        self.verticalLayout_33.setObjectName(u"verticalLayout_33")
        self.label_46 = QLabel(self.verticalLayoutWidget_19)
        self.label_46.setObjectName(u"label_46")
        self.label_46.setMaximumSize(QSize(16777215, 20))
        self.label_46.setFont(font4)
        self.label_46.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_33.addWidget(self.label_46)

        self.lineEdit_sky_rot = QLineEdit(self.verticalLayoutWidget_19)
        self.lineEdit_sky_rot.setObjectName(u"lineEdit_sky_rot")
        self.lineEdit_sky_rot.setMinimumSize(QSize(0, 20))
        self.lineEdit_sky_rot.setMaximumSize(QSize(16777215, 25))
        self.lineEdit_sky_rot.setReadOnly(True)

        self.verticalLayout_33.addWidget(self.lineEdit_sky_rot)


        self.verticalLayout_29.addLayout(self.verticalLayout_33)


        self.verticalLayout_26.addLayout(self.verticalLayout_29)

        self.verticalLayout_30 = QVBoxLayout()
        self.verticalLayout_30.setObjectName(u"verticalLayout_30")
        self.label_43 = QLabel(self.verticalLayoutWidget_19)
        self.label_43.setObjectName(u"label_43")
        self.label_43.setMaximumSize(QSize(16777215, 20))
        self.label_43.setFont(font4)
        self.label_43.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_30.addWidget(self.label_43)

        self.verticalLayout_31 = QVBoxLayout()
        self.verticalLayout_31.setObjectName(u"verticalLayout_31")
        self.label_44 = QLabel(self.verticalLayoutWidget_19)
        self.label_44.setObjectName(u"label_44")
        self.label_44.setMaximumSize(QSize(16777215, 20))
        self.label_44.setFont(font4)
        self.label_44.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_31.addWidget(self.label_44)

        self.lineEdit_7 = QLineEdit(self.verticalLayoutWidget_19)
        self.lineEdit_7.setObjectName(u"lineEdit_7")
        self.lineEdit_7.setMinimumSize(QSize(0, 20))
        self.lineEdit_7.setMaximumSize(QSize(16777215, 25))
        self.lineEdit_7.setReadOnly(True)

        self.verticalLayout_31.addWidget(self.lineEdit_7)


        self.verticalLayout_30.addLayout(self.verticalLayout_31)

        self.verticalLayout_32 = QVBoxLayout()
        self.verticalLayout_32.setObjectName(u"verticalLayout_32")
        self.label_45 = QLabel(self.verticalLayoutWidget_19)
        self.label_45.setObjectName(u"label_45")
        self.label_45.setMaximumSize(QSize(16777215, 20))
        self.label_45.setFont(font4)
        self.label_45.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_32.addWidget(self.label_45)

        self.lineEdit_9 = QLineEdit(self.verticalLayoutWidget_19)
        self.lineEdit_9.setObjectName(u"lineEdit_9")
        self.lineEdit_9.setMinimumSize(QSize(0, 20))
        self.lineEdit_9.setMaximumSize(QSize(16777215, 25))
        self.lineEdit_9.setReadOnly(True)

        self.verticalLayout_32.addWidget(self.lineEdit_9)


        self.verticalLayout_30.addLayout(self.verticalLayout_32)

        self.verticalLayout_34 = QVBoxLayout()
        self.verticalLayout_34.setObjectName(u"verticalLayout_34")
        self.label_47 = QLabel(self.verticalLayoutWidget_19)
        self.label_47.setObjectName(u"label_47")
        self.label_47.setMaximumSize(QSize(16777215, 20))
        self.label_47.setFont(font2)
        self.label_47.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_34.addWidget(self.label_47)

        self.lineEdit_11 = QLineEdit(self.verticalLayoutWidget_19)
        self.lineEdit_11.setObjectName(u"lineEdit_11")
        self.lineEdit_11.setMaximumSize(QSize(16777215, 25))
        self.lineEdit_11.setReadOnly(True)

        self.verticalLayout_34.addWidget(self.lineEdit_11)


        self.verticalLayout_30.addLayout(self.verticalLayout_34)


        self.verticalLayout_26.addLayout(self.verticalLayout_30)

        self.tabWidget_2.addTab(self.tab_4, "")

        self.verticalLayout_5.addWidget(self.tabWidget_2)

        self.verticalLayoutWidget_7 = QWidget(self.centralwidget)
        self.verticalLayoutWidget_7.setObjectName(u"verticalLayoutWidget_7")
        self.verticalLayoutWidget_7.setGeometry(QRect(1140, 0, 221, 151))
        self.verticalLayout_9 = QVBoxLayout(self.verticalLayoutWidget_7)
        self.verticalLayout_9.setObjectName(u"verticalLayout_9")
        self.verticalLayout_9.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout_12 = QHBoxLayout()
        self.horizontalLayout_12.setObjectName(u"horizontalLayout_12")
        self.label_7 = QLabel(self.verticalLayoutWidget_7)
        self.label_7.setObjectName(u"label_7")
        font5 = QFont()
        font5.setPointSize(14)
        self.label_7.setFont(font5)

        self.horizontalLayout_12.addWidget(self.label_7)

        self.lcdNumber_batt_volt = QLCDNumber(self.verticalLayoutWidget_7)
        self.lcdNumber_batt_volt.setObjectName(u"lcdNumber_batt_volt")

        self.horizontalLayout_12.addWidget(self.lcdNumber_batt_volt)


        self.verticalLayout_9.addLayout(self.horizontalLayout_12)

        self.horizontalLayout_11 = QHBoxLayout()
        self.horizontalLayout_11.setObjectName(u"horizontalLayout_11")
        self.label_6 = QLabel(self.verticalLayoutWidget_7)
        self.label_6.setObjectName(u"label_6")
        self.label_6.setFont(font5)

        self.horizontalLayout_11.addWidget(self.label_6)

        self.lcdNumber_batt_curr = QLCDNumber(self.verticalLayoutWidget_7)
        self.lcdNumber_batt_curr.setObjectName(u"lcdNumber_batt_curr")

        self.horizontalLayout_11.addWidget(self.lcdNumber_batt_curr)


        self.verticalLayout_9.addLayout(self.horizontalLayout_11)

        self.verticalLayoutWidget_5 = QWidget(self.centralwidget)
        self.verticalLayoutWidget_5.setObjectName(u"verticalLayoutWidget_5")
        self.verticalLayoutWidget_5.setGeometry(QRect(210, 160, 201, 541))
        self.verticalLayout_3 = QVBoxLayout(self.verticalLayoutWidget_5)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.verticalLayout_3.setContentsMargins(0, 10, 0, 10)
        self.verticalLayout_14 = QVBoxLayout()
        self.verticalLayout_14.setObjectName(u"verticalLayout_14")
        self.verticalLayout_4 = QVBoxLayout()
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.verticalLayout_23 = QVBoxLayout()
        self.verticalLayout_23.setObjectName(u"verticalLayout_23")
        self.pushButton_4 = QPushButton(self.verticalLayoutWidget_5)
        self.pushButton_4.setObjectName(u"pushButton_4")

        self.verticalLayout_23.addWidget(self.pushButton_4)

        self.horizontalLayout_21 = QHBoxLayout()
        self.horizontalLayout_21.setObjectName(u"horizontalLayout_21")
        self.label_37 = QLabel(self.verticalLayoutWidget_5)
        self.label_37.setObjectName(u"label_37")
        self.label_37.setMaximumSize(QSize(16777215, 25))
        self.label_37.setFont(font1)
        self.label_37.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_21.addWidget(self.label_37)

        self.spinBox_setExposure = QSpinBox(self.verticalLayoutWidget_5)
        self.spinBox_setExposure.setObjectName(u"spinBox_setExposure")
        self.spinBox_setExposure.setMaximumSize(QSize(16777215, 25))
        self.spinBox_setExposure.setMinimum(100)
        self.spinBox_setExposure.setMaximum(20000)
        self.spinBox_setExposure.setValue(500)

        self.horizontalLayout_21.addWidget(self.spinBox_setExposure)


        self.verticalLayout_23.addLayout(self.horizontalLayout_21)

        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.label_36 = QLabel(self.verticalLayoutWidget_5)
        self.label_36.setObjectName(u"label_36")
        self.label_36.setMaximumSize(QSize(16777215, 25))
        self.label_36.setFont(font1)
        self.label_36.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_2.addWidget(self.label_36)

        self.spinBox_setInterval = QSpinBox(self.verticalLayoutWidget_5)
        self.spinBox_setInterval.setObjectName(u"spinBox_setInterval")
        self.spinBox_setInterval.setMinimum(100)
        self.spinBox_setInterval.setMaximum(20000)
        self.spinBox_setInterval.setValue(1000)

        self.horizontalLayout_2.addWidget(self.spinBox_setInterval)


        self.verticalLayout_23.addLayout(self.horizontalLayout_2)

        self.horizontalLayout_22 = QHBoxLayout()
        self.horizontalLayout_22.setObjectName(u"horizontalLayout_22")
        self.label_35 = QLabel(self.verticalLayoutWidget_5)
        self.label_35.setObjectName(u"label_35")
        self.label_35.setMaximumSize(QSize(16777215, 25))
        self.label_35.setFont(font1)
        self.label_35.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout_22.addWidget(self.label_35)

        self.spinBox_setGain = QSpinBox(self.verticalLayoutWidget_5)
        self.spinBox_setGain.setObjectName(u"spinBox_setGain")
        self.spinBox_setGain.setMaximum(100)
        self.spinBox_setGain.setValue(70)

        self.horizontalLayout_22.addWidget(self.spinBox_setGain)


        self.verticalLayout_23.addLayout(self.horizontalLayout_22)


        self.verticalLayout_4.addLayout(self.verticalLayout_23)


        self.verticalLayout_14.addLayout(self.verticalLayout_4)


        self.verticalLayout_3.addLayout(self.verticalLayout_14)

        self.label_39 = QLabel(self.verticalLayoutWidget_5)
        self.label_39.setObjectName(u"label_39")
        self.label_39.setFont(font1)
        self.label_39.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_3.addWidget(self.label_39)

        self.verticalLayout_24 = QVBoxLayout()
        self.verticalLayout_24.setObjectName(u"verticalLayout_24")
        self.verticalLayout_24.setContentsMargins(-1, 3, -1, 3)
        self.label_12 = QLabel(self.verticalLayoutWidget_5)
        self.label_12.setObjectName(u"label_12")
        self.label_12.setFont(font1)
        self.label_12.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_24.addWidget(self.label_12)

        self.lineEdit_rec_title = QLineEdit(self.verticalLayoutWidget_5)
        self.lineEdit_rec_title.setObjectName(u"lineEdit_rec_title")
        self.lineEdit_rec_title.setReadOnly(True)

        self.verticalLayout_24.addWidget(self.lineEdit_rec_title)


        self.verticalLayout_3.addLayout(self.verticalLayout_24)

        self.verticalLayout_12 = QVBoxLayout()
        self.verticalLayout_12.setObjectName(u"verticalLayout_12")
        self.verticalLayout_12.setContentsMargins(-1, 3, -1, 3)
        self.label_10 = QLabel(self.verticalLayoutWidget_5)
        self.label_10.setObjectName(u"label_10")
        self.label_10.setFont(font1)
        self.label_10.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_12.addWidget(self.label_10)

        self.lineEdit_rec_exp = QLineEdit(self.verticalLayoutWidget_5)
        self.lineEdit_rec_exp.setObjectName(u"lineEdit_rec_exp")
        self.lineEdit_rec_exp.setReadOnly(True)

        self.verticalLayout_12.addWidget(self.lineEdit_rec_exp)


        self.verticalLayout_3.addLayout(self.verticalLayout_12)

        self.verticalLayout_11 = QVBoxLayout()
        self.verticalLayout_11.setObjectName(u"verticalLayout_11")
        self.verticalLayout_11.setContentsMargins(-1, 3, -1, 3)
        self.label_9 = QLabel(self.verticalLayoutWidget_5)
        self.label_9.setObjectName(u"label_9")
        self.label_9.setFont(font1)
        self.label_9.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_11.addWidget(self.label_9)

        self.lineEdit_rec_capt_time = QLineEdit(self.verticalLayoutWidget_5)
        self.lineEdit_rec_capt_time.setObjectName(u"lineEdit_rec_capt_time")
        self.lineEdit_rec_capt_time.setReadOnly(True)

        self.verticalLayout_11.addWidget(self.lineEdit_rec_capt_time)


        self.verticalLayout_3.addLayout(self.verticalLayout_11)

        self.verticalLayout_15 = QVBoxLayout()
        self.verticalLayout_15.setObjectName(u"verticalLayout_15")
        self.verticalLayout_15.setContentsMargins(-1, 3, -1, 3)
        self.label_11 = QLabel(self.verticalLayoutWidget_5)
        self.label_11.setObjectName(u"label_11")
        self.label_11.setFont(font1)
        self.label_11.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_15.addWidget(self.label_11)

        self.lineEdit_rec_interval = QLineEdit(self.verticalLayoutWidget_5)
        self.lineEdit_rec_interval.setObjectName(u"lineEdit_rec_interval")
        self.lineEdit_rec_interval.setReadOnly(True)

        self.verticalLayout_15.addWidget(self.lineEdit_rec_interval)


        self.verticalLayout_3.addLayout(self.verticalLayout_15)

        self.verticalLayout_25 = QVBoxLayout()
        self.verticalLayout_25.setObjectName(u"verticalLayout_25")
        self.verticalLayout_25.setContentsMargins(-1, 3, -1, 3)
        self.label_33 = QLabel(self.verticalLayoutWidget_5)
        self.label_33.setObjectName(u"label_33")
        self.label_33.setFont(font1)
        self.label_33.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout_25.addWidget(self.label_33)

        self.lineEdit_rec_gain = QLineEdit(self.verticalLayoutWidget_5)
        self.lineEdit_rec_gain.setObjectName(u"lineEdit_rec_gain")
        self.lineEdit_rec_gain.setReadOnly(True)

        self.verticalLayout_25.addWidget(self.lineEdit_rec_gain)


        self.verticalLayout_3.addLayout(self.verticalLayout_25)

        self.imageWidget = RawImageWidget(self.centralwidget)
        self.imageWidget.setObjectName(u"imageWidget")
        self.imageWidget.setGeometry(QRect(410, 160, 720, 540))
        self.imageWidget.setMouseTracking(True)
        self.histogramWidget = PlotWidget(self.centralwidget)
        self.histogramWidget.setObjectName(u"histogramWidget")
        self.histogramWidget.setGeometry(QRect(1140, 240, 211, 461))
        self.widget_3 = QWidget(self.centralwidget)
        self.widget_3.setObjectName(u"widget_3")
        self.widget_3.setGeometry(QRect(1260, 200, 15, 15))
        palette = QPalette()
        brush = QBrush(QColor(0, 0, 0, 255))
        brush.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.WindowText, brush)
        brush1 = QBrush(QColor(255, 255, 0, 255))
        brush1.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Button, brush1)
        brush2 = QBrush(QColor(255, 255, 127, 255))
        brush2.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Light, brush2)
        brush3 = QBrush(QColor(255, 255, 63, 255))
        brush3.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Midlight, brush3)
        brush4 = QBrush(QColor(127, 127, 0, 255))
        brush4.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Dark, brush4)
        brush5 = QBrush(QColor(170, 170, 0, 255))
        brush5.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Mid, brush5)
        palette.setBrush(QPalette.Active, QPalette.Text, brush)
        brush6 = QBrush(QColor(255, 255, 255, 255))
        brush6.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.BrightText, brush6)
        palette.setBrush(QPalette.Active, QPalette.ButtonText, brush)
        palette.setBrush(QPalette.Active, QPalette.Base, brush6)
        palette.setBrush(QPalette.Active, QPalette.Window, brush1)
        palette.setBrush(QPalette.Active, QPalette.Shadow, brush)
        palette.setBrush(QPalette.Active, QPalette.AlternateBase, brush2)
        brush7 = QBrush(QColor(255, 255, 220, 255))
        brush7.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.ToolTipBase, brush7)
        palette.setBrush(QPalette.Active, QPalette.ToolTipText, brush)
        brush8 = QBrush(QColor(0, 0, 0, 127))
        brush8.setStyle(Qt.SolidPattern)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette.Active, QPalette.PlaceholderText, brush8)
#endif
        palette.setBrush(QPalette.Active, QPalette.Accent, brush6)
        palette.setBrush(QPalette.Inactive, QPalette.WindowText, brush)
        palette.setBrush(QPalette.Inactive, QPalette.Button, brush1)
        palette.setBrush(QPalette.Inactive, QPalette.Light, brush2)
        palette.setBrush(QPalette.Inactive, QPalette.Midlight, brush3)
        palette.setBrush(QPalette.Inactive, QPalette.Dark, brush4)
        palette.setBrush(QPalette.Inactive, QPalette.Mid, brush5)
        palette.setBrush(QPalette.Inactive, QPalette.Text, brush)
        palette.setBrush(QPalette.Inactive, QPalette.BrightText, brush6)
        palette.setBrush(QPalette.Inactive, QPalette.ButtonText, brush)
        palette.setBrush(QPalette.Inactive, QPalette.Base, brush6)
        palette.setBrush(QPalette.Inactive, QPalette.Window, brush1)
        palette.setBrush(QPalette.Inactive, QPalette.Shadow, brush)
        palette.setBrush(QPalette.Inactive, QPalette.AlternateBase, brush2)
        palette.setBrush(QPalette.Inactive, QPalette.ToolTipBase, brush7)
        palette.setBrush(QPalette.Inactive, QPalette.ToolTipText, brush)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette.Inactive, QPalette.PlaceholderText, brush8)
#endif
        palette.setBrush(QPalette.Inactive, QPalette.Accent, brush6)
        palette.setBrush(QPalette.Disabled, QPalette.WindowText, brush4)
        palette.setBrush(QPalette.Disabled, QPalette.Button, brush1)
        palette.setBrush(QPalette.Disabled, QPalette.Light, brush2)
        palette.setBrush(QPalette.Disabled, QPalette.Midlight, brush3)
        palette.setBrush(QPalette.Disabled, QPalette.Dark, brush4)
        palette.setBrush(QPalette.Disabled, QPalette.Mid, brush5)
        palette.setBrush(QPalette.Disabled, QPalette.Text, brush4)
        palette.setBrush(QPalette.Disabled, QPalette.BrightText, brush6)
        palette.setBrush(QPalette.Disabled, QPalette.ButtonText, brush4)
        palette.setBrush(QPalette.Disabled, QPalette.Base, brush1)
        palette.setBrush(QPalette.Disabled, QPalette.Window, brush1)
        palette.setBrush(QPalette.Disabled, QPalette.Shadow, brush)
        palette.setBrush(QPalette.Disabled, QPalette.AlternateBase, brush1)
        palette.setBrush(QPalette.Disabled, QPalette.ToolTipBase, brush7)
        palette.setBrush(QPalette.Disabled, QPalette.ToolTipText, brush)
        brush9 = QBrush(QColor(127, 127, 0, 127))
        brush9.setStyle(Qt.SolidPattern)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette.Disabled, QPalette.PlaceholderText, brush9)
#endif
        brush10 = QBrush(QColor(255, 255, 76, 255))
        brush10.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Disabled, QPalette.Accent, brush10)
        self.widget_3.setPalette(palette)
        self.widget_3.setAutoFillBackground(True)
        self.label_48 = QLabel(self.centralwidget)
        self.label_48.setObjectName(u"label_48")
        self.label_48.setGeometry(QRect(1280, 200, 91, 21))
        self.widget = QWidget(self.centralwidget)
        self.widget.setObjectName(u"widget")
        self.widget.setGeometry(QRect(1140, 170, 15, 15))
        palette1 = QPalette()
        palette1.setBrush(QPalette.Active, QPalette.WindowText, brush)
        brush11 = QBrush(QColor(0, 255, 0, 255))
        brush11.setStyle(Qt.SolidPattern)
        palette1.setBrush(QPalette.Active, QPalette.Button, brush11)
        brush12 = QBrush(QColor(127, 255, 127, 255))
        brush12.setStyle(Qt.SolidPattern)
        palette1.setBrush(QPalette.Active, QPalette.Light, brush12)
        brush13 = QBrush(QColor(63, 255, 63, 255))
        brush13.setStyle(Qt.SolidPattern)
        palette1.setBrush(QPalette.Active, QPalette.Midlight, brush13)
        brush14 = QBrush(QColor(0, 127, 0, 255))
        brush14.setStyle(Qt.SolidPattern)
        palette1.setBrush(QPalette.Active, QPalette.Dark, brush14)
        brush15 = QBrush(QColor(0, 170, 0, 255))
        brush15.setStyle(Qt.SolidPattern)
        palette1.setBrush(QPalette.Active, QPalette.Mid, brush15)
        palette1.setBrush(QPalette.Active, QPalette.Text, brush)
        palette1.setBrush(QPalette.Active, QPalette.BrightText, brush6)
        palette1.setBrush(QPalette.Active, QPalette.ButtonText, brush)
        palette1.setBrush(QPalette.Active, QPalette.Base, brush6)
        palette1.setBrush(QPalette.Active, QPalette.Window, brush11)
        palette1.setBrush(QPalette.Active, QPalette.Shadow, brush)
        palette1.setBrush(QPalette.Active, QPalette.AlternateBase, brush12)
        palette1.setBrush(QPalette.Active, QPalette.ToolTipBase, brush7)
        palette1.setBrush(QPalette.Active, QPalette.ToolTipText, brush)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette.Active, QPalette.PlaceholderText, brush8)
#endif
        palette1.setBrush(QPalette.Active, QPalette.Accent, brush6)
        palette1.setBrush(QPalette.Inactive, QPalette.WindowText, brush)
        palette1.setBrush(QPalette.Inactive, QPalette.Button, brush11)
        palette1.setBrush(QPalette.Inactive, QPalette.Light, brush12)
        palette1.setBrush(QPalette.Inactive, QPalette.Midlight, brush13)
        palette1.setBrush(QPalette.Inactive, QPalette.Dark, brush14)
        palette1.setBrush(QPalette.Inactive, QPalette.Mid, brush15)
        palette1.setBrush(QPalette.Inactive, QPalette.Text, brush)
        palette1.setBrush(QPalette.Inactive, QPalette.BrightText, brush6)
        palette1.setBrush(QPalette.Inactive, QPalette.ButtonText, brush)
        palette1.setBrush(QPalette.Inactive, QPalette.Base, brush6)
        palette1.setBrush(QPalette.Inactive, QPalette.Window, brush11)
        palette1.setBrush(QPalette.Inactive, QPalette.Shadow, brush)
        palette1.setBrush(QPalette.Inactive, QPalette.AlternateBase, brush12)
        palette1.setBrush(QPalette.Inactive, QPalette.ToolTipBase, brush7)
        palette1.setBrush(QPalette.Inactive, QPalette.ToolTipText, brush)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette.Inactive, QPalette.PlaceholderText, brush8)
#endif
        palette1.setBrush(QPalette.Inactive, QPalette.Accent, brush6)
        palette1.setBrush(QPalette.Disabled, QPalette.WindowText, brush14)
        palette1.setBrush(QPalette.Disabled, QPalette.Button, brush11)
        palette1.setBrush(QPalette.Disabled, QPalette.Light, brush12)
        palette1.setBrush(QPalette.Disabled, QPalette.Midlight, brush13)
        palette1.setBrush(QPalette.Disabled, QPalette.Dark, brush14)
        palette1.setBrush(QPalette.Disabled, QPalette.Mid, brush15)
        palette1.setBrush(QPalette.Disabled, QPalette.Text, brush14)
        palette1.setBrush(QPalette.Disabled, QPalette.BrightText, brush6)
        palette1.setBrush(QPalette.Disabled, QPalette.ButtonText, brush14)
        palette1.setBrush(QPalette.Disabled, QPalette.Base, brush11)
        palette1.setBrush(QPalette.Disabled, QPalette.Window, brush11)
        palette1.setBrush(QPalette.Disabled, QPalette.Shadow, brush)
        palette1.setBrush(QPalette.Disabled, QPalette.AlternateBase, brush11)
        palette1.setBrush(QPalette.Disabled, QPalette.ToolTipBase, brush7)
        palette1.setBrush(QPalette.Disabled, QPalette.ToolTipText, brush)
        brush16 = QBrush(QColor(0, 127, 0, 127))
        brush16.setStyle(Qt.SolidPattern)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette.Disabled, QPalette.PlaceholderText, brush16)
#endif
        brush17 = QBrush(QColor(76, 255, 76, 255))
        brush17.setStyle(Qt.SolidPattern)
        palette1.setBrush(QPalette.Disabled, QPalette.Accent, brush17)
        self.widget.setPalette(palette1)
        self.widget.setAutoFillBackground(True)
        self.label_49 = QLabel(self.centralwidget)
        self.label_49.setObjectName(u"label_49")
        self.label_49.setGeometry(QRect(1280, 170, 111, 21))
        self.widget_2 = QWidget(self.centralwidget)
        self.widget_2.setObjectName(u"widget_2")
        self.widget_2.setGeometry(QRect(1140, 200, 15, 15))
        palette2 = QPalette()
        palette2.setBrush(QPalette.Active, QPalette.WindowText, brush)
        brush18 = QBrush(QColor(0, 0, 255, 255))
        brush18.setStyle(Qt.SolidPattern)
        palette2.setBrush(QPalette.Active, QPalette.Button, brush18)
        brush19 = QBrush(QColor(127, 127, 255, 255))
        brush19.setStyle(Qt.SolidPattern)
        palette2.setBrush(QPalette.Active, QPalette.Light, brush19)
        brush20 = QBrush(QColor(63, 63, 255, 255))
        brush20.setStyle(Qt.SolidPattern)
        palette2.setBrush(QPalette.Active, QPalette.Midlight, brush20)
        brush21 = QBrush(QColor(0, 0, 127, 255))
        brush21.setStyle(Qt.SolidPattern)
        palette2.setBrush(QPalette.Active, QPalette.Dark, brush21)
        brush22 = QBrush(QColor(0, 0, 170, 255))
        brush22.setStyle(Qt.SolidPattern)
        palette2.setBrush(QPalette.Active, QPalette.Mid, brush22)
        palette2.setBrush(QPalette.Active, QPalette.Text, brush)
        palette2.setBrush(QPalette.Active, QPalette.BrightText, brush6)
        palette2.setBrush(QPalette.Active, QPalette.ButtonText, brush)
        palette2.setBrush(QPalette.Active, QPalette.Base, brush6)
        palette2.setBrush(QPalette.Active, QPalette.Window, brush18)
        palette2.setBrush(QPalette.Active, QPalette.Shadow, brush)
        palette2.setBrush(QPalette.Active, QPalette.AlternateBase, brush19)
        palette2.setBrush(QPalette.Active, QPalette.ToolTipBase, brush7)
        palette2.setBrush(QPalette.Active, QPalette.ToolTipText, brush)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette.Active, QPalette.PlaceholderText, brush8)
#endif
        palette2.setBrush(QPalette.Active, QPalette.Accent, brush6)
        palette2.setBrush(QPalette.Inactive, QPalette.WindowText, brush)
        palette2.setBrush(QPalette.Inactive, QPalette.Button, brush18)
        palette2.setBrush(QPalette.Inactive, QPalette.Light, brush19)
        palette2.setBrush(QPalette.Inactive, QPalette.Midlight, brush20)
        palette2.setBrush(QPalette.Inactive, QPalette.Dark, brush21)
        palette2.setBrush(QPalette.Inactive, QPalette.Mid, brush22)
        palette2.setBrush(QPalette.Inactive, QPalette.Text, brush)
        palette2.setBrush(QPalette.Inactive, QPalette.BrightText, brush6)
        palette2.setBrush(QPalette.Inactive, QPalette.ButtonText, brush)
        palette2.setBrush(QPalette.Inactive, QPalette.Base, brush6)
        palette2.setBrush(QPalette.Inactive, QPalette.Window, brush18)
        palette2.setBrush(QPalette.Inactive, QPalette.Shadow, brush)
        palette2.setBrush(QPalette.Inactive, QPalette.AlternateBase, brush19)
        palette2.setBrush(QPalette.Inactive, QPalette.ToolTipBase, brush7)
        palette2.setBrush(QPalette.Inactive, QPalette.ToolTipText, brush)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette.Inactive, QPalette.PlaceholderText, brush8)
#endif
        palette2.setBrush(QPalette.Inactive, QPalette.Accent, brush6)
        palette2.setBrush(QPalette.Disabled, QPalette.WindowText, brush21)
        palette2.setBrush(QPalette.Disabled, QPalette.Button, brush18)
        palette2.setBrush(QPalette.Disabled, QPalette.Light, brush19)
        palette2.setBrush(QPalette.Disabled, QPalette.Midlight, brush20)
        palette2.setBrush(QPalette.Disabled, QPalette.Dark, brush21)
        palette2.setBrush(QPalette.Disabled, QPalette.Mid, brush22)
        palette2.setBrush(QPalette.Disabled, QPalette.Text, brush21)
        palette2.setBrush(QPalette.Disabled, QPalette.BrightText, brush6)
        palette2.setBrush(QPalette.Disabled, QPalette.ButtonText, brush21)
        palette2.setBrush(QPalette.Disabled, QPalette.Base, brush18)
        palette2.setBrush(QPalette.Disabled, QPalette.Window, brush18)
        palette2.setBrush(QPalette.Disabled, QPalette.Shadow, brush)
        palette2.setBrush(QPalette.Disabled, QPalette.AlternateBase, brush18)
        palette2.setBrush(QPalette.Disabled, QPalette.ToolTipBase, brush7)
        palette2.setBrush(QPalette.Disabled, QPalette.ToolTipText, brush)
        brush23 = QBrush(QColor(0, 0, 127, 127))
        brush23.setStyle(Qt.SolidPattern)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette.Disabled, QPalette.PlaceholderText, brush23)
#endif
        brush24 = QBrush(QColor(76, 76, 255, 255))
        brush24.setStyle(Qt.SolidPattern)
        palette2.setBrush(QPalette.Disabled, QPalette.Accent, brush24)
        self.widget_2.setPalette(palette2)
        self.widget_2.setAutoFillBackground(True)
        self.widget_4 = QWidget(self.centralwidget)
        self.widget_4.setObjectName(u"widget_4")
        self.widget_4.setGeometry(QRect(1260, 170, 15, 15))
        palette3 = QPalette()
        palette3.setBrush(QPalette.Active, QPalette.WindowText, brush)
        brush25 = QBrush(QColor(255, 0, 0, 255))
        brush25.setStyle(Qt.SolidPattern)
        palette3.setBrush(QPalette.Active, QPalette.Button, brush25)
        brush26 = QBrush(QColor(255, 127, 127, 255))
        brush26.setStyle(Qt.SolidPattern)
        palette3.setBrush(QPalette.Active, QPalette.Light, brush26)
        brush27 = QBrush(QColor(255, 63, 63, 255))
        brush27.setStyle(Qt.SolidPattern)
        palette3.setBrush(QPalette.Active, QPalette.Midlight, brush27)
        brush28 = QBrush(QColor(127, 0, 0, 255))
        brush28.setStyle(Qt.SolidPattern)
        palette3.setBrush(QPalette.Active, QPalette.Dark, brush28)
        brush29 = QBrush(QColor(170, 0, 0, 255))
        brush29.setStyle(Qt.SolidPattern)
        palette3.setBrush(QPalette.Active, QPalette.Mid, brush29)
        palette3.setBrush(QPalette.Active, QPalette.Text, brush)
        palette3.setBrush(QPalette.Active, QPalette.BrightText, brush6)
        palette3.setBrush(QPalette.Active, QPalette.ButtonText, brush)
        palette3.setBrush(QPalette.Active, QPalette.Base, brush6)
        palette3.setBrush(QPalette.Active, QPalette.Window, brush25)
        palette3.setBrush(QPalette.Active, QPalette.Shadow, brush)
        palette3.setBrush(QPalette.Active, QPalette.AlternateBase, brush26)
        palette3.setBrush(QPalette.Active, QPalette.ToolTipBase, brush7)
        palette3.setBrush(QPalette.Active, QPalette.ToolTipText, brush)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette3.setBrush(QPalette.Active, QPalette.PlaceholderText, brush8)
#endif
        palette3.setBrush(QPalette.Active, QPalette.Accent, brush6)
        palette3.setBrush(QPalette.Inactive, QPalette.WindowText, brush)
        palette3.setBrush(QPalette.Inactive, QPalette.Button, brush25)
        palette3.setBrush(QPalette.Inactive, QPalette.Light, brush26)
        palette3.setBrush(QPalette.Inactive, QPalette.Midlight, brush27)
        palette3.setBrush(QPalette.Inactive, QPalette.Dark, brush28)
        palette3.setBrush(QPalette.Inactive, QPalette.Mid, brush29)
        palette3.setBrush(QPalette.Inactive, QPalette.Text, brush)
        palette3.setBrush(QPalette.Inactive, QPalette.BrightText, brush6)
        palette3.setBrush(QPalette.Inactive, QPalette.ButtonText, brush)
        palette3.setBrush(QPalette.Inactive, QPalette.Base, brush6)
        palette3.setBrush(QPalette.Inactive, QPalette.Window, brush25)
        palette3.setBrush(QPalette.Inactive, QPalette.Shadow, brush)
        palette3.setBrush(QPalette.Inactive, QPalette.AlternateBase, brush26)
        palette3.setBrush(QPalette.Inactive, QPalette.ToolTipBase, brush7)
        palette3.setBrush(QPalette.Inactive, QPalette.ToolTipText, brush)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette3.setBrush(QPalette.Inactive, QPalette.PlaceholderText, brush8)
#endif
        palette3.setBrush(QPalette.Inactive, QPalette.Accent, brush6)
        palette3.setBrush(QPalette.Disabled, QPalette.WindowText, brush28)
        palette3.setBrush(QPalette.Disabled, QPalette.Button, brush25)
        palette3.setBrush(QPalette.Disabled, QPalette.Light, brush26)
        palette3.setBrush(QPalette.Disabled, QPalette.Midlight, brush27)
        palette3.setBrush(QPalette.Disabled, QPalette.Dark, brush28)
        palette3.setBrush(QPalette.Disabled, QPalette.Mid, brush29)
        palette3.setBrush(QPalette.Disabled, QPalette.Text, brush28)
        palette3.setBrush(QPalette.Disabled, QPalette.BrightText, brush6)
        palette3.setBrush(QPalette.Disabled, QPalette.ButtonText, brush28)
        palette3.setBrush(QPalette.Disabled, QPalette.Base, brush25)
        palette3.setBrush(QPalette.Disabled, QPalette.Window, brush25)
        palette3.setBrush(QPalette.Disabled, QPalette.Shadow, brush)
        palette3.setBrush(QPalette.Disabled, QPalette.AlternateBase, brush25)
        palette3.setBrush(QPalette.Disabled, QPalette.ToolTipBase, brush7)
        palette3.setBrush(QPalette.Disabled, QPalette.ToolTipText, brush)
        brush30 = QBrush(QColor(127, 0, 0, 127))
        brush30.setStyle(Qt.SolidPattern)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette3.setBrush(QPalette.Disabled, QPalette.PlaceholderText, brush30)
#endif
        brush31 = QBrush(QColor(255, 76, 76, 255))
        brush31.setStyle(Qt.SolidPattern)
        palette3.setBrush(QPalette.Disabled, QPalette.Accent, brush31)
        self.widget_4.setPalette(palette3)
        self.widget_4.setAutoFillBackground(True)
        self.label_30 = QLabel(self.centralwidget)
        self.label_30.setObjectName(u"label_30")
        self.label_30.setGeometry(QRect(1160, 200, 91, 16))
        self.label_27 = QLabel(self.centralwidget)
        self.label_27.setObjectName(u"label_27")
        self.label_27.setGeometry(QRect(1160, 170, 111, 21))
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 1368, 21))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        self.spinBox_RaSpeed.valueChanged.connect(self.horizontalSlider_RaSpeed.setValue)
        self.horizontalSlider_RaSpeed.valueChanged.connect(self.spinBox_RaSpeed.setValue)
        self.horizontalSlider_DecSpeed.valueChanged.connect(self.spinBox_DecSpeed.setValue)
        self.spinBox_DecSpeed.valueChanged.connect(self.horizontalSlider_DecSpeed.setValue)
        self.checkBox.clicked["bool"].connect(MainWindow.requestCompressed)
        self.pushButton_4.clicked.connect(MainWindow.setupCamera)
        self.pushButton.clicked.connect(MainWindow.connectMqtt)
        self.pushButton_find_location.clicked.connect(MainWindow.localizeField)
        self.pushButton_2.clicked.connect(MainWindow.runIdentification)
        self.spinBox_RaSpeed.valueChanged.connect(MainWindow.setRaSpeed)
        self.spinBox_DecSpeed.valueChanged.connect(MainWindow.setDecSpeed)

        self.tabWidget.setCurrentIndex(1)
        self.tabWidget_2.setCurrentIndex(1)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"MainWindow", None))
        self.comboBox_address.setItemText(0, QCoreApplication.translate("MainWindow", u"10.42.0.1", None))
        self.comboBox_address.setItemText(1, QCoreApplication.translate("MainWindow", u"192.168.1.101", None))

        self.pushButton.setText(QCoreApplication.translate("MainWindow", u"Connect", None))
        self.label_20.setText(QCoreApplication.translate("MainWindow", u"Broker", None))
        self.label_19.setText(QCoreApplication.translate("MainWindow", u"Guider", None))
        self.label_21.setText(QCoreApplication.translate("MainWindow", u"Stepper", None))
        self.label_broker_status.setText("")
        self.label_guider_status.setText("")
        self.label_stepper_status.setText("")
        self.radioButton_manual_speed.setText(QCoreApplication.translate("MainWindow", u"Manual speed control", None))
        self.radioButton_auto_speed.setText(QCoreApplication.translate("MainWindow", u"Auto speed control", None))
        self.radioButton_manual_steps.setText(QCoreApplication.translate("MainWindow", u"Manual Steps", None))
        self.radioButton_manual_coil.setText(QCoreApplication.translate("MainWindow", u"Manual coil control", None))
        self.label.setText(QCoreApplication.translate("MainWindow", u"RA Speed ", None))
        self.label_3.setText(QCoreApplication.translate("MainWindow", u"Dec Speed", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab), QCoreApplication.translate("MainWindow", u"Manual Speed", None))
        self.label_4.setText(QCoreApplication.translate("MainWindow", u"Dec gain", None))
        self.label_2.setText(QCoreApplication.translate("MainWindow", u"Ra gain", None))
        self.checkBox_startGuiding.setText(QCoreApplication.translate("MainWindow", u"Guide", None))
        self.pushButton_3.setText(QCoreApplication.translate("MainWindow", u"Keep current position", None))
        self.pushButton_2.setText(QCoreApplication.translate("MainWindow", u"Identify System", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_2), QCoreApplication.translate("MainWindow", u"Auto speed", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_3), QCoreApplication.translate("MainWindow", u"Manual coil", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_6), QCoreApplication.translate("MainWindow", u"Control", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_7), QCoreApplication.translate("MainWindow", u"Error", None))
        self.label_8.setText(QCoreApplication.translate("MainWindow", u"Preview mode", None))
        self.comboBox_preview.setItemText(0, QCoreApplication.translate("MainWindow", u"Original", None))
        self.comboBox_preview.setItemText(1, QCoreApplication.translate("MainWindow", u"Binarized", None))
        self.comboBox_preview.setItemText(2, QCoreApplication.translate("MainWindow", u"No background, sqrt", None))

        self.checkBox.setText(QCoreApplication.translate("MainWindow", u"Enable compression", None))
        self.checkBox_show_sky_coo.setText(QCoreApplication.translate("MainWindow", u"Show Sky coordinates", None))
        self.checkBox_show_motor_coo.setText(QCoreApplication.translate("MainWindow", u"Show motors coordinates", None))
        self.checkBox_show_all_detected.setText(QCoreApplication.translate("MainWindow", u"Show all detected stars", None))
        self.checkBox_show_sky_grid.setText(QCoreApplication.translate("MainWindow", u"Show Sky grid", None))
        self.checkBox_show_guiding_star.setText(QCoreApplication.translate("MainWindow", u"Show Guiding star", None))
        self.tabWidget_2.setTabText(self.tabWidget_2.indexOf(self.Mott), QCoreApplication.translate("MainWindow", u"Preview", None))
        self.label_5.setText(QCoreApplication.translate("MainWindow", u"Buck1 V", None))
        self.label_13.setText(QCoreApplication.translate("MainWindow", u"Buck2 V", None))
        self.label_18.setText(QCoreApplication.translate("MainWindow", u"Currents[A]", None))
        self.label_14.setText(QCoreApplication.translate("MainWindow", u"M1Coil1", None))
        self.label_31.setText("")
        self.label_15.setText(QCoreApplication.translate("MainWindow", u"M1Coil2", None))
        self.label_34.setText("")
        self.label_17.setText(QCoreApplication.translate("MainWindow", u"M2Coil1", None))
        self.label_32.setText("")
        self.label_16.setText(QCoreApplication.translate("MainWindow", u"M2Coil2", None))
        self.label_38.setText("")
        self.label_29.setText(QCoreApplication.translate("MainWindow", u"Resistance[Ohm]", None))
        self.label_24.setText(QCoreApplication.translate("MainWindow", u"M1C1", None))
        self.label_28.setText(QCoreApplication.translate("MainWindow", u"M1C2", None))
        self.label_22.setText(QCoreApplication.translate("MainWindow", u"M2C1", None))
        self.label_23.setText(QCoreApplication.translate("MainWindow", u"M2C2", None))
        self.tabWidget_2.setTabText(self.tabWidget_2.indexOf(self.tab_5), QCoreApplication.translate("MainWindow", u"Motors", None))
        self.pushButton_find_location.setText(QCoreApplication.translate("MainWindow", u"Find location", None))
        self.label_42.setText(QCoreApplication.translate("MainWindow", u"Sky patch info", None))
        self.label_40.setText(QCoreApplication.translate("MainWindow", u"Ra Location", None))
        self.lineEdit_sky_ra.setText(QCoreApplication.translate("MainWindow", u"0 0 0", None))
        self.label_41.setText(QCoreApplication.translate("MainWindow", u"Dec Location", None))
        self.lineEdit_sky_dec.setText(QCoreApplication.translate("MainWindow", u"0 0 0", None))
        self.label_46.setText(QCoreApplication.translate("MainWindow", u"Rotation", None))
        self.lineEdit_sky_rot.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.label_43.setText(QCoreApplication.translate("MainWindow", u"Telescope Info", None))
        self.label_44.setText(QCoreApplication.translate("MainWindow", u"Ra Location", None))
        self.lineEdit_7.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.label_45.setText(QCoreApplication.translate("MainWindow", u"Dec Location", None))
        self.lineEdit_9.setText(QCoreApplication.translate("MainWindow", u"0", None))
        self.label_47.setText(QCoreApplication.translate("MainWindow", u"Rotation", None))
        self.tabWidget_2.setTabText(self.tabWidget_2.indexOf(self.tab_4), QCoreApplication.translate("MainWindow", u"Ident", None))
        self.label_7.setText(QCoreApplication.translate("MainWindow", u"<html><head/><body><p><span style=\" font-weight:700;\">Battery</span></p><p><span style=\" font-weight:700;\">Voltage</span></p></body></html>", None))
        self.label_6.setText(QCoreApplication.translate("MainWindow", u"<html><head/><body><p><span style=\" font-weight:700;\">Battery</span></p><p><span style=\" font-weight:700;\">Current</span></p></body></html>", None))
        self.pushButton_4.setText(QCoreApplication.translate("MainWindow", u"Setup", None))
        self.label_37.setText(QCoreApplication.translate("MainWindow", u"Exposure", None))
        self.label_36.setText(QCoreApplication.translate("MainWindow", u"Interval[ms]", None))
        self.label_35.setText(QCoreApplication.translate("MainWindow", u"Gain", None))
        self.label_39.setText(QCoreApplication.translate("MainWindow", u"Captured Image", None))
        self.label_12.setText(QCoreApplication.translate("MainWindow", u"Title", None))
        self.label_10.setText(QCoreApplication.translate("MainWindow", u"Exposure", None))
        self.label_9.setText(QCoreApplication.translate("MainWindow", u"Capture time", None))
        self.label_11.setText(QCoreApplication.translate("MainWindow", u"Interval", None))
        self.label_33.setText(QCoreApplication.translate("MainWindow", u"Gain", None))
        self.label_48.setText(QCoreApplication.translate("MainWindow", u"Sky Ra", None))
        self.label_49.setText(QCoreApplication.translate("MainWindow", u"Sky Dec", None))
        self.label_30.setText(QCoreApplication.translate("MainWindow", u"Telescope Ra", None))
        self.label_27.setText(QCoreApplication.translate("MainWindow", u"Telescope Dec", None))
    # retranslateUi

