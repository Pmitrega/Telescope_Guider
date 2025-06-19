import numpy as np


class PID_Controller:
    def __init__(self, P: float, I: float, D: float, A_matr: np.ndarray):
        self.A = A_matr[:, :]
        self.P = 0.6
        self.I = 0
        self.D = 0
        self.integral_Deadband = 2  # 2"
        self.integral_x = 0
        self.integral_y = 0
        self.antiwindup = 20
        self.last_error_x = float("NaN")
        self.last_error_y = float("NaN")

    def set_gains(self, P: float, I: float, D: float):
        self.P = P
        self.I = I
        self.D = D

    def get_control(self, err_x, err_y):
        if self.last_error_x == float("NaN") or self.last_error_y == float("NaN"):
            tot_x = err_x * self.P + self.integral_x * self.I
            tot_y = err_y * self.P + self.integral_y * self.I
        else:
            tot_x = err_x * self.P + self.integral_x * self.I + (err_x - self.last_error_x) * self.D
            tot_y = err_y * self.P + self.integral_y * self.I + (err_y - self.last_error_y) * self.D

        B = np.array([tot_x, tot_y])
        sol = np.linalg.solve(self.A, B)

        self.last_error_x = err_x
        self.last_error_y = err_y
        if self.integral_x > self.integral_Deadband or self.integral_x < -self.integral_Deadband:
            self.integral_x = self.integral_x + err_x
        if self.integral_y > self.integral_Deadband or self.integral_y < -self.integral_Deadband:
            self.integral_y = self.integral_y + err_y

        if self.integral_x > self.antiwindup:
            self.integral_x = self.antiwindup
        elif self.integral_x < -self.antiwindup:
            self.integral_x = -self.antiwindup

        if self.integral_y > self.antiwindup:
            self.integral_y = self.antiwindup
        elif self.integral_y < -self.antiwindup:
            self.integral_y = -self.antiwindup

        return sol[0], sol[1]


class FF_PID_Controller:
    def __init__(self, P: float, I: float, D: float, A_matr: np.ndarray):
        self.A = A_matr[:, :]
        self.P = 0.6
        self.I = 0
        self.D = 0
        self.integral_Deadband = 2  # 2"
        self.integral_x = 0
        self.integral_y = 0
        self.antiwindup = 20
        self.last_error_x = float("NaN")
        self.last_error_y = float("NaN")
        self.Delta = 0
        self.gamma = 0
        self.dec_star = 0

    def get_declination_from_hr_angle(self, t: float):
        t = t / 180 * np.pi
        gamma = self.gamma / 180 * np.pi
        dec_star = self.dec_star / 180 * np.pi
        Delta = self.Delta / 180 * np.pi
        if (gamma < np.pi and (t > gamma and t < (gamma + np.pi))) or (
                gamma >= np.pi and (t > gamma or t < gamma - np.pi)):
            cos_dec_t = np.cos(dec_star) * np.cos(Delta) + np.sin(dec_star) * np.sin(Delta) * np.cos(t - gamma)
        else:
            cos_dec_t = np.cos(dec_star) * np.cos(Delta) + np.sin(dec_star) * np.sin(Delta) * np.cos(gamma - t)

        return np.acos(cos_dec_t) * 180 / np.pi

    def get_tel_hr_angle(self, t: float):
        exp_tel_dec = self.get_declination_from_hr_angle(t)
        print(self.dec_star)
        exp_tel_dec = exp_tel_dec / 180 * np.pi
        t = t / 180 * np.pi
        gamma = self.gamma / 180 * np.pi
        dec_star = self.dec_star / 180 * np.pi
        Delta = self.Delta / 180 * np.pi
        if np.sin(dec_star) * np.sin(exp_tel_dec) != 0:
            cos_phi_t = (np.cos(Delta) - np.cos(dec_star) * np.cos(exp_tel_dec)) / np.sin(dec_star) * np.sin(
                exp_tel_dec)
        else:
            cos_phi_t = 1
        phi = np.arccos(cos_phi_t)
        if (gamma < np.pi and (t > gamma and t < (gamma + np.pi))) or (
                gamma >= np.pi and (t > gamma or t < gamma - np.pi)):
            return (t + phi) * 180 / np.pi
        else:
            return (t - phi) * 180 / np.pi

    def get_dec_speed(self, t, Delta, dec_star, gamma):
        pass

    def calculate_open_loop_ctrl(self, time):
        pass

    def set_gains(self, P: float, I: float, D: float):
        self.P = P
        self.I = I
        self.D = D

    def get_control(self, err_x, err_y):
        if self.last_error_x == float("NaN") or self.last_error_y == float("NaN"):
            tot_x = err_x * self.P + self.integral_x * self.I
            tot_y = err_y * self.P + self.integral_y * self.I
        else:
            tot_x = err_x * self.P + self.integral_x * self.I + (err_x - self.last_error_x) * self.D
            tot_y = err_y * self.P + self.integral_y * self.I + (err_y - self.last_error_y) * self.D

        B = np.array([tot_x, tot_y])
        sol = np.linalg.solve(self.A, B)

        self.last_error_x = err_x
        self.last_error_y = err_y

        self.integral_x = self.integral_x + err_x
        self.integral_y = self.integral_y + err_y

        if self.integral_x > self.antiwindup:
            self.integral_x = self.antiwindup
        elif self.integral_x < -self.antiwindup:
            self.integral_x = -self.antiwindup

        if self.integral_y > self.antiwindup:
            self.integral_y = self.antiwindup
        elif self.integral_y < -self.antiwindup:
            self.integral_y = -self.antiwindup

        return sol[0], sol[1] + 144


class FF_PID_Adaptive_Avr_Controller:
    def __init__(self, P: float, I: float, D: float, A_matr: np.ndarray):
        self.A = A_matr[:, :]
        self.P = 2
        self.I = 0
        self.D = 0
        self.integral_Deadband = 2  # 2"
        self.integral_x = 0
        self.integral_y = 0
        self.antiwindup = 20
        self.last_error_x = float("NaN")
        self.last_error_y = float("NaN")

        self.error_range = 150
        self.last_error_in_range_exp = 20
        self.last_error_in_range_cnt = 0
        self.filter_ptr = 0
        self.filter_size = 120
        self.filter_array_dec = np.zeros(self.filter_size)
        self.filter_array_ra = np.zeros(self.filter_size)
        self.last_control_ra = 0
        self.last_control_dec = 0
        self.Delta = 0
        self.gamma = 0
        self.dec_star = 0

    def set_gains(self, P: float, I: float, D: float):
        self.P = P
        self.I = I
        self.D = D

    def get_control(self, err_x, err_y):
        if np.isnan(self.last_error_x) or np.isnan(self.last_error_y):
            tot_x = err_x * self.P + self.integral_x * self.I
            tot_y = err_y * self.P + self.integral_y * self.I
        else:
            tot_x = err_x * self.P + self.integral_x * self.I + (err_x - self.last_error_x) * self.D
            tot_y = err_y * self.P + self.integral_y * self.I + (err_y - self.last_error_y) * self.D

        B = np.array([tot_x, tot_y])
        sol = np.linalg.solve(self.A, B)
        if (err_y ** 2 + err_x ** 2) ** (1 / 2) < self.error_range:
            self.last_error_in_range_cnt = self.last_error_in_range_cnt + 1
            print("range is ok")
        else:
            self.last_error_in_range_cnt = 0
            self.filter_array_dec = np.zeros(self.filter_size)
            self.filter_array_ra = np.zeros(self.filter_size)

        if self.last_error_in_range_cnt > self.last_error_in_range_exp:
            self.filter_array_dec[self.filter_ptr % self.filter_size] = self.last_control_dec
            self.filter_array_ra[self.filter_ptr % self.filter_size] = self.last_control_ra
            self.filter_ptr += 1

        window = np.ones(self.filter_size) / self.filter_size  # Jednostajny filtr
        FF_ctrl_dec = np.convolve(self.filter_array_dec, window, mode='valid')
        FF_ctrl_ra = np.convolve(self.filter_array_ra, window, mode='valid')
        # print(self.filter_array_dec)
        # print(FF_ctrl_ra)
        # print(FF_ctrl_dec)
        self.last_error_x = err_x
        self.last_error_y = err_y

        self.integral_x = self.integral_x + err_x
        self.integral_y = self.integral_y + err_y

        if self.integral_x > self.antiwindup:
            self.integral_x = self.antiwindup
        elif self.integral_x < -self.antiwindup:
            self.integral_x = -self.antiwindup

        if self.integral_y > self.antiwindup:
            self.integral_y = self.antiwindup
        elif self.integral_y < -self.antiwindup:
            self.integral_y = -self.antiwindup
        self.last_control_dec = sol[0] + float(FF_ctrl_dec)
        self.last_control_ra = sol[1] + float(FF_ctrl_ra)
        return sol[0] + float(FF_ctrl_dec), sol[1] + float(FF_ctrl_ra)


class WCS_PID_Controller:
    def __init__(self, P: float, I: float, D: float, A_matr, WCS_matr, openloop=False):
        """
        :param P:
        :param I:
        :param D:
        :param A_matr - matrix for motor position - imager transformation A * v_tel = v_pix:
        :param WCS_matr - matrix for WCS - already scaled with declination of object and pixel scale:
        :param openloop -  if controller should only work on WCS values:

        A * v_tel = WCS^(-1) v_sky
        v_vel [v_ra_tel;
               v_dec_tel
         v_sky = [v_sky_ra - should be equal 15"/s;
                 [v_sky_dec
        """
        self.A_tel_matr = np.array(A_matr)

        self.B_sky_matr = np.linalg.inv(np.array(WCS_matr))
        print(np.array(WCS_matr))
        print(self.B_sky_matr)

    def get_control(self, err_x, err_y):
        # alpha = 1.5625 * 4  # seconds per step
        # beta = 0.245  # pixels per second
        alpha = 6.25
        beta = 0.245
        B_sky_matr = self.B_sky_matr
        A_tel_matr = self.A_tel_matr
        star_movement = B_sky_matr @ np.array([[15], [0]])
        sol = 1 / (alpha * beta) * np.linalg.inv(A_tel_matr) @ star_movement
        v_ra = sol[0][0] * 60  # steps/s -> steps/min
        v_dec = sol[1][0] * 60  # steps/s -> steps/min
        # print("ctrl ra/dec: ", sol[0][0], sol[1][0])
        return v_ra, v_dec  # sol[0][0] * 144 / 15 * sec_per_pix, sol[1][0] * 144 / 15 * sec_per_pix

    def update_matrices(self, A_matr, WCS_matr):
        self.A_tel_matr = np.array(A_matr)
        print('WCS matr:', WCS_matr)
        self.B_sky_matr = -np.array(WCS_matr)


class Controller:
    def __init__(self, P: float, I: float, D: float, A_matr: np.ndarray):
        self.FF_PID_Adaptive_Avr_controller = FF_PID_Adaptive_Avr_Controller(P, I, D, A_matr)
        self.PID_controller = PID_Controller(P, I, D, A_matr)
        self.FF_PID_controller = FF_PID_Controller(P, I, D, A_matr)
        self.WCS_PID_controller = WCS_PID_Controller(P, I, D, A_matr, [[1, 0], [0, 1]])

    def updateControllers(self, P: float, I: float, D: float, A_matr: np.ndarray):
        self.PID_controller = PID_Controller(P, I, D, A_matr)
        self.FF_PID_controller = FF_PID_Controller(P, I, D, A_matr)
        self.FF_PID_Adaptive_Avr_controller = FF_PID_Adaptive_Avr_Controller(P, I, D, A_matr)

    def setPIDGains(self, P: float, I: float, D: float):
        self.PID_controller.P = P
        self.PID_controller.I = I
        self.PID_controller.D = D

        self.FF_PID_Adaptive_Avr_controller.P = P
        self.FF_PID_Adaptive_Avr_controller.I = I
        self.FF_PID_Adaptive_Avr_controller.D = D

        self.WCS_PID_controller.P = P
        self.WCS_PID_controller.I = I
        self.WCS_PID_controller.D = D
