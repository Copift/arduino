import datetime
import time
from collections import deque

import paho.mqtt.client as mqtt_client
import random
from uuid import getnode as get_mac
import hashlib
import serial
import time
import requests

h = hashlib.new('sha256')
mac = get_mac()
h.update(str(mac).encode())
pub_id = h.hexdigest()[:10]
payload = {
    "message": pub_id
}
response = requests.post("http://copift.ru:5000/refresh", json=payload)
if response.status_code == 200:
    print("OK")
else:
    print(f"Request failed with status code {response.status_code}")
    raise Exception("Can't publish id")


broker="broker.emqx.io"
port_photo = "/dev/ttyUSB0"
connection_photo = serial.Serial(port_photo, timeout=1,baudrate=115200) # baudrate=9600
client = mqtt_client.Client(
    mqtt_client.CallbackAPIVersion.VERSION2,
    pub_id
)

print("Connecting to broker",broker)
print(pub_id)
client.loop_start()
print("Publishing")
import time
stream_active=False
time.sleep(5)
values = []
# Обработчик сообщений для управления потоком
def on_message(client, userdata, msg):
    global stream_active
    if msg.topic == f"lab/{pub_id}/photo/activate_stream":
        command = msg.payload.decode()
        if command == "on":
            stream_active = True
        elif command == "off":
            stream_active = False

client.subscribe(f"lab/{pub_id}/photo/activate_stream")

def send_command(cmd, connection: serial.Serial):
    connection.write(cmd)
    resp: bytes = connection.read()
    return resp
time_start=datetime.datetime.now()
sum_time=0.0
count=0
while True:
    time_seek = datetime.datetime.now()
    photo_val_resp: bytes = send_command(b'p',connection_photo)
    value=int.from_bytes(photo_val_resp)
    time_now=datetime.datetime.now()
    if len(values) == 100:
        values.pop(0)
    #print(f'value {value} {photo_val_resp}, stream: {stream_active}')
    values.append(value)
    if stream_active:
        client.publish(f"lab/{pub_id}/photo/activate_stream",value)
    if (time_now - time_start).total_seconds() > 1:
        time_start=time_now
        client.publish(f"lab/{pub_id}/photo/instant", value)
        client.publish(f"lab/{pub_id}/photo/average",sum(values)/100)
        print(f'publish {value} instant, average')
    sum_time+=(datetime.datetime.now()-time_seek).total_seconds()
    count+=1
    if count%10000 == 0:
        print(f'    average time for 1 iteration:{round(sum_time/count,5)}')



client.disconnect()
client.loop_stop()