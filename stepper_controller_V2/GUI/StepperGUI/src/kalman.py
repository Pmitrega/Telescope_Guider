import numpy as np
from numpy import sin, cos
import numpy.linalg as linalg


class EKF:
    def __init__(self):
        self.model_func = None
        self.meassure_func = None
        self.P_cov_matrix = np.ndarray([])
        self.Q_prcs_noise = np.ndarray([])
        self.R_meass_noise = np.ndarray([])
        self.state = np.ndarray([])
        self.state_apriori = np.ndarray([])
        self.state_aprosteriori = np.ndarray([])
        self.F_jacobian = None
        self.H_jacobian = None

    def predict(self, dt: float, u: np.ndarray):
        self.state_apriori = self.model_func(self.state, dt, u)
        F = self.F_jacobian(self.state_apriori, dt, u)
        self.P_cov_matrix = F @ self.P_cov_matrix @ np.transpose(F) + self.Q_prcs_noise

    def update(self, z: np.ndarray):
        y_pred = z - self.meassure_func(self.state_apriori)
        H = self.H_jacobian(self.state_apriori)
        Sk = H @ self.P_cov_matrix @ np.transpose(H) + self.R_meass_noise
        Kk = self.P_cov_matrix @ np.transpose(H) @ linalg.inv(Sk)
        self.state_aprosteriori = self.state_apriori + Kk @ y_pred
        w = self.state.shape
        self.P_cov_matrix = (np.eye(self.state.shape[0], self.state.shape[0]) - Kk @ H) @ self.P_cov_matrix
        self.state = self.state_aprosteriori

    def getState(self):
        return self.state


class guiderEKF():
    def __init__(self, initial_state=np.array([500, 500, 0, 0, 0, 0, 0, 5.00]).transpose()):
        self.EKF = EKF()
        self.EKF.model_func = self.model_func
        self.EKF.meassure_func = self.meassure_func
        self.EKF.F_jacobian = self.Modeljacobian
        self.EKF.H_jacobian = self.Meassurejacobian
        self.EKF.state = initial_state
        self.EKF.Q_prcs_noise = np.diag([0, 0, 0, 0, 0, 0, 0, 0])
        self.EKF.R_meass_noise = np.diag([1, 1])
        self.EKF.P_cov_matrix = np.diag([1, 1, 1e-3, 1e-3, 1e-3, 1e-3, 1e-3, 1e-2])
        self.isDecAvailable = False
        self.star_dec = 0

    def setDeclination(self, dec_deg):
        self.isDecAvailable = True
        self.star_dec = dec_deg/180*np.pi
    def predict(self, dt: float, v_dec: float, v_ra: float):
        u = np.array([v_dec, v_ra])
        self.EKF.predict(dt, u)

    def update(self, x, y):
        meass = np.array([x, y]).transpose()
        self.EKF.update(meass)

    def getState(self):
        state = self.EKF.getState()
        t_state = {
            "x": state[0],
            "y": state[1],
            "Telescope_Dec": state[2],
            "Telescope_Ra": state[3],
            "Sky_Rot": state[4],
            "Telescope_Rot": state[5],
            "Star_Dec": state[6],
            "pix_size": state[7]
        }
        return t_state

    def model_func(self, state: np.ndarray, dt: float, u: np.ndarray) -> np.ndarray:
        kappa = 15  # "/s
        x1 = state[0]
        x2 = state[1]
        x3 = state[2]
        x4 = state[3]
        x5 = state[4]
        x6 = state[5]
        x7 = state[6]
        x8 = state[7]
        v_dec = u[0]
        v_ra = u[1]
        dx1 = 1 / x8 * (sin(x6) * v_dec + cos(x3) * cos(x6) * v_ra + cos(x5) * cos(x7) * kappa) * dt
        dx2 = 1 / x8 * (sin(x6) * v_dec + cos(x3) * cos(x6) * v_ra + cos(x5) * cos(x7) * kappa) * dt
        x1_next = x1 + 1 / x8 * (cos(x6) * v_dec - cos(x3) * sin(x6) * v_ra - sin(x5) * cos(x7) * kappa) * dt
        x2_next = x2 + 1 / x8 * (sin(x6) * v_dec + cos(x3) * cos(x6) * v_ra + cos(x5) * cos(x7) * kappa) * dt
        x3_next = x3 + v_dec * dt / 3600 / 180 * np.pi
        x4_next = x4 + v_ra * dt / 3600 / 180 * np.pi
        x5_next = x5 + (kappa - v_ra) * dt / 3600 / 180 * np.pi
        x6_next = x6
        if self.isDecAvailable:
            x7_next = self.star_dec
        else:
            x7_next = x7
        x8_next = x8
        return np.array([x1_next, x2_next, x3_next, x4_next, x5_next, x6_next, x7_next, x8_next])

    def meassure_func(self, state: np.ndarray) -> np.ndarray:
        return np.array([state[0], state[1]])

    def Modeljacobian(self, state: np.ndarray, dt: float, u: np.ndarray) -> np.ndarray:
        x1 = state[0]
        x2 = state[1]
        x3 = state[2]
        x4 = state[3]
        x5 = state[4]
        x6 = state[5]
        x7 = state[6]
        x8 = state[7]
        v_dec = u[0]
        v_ra = u[1]
        kappa = 15  # "/s
        jac = np.array(
            [
                [1, 0, (v_ra * sin(x3) * sin(x6)) / x8, 0, -(kappa * cos(x5) * cos(x7)) / x8,
                 -(v_dec * sin(x6) + v_ra * cos(x3) * cos(x6)) / x8, (kappa * sin(x5) * sin(x7)) / x8,
                 (kappa * cos(x7) * sin(x5) - v_dec * cos(x6) + v_ra * cos(x3) * sin(x6)) / x8 ** 2],
                [0, 1, -(v_ra * cos(x6) * sin(x3)) / x8, 0, -(kappa * cos(x7) * sin(x5)) / x8,
                 (v_dec * cos(x6) - v_ra * cos(x3) * sin(x6)) / x8, -(kappa * cos(x5) * sin(x7)) / x8,
                 -(v_dec * sin(x6) + kappa * cos(x5) * cos(x7) + v_ra * cos(x3) * cos(x6)) / x8 ** 2],
                [0, 0, 1, 0, 0, 0, 0, 0],
                [0, 0, 0, 1, 0, 0, 0, 0],
                [0, 0, 0, 0, 1, 0, 0, 0],
                [0, 0, 0, 0, 0, 1, 0, 0],
                [0, 0, 0, 0, 0, 0, 1, 0],
                [0, 0, 0, 0, 0, 0, 0, 1]
            ]
        )
        return (jac - np.eye(8, 8)) * dt + np.eye(8, 8)

    def Meassurejacobian(self, state: np.ndarray) -> np.ndarray:
        jac = np.array(
            [
                [1, 0, 0, 0, 0, 0, 0, 0],
                [0, 1, 0, 0, 0, 0, 0, 0]
            ]
        )
        return jac


def calculate_control(err_x, err_y, rot_cam, rot_star, tel_dec):
    K = -0.5
    ra_vect = (cos(rot_cam), sin(rot_cam))
    dec_vect = (-sin(rot_cam), cos(rot_cam))
    # print(ra_vect)
    # print(dec_vect)
    Pg = cos(tel_dec)
    A = np.array([[dec_vect[0], ra_vect[0] * Pg],
                  [dec_vect[1], ra_vect[1] * Pg]])

    B = np.array([err_x, err_y])

    sol = np.linalg.solve(A, B)
    # print(sol)
    if (err_x ** 2 + err_y ** 2) ** (1 / 2) > 1:
        sol = sol / np.linalg.norm(sol) * (err_x ** 2 + err_y ** 2) ** (1 / 2) *K
        dec_control = sol[0]
        ra_control = sol[1]
        if dec_control > 200:
            dec_control = 200
        elif dec_control < -200:
            dec_control = -200
        if ra_control > 200:
            ra_control = 200
        elif ra_control < -200:
            ra_control = -200
    else:
        dec_control = 0
        ra_control = 0
    return [ra_control, dec_control]

'''
            "x": state[0],
            "y": state[1],
            "Telescope_Dec": state[2],
            "Telescope_Ra": state[3],
            "Sky_Rot": state[4],
            "Telescope_Rot": state[5],
            "Star_Dec": state[6],
            "pix_size": state[7]
'''
def control_loop():
    guid_ekf = guiderEKF()
    guid_ekf.setDeclination(35)
    state_true = np.array([490, 500, 35/180*np.pi, 0.0, 0, 0, 0, 8.42])
    sim_time = 20000
    sampling_time = 0.5
    center_point = [510, 501]
    iterations = int(sim_time / sampling_time)
    x_all = []
    y_all = []
    x_pred = []
    y_pred = []
    pix_size = []
    controls = []
    tel_rotation = []
    tel_decs = []
    star_decs = []
    controls_ss = []
    for i in range(iterations):
        # controls.append([0, 0])
        controls.append([0, -15])
        #controls.append([32*sin(i/100), 15*sin(i/100)])
    for i in range(iterations):
        if i == 14200:
            stop = 1
        m_state = guid_ekf.getState()
        err_x = (m_state["x"] - center_point[0])
        err_y = (m_state["y"] - center_point[1])
        control = calculate_control(err_x, err_y, m_state["Telescope_Rot"], m_state["Sky_Rot"], m_state["Telescope_Dec"])
        controls_ss.append(control)
        control = controls[i]
        guid_ekf.predict(sampling_time, control[0], control[1])
        state_true = guid_ekf.model_func(state_true, sampling_time, control)
        output = guid_ekf.meassure_func(state_true)
        guid_ekf.update(output[0], output[1])
        tel_rotation.append(m_state["Telescope_Rot"])
        x_pred.append(m_state["x"])
        y_pred.append(m_state["y"])
        x_all.append(output[0])
        y_all.append(output[1])
        pix_size.append(m_state["pix_size"])
        tel_decs.append(m_state["Telescope_Dec"])
        tel_dec_cos = np.cos((m_state["Telescope_Dec"]/180*np.pi))
        star_decs.append(np.sqrt((control[0])**2 + (control[1] * tel_dec_cos )**2)/15)
    plt.plot(x_all, y_all)
    plt.plot(x_pred, y_pred)
    plt.figure()
    plt.plot(pix_size)
    plt.title("pixel size")
    plt.figure()
    plt.plot(tel_rotation)
    plt.title("telescope rotation")
    plt.figure()
    plt.plot(controls_ss)
    plt.title("contorls")
    plt.figure()
    plt.plot(tel_decs)
    plt.title("Telescope Declination")
    plt.figure()

    plt.plot(star_decs)
    plt.title("Star Declination")
    plt.show()

if __name__ == "__main__":
    import matplotlib.pyplot as plt

    control_loop()
    # guid_ekf = guiderEKF()
    # state_true = np.array([500, 500, 0.1, 0.1, 0.1, 0.1, 0.1, 4])
    # controls = [] #[[15, 10]] * 1000
    # iterations = 1000
    # t_samp = 1
    # for i in range(iterations):
    #     #controls.append([15, 0])
    #     controls.append([32*sin(i/100), 15*sin(i/100)])
    # print(controls)
    # outputs = []
    # for i in range(iterations):
    #     state_true = guid_ekf.model_func(state_true,1, controls[i])
    #     outputs.append(guid_ekf.meassure_func(state_true))
    # pix_size = []
    # x = []
    # y = []
    # for i in range(iterations):
    #     guid_ekf.predict(t_samp, controls[i][0],controls[i][1])
    #     guid_ekf.update(outputs[i][0], outputs[i][1])
    #     x.append(guid_ekf.getState()["x"])
    #     y.append(guid_ekf.getState()["y"])
    #     pix_size.append(guid_ekf.getState()["pix_size"])
    #
    # plt.figure(1)
    # plt.plot(x,y)
    # plt.figure(2)
    # plt.plot(pix_size)
    # plt.show()
