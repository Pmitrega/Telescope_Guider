import cv2
import numpy as np
from astropop import astrometry


def segmentation(im: np.ndarray, thr=-10):
    im_filt = cv2.GaussianBlur(im, (5, 5), 2)
    im_th = cv2.adaptiveThreshold(im_filt, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, \
                                  cv2.THRESH_BINARY, 399, thr)
    seg = cv2.connectedComponentsWithStats(im_th)
    im_ret = cv2.cvtColor(im.astype(np.uint8), cv2.COLOR_GRAY2RGB)
    current_star_centroids = []
    for i in range(1, seg[0]):
        '''Exclude abnormaly large stars'''
        if (4 < seg[2][i, cv2.CC_STAT_AREA] < 2500):
            new_star = [seg[2][i, cv2.CC_STAT_AREA], seg[3][i][0], seg[3][i][1], np.sqrt(seg[2][i, cv2.CC_STAT_AREA])]
            # print(new_star[3])
            current_star_centroids.append(new_star)
            x_start = (int(new_star[1]) - int(new_star[3])) + 1
            if (x_start < 0): x_start = 0
            x_end = (int(new_star[1]) + int(new_star[3])) + 1
            if (x_start >= im_th.shape[1]): x_start = im_th.shape[1] - 1
            y_start = (int(new_star[2]) - int(new_star[3])) + 1
            if (y_start < 0): y_start = 0
            y_end = (int(new_star[2]) + int(new_star[3])) + 1
            if (y_start >= im_th.shape[0]): y_start = im_th.shape[0] - 1

            I = im_th[y_start:y_end, x_start:x_end]
            circularity_meassure(I, new_star[3], new_star[0], new_star[1])
            cv2.circle(im_ret, (int(new_star[1]), int(new_star[2])), 6, (0, 0, 255), 1)

    return im_th, current_star_centroids


def requestStarsLocation(star_list: list, mqtt_client):
    solver_star_list_req = []
    for el in star_list:
        solver_star_list_req.append([round(float(el[1]),2), round(float(el[2]),2)])
    print(str(solver_star_list_req))
    if len(solver_star_list_req) >= 3:
        mqtt_client.publish("solver/star_locs", str(solver_star_list_req))


def testRequestStarsLocation(mqtt_client):
    print("Testing solver")
    solver_star_list_req = [[1150.74, 1.66], [667.31, 10.43], [458.09, 64.36], [444.09, 70.09], [285.5, 76.5],
                        [1007.45, 98.73], [562.7, 151.17], [1000.67, 153.5], [204.78, 158.73], [705.39, 163.08],
                        [584.11, 176.02], [801.49, 180.82], [800.5, 191.5], [722.83, 190.83], [649.7, 265.95],
                        [974.33, 352.54], [10.86, 354.58], [876.5, 365.0], [1127.96, 530.13], [177.72, 554.88],
                        [459.53, 590.88], [773.07, 632.95], [1000.8, 636.6], [195.27, 665.42], [1215.12, 733.0],
                        [77.43, 742.17], [1258.93, 777.71], [1092.67, 817.28], [958.07, 899.75], [1186.17, 941.8]]
    # solver_star_list_req = [[777.71, 20.07], [733.0, 63.88], [941.8, 92.83], [1.66, 128.26], [530.13, 151.04],
    #                     [817.37, 186.42], [98.73, 271.55], [153.5, 278.33], [636.6, 278.2], [352.54, 304.67], [899.67, 320.98], [365.2,
    #                     402.6], [180.82, 477.51], [191.5, 478.5], [632.95, 505.93], [190.83, 556.17], [163.08, 573.61], [10.43,
    #                     611.69], [265.95, 629.3], [176.02, 694.89], [151.12, 716.37], [590.88, 819.47], [64.36, 820.91], [70.09,
    #                     834.91], [76.5, 993.5], [158.73, 1074.22], [665.42, 1083.73], [554.88, 1101.28], [742.17, 1201.57], [354.58,
    #                     1268.14]]

    solver_star_list_req = [[307.67, 39.11], [1115.57, 69.71], [349.93, 152.05], [120.17, 236.2], [84.3, 281.51], [1199.38, 389.59], [248.71, 479.87], [637.31, 535.54], [656.23, 534.31], [139.45, 567.47],
                            [431.42, 706.29], [55.48, 744.81], [829.67, 776.95], [210.68, 810.51], [870.38, 812.62], [1194.3, 897.75]]

    mqtt_client.publish("solver/star_locs", str(solver_star_list_req))


def circularity_meassure(obj: np.ndarray, non_zero, cx, cy):
    return 1


if __name__ == "__main__":
    test_images = [
        "im1.jpg",
        "im1_rot90.jpg"
    ]
    for im in test_images:
        I = cv2.imread(im, cv2.IMREAD_GRAYSCALE)
        I_bin, star_list = segmentation(I)
        solver_star_list = []
        for el in star_list:
            solver_star_list.append([round(float(el[1]), 2), round(float(el[2]), 2)])
        print(str(solver_star_list))
'''
star_list = "[[1150.74, 1.66], [667.31, 10.43], [458.09, 64.36], [444.09, 70.09], [285.5, 76.5], [1007.45, 98.73], [562.7, 151.17], [1000.67, 153.5], [204.78, 158.73], [705.39, 163.08], [584.11, 176.02], [801.49, 180.82], [800.5, 191.5], [722.83, 190.83], [649.7, 265.95], [974.33, 352.54], [10.86, 354.58], [876.5, 365.0], [1127.96, 530.13], [177.72, 554.88], [459.53, 590.88], [773.07, 632.95], [1000.8, 636.6], [195.27, 665.42], [1215.12, 733.0], [77.43, 742.17], [1258.93, 777.71], [1092.67, 817.28], [958.07, 899.75], [1186.17, 941.8]]"
'''
