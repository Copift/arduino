//
// 00_led_blink.ino
//
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include "config.h"
WiFiClient espClient;
PubSubClient client(espClient);
String clientId = String(ESP.getChipId());

long lastMsg = 0;      // Время публикации предыдущего сообщения  (мс)
int  value = 0;        // Переменная для формирования публикуемого сообщения

// Функция обработки входящих сообщений
void callback(char* topic, byte* payload, unsigned int length) {
  // Печать информации о полученном сообщенийй
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String result;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    result+=(char)payload[i];
  }
  Serial.println();
  Serial.println();
  int res=result.toInt();
  Serial.println(res);
  Serial.println();
  Serial.println();
  
    if (res < 100) {
      digitalWrite(2, HIGH); 
    }
    else {
      digitalWrite(2, LOW);
    }
  
}



// Функция установления соединения по WiFi
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
void reconnect(char* mqtt_topic) {
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
      client.subscribe(mqtt_topic);
      // Если нужно подписаться на несколько топиков, то для каждого из них вызываем client.subscribe()
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F("try again in 5 seconds"));
      delay(5000);
    }
  }
}
char mqtt_topic[1024];
// Функция настройки MCU
void setup() {
  pinMode(2, OUTPUT);    
  digitalWrite(2, HIGH);  

  Serial.begin(9600);
  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  //получение топика с сервера
       HTTPClient httpClient;
  httpClient.begin(espClient,"http://copift.ru:5000/get_id");


// Send HTTP GET request
int httpResponseCode = httpClient.GET();
 Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  String payload = httpClient.getString();
  Serial.println(payload);
  int a=payload.indexOf(":")+2;
  int b=payload.indexOf('"',a);
  payload=payload.substring(a,b);
  
strcpy(mqtt_topic, payload.c_str());
   digitalWrite(2, HIGH);  
  9
}

void loop() {
  if (!client.connected()) {
    reconnect(mqtt_topic);
  }
  client.loop();

}