#include <Arduino.h>
#include "Config.h"
#include "WIFI.h"
#include "Server.h"
#include "MQTT.h"
#include "EEPROM.h"
#include <ESP8266WebServer.h>
#define LED_PIN 2  // Встроенный светодиод (D4)
#define EEPROM_SIZE 96//ппамять под сохранение

char savedSSID[32];//сохранение название вай фай
char savedPassword[32];//сохранение пароля
bool isConnected = false;//переменная подключения к wifi
String ssid = "ESP_Default"; // Название AP
String password = "password123"; // Пароль AP
ESP8266WebServer server(80);
void saveWiFiConfig(const char* ssid, const char* pass) { //функция сохранения в память данных о вай фай
  EEPROM.begin(EEPROM_SIZE);
  memset(savedSSID, 0, sizeof(savedSSID));
  memset(savedPassword, 0, sizeof(savedPassword));
  strncpy(savedSSID, ssid, sizeof(savedSSID) - 1);
  strncpy(savedPassword, pass, sizeof(savedPassword) - 1);
  for (int i = 0; i < 32; i++) {
    EEPROM.write(i, savedSSID[i]);
    EEPROM.write(32 + i, savedPassword[i]);
  }
  EEPROM.commit();
    EEPROM.end();
}

// Загрузка данных из EEPROM
void loadWiFiConfig() {
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 32; i++) {
    savedSSID[i] = EEPROM.read(i);
    savedPassword[i] = EEPROM.read(32 + i);
  }
  EEPROM.end();
}

//выдача основной странички с вводом данных
void handleRoot() {
  String html = R"=====(
    <html>
      <body>
        <h2>Configure WiFi</h2>
        <form action="/configure" method="POST">
          SSID: <input type="text" name="ssid"><br>
          Password: <input type="password" name="password"><br>
          <input type="submit" value="Save and Connect">
        </form>
      </body>
    </html>
  )=====";

  server.send(200, "text/html", html);
  Serial.println("get root html ");
}

// 404 ошибка
void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");
}

//подключение к вай фай
bool connectToWiFi() {
  WiFi.begin(savedSSID, savedPassword);
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(savedSSID);
  int delayT=600//задержка между попытками подключения
  int attempts = 15;// количество попыток
  while (WiFi.status() != WL_CONNECTED && attempts > 0) {
    if (attempts%2==0){//моргание при подключении
      delayT=300;
    }else{
      delayT=600;
    }
    digitalWrite(2,LOW);
    attempts--;
    delay(delayT);
    digitalWrite(2,HIGH);
    delay(delayT);
    Serial.print("attempt: ");
    Serial.println(attempts);
  }

  return WiFi.status() == WL_CONNECTED;
}

//функция загрузки и проверок подключения вызывается при изменении подключений
void boot();

// Обработчик сохранения данных вай фай
void handleConfigure() {

  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    saveWiFiConfig(ssid.c_str(), password.c_str());// вызываем сохранение
    server.send(200, "text/html", "Configuration saved. Restarting...");
    delay(2000);
    boot();//вызываем перезагрузку
  } else {
    server.send(400, "text/html", "Missing SSID or Password");
  }
}

//запуск веб сервера
void server_init() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/configure", HTTP_POST, handleConfigure);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

//функция загрузки и проверок подключения вызывается при изменении подключений
void boot(){
  loadWiFiConfig();//загружаем из памяти настройки вай фай
  Serial.println("Start boot");

  if (strlen(savedSSID) > 0 && strlen(savedPassword) > 0) {
      Serial.println("load settings complete. wifi settings  ");
      Serial.print(savedSSID);
      Serial.print(" passwd: ");
      Serial.println(savedPassword);
      isConnected = connectToWiFi();//подключаемся
  }

  if (isConnected) {
    //подключаем mqtt  и отключаем  AP
    Serial.println("Connected to Wi-Fi");
    Serial.println(WiFi.localIP());
    WiFi.softAPdisconnect (true);
    MQTT_init();
  } else {
    // Поднятие точки доступа
    Serial.println("Starting Access Point");
    StartAPMode(ssid,password);
    Serial.println(WiFi.softAPIP());
  }
  server_init();
 }
void setup(void){
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  pinMode(led, OUTPUT);
  boot();
}
long lastMillis=millis();
int ledState=LOW;
int c=3;//множитель задержки для двойного мигания
int count=1;
void loop(void){
  server.handleClient();    //http сервер
  client.loop();//mqtt
  if (isConnected){
      if (WiFi.status() != WL_CONNECTED){
        Serial.println("wifi disconnected, reboot... ");
        boot();
      }
      // Управляем светодиодом
      if (millis() - lastMillis >=(long) 500*c) {
        lastMillis = millis();
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }
        if (ledState==HIGH){
        count+=1;
        }
        digitalWrite(2, ledState);
        if (count==2){
          c=4;
          count=0;
        }else{
          c=1;
        }
      }
  }else {// если не подклчючены к  wifi
      // Управляем светодиодом
      if (millis() - lastMillis >= 1000) {
        lastMillis = millis();
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }
        digitalWrite(2, ledState);
      }


  }


}