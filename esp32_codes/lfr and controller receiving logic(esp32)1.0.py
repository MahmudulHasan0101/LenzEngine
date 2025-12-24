import machine, gc, time, network, json, espnow
from machine import Pin, time_pulse_us, UART, PWM

sensor_pins = [21, 19, 18, 5, 25, 26, 27, 14, 12, 13]
sensors = [Pin(pin, Pin.IN) for pin in sensor_pins]
#wifi
wifi = network.WLAN(network.STA_IF)
wifi.active(True)
#espnow comm
espcom = espnow.ESPNow()
espcom.active(True)
#to send motor speed info when using controller
def control(data):
    x = data["rjoyx"]
    y = data["ljoyy"]
    if (x<=10 and x>=-10):
        x = 0
    if (y<=10 and y>=-10):
        y = 0
    if y == 0:
        x = int(x*0.65)
    if y<0:
        left = y-x
        right = y+x
    else:
        left = y+x
        right = y-x
    val_list = [left, right]
    for i in range(2):
        if val_list[i] > 100:
            val_list[i] = 100
        if val_list[i] < -100:
            val_list[i] = -100
    return val_list
#to read sensor array
def array_val():
    #charge time for '10RC' arrays
    readings = []
    for i in sensors:
        i.init(Pin.OUT)
        i.value(1)
        time.sleep_us(10)
        #discharge and checking the discharge time based on black/white surface
        i.init(Pin.IN)
        charge_length = time_pulse_us(i, 1, 5000) #checks for 1 value in 'i' pin for 5000us or 5ms
        if (charge_length > 900) or (charge_length < 0):
            val = 1
        else:
            val = 0
        readings.append(val)
    print(readings)
    return readings
#to measure how far the line is
def error_from_line(array_val):
    if 1 in array_val:
        pos = [-4, -3, -2, -1, 0, 0, 1, 2, 3, 4]
        high_val_index = [i for i, x in enumerate(array_val) if x == 1]
        high_val_pos = []
        for i in range(len(high_val_index)):
            high_val_pos.append(pos[high_val_index[i]])
        if (max(high_val_pos) > abs(min(high_val_pos))) or (max(high_val_pos) == abs(min(high_val_pos))):
            error = max(high_val_pos)
        else:
            error = min(high_val_pos)
    else:
        error = 1000 #line not found
    return error
#PID function determine motor speed
last_error = 0 #memory for pid
def motor_speed(error):
    if error != 1000:
        # the pid constants might need to be changed depending on the speed and size of the rover
        Kp = 5*0.5
        Ki = 0 #no integration required for slow rover
        Kd = 2*0.5
        global last_error
        derivative = error + last_error
        correction = Kd*derivative + Kp*error
        max_speed = 50 #rover needs to move slow
        min_speed = -50 #reverse
        lmotor = int(max_speed + correction)
        rmotor = int(max_speed-correction)
        last_error += error
        motor_speed = [lmotor,rmotor]
        for i in range(2):
            if motor_speed[i] < min_speed:
                motor_speed[i] = min_speed
            elif motor_speed[i] > max_speed:
                motor_speed[i] = max_speed
    else:
        motor_speed = [0,0]
    return motor_speed

#now UART COMM##
uart_motor = UART(2, baudrate=9600, tx=23)
uart_arm = UART(1, baudrate=9600, tx=22, rx=32)
uart_cam = UART(2, baudrate=9600, rx=33)

#mac address of my controller
mac = b'\x00K\x12<\xf28'
recv_error = 0 # to neglect tiny misses by controller signal and wait before going into lfr mode
motor_data = '0,0' #default to stop motors when nothing is happening


while True:
    try:
        try:
            host, msg = espcom.recv(100)
            if host==mac:
                controller = True #controller is being used
                try:
                    recv_data = json.loads(msg)
                    recv_error = 0
                except Exception as e:
                    controller = False #controller isn't being used
            else:
                controller = False
        except Exception as e:
            controller = False
        if controller:
            motor_data = ','.join(str(info) for info in control(recv_data))+'\n'
            time.sleep(0.07)
        else:
            recv_error+=1
            if recv_error >=3:
                motor_data = ','.join(str(info) for info in motor_speed(error_from_line(array_val())))+'\n'
                time.sleep(0.25)
            else:
                time.sleep(0.07)
        print(data)
        uart_motor.write(motor_data.encode())
    except Exception as e:
        print(e)
        time.sleep(1)
