import numpy as np
import cv2
from ui_form import Ui_MainWindow
from src.telescope_controller import TelescopeController, starCentroid
import math


def drawSkyCoordinates(image: np.ndarray, sky_ra_vec, sky_dec_vect):
    sky_ra_vect = sky_ra_vec
    sky_dec_vect = sky_dec_vect
    cv2.arrowedLine(image, (100, 1000), (100 + int(sky_ra_vect[0] * 50), 1000 + int(sky_ra_vect[1] * 50)),
                    (255, 0, 0), 3)
    cv2.arrowedLine(image, (100, 1000), (100 + int(sky_dec_vect[0] * 50), 1000 + int(sky_dec_vect[1] * 50)),
                    (255, 255, 0), 3)
    print(sky_dec_vect)
    print(sky_ra_vect)
    return image


def drawTelescopeCoordinates(image: np.ndarray, tel_ra_vect, tel_dec_vect):
    ra_vect = tel_ra_vect
    dec_vect = tel_dec_vect
    if not (math.isnan(ra_vect[0]) or math.isnan(ra_vect[1]) or math.isnan(dec_vect[0]) or math.isnan(dec_vect[1])):
        cv2.arrowedLine(image, (100, 100), (100 + int(ra_vect[0] * 50), 100 + int(ra_vect[1] * 50)),
                        (0, 0, 255), 3)
        cv2.arrowedLine(image, (100, 100), (100 + int(dec_vect[0] * 50), 100 + int(dec_vect[1] * 50)),
                        (0, 255, 0), 3)
    return image


def drawSkyGrid(image: np.ndarray, sky_ra_vec, sky_dec_vect):
    return image


def drawAllStars(image: np.ndarray, stars_all):
    if stars_all is None:
        return image
    for star in stars_all:
        cv2.circle(image, (int(star.x_cent), int(star.y_cent)),
                   int(math.sqrt(star.brightness / 2)), (0, 255, 0), 3)
    return image


def drawGuiding(image: np.ndarray, guiding_star: starCentroid):
    if guiding_star is not None:
        cv2.circle(image, (int(guiding_star.x_cent), int(guiding_star.y_cent)),
                   int(math.sqrt(guiding_star.brightness / 2)), (0, 0, 255), 3)
    return image

def drawSetPoint(image: np.ndarray, setPoint: starCentroid):
    print((int(setPoint.x_cent),int(setPoint.y_cent)))
    p1 = (int(setPoint.x_cent), 0)
    p2 = (int(setPoint.x_cent), image.shape[0])
    cv2.line(image, p1 , p2 , (255,0,0),1)
    p1 = (0, int(setPoint.y_cent))
    p2 = (image.shape[1], int(setPoint.y_cent))
    cv2.line(image, p1 , p2 , (255,0,0),1)
    
    return image


def transformImage(ui: Ui_MainWindow, image: np.ndarray, tel_controller: TelescopeController) -> np.ndarray:
    transform_type = ui.comboBox_preview.currentText()
    opts = ["Original", "Binarized", "No background, sqrt"]
    if np.max(image) > 256:
        transformed_image = image / 2 ** 8
        transformed_image = transformed_image.astype(np.uint8)
    else:
        transformed_image = image[:, :]

    if transform_type == opts[1]:
        transformed_image = cv2.medianBlur(transformed_image, 3)
        transformed_image = cv2.GaussianBlur(transformed_image, (5, 5), 2)
        transformed_image = cv2.adaptiveThreshold(transformed_image, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                                  cv2.THRESH_BINARY, 399, tel_controller.adaptive_thre)
    if transform_type == opts[2]:
        bgrd = np.min(transformed_image)
        transformed_image = transformed_image - bgrd
        transformed_image =  np.sqrt(transformed_image)
        transformed_image = transformed_image/transformed_image.max() * 255
        transformed_image = transformed_image.astype(np.uint8)

    transformed_image = cv2.cvtColor(transformed_image, cv2.COLOR_GRAY2RGB)
    if ui.checkBox_show_motor_coo.isChecked():
        transformed_image = drawTelescopeCoordinates(transformed_image, tel_controller.telescope_ra_vect,
                                                     tel_controller.telescope_dec_vect)

    if ui.checkBox_show_sky_coo.isChecked():
        transformed_image = drawSkyCoordinates(transformed_image, tel_controller.sky_ra_vect,
                                               tel_controller.sky_dec_vect)
    if ui.checkBox_show_all_detected.isChecked():
        transformed_image = drawAllStars(transformed_image, tel_controller.last_star_centroids)

    if ui.checkBox_show_guiding_star.isChecked():
        transformed_image = drawGuiding(transformed_image, tel_controller.reference_star_current)
    if True: # show setpoint
        transformed_image = drawSetPoint(transformed_image, tel_controller.go_to_loc)

    return transformed_image
