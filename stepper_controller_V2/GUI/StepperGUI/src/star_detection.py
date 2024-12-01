import cv2
import numpy as np


def segmentation(im: np.ndarray):
    im_filt = cv2.GaussianBlur(im, (5,5), 2)
    im_th = cv2.adaptiveThreshold(im_filt,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,\
            cv2.THRESH_BINARY,399,-20)
    seg = cv2.connectedComponentsWithStats(im_th)
    im_ret = cv2.cvtColor(im.astype(np.uint8), cv2.COLOR_GRAY2RGB)
    current_star_centroids = []
    for i in range(1,seg[0]):
        '''Exclude abnormaly large stars'''
        if(4 < seg[2][i, cv2.CC_STAT_AREA] < 2500  ):
            new_star = [seg[2][i, cv2.CC_STAT_AREA], seg[3][i][0], seg[3][i][1], np.sqrt(seg[2][i, cv2.CC_STAT_AREA])]
            print(new_star[3])
            current_star_centroids.append(new_star)
            x_start = (int(new_star[1]) - int(new_star[3])) + 1
            if(x_start < 0): x_start = 0
            x_end = (int(new_star[1]) + int(new_star[3])) + 1
            if(x_start >= im_th.shape[1]): x_start = im_th.shape[1] - 1
            y_start = (int(new_star[2]) - int(new_star[3])) + 1
            if(y_start < 0): y_start = 0
            y_end = (int(new_star[2]) + int(new_star[3])) + 1
            if(y_start >= im_th.shape[0]): y_start = im_th.shapze[0] - 1
            print(x_start, x_end, y_start, y_end)
            I = im_th[y_start:y_end, x_start:x_end]
            circularity_meassure(I, new_star[3], new_star[0], new_star[1])
            # cv2.imshow("dds", I)
            # cv2.waitKey()
            cv2.circle(im_ret,(int(new_star[1]), int(new_star[2])),6 , (0,0,255), 1)

    return im_ret


def circularity_meassure(obj:np.ndarray, non_zero, cx , cy):
   return 1




if __name__ == "__main__":
    test_images = [
        "test/test_images/im1.jpg"
    ]

    for im in test_images:
        I = cv2.imread(im, cv2.IMREAD_GRAYSCALE)
        I_bin = segmentation(I)
        cv2.imshow("image", I_bin)
        cv2.waitKey()
    I = np.zeros((40,40))
