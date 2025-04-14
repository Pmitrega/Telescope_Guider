import astropy.wcs
import numpy as np
import numpy.linalg as linalg
import astropy.wcs as wcs
from astropy.coordinates import SkyCoord  # High-level coordinates
from astropy.coordinates import ICRS, Galactic, FK4, FK5  # Low-level frames
from astropy.time import Time
from astropy.coordinates import Angle, Latitude, Longitude  # Angles
import astropy.units as u
from astropy.io.fits import Header


def deg2dms(degrees_float):
    degrees = int(degrees_float)
    minutes_float = abs(degrees_float - degrees) * 60
    minutes = int(minutes_float)
    seconds = round((minutes_float - minutes) * 60, 2)

    # Handle negative values for proper formatting
    sign = '-' if degrees_float < 0 else ''

    return f"{sign}{abs(degrees)}° {minutes}' {seconds}\""


def deg2hms(degrees_float):
    total_hours = degrees_float / 15.0
    hours = int(total_hours)
    minutes_float = abs(total_hours - hours) * 60
    minutes = int(minutes_float)
    seconds = round((minutes_float - minutes) * 60, 2)

    # Handle negative values for proper formatting
    sign = '-' if degrees_float < 0 else ''

    return f"{sign}{abs(hours)}h {minutes}m {seconds}s"


class WCS:
    def __init__(self, w11, w12, w21, w22, crpix1, crpix2, crval1, crval2):
        header = Header()
        header['CD1_1'] = w11
        header['CD1_2'] = w12
        header['CD2_1'] = w21
        header['CD2_2'] = w22
        header['NAXIS1'] = 1280
        header['NAXIS2'] = 960
        # WCS definition
        header['CTYPE1'] = 'RA---TAN-SIP'
        header['CTYPE2'] = 'DEC--TAN-SIP'
        header['CRVAL1'] = crval1
        header['CRVAL2'] = crval2
        header['CRPIX1'] = crpix1
        header['CRPIX2'] = crpix2
        header['CUNIT1'] = 'deg'
        header['CUNIT2'] = 'deg'

        # Image size (optional but helpful)
        header['NAXIS1'] = 1280
        header['NAXIS2'] = 960

        # # WCS reference frame and projection type
        # header['CTYPE1'] = 'RA---TAN'  # 'RA---TAN-SIP' if you add SIP distortion
        # header['CTYPE2'] = 'DEC--TAN'
        #
        # # Reference pixel coordinates
        # header['CRPIX1'] = 988.6375
        # header['CRPIX2'] = 666.3400000000001
        #
        # # Sky coordinates (RA/Dec) at reference pixel
        # header['CRVAL1'] = 308.26740797321753
        # header['CRVAL2'] = 83.66293691523461
        #
        # # Units for world coordinates
        # header['CUNIT1'] = 'deg'
        # header['CUNIT2'] = 'deg'
        #
        # # CD matrix for scale, rotation, and skew
        # header['CD1_1'] = 0.000761532972101924
        # header['CD1_2'] = 0.000854737883087284
        # header['CD2_1'] = 0.000862878377948734
        # header['CD2_2'] = -0.00075439484917592

        self.wcs = astropy.wcs.WCS(header=header)

    def word_to_pixel(self, ra, dec, obstime=None):
        if obstime is None:
            obstime = Time.now()
        pos = SkyCoord(ra, dec, unit="deg", frame='fk5', equinox=obstime)
        pos = pos.transform_to(FK5(equinox=Time("J2000.000")))
        sol = self.wcs.wcs_world2pix([[pos.ra.deg, pos.dec.deg]], 0)
        return sol[0][0], sol[0][1]

    def pixel_to_world(self, x, y, J2000 = False):
        sky_coord = self.wcs.pixel_to_world(x, y)
        obstime = Time.now()
        if J2000 == False:
            coord_now = sky_coord.transform_to(FK5(equinox=obstime))
        else:
            coord_now = sky_coord

        return coord_now.ra.deg, coord_now.dec.deg

    def get_meridian_grid_lines(self, step_deg: float = 0.25):
        steps = 10
        pos_1 = self.pixel_to_world(0, 0)
        pos_2 = self.pixel_to_world(1280, 0)
        pos_3 = self.pixel_to_world(0, 960)
        pos_4 = self.pixel_to_world(1280,960)
        min_ra = min(pos_1[0], pos_2[0], pos_3[0], pos_4[0])
        min_dec = min(pos_1[1], pos_2[1], pos_3[1], pos_4[1])
        max_ra = max(pos_1[0], pos_2[0], pos_3[0], pos_4[0])
        max_dec = max(pos_1[1], pos_2[1], pos_3[1], pos_4[1])
        ra = np.floor(min_ra)  - step_deg
        dec_step = (max_dec-min_dec)/steps

        lines = []
        while ra < (max_ra + step_deg):
            line = []
            for i in range(steps + 1):
                x, y = self.word_to_pixel(ra, min_dec + i * dec_step)
                line.append([y,x])
            lines.append(line)
            ra += step_deg
        return lines

    def get_latitude_grid_lines(self, step_deg: float = 0.25):
        steps = 10
        pos_1 = self.pixel_to_world(0, 0)
        pos_2 = self.pixel_to_world(1280, 0)
        pos_3 = self.pixel_to_world(0, 960)
        pos_4 = self.pixel_to_world(1280,960)
        min_ra = min(pos_1[0], pos_2[0], pos_3[0], pos_4[0])
        min_dec = min(pos_1[1], pos_2[1], pos_3[1], pos_4[1])
        max_ra = max(pos_1[0], pos_2[0], pos_3[0], pos_4[0])
        max_dec = max(pos_1[1], pos_2[1], pos_3[1], pos_4[1])
        dec = np.floor(min_dec) - step_deg
        ra_step = (max_ra-min_ra)/steps

        lines = []
        while dec < (max_dec + step_deg):
            line = []
            for i in range(steps + 1):
                x, y = self.word_to_pixel(min_ra + i * ra_step, dec)
                line.append([y,x])
            lines.append(line)
            dec += step_deg
        return lines


if __name__ == '__main__':
    w11 = 0.0007261056642957787
    w12 = 0.0008789484882833199
    w21 = 0.0008792146556405366
    w22 = -0.0007258959086414979
    crpix1 = 669.825
    crpix2 = 716.5725
    crval1 = 306.5674509888222
    crval2 = 83.34908306568481

    wcs = WCS(w11, w12, w21, w22, crpix1, crpix2, crval1, crval2)
    sol = wcs.pixel_to_world(900, 612, )
    # obstime = Time.now()
    # sky_cords = SkyCoord(float(sol.ra.deg), float(sol.dec.deg), unit="deg", obstime="J2000.00")
    # coord_now = sky_cords.transform_to(FK5(equinox=obstime))
    print(sol)
    print(wcs.word_to_pixel(sol[0], sol[1]))
    print(len(wcs.get_meridian_grid_lines(1)))
    print(len(wcs.get_latitude_grid_lines(0.25)))
    # ra, dec = wcs.pixel_to_world(900, 612)
    # print(deg2hms(ra))
    # print(deg2dms(dec))
    # ra, dec = wcs.pixel_to_world(763, 529)
    # print(deg2hms(ra))
    # print(deg2dms(dec))
    # ra, dec = wcs.pixel_to_world(539, 787)
    # print(deg2hms(ra))
    # print(deg2dms(dec))
    # ra, dec = wcs.pixel_to_world(308, 126)
    # print(deg2hms(ra))
    # print(deg2dms(dec))

    # ra, dec = wcs.pixel_to_world(900, 612)
    # print(ra, dec)
    # x, y = wcs.word_to_pixel(ra, dec)
    # print(x, y)

    # ra, dec = wcs.pixel_to_world(308, 126)
    # print(ra, dec)
    # x, y = wcs.word_to_pixel(ra, dec)
    # print(308, 126)
    # print(x, y)
