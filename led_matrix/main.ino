#include <Arduino.h>

const int D_7 = 6;
const int D_8 = 7;
const int D_11 = 3;
const int D_12 = 4;

const int ZERO[4] = {0, 0, 0, 0};

const int MODES[8][4] = {
  {1, 0, 0, 1}, // 1
  {1, 0, 1, 0}, // 2
  {0, 1, 0, 1}, // 3
  {0, 1, 1, 0}, // 4

  {1, 0, 0, 0}, // 1, 2
  {0, 1, 0, 0}, // 3, 4
  {1, 1, 0, 1}, // 1, 3
  {1, 1, 1, 0}}; // 2, 4

const int MODES_BLINKING[8][4] = {
  {1, 0, 0, 1}, // 1, 4
  {0, 1, 1, 0}, // 2, 3
  {1, 1, 1, 0}, // 1, 2, 3
  {0, 1, 1, 1}, // 2, 3, 4

  {1, 0, 1, 1}, // 3, 4, 1
  {1, 1, 0, 1}, // 4, 1, 2
  {1, 1, 1, 1}, // 1, 2, 3, 4
  {0, 0, 0, 0}}; // no one

void setup() 
{ 
  Serial.begin(9600);
  pinMode(D_7, OUTPUT);
  pinMode(D_8, OUTPUT);
  pinMode(D_11, OUTPUT);
  pinMode(D_12, OUTPUT);
}

void is_light_simple(const int on_off, const int led)
{
  if(on_off)
  {
    digitalWrite(led, HIGH);
  }
  else
  {
    digitalWrite(led, LOW);
  }
}

void led_simple(const int arr[4])
{
  is_light_simple(arr[0], 3);
  is_light_simple(arr[1], 4);
  is_light_simple(arr[2], 6);
  is_light_simple(arr[3], 7);
}

void is_light_comp(const int on_off, const int led)
{
  if(on_off)
  {
    led_simple(MODES[led]);
  }
  else
  {
    led_simple(ZERO);
  }
}

void led_comp(const int arr[4])
{
  int t_start = millis();
  int t_total = millis();
  while(t_total-t_start<600)
  {
        for (int i = 0; i<4; i++)
        { is_light_comp(arr[i], i);
          int t_start_i = millis();
          int t_total_i = millis();
          while (t_total_i-t_start_i<2){t_total_i = millis();}
        }
        t_total = millis();
  }

}

void loop() {
  for (int i = 0; i<8; i++)
  {
          led_simple(MODES[i]);
          int t_start = millis();
          int t_total = millis();
          while (t_total-t_start<600){t_total = millis();}
  }
  for (int i = 0; i<8; i++)
  {
    led_comp(MODES_BLINKING[i]);
  }
}