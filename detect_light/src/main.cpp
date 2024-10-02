#include <Arduino.h>

#define leftPin A5
#define rightPin A0
#define leftLed 6
#define serviceLed 7
#define rightLed 8
// минимум максимум для датчиков

// 0 - maxLeft 1 - minLeft 2 - maxRight 3 - minRight массив для хранения калибровок датчиков
int values[] = {0, 0, 0,0}; 
// время на калибровку
int delayForCalibrate=5000;
// время на смену позиции с светлого на черное
int delayForSwithPos=1200;
void setup() {
  pinMode(leftLed, OUTPUT);
  pinMode(rightLed, OUTPUT);
  pinMode(serviceLed, OUTPUT);
  digitalWrite(serviceLed,HIGH);
  //delay(delayForSwithPos);
  // change delay to millis
  long start=millis();
  while (start+delayForSwithPos>=millis()){
  }
  // 
  Serial.begin(9600);
 

  //переменные для цикла переключают  датчики  и светодиоды в цикле
  int ledNow=leftLed;
  int pinNow=leftPin;

  // цикл калибровки
  for (size_t i = 0; i < 4; i++)
  {
    //ждем смену состояния 
    digitalWrite(serviceLed,LOW);
//    delay(delayForSwithPos);
       // change delay to millis
      long start=millis();
      while (start+delayForSwithPos>=millis()){
      }

    digitalWrite(serviceLed,HIGH);
    //переключаем пины
    if (i==2){
      ledNow=rightLed;
      pinNow=rightPin;
    }
  // переключаем состояние с светолого на темное
  if (i==1 or i ==3){
    digitalWrite(ledNow,LOW);
  }else {
    digitalWrite(ledNow,HIGH);
  }
  //для подсчета среднего значения
  long count=0;
  long sum=0;
  long startCalibrate=millis();
  //цикл на калибровку одного состояния
  while (startCalibrate+delayForCalibrate>=millis()){
  
   sum+= analogRead(pinNow);
    count+=1;
  }
  //считаем среднее
  values[i]=sum/count;
  //Serial.println(values[i]);
  digitalWrite(ledNow,LOW);

}
//выводим
Serial.print("values");
for (size_t i = 0; i < 4; i++)
{
  Serial.print(values[i]);
  Serial.print("  ");
  Serial.println();

}
  digitalWrite(serviceLed,LOW);
}


void loop() {
  float left = analogRead(leftPin);
  float right = analogRead(rightPin);

  float left_p = right/(values[0] - values[1])*100;
  float right_p = right/(values[2] - values[3])*100;

  Serial.print("left ");
  Serial.print(left);
  Serial.print("% right ");
  Serial.print(right);
  Serial.println("%");

  if (left_p - right_p > 3.0)
  {
    digitalWrite(rightLed,LOW);
    digitalWrite(leftLed,HIGH);
  }
  else if (left_p - right_p < -3.0)
  {
    digitalWrite(leftLed,LOW);
    digitalWrite(rightLed,HIGH);
  }
  else
  {
    digitalWrite(leftLed,HIGH);
    digitalWrite(rightLed,HIGH);
  }


  delay(3000);
}