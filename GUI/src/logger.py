import os
from datetime import datetime


class Logger:
    def __init__(self):
        if not os.path.exists("./logs"):
            os.mkdir("./logs")
        # Get the current time
        self.start_time  = datetime.now()
        # Format the time as "month day hour minute second millisecond"
        formatted_time = self.start_time.strftime("%m_%d_%H_%M_%S")
        self.ra_ctrl_path =  "./logs/" + formatted_time + "_ra_ctrl.csv"
        self.dec_ctrl_path = "./logs/" + formatted_time + "_dec_ctrl.csv"
        self.x_err_path =    "./logs/" + formatted_time + "_x_err.csv"
        self.y_err_path =    "./logs/" + formatted_time + "_y_err.csv"
    
    def logRaCtrl(self, raCtrl:float):
        with open(self.ra_ctrl_path, "a") as file:
            t_diff = datetime.now() - self.start_time
            new_line = f"{raCtrl:.{3}f}, {t_diff.total_seconds()} \r\n"
            file.write(new_line)
        

    def logDecCtrl(self, decCtrl:float):
        with open(self.dec_ctrl_path, "a") as file:
            t_diff = datetime.now() - self.start_time
            new_line = f"{decCtrl:.{3}f}, {t_diff.total_seconds()} \r\n"
            file.write(new_line)


    def LogErrX(self, err_x:float):
        pass
        
    def LogErrY(self, err_y:float):
        pass