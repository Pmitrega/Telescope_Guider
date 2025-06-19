import math
from typing import Callable
import cv2
import numpy as np
import src.star_detection as star_detection
import csv
from src.logger import Logger
from ui_form import Ui_MainWindow
from src.control_algorithm import Controller
import astropy
from astropy.utils import iers
import warnings
from datetime import datetime, timezone


# Use built-in IERS-B data, no downloading
iers.conf.auto_download = False
iers.conf.auto_max_age = None  # avoids warnings about data being "too old"

# Suppress related warnings
warnings.simplefilter('ignore', category=iers.IERSWarning)

STEPS_TO_SECS = 0.104166666666667  # Steps to seconds for both motors.


def TelescopeToSkyTransform(tel_dec, angle_diff, c_sky_dec, c_sky_ra, e_sky_dec, e_sky_ra):
    if tel_dec == c_sky_dec and angle_diff == 0:
        return [e_sky_ra - c_sky_ra, e_sky_dec - c_sky_dec]
    else:
        return [0, 0]


def stepsToSecs(steps: float) -> float:
    return steps * STEPS_TO_SECS


def deg2dms(degrees_float):
    degrees = int(degrees_float)
    minutes_float = abs(degrees_float - degrees) * 60
    minutes = int(minutes_float)
    seconds = round((minutes_float - minutes) * 60, 2)

    # Handle negative values for proper formatting
    sign = '-' if degrees_float < 0 else ''

    return f"{sign}{abs(degrees)}° {minutes}' {seconds}\""


def deg2hms(degrees_float):
    total_hours = degrees_float / 15.0
    hours = int(total_hours)
    minutes_float = abs(total_hours - hours) * 60
    minutes = int(minutes_float)
    seconds = round((minutes_float - minutes) * 60, 2)

    # Handle negative values for proper formatting
    sign = '-' if degrees_float < 0 else ''

    return f"{sign}{abs(hours)}h {minutes}m {seconds}s"

class starCentroid:
    def __init__(self, brightness, x_cent, y_cent):
        self.brightness = brightness
        self.x_cent = x_cent
        self.y_cent = y_cent
        self.sat_pix = 0

    def getDistance(self, star):
        return ((self.x_cent - star.x_cent) ** 2 + (self.y_cent - star.y_cent) ** 2) ** (1 / 2)

    def does_match(self, star):
        distance = self.getDistance(star)
        if distance < 30:
            return True
        else:
            return False

    def __eq__(self, other):
        if self.x_cent == other.x_cent and self.y_cent == other.y_cent:
            return True

    def __repr__(self):
        return "StarCentroid()"

    def __str__(self):
        return f"br: {self.brightness}, x_cent: {round(self.x_cent, 2)}, y_cent: {round(self.y_cent, 2)}"


class TelescopeController:
    def __init__(self, set_ra_speed_func: Callable[[int], None], set_dec_speed_func: Callable[[int], None],
                 logger: Logger, mqtt_handler, ui: Ui_MainWindow):
        self.logger = logger
        self.image_buffer = None
        self.new_image = False
        self.ui = ui
        self.mqtt_handler = mqtt_handler
        self.setRaSpeed = set_ra_speed_func
        self.setDecSpeed = set_dec_speed_func
        self.last_star_centroids = []
        self.reference_star_initial = None
        self.reference_star_current = None
        self.time_interval = -1
        self.sec_per_pixel = 4.1
        self.camera_rotation = 0 / 180 * math.pi
        self.telescope_ra_vect = (math.cos(self.camera_rotation), math.sin(self.camera_rotation))
        self.telescope_dec_vect = (-math.sin(self.camera_rotation), math.cos(self.camera_rotation))
        self.sky_ra_vect = (1, 0)
        self.sky_dec_vect = (0, 1)
        self.gd_star_wrld2pix_matr = [[1,0],[0,1]]
        self.sky_rot_angle = np.atan2(self.sky_ra_vect[0]/np.sqrt(self.sky_ra_vect[0]**2 + self.sky_ra_vect[1]**2), self.sky_ra_vect[1]/np.sqrt(self.sky_ra_vect[0]**2 + self.sky_ra_vect[1]**2)) * 180 / np.pi
        self.motor_rot_angle = 0
        self.telescope_dec_angle = 70
        self.go_to_loc = starCentroid(0, 960 / 2, 1280 / 2)
        self.setSetPoint(960 / 2, 1280 / 2)
        self.mode = "IDEN_TEL_RA"
        self.run_ra_ident = False
        self.run_ra_ident_iter = 0
        self.find_new_ref = False
        self.run_dec_ident = False
        self.run_dec_ident_iter = 0
        self.last_error = (0, 0)
        self.run_ident = False
        self.dec_deg = float('nan')
        self.ra_deg = float('nan')
        self.log_info_en = False
        self.adaptive_thre = -15
        self.curr_ctrl = [0, 0]
        self.error_x_int = 0
        self.error_y_int = 0
        self.Dec_Delta = 0
        self.last_errx = None
        self.last_erry = None
        self.delta_errx = 0
        self.delta_erry = 0
        self.controls_ra = []
        self.controls_dec = []
        self.controller = Controller(0.6, 0, 0, np.array([[1, 0], [0, 1]]))
        self.guiding_star_dec = 0
        self.guiding_star_ra = 0
        self.gd_star_loc_init = 0
        self.telescope_ra_offset = 0
        self.telescope_dec_offset = 0
        self.ra_ident_points_x = []
        self.ra_ident_points_y = []
        self.dec_ident_points_x = []
        self.dec_ident_points_y = []
        self.ra_ident_beg_point = (0,0)
        self.dec_ident_beg_point = (0,0)

    def log_info(self, dec_speed, ra_speed, iterval):
        x = self.reference_star_current.x_cent
        y = self.reference_star_current.y_cent
        with open('ctrl_logs.csv', 'a', newline='') as csvfile:
            writer = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
            writer.writerow([x, y, stepsToSecs(dec_speed), stepsToSecs(ra_speed), iterval])

    def genNewImage(self, image: np.ndarray, time_interval: float, auto_control: bool):
        self.new_image = True
        self.image_buffer = image[:, :]
        self.time_interval = time_interval
        self.findStars()
        if self.run_ident:
            self.runIdent()
        if self.reference_star_current is not None:
            self.mqtt_handler.ref_star_plot = self.reference_star_current
            err = self.getErrorToRefStar(self.go_to_loc)
            if self.last_errx is not None and self.last_erry is not None:
                self.delta_errx = (err[0] - self.last_errx)
                self.delta_erry = (err[1] - self.last_erry)
                self.delta_errx = self.delta_errx if abs(self.delta_errx) < 80 and abs(self.delta_erry) < 80 else 0
                self.delta_erry = self.delta_erry if abs(self.delta_errx) < 80 and abs(self.delta_erry) < 80 else 0

            self.last_errx = err[0]
            self.last_erry = err[1]
            self.logger.LogErrX(err[0])
            self.logger.LogErrY(err[1])
            speeds = self.setControl(err[0], err[1], auto_control)
            if self.log_info_en:
                self.log_info(speeds[0], speeds[1], time_interval)

    def runIdent(self):
        ra_ident_count = 11
        dec_ident_count = 11
        elements_excluded = 4
        if self.reference_star_current is not None and self.reference_star_initial is not None:
            err = self.getErrorToRefStar(self.go_to_loc)
            if self.run_dec_ident:
                if self.run_dec_ident_iter == 0:
                    self.setDecSpeed(500)
                    self.setRaSpeed(0)
                    self.dec_ident_points_x = []
                    self.dec_ident_points_y = []
                    self.dec_ident_beg_point = (err[0], err[1])
                elif self.run_dec_ident_iter < dec_ident_count:
                    self.dec_ident_points_x.append(self.dec_ident_beg_point[0] - err[0])
                    self.dec_ident_points_y.append(self.dec_ident_beg_point[1] - err[1])
                elif self.run_dec_ident_iter == dec_ident_count:
                    try:
                        coeff_x = np.polyfit(np.arange(1, len(self.dec_ident_points_x[elements_excluded:]) + 1), self.dec_ident_points_x[elements_excluded:], 1)
                        coeff_y = np.polyfit(np.arange(1, len(self.dec_ident_points_y[elements_excluded:]) + 1), self.dec_ident_points_y[elements_excluded:], 1)
                        ident_dec = (coeff_x[0], coeff_y[0])
                        print("dec_y: ", self.dec_ident_points_y)
                        print("dec_x: ", self.dec_ident_points_x)
                        #ident_dec = (self.last_error[0] - err[0], self.last_error[1] - err[1])
                        amp = math.sqrt(ident_dec[0] ** 2 + ident_dec[1] ** 2)
                        self.diff_amp = amp
                        self.telescope_dec_vect = (-ident_dec[0] / amp, -ident_dec[1] / amp)
                        # self.telescope_ra_vect = (-ident_dec[1] / amp, ident_dec[0] / amp)
                        self.run_dec_ident = False
                        self.setDecSpeed(0)
                        self.setRaSpeed(0)
                        self.motor_rot_angle = 270 - np.atan2(-ident_dec[0]/amp, ident_dec[1]/amp) * 180/np.pi

                    except:
                        print("faied identtification")
                        self.setDecSpeed(0)
                        self.setRaSpeed(0)
                self.run_dec_ident_iter += 1
                self.last_error = err[:]

                return

            if self.run_ra_ident:
                err = self.getErrorToRefStar(self.go_to_loc)
                if self.run_ra_ident_iter == 0:
                    self.setRaSpeed(500)
                    self.setDecSpeed(0)
                    self.ra_ident_points_x = []
                    self.ra_ident_points_y = []
                    self.ra_ident_beg_point = (err[0], err[1])
                elif self.run_ra_ident_iter  < ra_ident_count:
                    self.ra_ident_points_x.append(self.ra_ident_beg_point[0] - err[0])
                    self.ra_ident_points_y.append(self.ra_ident_beg_point[1] - err[1])
                elif self.run_ra_ident_iter == ra_ident_count:
                    coeff_x = np.polyfit(np.arange(1, len(self.ra_ident_points_x[elements_excluded:]) + 1),
                                         self.ra_ident_points_x[elements_excluded:], 1)
                    coeff_y = np.polyfit(np.arange(1, len(self.ra_ident_points_y[elements_excluded:]) + 1),
                                         self.ra_ident_points_y[elements_excluded:], 1)
                    ident_ra = (coeff_x[0], coeff_y[0])
                    print("ra_y: ", self.ra_ident_points_y)
                    print("ra_x: ", self.ra_ident_points_x)
                    # print("ident_ra_now: ", ident_ra)
                    # ident_ra = (self.last_error[0] - err[0], self.last_error[1] - err[1])
                    # print("ident_ra_last: ", ident_ra)

                    amp = math.sqrt(ident_ra[0] ** 2 + ident_ra[1] ** 2)
                    # print(amp)
                    # print(self.diff_amp)
                    cos_ident = amp / self.diff_amp
                    self.setDecSpeed(0)
                    self.setRaSpeed(0)
                    if cos_ident < 1:
                        self.ui.lineEdit_curr_dec.setText(str(math.acos(cos_ident) * 180 / math.pi))
                        self.dec_deg = math.acos(cos_ident) * 180 / math.pi
                        self.telescope_dec_angle = math.acos(cos_ident) * 180 / math.pi
                        print("identified  angle:", cos_ident)

                        phi = self.sky_rot_angle - self.motor_rot_angle
                        self.identify_from_dec_phi(self.telescope_dec_angle, self.guiding_star_dec, phi)
                        gd_star = self.getGuidingStarDecHr()
                        self.gd_star_loc_init = gd_star["hr"]
                        Phi = (self.sky_rot_angle - self.motor_rot_angle) * np.pi / 180
                        dec_t0 = self.dec_deg * np.pi / 180
                        dec_s = gd_star["dec"] * np.pi / 180
                        Delta = np.acos(np.cos(dec_t0) * np.cos(dec_s) + np.sin(dec_t0) * np.sin(dec_s) * np.cos(Phi))

                        licz = np.cos(dec_t0) - np.cos(dec_s) * np.cos(Delta)
                        mian = np.sin(dec_s) * np.sin(Delta)
                        if (mian != 0) and licz / mian < 1:
                            C = np.acos(licz / mian) * 180 / np.pi
                            if Phi > 0:
                                gamma = gd_star["hr"] - C
                            else:
                                gamma = gd_star["hr"] + C
                            print("Setting gamma and Delta to:", gamma, Delta * 180 / np.pi )
                            init_x, init_y = self.mqtt_handler.wcs.word_to_pixel(gd_star["hr"], gd_star["dec"])
                            ra_shift_x, ra_shift_y = self.mqtt_handler.wcs.word_to_pixel(gd_star["hr"] + 1/3600, gd_star["dec"])
                            dec_shift_x, dec_shift_y = self.mqtt_handler.wcs.word_to_pixel(gd_star["hr"], gd_star["dec"] + 1/3600)
                            self.gd_star_wrld2pix_matr = [[init_x - ra_shift_x, init_x - dec_shift_x],
                                                          [init_y - ra_shift_y, init_y - dec_shift_y]]
                            self.update_controller()
                            self.controller.FF_PID_controller.gamma = gamma
                            self.controller.FF_PID_controller.Delta = Delta * 180 / np.pi
                            self.controller.FF_PID_controller.dec_star = gd_star["dec"]
                    self.telescope_ra_vect = (-ident_ra[0] / amp, -ident_ra[1] / amp)
                    self.run_ra_ident = False
                    self.run_ident = False
                    #

                self.run_ra_ident_iter += 1
                self.last_error = err[:]
                return

    def update_controller(self):
        Pg = math.cos(self.telescope_dec_angle / 180 * math.pi)
        dec_x = self.telescope_dec_vect[0]
        dec_y = self.telescope_dec_vect[1]
        ra_x = self.telescope_ra_vect[0]
        ra_y = self.telescope_ra_vect[1]
        A = np.array([[dec_x, ra_x * Pg],
                      [dec_y, ra_y * Pg]])
        self.controller = Controller(0.6, 0.0, 0.0, A)
        self.controller.WCS_PID_controller.update_matrices(A_matr=A, WCS_matr=self.gd_star_wrld2pix_matr)

    def setControl(self, error_x, error_y, enabled: bool):
        self.error_x_int += error_x
        self.error_y_int += error_y
        if self.error_y_int > 250:
            self.error_y_int = 250
        elif self.error_y_int < -250:
            self.error_y_int = -250

        if self.error_x_int > 250:
            self.error_x_int = 250
        elif self.error_x_int < -250:
            self.error_x_int = -250

        self.controller.setPIDGains(self.ui.doubleSpinBox_P_gain.value(), self.ui.doubleSpinBox_I_gain.value(), self.ui.doubleSpinBox_D_gain.value())
        if self.ui.comboBox_controller.currentText() == "PID":
            dec_control, ra_control = self.controller.PID_controller.get_control(error_x, error_y)
        elif self.ui.comboBox_controller.currentText() == "Adaptive_PID":
            dec_control, ra_control = self.controller.FF_PID_Adaptive_Avr_controller.get_control(error_x, error_y)
        elif self.ui.comboBox_controller.currentText() == "WCS_FF_PID":
            dec_control, ra_control = self.controller.WCS_PID_controller.get_control(error_x, error_y)

        if dec_control > 1000:
            dec_control = 1000
        elif dec_control < -1000:
            dec_control = -1000

        if ra_control > 1000:
            ra_control = 1000
        elif ra_control < -1000:
            ra_control = -1000

        # print(dec_control)
        # print(ra_control)
        ra_control = - ra_control
        dec_control = - dec_control
        if enabled:
            self.setRaSpeed(ra_control)
            self.setDecSpeed(dec_control)
            self.controls_dec.append(dec_control)
            self.controls_ra.append(ra_control)

        self.curr_ctrl = [ra_control, dec_control]
        return [ra_control, dec_control]

    def getErrorToRefStar(self, star: starCentroid):
        return (round(float((self.reference_star_current.x_cent - star.x_cent) * self.sec_per_pixel), 2),
                round(float((self.reference_star_current.y_cent - star.y_cent) * self.sec_per_pixel), 2))

    def getErrorToRefStarPix(self, star: starCentroid):
        return (round(float((self.reference_star_current.x_cent - star.x_cent)), 2),
                round(float((self.reference_star_current.y_cent - star.y_cent)), 2))

    def findStars(self):
        if self.image_buffer is None:
            return []
        I2Bin = self.image_buffer / 2 ** 8
        I2Bin = I2Bin.astype(np.uint8)
        self.adaptive_thre = int(self.ui.horizontalSlider_sens.value())
        if I2Bin is None or I2Bin.size == 0:
            raise ValueError("Input image is empty!")
        bin_im, star_centroids = star_detection.segmentation(I2Bin.astype(np.uint8), self.adaptive_thre,
                                                             self.ui.spinBox_sigma.value())
        # print(len(star_centroids))
        self.ui.lineEdit_detected.setText(str(len(star_centroids)))
        # ret, I_mat = cv2.threshold(I_mat, min_I * 1.4, 65535, cv2.THRESH_BINARY)
        if len(star_centroids) > 150:
            return
        # cv2.imwrite("../binaraized.png", bin_im)
        # output = cv2.connectedComponentsWithStats(bin_im.astype(np.uint8))
        current_star_centroids = []
        for i in range(len(star_centroids)):
            new_star = starCentroid(star_centroids[i][0], star_centroids[i][1], star_centroids[i][2])
            new_star.sat_pix = star_centroids[i][4]
            current_star_centroids.append(new_star)
        found_reference = False
        if not self.last_star_centroids or self.find_new_ref is True:
            self.find_new_ref = False
            self.last_star_centroids = current_star_centroids[:]
            self.find_new_reference_star(current_star_centroids)
            print("initial reference star:", self.reference_star_initial)
        else:
            for star_last in self.last_star_centroids:
                for star_curr in current_star_centroids:
                    if star_last.does_match(star_curr) and star_last == self.reference_star_current:
                        self.reference_star_current = star_curr
                        found_reference = True
                        # print("reference current:", self.reference_star_current)
                        break
        self.last_star_centroids = current_star_centroids

        if found_reference is False:
            self.find_new_reference_star(self.last_star_centroids)
        if self.reference_star_current is not None and self.reference_star_initial is not None:
            if self.reference_star_current is not None:
                error = self.getErrorToRefStarPix(self.go_to_loc)
                # print(error)
            # f = open("error.csv", "a")
            # f.write(f"{error[0]},{error[1]}\n")
            # f.close()

    def find_new_reference_star(self, centroids):
        x_center = 960 / 2
        y_center = 1280 / 2

        best_weight = -math.inf
        best_centroid = starCentroid(-1, -1, -1)

        for star in centroids:
            if star.brightness > 2:
                distance_sq = (star.x_cent - x_center) ** 2 + (star.y_cent - y_center) ** 2
                weight = star.brightness * 100 - 1/2 * np.sqrt(distance_sq) - star.sat_pix * 250
                # print("BRIGH_W = ", star.brightness * 100 )
                # print("DIS_W = ", np.sqrt(distance_sq))
                # print("SAT_W = ", star.sat_pix * 250)
                # print("TOTAL = ", weight)

                if weight > best_weight:
                    best_weight = weight
                    best_centroid = star

        self.reference_star_initial = best_centroid
        self.reference_star_current = best_centroid

    def getTrackedStar(self) -> starCentroid:
        # return starCentroid(500,500,500)
        return self.reference_star_current

    def setSetPoint(self, sp_x, sp_y):
        self.go_to_loc = starCentroid(0, sp_x, sp_y)
        self.logger.LogSetPointX(sp_x)
        self.logger.LogSetPointY(sp_y)

    def setTelescopePos(self, ra_pos, dec_pos):
        self.telescope_ra_offset = self.mqtt_handler.telescope_ra - ra_pos
        self.telescope_dec_offset = self.mqtt_handler.telescope_dec - dec_pos

    def getTelescopePos(self):
        return {"hr": self.mqtt_handler.telescope_ra - self.telescope_ra_offset,
                "dec": self.mqtt_handler.telescope_dec - self.telescope_dec_offset}

    def identify_from_dec_phi(self, dec, phi, dec_star):
        self.ui.lineEdit_curr_dec.setText(str(dec))
        self.ui.lineEdit_curr_dec_delta.setText(str(np.arccos(np.cos(dec) * np.cos(dec_star) + np.sin(dec) * np.sin(dec_star) * np.cos(phi)) * 180 / np.pi))
        self.ui.lineEdit_curr_hr.setText(str(phi))


    def getGuidingStarDecHr(self):
        if self.reference_star_current is not None:
            ra, dec = self.mqtt_handler.wcs.pixel_to_world(self.reference_star_current.y_cent, self.reference_star_current.x_cent)
            utc_now = datetime.now(timezone.utc)
            now = astropy.time.Time(utc_now)
            gst = now.sidereal_time('mean', longitude=0)
            ha = gst.deg - ra
            if(ha < 0):
                ha = ha + 360
        else:
            ha = float("NaN")
            dec = float("NaN")

        return {"hr":ha, "dec":dec}


if __name__ == "__main__":
    def setRaDummy(val: int):
        print(f"Setting Ra control to {val}")


    def setDecDummy(val: int):
        print(f"Setting Dec control to {val}")


    hler = TelescopeController(setRaDummy, setDecDummy)
    sol = controller.setControl(100, 0)
    print(sol)
