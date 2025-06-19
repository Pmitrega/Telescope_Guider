import numpy as np

def get_control(err_x, err_y):
    # alpha = 1.5625 * 4  # seconds per step
    # beta = 0.245  # pixels per second
    alpha = 6.25
    beta = 0.245
    B_sky_matr = np.array([[beta, 0],
                           [0, beta]])
    A_tel_matr = np.array([[0.25, 0],
                            [0, 1]])
    star_movement = B_sky_matr @ np.array([[15], [0]])
    print(star_movement)
    sol = 1 / (alpha * beta) * np.linalg.inv(A_tel_matr) @ star_movement
    v_ra = sol[0][0] * 60 # steps/s -> steps/min
    v_dec = sol[1][0] * 60 # steps/s -> steps/min
    # print("ctrl ra/dec: ", sol[0][0], sol[1][0])
    return v_ra, v_dec  # sol[0][0] * 144 / 15 * sec_per_pix, sol[1][0] * 144 / 15 * sec_per_pix

if __name__ == "__main__":
    print(get_control(err_x=1, err_y=1))