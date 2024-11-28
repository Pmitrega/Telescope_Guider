import numpy as np
import cv2


def transformImage(image: np.ndarray, transform_type: str):
    print("Transforming")
    if transform_type == "NO_TRANS":
        return image
    if transform_type == "NO_BG_SQRT":
        mmin = np.average(image) + 5
        LUT = np.zeros(256)
        for i in range(256):
            if i > mmin:
                LUT[i] = (i - mmin) ** (1 / 2)
        LUT = LUT / np.max(LUT) * 255
        im_ret = cv2.GaussianBlur(image, (5, 5), 2)
        im_ret = cv2.medianBlur(im_ret, 3)
        return cv2.LUT(im_ret, LUT)
    if transform_type == "NO_BG_CUBERT":
        mmin = np.average(image) + 5
        LUT = np.zeros(256)
        for i in range(256):
            if i > mmin:
                LUT[i] = (i - mmin) ** (1 / 3)
        LUT = LUT / np.max(LUT) * 255
        im_ret = cv2.GaussianBlur(image, (5, 5), 2)
        im_ret = cv2.medianBlur(im_ret, 3)
        return cv2.LUT(im_ret, LUT)
    if transform_type == "NO_BG_LOC_CUBERT":
        i_mm = cv2.GaussianBlur(image, (7,7),4)
        i_mm = cv2.erode(i_mm, (100,100));
        mmin = np.average(image) + 5
        LUT = np.zeros(256)
        for i in range(256):
            if i > mmin:
                LUT[i] = (i - mmin) ** (1 / 3)
        LUT = LUT / np.max(LUT) * 255
        im_ret = cv2.GaussianBlur(image, (5, 5), 2)
        im_ret = cv2.medianBlur(im_ret, 3)
        im_ret = im_ret - i_mm
        return cv2.LUT(im_ret, LUT)