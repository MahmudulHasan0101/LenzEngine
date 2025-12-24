from machine import Pin, PWM, UART
import time
time.sleep(1)
#ENA and ENB pins, here all motor's ena and enb pins are connected in parallel respectively to use only 2 pins as this code is made for tank wheel drive purpose
ENR = PWM(4, freq=1000) #ENA
ENL = PWM(2, freq=1000) #ENB


#i/o signal pins in motor driver [from left(in1,in2) pin for 3 motors to right(in1,in2) pin for 3 motors]
in_pins = [5, 18, 19, 21, 25, 26, 27, 14, 13, 12, 23, 22]
for i, val in enumerate(in_pins, start=1):
    globals()[f'in{i}'] = Pin(val, Pin.OUT) #assigning pins from the list with variable names in1, in2, ..., in12
    globals()[f'in{i}'].value(0)

#left in1 pins for multiple left side motors func
def left_in1(value):
    for i in range(len(in_pins)//2):
        if i%2 == 0:
            globals()[f'in{i+1}'].value(value)

#left in2 pins for multiple left side motors func
def left_in2(value):
    for i in range(len(in_pins)//2):
        if i%2 != 0:
            globals()[f'in{i+1}'].value(value)

#right in1 pins for multiple left side motors func
def right_in1(value):
    for i in range(len(in_pins)//2):
        if i%2 == 0:
            globals()[f'in{i+(len(in_pins)//2)+1}'].value(value)

#right in2 pins for multiple left side motors func
def right_in2(value):
    for i in range(len(in_pins)//2):
        if i%2 != 0:
            globals()[f'in{i+(len(in_pins)//2)+1}'].value(value)


#UART
uart = UART(2, baudrate=9600, tx=17, rx=16)
print("✅ UART ready. Send commands in format: [left,right]")

#separating 2 values from str 'left_motor, right_motor' from UART
def event(data):
    values = [int(x) for x in data.split(',')]
    if values[0] < 0:
        left_in2(1)
        left_in1(0)
        right_in2(0)
        right_in1(1)
    if values[1] < 0:
        left_in2(1)
        left_in1(0)
        right_in2(0)
        right_in1(1)
    ENL.duty(int((abs(values[0])/100)*1023))
    ENR.duty(int((abs(values[1])/100)*1023))
    print(values)

#buffer for uart, expected data, data = '{left_motor},{right_motor}\n'
buffer = b''
while True:
    if uart.any():
        buffer+=uart.read()
        if b'\n' in buffer:
            lines = buffer.split(b'\n')
            for data in lines[:-1]:
                data = data.decode().strip()
            buffer = lines[-1]
            event(data)
        else:
            print('noob')
    time.sleep(0.125)