// 
// 00_led_blink.ino
//

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#define BUILTIN_LED 2
WiFiClient espClient;
PubSubClient client(espClient);
String clientId = String(ESP.getChipId());

int  delayMS = 10000;  // Задаержка в мс между публикацией сообщений
long lastMsg = 0;      // Время публикации предыдущего сообщения  (мс) 
int  value = 0;        // Переменная для формирования публикуемого сообщения
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(wifi_ssid);
  Serial.println(wifi_pass);
  WiFi.begin(wifi_ssid, wifi_pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(F("."));
    WiFi.printDiag(Serial);
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
}


// Функция установления подключения к MQTT брокеру
void reconnect() { 
  while (!client.connected()) {
    Serial.println(F("Attempting MQTT connection..."));
    Serial.print(F("Client ID:"));
    Serial.print(clientId);
    Serial.print(F(" "));
    Serial.print(mqtt_login);
    Serial.print(F(" "));
    Serial.print(mqtt_pass);
    Serial.print(F(" "));
    if (client.connect(clientId.c_str(), mqtt_login, mqtt_pass)) {  
      Serial.println(F("connected"));

      // Публикация сообщения с идентификаторм клиента в топик, заданный значением 'mqtt_topic_status'
      client.publish(mqtt_topic_status, clientId.c_str());

      // Подписка на сообщения в топике, заданном значением mqtt_topic_in
      client.subscribe(mqtt_topic_in);
      // Если нужно подписаться на несколько топиков, то для каждого из них вызываем client.subscribe()
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F("try again in 5 seconds"));
      delay(5000);
    }
  }
}
// Функция обработки входящих сообщений
void callback(char* topic, byte* payload, unsigned int length) {
  // Печать информации о полученном сообщенийй
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Если получено сообщение и у нас подписка на несколько топиков, то определяем в каком топике сообщение опубликовано
  if (strcmp(topic, mqtt_topic_in) == 0) { 
    // Определяем поведение MCU при различных значениях сообщения (payload)
    if ((char)payload[0] == '0') {
      digitalWrite(BUILTIN_LED, HIGH); // BUILTIN_LED имеет подтягивающий резистор, HIGH = OFF, LOW = ON
    }
    if ((char)payload[0] == '1') {
      digitalWrite(BUILTIN_LED, LOW);
    }
  }
}

// Функция настройки MCU
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Установка BUILTIN_LED как порт вывода
  digitalWrite(BUILTIN_LED, LOW);  // BUILTIN_LED имеет подтягивающий резистор, HIGH = OFF, LOW = ON

  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// Основная функция - вызывается на каждой итерации цикла работы MCU
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Публикация сообщения с заданной периодичностью
  long now = millis();
  if (now - lastMsg > delayMS) {
    lastMsg = now;
    ++value;

    // Формирование сообщения и его публикация 
    char msg[200];
    snprintf (msg, sizeof(msg), "heartbeat #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(mqtt_topic_out, msg);
  }
}