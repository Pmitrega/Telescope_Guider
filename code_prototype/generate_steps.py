import math
steps = 32
full_angle = 2* 3.14159265

ctrl_table = "["

for i in range(32):
    angle = full_angle/steps * i
    c1 = math.cos(angle) * 1000
    c2 = math.sin(angle) * 1000
    ctrl_table = ctrl_table + "{" +str(int(c1))+","+str(int(c2))+ "}" + ","

print(ctrl_table)
    
