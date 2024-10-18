import serial
import time
responses = {'d': 7,  # "led_off"
             'u': 6,  # "led_on"
             'p': 4}  # 0 -> 1023 zero fill to leftside
port_led = "COM6" # "COM4" or similar for windows
port_photo = "COM7" # "COM4" or similar for windows
connection_led = serial.Serial(port_led, timeout=1) # baudrate=9600
connection_photo = serial.Serial(port_photo, timeout=1) # baudrate=9600

def send_command(cmd: str, response_len: int, connection: serial.Serial) -> str:
    connection.write(cmd.encode())
    if response_len > 0:
        # connection.in_waiting <-> available()
        resp: bytes = connection.read(response_len)
    return resp

while True:
    photo_val_resp: str = send_command('p', responses['p'], connection_photo)
    if photo_val_resp:
        photo_val = int.from_bytes(photo_val_resp,"big")
        print(photo_val)
        if photo_val > 50:
            resp = send_command('u', responses['u'], connection_led)
            print("up")
        else:
            resp = send_command('d', responses['d'], connection_led)
            print("down")