import numpy as np
import matplotlib.pyplot as plt


class CartCoords:
    def __init__(self, x: float, y: float, z: float):
        self.x = x
        self.y = y
        self.z = z

    def as_array(self):
        return np.array([self.x, self.y, self.z])

    def __repr__(self):
        return "x: " + str(self.x) + ", y: " + str(self.y) + ", z: " + str(self.z)

    def toSpherical(self):
        Az = np.atan2(self.y, self.x) * 180 / np.pi
        if Az < 0:
            Az = Az + 360
        return [Az, np.asin(self.z) * 180 / np.pi]


class SphCoord:

    def __init__(self, A, a):
        self.A = A / 180 * np.pi
        self.a = a / 180 * np.pi

    def toCartesian(self) -> CartCoords:
        x = np.cos(self.A) * np.cos(self.a)
        y = np.sin(self.A) * np.cos(self.a)
        z = np.sin(self.a)
        return CartCoords(x, y, z)

    def __repr__(self):
        return "Az: " + str(self.A * 180 / np.pi) + ", al: " + str(self.a * 180 / np.pi)


def rotKabsch(P: np.ndarray, Q: np.ndarray):
    H = P.transpose() * Q
    U, S, Vh = np.linalg.svd(H)

    d = np.linalg.det(U @ Vh)
    midM = np.array([[1, 0, 0],
                     [0, 1, 0],
                     [0, 0, d]])
    R = U @ midM @ Vh
    rmds = 0
    # rmds = np.sqrt(np.sum(np.square(np.matmul(P, R.transpose(0, 2, 1)) - Q), axis=(1, 2)) / P.shape[1])
    return R, rmds


def horToEquat(coord_hor: SphCoord, lat):
    dec = np.asin(
        np.sin(coord_hor.a) * np.sin(lat / 180 * np.pi) + np.cos(coord_hor.a) * np.cos(lat / 180 * np.pi) * np.cos(
            coord_hor.A))
    sinH = - np.sin(coord_hor.A) * np.cos(coord_hor.a) / np.cos(dec)
    cosH = (np.sin(coord_hor.a) - np.sin(dec) * np.sin(lat / 180 * np.pi)) / (np.cos(dec) * np.cos(lat / 180 * np.pi))
    H = np.atan2(sinH, cosH)
    if H < 0:
        H = H + 2 * np.pi
    return SphCoord(H * 180 / np.pi, dec * 180 / np.pi)


def horToEquatMatr(coord_hor: SphCoord, lat):
    lat = lat / 180 * np.pi
    R = np.array([[np.cos(lat - np.pi / 2), 0, np.sin(lat - np.pi / 2)],
                  [0, 1, 0],
                  [-np.sin(lat - np.pi / 2), 0, np.cos(lat - np.pi / 2)]])

    # R = np.array([[1, 0, 0],
    #               [0, np.cos(lat - np.pi / 2), np.sin(lat - np.pi / 2)],
    #               [0, -np.sin(lat - np.pi / 2), np.cos(lat - np.pi / 2)]])
    print(R)
    rotated = R @ np.transpose(np.array(coord_hor.toCartesian().as_array()))
    cart = CartCoords(rotated[0], rotated[1], rotated[2])
    sphc = cart.toSpherical()
    return SphCoord(sphc[0] + 180, sphc[1])


def kabsch_numpy(P, Q):
    """
    Computes the optimal rotation and translation to align two sets of points (P -> Q),
    and their RMSD.

    :param P: A Nx3 matrix of points
    :param Q: A Nx3 matrix of points
    :return: A tuple containing the optimal rotation matrix, the optimal
             translation vector, and the RMSD.
    """
    assert P.shape == Q.shape, "Matrix dimensions must match"

    # Center the points
    p = P
    q = Q

    # Compute the covariance matrix
    H = np.dot(p.T, q)

    # SVD
    U, S, Vt = np.linalg.svd(H)

    # Validate right-handed coordinate system
    if np.linalg.det(np.dot(Vt.T, U.T)) < 0.0:
        Vt[-1, :] *= -1.0

    # Optimal rotation
    R = np.dot(Vt.T, U.T)

    # RMSD
    rmsd = 0

    return R, rmsd


if __name__ == "__main__":
    # coords = SphCoord(90, 45)
    # cart = coords.toCartesian()
    # p1_h = SphCoord(0, 30)
    # p2_h = SphCoord(0, 32)
    # p3_h = SphCoord(2, 32)
    #
    # p1_eq = SphCoord(180.0000, 75)
    # p2_eq = SphCoord(180.0000, 77)
    # p3_eq = SphCoord(187.5073333333, 76.9073)
    #
    # P = np.array([p1_h.toCartesian().as_array(),
    #               p2_h.toCartesian().as_array(),
    #               p3_h.toCartesian().as_array()])
    #
    # Q = np.array([p1_eq.toCartesian().as_array(),
    #               p2_eq.toCartesian().as_array(),
    #               p3_eq.toCartesian().as_array()])
    #
    # # R, rdms = rotKabsch(Q, P)
    # R, rmds = kabsch_numpy(Q, P)
    # # print(rmds)
    #
    # print("P: \n", P)
    # print("Q: \n", Q)
    # print("R@Q: \n", R @ Q[:, :])
    # pp = SphCoord(0, 30)
    # r = np.linalg.inv(R) @ pp.toCartesian().as_array()
    #
    # print(CartCoords(r[0], r[1], r[2]).toSpherical())

    p1_h = SphCoord(25, 30)
    p2_h = SphCoord(25, 32)
    p3_h = SphCoord(27, 32)

    p1_eq = SphCoord(302.70, 79.98)
    p2_eq = SphCoord(304.497, 80.616)
    p3_eq = SphCoord(296.891, 81.0585)
    # Az: 232.59637370811663, al: 62.56535313865119
    # p1_eq = horToEquatMatr(p1_h, 89.99)
    # p2_eq = horToEquat(p2_h, 89.99)
    # p3_eq = horToEquat(p3_h, 89.99)
    P = np.array([p1_h.toCartesian().as_array(),
                  p2_h.toCartesian().as_array(),
                  p3_h.toCartesian().as_array()])

    Q = np.array([p1_eq.toCartesian().as_array(),
                  p2_eq.toCartesian().as_array(),
                  p3_eq.toCartesian().as_array()])
    print(Q)

    R, rmds = kabsch_numpy(Q, P)

    # print("P: \n", P)
    # print("Q: \n", Q)
    # print("R@Q: \n", R @ Q[:, :])
    pp = SphCoord(256.75, 61)

    r = R @ pp.toCartesian().as_array()
    # print(CartCoords(r[0], r[1], r[2]).toSpherical())
    time = []
    points_az = []
    points_al = []
    for i in range(360):
        pp = SphCoord(61.17 + i, 65)
        r = R @ pp.toCartesian().as_array()
        sph = CartCoords(r[0], r[1], r[2]).toSpherical()
        time.append(i / 360 * 24)
        if sph[0] > 180:
            sph[0] -= 360
        points_az.append(sph[0])
        points_al.append(sph[1])

    plt.plot(np.array(time), points_az)
    plt.plot(np.array(time), points_al)
    plt.legend(["Az", "Al"])
    plt.show()

    # 241.0166667
    # dec = 61
