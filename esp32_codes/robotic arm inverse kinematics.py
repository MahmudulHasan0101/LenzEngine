import machine, gc, time, network, json, espnow, math
from machine import Pin, time_pulse_us, UART, PWM
#6DOF arm servo pins
base = PWM(Pin(15), freq=50)
shoulder = PWM(Pin(2), freq=50)
elbow = PWM(Pin(4), freq=50)
wrist = PWM(Pin(16), freq=50)
wrist_pitch = PWM(Pin(5), freq=50)
claw = PWM(Pin(22), freq=50)
'''
Inverse Kinematics Formula
'''
#func for robotic arm angles of 3 joints using angle and distance values
def arm(angle, distance): #distance in cm
    l1 = 30 #distance from shoulder's servo point to elbow's servo point
    l2 = 40 #distance from elbow's servo point to wrist's servo point
    claw_space = 2 #so the claw's servo head sits slightly before the object, so the extended claw can grab it
    angle = math.radians(angle)
    changed_angle = abs(math.atan((math.sin(angle)*distance-claw_space)/(math.cos(angle)*distance)))
    print(changed_angle)
    b = (math.sin(angle)*distance-claw_space)/(math.sin(changed_angle))#
    shoulder_angle = changed_angle + ((l1**2+b**2-l2**2)/(2*l1*b)) #done
    shoulder_angle = abs(math.degrees(shoulder_angle))
    elbow_angle = math.acos((l2**2+l1**2-b**2)/(2*l1*l2))#done
    elbow_angle = abs(math.degrees(elbow_angle))
    wrist_angle = 180-(360-(shoulder_angle+elbow_angle+90))
    return [shoulder_angle, elbow_angle, wrist_angle]
    #outputs 3 different angle, shoulder angle, elbow angle, wrist angle in degrees