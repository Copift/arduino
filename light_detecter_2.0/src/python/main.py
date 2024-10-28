import serial
import time
responses = {'d': 7,  # "led_off"
             'u': 6,  # "led_on"
             'p': 4}  # 0 -> 1023 zero fill to leftside
port_led = "COM6" # "COM4" or similar for windows
port_photo = "/dev/ttyUSB0" # "COM4" or similar for windows
#connection_led = serial.Serial(port_led, timeout=1) # baudrate=9600
connection_photo = serial.Serial(port_photo, timeout=10,baudrate=115200) # baudrate=9600
time.sleep(5)
print('sleep end')
def send_command(cmd, connection: serial.Serial):
    connection.write(cmd)

    resp: bytes = connection.read()
    return resp

while True:
    photo_val_resp: bytes = send_command(b'p',connection_photo)
    print(int.from_bytes(photo_val_resp))