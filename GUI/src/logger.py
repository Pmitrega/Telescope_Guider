import os
from datetime import datetime


class Logger:
    def __init__(self):
        if not os.path.exists("./logs"):
            os.mkdir("./logs")
        # Get the current time
        self.start_time = datetime.now()
        date = self.start_time.strftime("%Y_%m_%d")
        if not os.path.exists("./logs/" + date):
            os.mkdir("./logs/" + date)
        # Format the time as "month day hour minute second millisecond"
        formatted_time = self.start_time.strftime("%Y_%m_%d_%H_%M_%S")
        time_dir = self.start_time.strftime("%H_%M_%S")
        os.mkdir("./logs/" + date + "/" + time_dir)
        self.ra_ctrl_path = "./logs/" + date + "/" + time_dir + "/" + formatted_time + "_ra_ctrl.csv"
        self.dec_ctrl_path = "./logs/" + date + "/" + time_dir + "/" + formatted_time + "_dec_ctrl.csv"
        self.x_err_path = "./logs/" + date + "/" + time_dir + "/" + formatted_time + "_x_err.csv"
        self.y_err_path = "./logs/" + date + "/" + time_dir + "/" + formatted_time + "_y_err.csv"
        self.x_SP_path = "./logs/" + date + "/" + time_dir + "/" + formatted_time + "_x_SP.csv"
        self.y_SP_path = "./logs/" + date + "/" + time_dir + "/" + formatted_time + "_y_SP.csv"
        self.skypatch_path = "./logs/" + date + "/" + time_dir + "/" + formatted_time + "_skypatch.csv"
        self.loc_capt_time = None

    def logRaCtrl(self, raCtrl: float):
        with open(self.ra_ctrl_path, "a") as file:
            t_diff = datetime.now() - self.start_time
            new_line = f"{raCtrl:.{3}f}, {t_diff.total_seconds()}\n"
            file.write(new_line)

    def logDecCtrl(self, decCtrl: float):
        with open(self.dec_ctrl_path, "a") as file:
            t_diff = datetime.now() - self.start_time
            new_line = f"{decCtrl:.{3}f}, {t_diff.total_seconds()}\n"
            file.write(new_line)

    def LogErrX(self, err_x: float):
        with open(self.x_err_path, "a") as file:
            t_diff = datetime.now() - self.start_time
            new_line = f"{err_x:.{3}f}, {t_diff.total_seconds()}\n"
            file.write(new_line)

    def LogErrY(self, err_y: float):
        with open(self.y_err_path, "a") as file:
            t_diff = datetime.now() - self.start_time
            new_line = f"{err_y:.{3}f}, {t_diff.total_seconds()}\n"
            file.write(new_line)

    def LogSetPointX(self, SP_x: float):
        with open(self.x_SP_path, "a") as file:
            t_diff = datetime.now() - self.start_time
            new_line = f"{SP_x:.{3}f}, {t_diff.total_seconds()}\n"
            file.write(new_line)

    def LogSetPointY(self, SP_y: float):
        with open(self.y_SP_path, "a") as file:
            t_diff = datetime.now() - self.start_time
            new_line = f"{SP_y:.{3}f}, {t_diff.total_seconds()}\n"
            file.write(new_line)

    def LogFacingLoc(self, dec: float, ra: float, rot_deg: float, time: str):
        with open(self.skypatch_path, "a") as file:
            t_diff = time - self.start_time
            new_line = f"{dec:.{5}f}, {ra:.{5}f}, {rot_deg:.{5}f}, {t_diff.total_seconds()}\n"
            file.write(new_line)
