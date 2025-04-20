import numpy as np

class PID_Controller:
    def __init__(self, P:float, I:float, D:float, A_matr:np.ndarray):
        self.A = A_matr[:,:]
        self.P = 0.6
        self.I = 0
        self.D = 0
        self.integral_Deadband = 2 # 2"
        self.integral_x = 0
        self.integral_y = 0
        self.antiwindup = 20
        self.last_error_x = float("NaN")
        self.last_error_y = float("NaN")
    
    def set_gains(self, P:float, I:float, D:float):
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
    def __init__(self, P:float, I:float, D:float, A_matr:np.ndarray):
        self.A = A_matr[:,:]
        self.P = 0.6
        self.I = 0
        self.D = 0
        self.integral_Deadband = 2 # 2"
        self.integral_x = 0
        self.integral_y = 0
        self.antiwindup = 20
        self.last_error_x = float("NaN")
        self.last_error_y = float("NaN")
    
    def set_gains(self, P:float, I:float, D:float):
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
        
    

class Controller:
    def __init__(self, P:float, I:float, D:float, A_matr:np.ndarray):
        self.PID_controller = PID_Controller(P, I, D, A_matr)
        self.FF_PID_controller = FF_PID_Controller(P, I, D, A_matr)
    
    def updateControllers(self, P:float, I:float, D:float, A_matr:np.ndarray):
        self.PID_controller = PID_Controller(P, I, D, A_matr)
        self.FF_PID_controller = FF_PID_Controller(P, I, D, A_matr)
    