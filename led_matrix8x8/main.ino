#include <Arduino.h>

const int IMAGE1[6][6] = {
  {1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 1},
  {1, 0, 1, 1, 0, 1},
  {1, 0, 1, 1, 0, 1},
  {1, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1}};

const int IMAGE2[6][6] = {
  {0, 0, 1, 1, 0, 0},
  {0, 1, 0, 0, 1, 0},
  {1, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 1},
  {0, 1, 0, 0, 1, 0},
  {1, 0, 1, 1, 0, 0}};

const int m[6] = {2, 3, 4, 5, 6, 7};
const int p[6] = {8, 9, 10, 11, 12, 13};

void setup()
{
  Serial.begin(9600);
  for (int i = 0; i<6; i++)
  {
    pinMode(p[i], OUTPUT);
    pinMode(m[i], OUTPUT);
  }
}

void light(int X)
{
  for (int i = 0; i<6; i++)
  {
    if(i!=X)
    {
      digitalWrite(p[i], HIGH);
    }
  }
  for (int i = 0; i<6; i++)
  {
    if(IMAGE2[X][i]==1)
    {
      digitalWrite(m[i], HIGH);
    }
    else
    {
        digitalWrite(m[i], LOW);
    }
  }
  digitalWrite(p[X], LOW);
}

void loop() {
  for (int i = 0; i<6; i++)
  {
      light(i);
      int t_start = millis();
      int t_total = millis();
      while (t_total-t_start<1){t_total = millis();}
  }
}