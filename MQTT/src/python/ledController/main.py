import time
import paho.mqtt.client as mqtt_client
import random
from uuid import getnode as get_mac
import hashlib
import requests

broker="broker.emqx.io"
pub_id=""

response = requests.get('http://copift.ru:5000/get_id')
if response.status_code == 200:
    data = response.json()
    pub_id = data['pub_id']
    print(pub_id)
else:
    print(f"Request failed with status code {response.status_code}")

h = hashlib.new('sha256')
mac = get_mac()
h.update(str(mac).encode())
sub_id = h.hexdigest()[10:20]
def on_message(client, userdata, message):
    data = str(message.payload.decode("utf-8"))
    print("received message =", data)

client = mqtt_client.Client(
    mqtt_client.CallbackAPIVersion.VERSION2,
    sub_id
)
client.on_message=on_message

print("Connecting to broker",broker)
client.connect(broker)
client.subscribe(f"lab/{pub_id}/photo/average")
client.loop_start()
print("Subcribing")
time.sleep(1800)
client.disconnect()
client.loop_stop()
