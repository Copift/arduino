#include <PubSubClient.h>
#include "Config_settings.h"

WiFiClient espClient;
PubSubClient client(espClient);
String clientId = String(ESP.getChipId());


void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    String result;
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
        result+=(char) payload[i];
    }
    if (result=="on"){
        digitalWrite(5,HIGH);
        Serial.println("   LED ON");
    }
    else{
  digitalWrite(5,LOW);
  Serial.println("    LED OFF");

    }
    Serial.println();

    Serial.println("-----------------------");
}


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

     
    //   client.publish(mqtt_topic_status, clientId.c_str());

     
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F("try again in 5 seconds"));
      delay(5000);
    }
  }
}

PubSubClient MQTT_init(){
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  int lastMillis=millis();
  while (!client.connected()) {
      String client_id = "esp8266-" + String(WiFi.macAddress());
      Serial.print("The client " + client_id);
      Serial.println(" connects to the public mqtt broker\n");
      if (client.connect(client_id.c_str())){
           client.subscribe("esp8266/commands");
        //   client.publish("esp8266/state", "hello emqx");

          Serial.println("MQTT Connected");

      } else {

  
      
          
          delay(2000);
      }
  } 
  return client; 
}