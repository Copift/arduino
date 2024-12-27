#include <Arduino.h>
#include "Config.h"
#include "WIFI.h"
#include "Server.h"
#include "MQTT.h"
#include "EEPROM.h"
#include <ESP8266WebServer.h>
#define LED_PIN 2  // Встроенный светодиод (D4)
#define EEPROM_SIZE 96
char savedSSID[32];
char savedPassword[32];
bool isConnected = false;
String ssid = "ESP_Default"; // Название AP
String password = "password123"; // Пароль AP
ESP8266WebServer server(80);    
void saveWiFiConfig(const char* ssid, const char* pass) {
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
void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); 
}

void blinkLED(int onTime, int offTime) {
  digitalWrite(LED_PIN, HIGH);
  delay(onTime);
  digitalWrite(LED_PIN, LOW);
  delay(offTime);
}

bool connectToWiFi() {
  WiFi.begin(savedSSID, savedPassword);
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(savedSSID);
  int delayT=600;
  int attempts = 15;
  while (WiFi.status() != WL_CONNECTED && attempts > 0) {
    if (attempts%2==0){
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

void twoBlinkDelay(){
  long timeNow=millis();
  digitalWrite(2,LOW);
  for (size_t i = 0; i < 2; i++)
  {
     
  while (millis()<timeNow+(long)500){

  }
  timeNow=millis();
digitalWrite(2,HIGH);
while (millis()<timeNow+(long)500){

  }

}
timeNow=millis();
while (millis()<timeNow+(long) 4000){

  }
}
void boot();

// Обработчик сохранения данных Wi-Fi
void handleConfigure() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    saveWiFiConfig(ssid.c_str(), password.c_str());
    server.send(200, "text/html", "Configuration saved. Restarting...");
    delay(2000);
    boot();
  } else {
    server.send(400, "text/html", "Missing SSID or Password");
  }
}



void handleLED() {                          
  digitalWrite(led, !digitalRead(led));
  server.sendHeader("Location","/"); // redirection to keep button on the screen
  server.send(303);
}

void handleSENSOR() {                          
  int data = analogRead(A0);
  //server.sendHeader("Location","/");
  server.send(200, "text/html", String(data));
}


void server_init() {
  server.on("/", HTTP_GET, handleRoot);     
  server.on("/LED", HTTP_POST, handleLED);  
  server.on("/SENSOR", HTTP_GET, handleSENSOR);  
  server.on("/configure", HTTP_POST, handleConfigure);
  server.onNotFound(handleNotFound);        

  server.begin();                          
  Serial.println("HTTP server started");    
}
 void boot(){
    loadWiFiConfig();
  Serial.println("Start boot");
  if (strlen(savedSSID) > 0 && strlen(savedPassword) > 0) {
      Serial.println("load settings complete. wifi settings  ");
      Serial.print(savedSSID);
      Serial.print(" passwd: ");
            Serial.println(savedPassword);
    isConnected = connectToWiFi();
  }
  if (isConnected) {
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
  //WIFI_init(true);
  boot();
 
  //mqtt_cli.publish("esp8266/state", "hello emqx");
}
long lastMillis=millis();
int ledState=LOW;
int c=3;
int count=1;
void loop(void){
  server.handleClient();    
  
client.loop();

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
}else{

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