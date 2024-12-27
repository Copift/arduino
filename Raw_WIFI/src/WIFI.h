#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;    
WiFiClient wifiClient;

String ip = "(IP unset)"; 

String id(){
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  return macID;
}

bool StartAPMode(String ssid,String password) {
  IPAddress apIP(192, 168, 4, 1);
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP((ssid + "_" + id()).c_str(), password.c_str());
  Serial.println("");
  Serial.println("WiFi up in AP mode with name: "+ ssidAP + "_" + id());
  return true;
}

bool StartCLIMode(String ssidC,String passwordC) {
  wifiMulti.addAP(ssidC.c_str(), passwordC.c_str());
  long lastMillis=millis();
  int attempts = 30;
  int ledState=LOW;
  while(wifiMulti.run() != WL_CONNECTED) {


  }
  return wifiMulti.run() == WL_CONNECTED;
}

void WIFI_init(bool mode_ap) {
    if (mode_ap) {
 //     StartAPMode();
      ip = WiFi.softAPIP().toString();
    } else {
   //   StartCLIMode();
      ip = WiFi.localIP().toString();
    }
    Serial.println("IP address: ");
    Serial.println(ip);  
}