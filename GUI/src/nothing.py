x = 1280/2
y = 960/2
dx= x - 988.7725
dy = y - 666.2525

CD11 =  0.0007527173064272584
CD12 =  0.0008555996319983658
CD21 =  0.0008620922213217879
CD22 = -0.0007549429514546843
get_ra =  308.2621102085086
get_dec = 83.66274169859135

trans_ra = get_ra + CD11 * 1/0.12 * dx + CD12* 1/0.12 * dy
trans_dec = get_dec + CD21 * dx + CD22 *dy
print(trans_ra)
print(trans_dec)
want_ra = 304.538
want_dec = 83.490

diff_ra = (want_ra - trans_ra)
diff_dec = want_dec - trans_dec
print(diff_ra)
print(diff_dec)


