import numpy as np

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
        Az = np.atan2(self.y,self.x)*180/np.pi
        if Az < 0:
            Az = Az + 360
        return [Az, np.asin(self.z)*180/np.pi]
    



class SphCoord:

    def __init__(self, A, a):
        self.A = A/180*np.pi
        self.a = a/180*np.pi

    def toCartesian(self)-> CartCoords:
        x = np.cos(self.A)*np.cos(self.a)
        y = np.sin(self.A)*np.cos(self.a)
        z = np.sin(self.a)
        return CartCoords(x, y, z)
    
    

    

    def __repr__(self):
        return "Az: " + str(self.A*180/np.pi) + ", al: " + str(self.a*180/np.pi)

    
def rotKabsch(P: np.ndarray, Q: np.ndarray):
        H = P.transpose()*Q
        U, S, Vh = np.linalg.svd(H)
        
        d = np.linalg.det(U@Vh)
        midM = np.array([[1, 0, 0],
                         [0, 1, 0],
                         [0, 0, d]]) 
        R = U @ midM @ Vh
        rmds = 0
        #rmds = np.sqrt(np.sum(np.square(np.matmul(P, R.transpose(0, 2, 1)) - Q), axis=(1, 2)) / P.shape[1])
        return R, rmds


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
    coords = SphCoord(90, 45)
    cart = coords.toCartesian()
    p1_h = SphCoord(0, 30)
    p2_h = SphCoord(0, 32)
    p3_h = SphCoord(2, 32)



    p1_eq = SphCoord(180.0000, 75)
    p2_eq = SphCoord(180.0000, 77)
    p3_eq = SphCoord(187.5073333333, 76.9073)


    P = np.array([  p1_h.toCartesian().as_array(),
                    p2_h.toCartesian().as_array(),
                    p3_h.toCartesian().as_array()])
    
    Q  = np.array(  [p1_eq.toCartesian().as_array(),
                     p2_eq.toCartesian().as_array(),
                     p3_eq.toCartesian().as_array()])
    
    #R, rdms = rotKabsch(Q, P)
    R, rmds = kabsch_numpy(Q, P)
    # print(rmds)

    print("P: \n", P)
    print("Q: \n", Q)
    print("R@Q: \n", R @ Q[:,:])
    pp = SphCoord(10, 30)
    r = np.linalg.inv(R) @ pp.toCartesian().as_array()

    print(CartCoords(r[0], r[1], r[2]).toSpherical())
    

    


