#include <Arduino.h>
#define leftPin A5
void setup() {
    Serial.begin(9600);
 
}

void loop() {
  if(Serial.available()>0){
    byte teststr = Serial.read();
  
    if (teststr==112){
     
    int left = analogRead(leftPin);
    

    Serial.println(left);
    }
    else{
        Serial.println("unknown command");
    }


  }
  }