import time
import paho.mqtt.client as mqtt_client
import random
from uuid import getnode as get_mac
import hashlib

broker="broker.emqx.io"

h = hashlib.new('sha256')
mac = get_mac()
h.update(str(mac).encode())
pub_id = h.hexdigest()[:10]
print(f"Listen me at id {pub_id}")

client = mqtt_client.Client(
    mqtt_client.CallbackAPIVersion.VERSION2,
    pub_id
)

print("Connecting to broker",broker)
print(client.connect(broker))
client.loop_start()
print("Publishing")

import time



start_second = 20  # Начальная секунда включения лампочки
light_duration = 20  # Начальное время свечения лампочки
min_duration = 10  # Минимальное время свечения лампочки

while True:
    current_time = time.localtime()  # Получаем текущее время
    current_second = current_time.tm_sec  # Текущая секунда

    # Лампочка горит, если текущая секунда в пределах времени свечения
    if start_second <= current_second < start_second + light_duration:
        print('on')
        client.publish(f"esp8266/commands", "on")
    else:
        print('off')
        client.publish(f"esp8266/commands", "off")
    # Проверяем окончание минуты для уменьшения времени свечения
    if current_second == 59:
        if light_duration > min_duration:
            light_duration -= 1  # Уменьшаем время свечения на 1 секунду
        else:
            light_duration = 20  # Сбрасываем время свечения на 20 секунд
        print(f"Новое время свечения: {light_duration} секунд")
    time.sleep(1)

client.disconnect()
client.loop_stop()