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

    def getDistance(self, star):
        return ((self.x_cent - star.x_cent) ** 2 + (self.y_cent - star.y_cent) ** 2) ** (1 / 2)

    def does_match(self, star):
        distance = self.getDistance(star)
        if distance < 10:
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
        self.camera_rotation = 45 / 180 * math.pi
        self.telescope_ra_vect = (math.cos(self.camera_rotation), math.sin(self.camera_rotation))
        self.telescope_dec_vect = (-math.sin(self.camera_rotation), math.cos(self.camera_rotation))
        self.sky_ra_vect = (1, 0)
        self.sky_dec_vect = (0, 1)
        self.telescope_dec_angle = 70
        self.go_to_loc = starCentroid(0, 960 / 2, 1280 / 2)
        self.setSetPoint(960 / 2, 1280 / 2)
        self.mode = "IDEN_TEL_RA"
        self.run_ra_ident = False
        self.run_ra_ident_iter = 0
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
        self.last_errx = None
        self.last_erry = None
        self.delta_errx = 0
        self.delta_erry = 0
        self.controls_ra = []
        self.controls_dec = []
        self.controller = Controller(0.6, 0, 0, np.array([[1, 0], [0, 1]]))
        self.guiding_star_dec = 0
        self.guiding_star_ra = 0
        self.telescope_ra_offset = 0
        self.telescope_dec_offset = 0

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
                self.delta_errx = self.delta_errx if abs(self.delta_errx) < 20 else 0
                self.delta_erry = self.delta_erry if abs(self.delta_erry) < 20 else 0

            self.last_errx = err[0]
            self.last_erry = err[1]
            self.logger.LogErrX(err[0])
            self.logger.LogErrY(err[1])
            speeds = self.setControl(err[0], err[1], auto_control)
            if self.log_info_en:
                self.log_info(speeds[0], speeds[1], time_interval)

    def runIdent(self):
        if self.reference_star_current is not None and self.reference_star_initial is not None:
            err = self.getErrorToRefStar(self.go_to_loc)
            if self.run_dec_ident:
                if self.run_dec_ident_iter == 0:
                    self.setDecSpeed(1000)
                elif self.run_dec_ident_iter == 1:
                    pass
                elif self.run_dec_ident_iter == 4:
                    try:
                        ident_dec = (self.last_error[0] - err[0], self.last_error[1] - err[1])
                        amp = math.sqrt(ident_dec[0] ** 2 + ident_dec[1] ** 2)
                        self.diff_amp = amp
                        self.telescope_dec_vect = (-ident_dec[0] / amp, -ident_dec[1] / amp)
                        # self.telescope_ra_vect = (-ident_dec[1] / amp, ident_dec[0] / amp)
                        self.run_dec_ident = False
                        self.setDecSpeed(0)
                    except:
                        print("faied identtification")
                        self.setDecSpeed(0)
                self.run_dec_ident_iter += 1
                self.last_error = err[:]
                return

            if self.run_ra_ident:
                err = self.getErrorToRefStar(self.go_to_loc)
                if self.run_ra_ident_iter == 0:
                    self.setRaSpeed(1000)
                elif self.run_ra_ident_iter == 1:
                    pass
                elif self.run_ra_ident_iter == 4:
                    ident_ra = (self.last_error[0] - err[0], self.last_error[1] - err[1])
                    amp = math.sqrt(ident_ra[0] ** 2 + ident_ra[1] ** 2)
                    cos_ident = amp / self.diff_amp
                    self.setRaSpeed(0)
                    if cos_ident < 1:
                        self.telescope_dec_angle = math.acos(cos_ident) * 180 / math.pi
                        print("identified  angle:", cos_ident)
                    self.telescope_ra_vect = (-ident_ra[0] / amp, -ident_ra[1] / amp)
                    self.run_ra_ident = False
                    self.run_ident = False
                    self.update_controller()

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

        # K_gain = 0.6
        # I_gain = 0.0000
        # Ra_ss_ctrl = -144
        # tot_x = error_x * K_gain + self.error_x_int * I_gain
        # tot_y = error_y * K_gain + self.error_y_int * I_gain

        # B = np.array([tot_x, tot_y])
        # sol = np.linalg.solve(A, B)
        # sol = sol / np.linalg.norm(sol) #* (tot_x ** 2 + tot_y ** 2) ** (1 / 2) * K_gain

        # dec_control = sol[0]
        # ra_control = sol[1]
        # if dec_control > 1500:
        #     dec_control = 1500
        # elif dec_control < -1500:
        #     dec_control = -1500

        # if ra_control > 1500:
        #     ra_control = 1500
        # elif ra_control < -1500:
        #     ra_control = -1500
        # print("ra ctrl:",-int(ra_control))
        # print("dec ctrl:",-int(dec_control))
        dec_control, ra_control = self.controller.FF_PID_controller.get_control(error_x, error_y)
        ra_control = - ra_control
        dec_control = - dec_control
        if enabled:
            self.setRaSpeed(ra_control)
            self.setDecSpeed(dec_control)
            self.controls_dec.append(dec_control)
            self.controls_ra.append(ra_control)
            if len(self.controls_ra) > 100:
                pass
                # print("dec avr: ", np.average(self.controls_dec[-100:]))
                # print("ra avr: ", np.average(self.controls_ra[-100:]))

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
            current_star_centroids.append(new_star)
        found_reference = False
        if not self.last_star_centroids:
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
        min_br = 0
        for star in centroids:
            if star.brightness > min_br:
                min_br = star.brightness
                self.reference_star_initial = star
                self.reference_star_current = star

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
        return {"ra": self.mqtt_handler.telescope_ra - self.telescope_ra_offset,
                "dec": self.mqtt_handler.telescope_dec - self.telescope_dec_offset}

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

        return {"ha":ha, "dec":dec}


if __name__ == "__main__":
    def setRaDummy(val: int):
        print(f"Setting Ra control to {val}")


    def setDecDummy(val: int):
        print(f"Setting Dec control to {val}")


    hler = TelescopeController(setRaDummy, setDecDummy)
    sol = controller.setControl(100, 0)
    print(sol)
