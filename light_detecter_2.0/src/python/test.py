import serial
import time
responses = {'d': 7,  # "led_off"
             'u': 6,  # "led_on"
             'p': 4}  # 0 -> 1023 zero fill to leftside
port_led = "/dev/ttyUSB0" # "COM4" or similar for windows
port_photo =  "/dev/ttyUSB0" # "COM4" or similar for windows
#connection_led = serial.Serial(port_led, timeout=1) # baudrate=9600
connection_photo = serial.Serial(port_photo, timeout=1,baudrate=115200) # baudrate=9600

def send_command(cmd: str, response_len: int, connection: serial.Serial) -> str:
    connection.write(cmd.encode())
    if response_len > 0:
        # connection.in_waiting <-> available()
        resp = connection.read()
    return resp

while True:
    photo_val_resp: bytes = send_command('p', responses['p'], connection_photo)

    print(photo_val_resp.decode())