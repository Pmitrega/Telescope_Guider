import math

def make_c_array(input_arr, name = "") -> str:
    output_string = "{"
    i = 1
    for el in input_arr:
        el_in_str = str(el)
        el_in_str = el_in_str + " "*(5 - len(el_in_str))
        output_string += el_in_str
        output_string += ','
        if(not i%16):
            output_string += '\n'
        i = i+1
    output_string = output_string[:-2] + "};"
    if name != "":
        output_string = name + " = " + output_string
    return output_string


if __name__ == "__main__":
    microsteps = input("How many microsteps?")
    microsteps = int(microsteps)
    PWM1_arr = []
    PWM2_arr = []
    PWM3_arr = []
    PWM4_arr = []

    for i in range(microsteps):
        angle = i/microsteps * 2*math.pi
        PWM1_arr.append(int(1000*math.sin(angle)))
        PWM2_arr.append(0)
        PWM3_arr.append(int(1000*math.cos(angle)))
        PWM4_arr.append(0)
    for i in range(len(PWM1_arr)):
        if PWM1_arr[i] >= 0:
            PWM1_arr[i] = 1000 - PWM1_arr[i]
            PWM2_arr[i] = 1000
        else:
            PWM2_arr[i] = 1000 + PWM1_arr[i]
            PWM1_arr[i] = 1000

        if PWM3_arr[i] > 0:
            PWM3_arr[i] = 1000 - PWM3_arr[i]
            PWM4_arr[i] = 1000
        else:
            PWM4_arr[i] = 1000 + PWM3_arr[i]
            PWM3_arr[i] = 1000

    validation_array1 = []
    validation_array2 = []
    for i in range(len(PWM1_arr)):
        validation_array1.append(PWM1_arr[i] - PWM2_arr[i])
        validation_array2.append(PWM3_arr[i] - PWM4_arr[i])
    print(make_c_array(PWM1_arr, "const uint16_t PWM1_microsteps_array[]"))
    print(make_c_array(PWM2_arr, "const uint16_t PWM2_microsteps_array[]"))
    print(make_c_array(PWM3_arr, "const uint16_t PWM3_microsteps_array[]"))
    print(make_c_array(PWM4_arr, "const uint16_t PWM4_microsteps_array[]"))

