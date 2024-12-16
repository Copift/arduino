// 
// 00_led_blink.ino
//

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include "config.h"
#define BUILTIN_LED 2
WiFiClient espClient;
PubSubClient client(espClient);
String clientId = String(ESP.getChipId());

int  delayMS = 1000;  // Задаержка в мс между публикацией сообщений
long lastMsg = 0;      // Время публикации предыдущего сообщения  (мс) 
int  value = 0;        // Переменная для формирования публикуемого сообщения
//настройка wifi
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

     
      client.publish(mqtt_topic_status, clientId.c_str());

     
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F("try again in 5 seconds"));
      delay(5000);
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
//генерация рандомного topic
static void get_random_string(char *str, unsigned int len)
{
    unsigned int i;

  
    srand(time(NULL));
    
    for (i = 0; i < len; i++)
    {
       
           str[i] =random(97,122);
    }
    str[i] = '\0';
}
  static char topicR[21];

void setup() {
  pinMode(A0, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);     
  digitalWrite(BUILTIN_LED, LOW);  

  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
get_random_string(topicR, 10);
//отправка сгенерированного топика на сервер
    HTTPClient httpClient;
    // Connect to server and try to send the message
    httpClient.begin(espClient,"http://copift.ru:5000/refresh");
  
     httpClient.addHeader("Content-Type", "application/json");
      int httpResponseCode = httpClient.POST("{\"message\":\"" + (String)topicR +"\"}");

String resultInfo;
    if (httpResponseCode > 0)
    {
        // Serial.println(httpClient.getString());
        resultInfo = "Success, HTTP Response Code: " + (String)httpResponseCode ;
    }
    else
    {
        resultInfo = "Error, HTTP Response Code: " + (String)httpResponseCode ;
    }

    httpClient.end();
  
  Serial.println(resultInfo);
  Serial.println(topicR);
}

  

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Публикация сообщения с заданной периодичностью
  long now = millis();
  if (now - lastMsg > delayMS) {
    lastMsg = now;
    int value=analogRead(A0);
    Serial.println(value);


    // Формирование сообщения и его публикация 
    char msg[200];
    snprintf (msg, sizeof(msg), "%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(topicR, msg);
 

  }
}