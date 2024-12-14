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

    def predict(self, dt:float, u: np.ndarray):
        self.state_apriori = self.model_func(self.state, dt, u)
        F = self.F_jacobian(self.state_apriori, dt, u)
        self.P_cov_matrix = F@self.P_cov_matrix@np.transpose(F) + self.Q_prcs_noise

    def update(self, z:np.ndarray):
        y_pred = z - self.meassure_func(self.state_apriori)
        H = self.H_jacobian(self.state_apriori)
        Sk = H@self.P_cov_matrix@np.transpose(H) + self.R_meass_noise
        Kk = self.P_cov_matrix@np.transpose(H)@linalg.inv(Sk)
        self.state_aprosteriori = self.state_apriori + Kk @ y_pred
        w = self.state.shape
        self.P_cov_matrix = (np.eye(self.state.shape[0],self.state.shape[0]) - Kk@H)@self.P_cov_matrix
        self.state = self.state_aprosteriori

    def getState(self):
        return self.state
    

class guiderEKF():
    def __init__(self, initial_state = np.array([500, 500, 0, 0, 0, 0, 0, 5.00]).transpose()):
        self.EKF = EKF()
        self.EKF.model_func = self.model_func
        self.EKF.meassure_func = self.meassure_func
        self.EKF.F_jacobian = self.Modeljacobian
        self.EKF.H_jacobian = self.Meassurejacobian
        self.EKF.state = initial_state
        self.EKF.Q_prcs_noise = np.diag([0, 0, 1e-3, 1e-3, 1e-3, 1e-3, 1e-3, 1e-2])
        self.EKF.R_meass_noise = np.diag([1, 1])
        self.EKF.P_cov_matrix = np.diag([1, 1, 1e-3, 1e-3, 1e-3, 1e-3, 1e-3, 1e-2])

    def predict(self, dt:float, v_dec: float, v_ra: float):
        u = np.array([v_dec, v_ra])
        self.EKF.predict(dt,  u)
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
        kappa = 15 # "/s
        x1 = state[0]
        x2 = state[1]
        x3 = state[2]
        x4 = state[3]
        x5 = state[4]
        x6 = state[5]
        x7 = state[6]
        x8 = state[7]
        v_dec = u[0]
        v_ra  = u[1]
        x1_next = x1 + 1/x8 * (cos(x6)*v_dec - cos(x3) * sin(x6) * v_ra - sin(x5) * cos(x7) * kappa) * dt
        x2_next = x2 + 1/x8 * (sin(x6)*v_dec + cos(x3) * cos(x6) * v_ra + cos(x5) * cos(x7) * kappa) * dt
        x3_next = x3 + v_dec * dt /3600/180 * np.pi
        x4_next = x4 + v_ra * dt /3600/180 * np.pi
        x5_next = x5 + (kappa - v_ra) * dt /3600/180 * np.pi
        x6_next = x6
        x7_next = x7
        x8_next = x8
        return np.array([x1_next, x2_next,x3_next, x4_next,x5_next, x6_next,x7_next, x8_next])

    def meassure_func(self, state: np.ndarray) -> np.ndarray:
        return np.array([state[0], state[1]])
    
    def Modeljacobian(self, state: np.ndarray, dt:float, u: np.ndarray) -> np.ndarray:
        x1 = state[0]
        x2 = state[1]
        x3 = state[2]
        x4 = state[3]
        x5 = state[4]
        x6 = state[5]
        x7 = state[6]
        x8 = state[7]
        v_dec = u[0]
        v_ra  = u[1]
        kappa = 15 # "/s
        jac =np.array(
        [
        [1, 0,  (v_ra*sin(x3)*sin(x6))/x8, 0, -(kappa*cos(x5)*cos(x7))/x8, -(v_dec*sin(x6) + v_ra*cos(x3)*cos(x6))/x8,  (kappa*sin(x5)*sin(x7))/x8,  (kappa*cos(x7)*sin(x5) - v_dec*cos(x6) + v_ra*cos(x3)*sin(x6))/x8**2],
        [0, 1, -(v_ra*cos(x6)*sin(x3))/x8, 0, -(kappa*cos(x7)*sin(x5))/x8,  (v_dec*cos(x6) - v_ra*cos(x3)*sin(x6))/x8, -(kappa*cos(x5)*sin(x7))/x8, -(v_dec*sin(x6) + kappa*cos(x5)*cos(x7) + v_ra*cos(x3)*cos(x6))/x8**2],
        [0, 0,                          1, 0,                           0,                                          0,                           0,                                                                    0],
        [0, 0,                          0, 1,                           0,                                          0,                           0,                                                                    0],
        [0, 0,                          0, 0,                           1,                                          0,                           0,                                                                    0],
        [0, 0,                          0, 0,                           0,                                          1,                           0,                                                                    0],            
        [0, 0,                          0, 0,                           0,                                          0,                           1,                                                                    0],
        [0, 0,                          0, 0,                           0,                                          0,                           0,                                                                    1]
        ]
        )
        return (jac - np.eye(8,8))*dt + np.eye(8,8)
    
    def Meassurejacobian(self, state: np.ndarray) -> np.ndarray:
        jac =np.array(
        [
            [1,0,0,0,0,0,0,0],
            [0,1,0,0,0,0,0,0]
        ]
        )
        return jac

if __name__ == "__main__":
    guid_ekf = guiderEKF()
    for i in range(1000):
        guid_ekf.predict(1.0, 10, -10)
        guid_ekf.update(250+10*i/5,250+10*i/5)
    print(guid_ekf.getState()["pix_size"])
        
        