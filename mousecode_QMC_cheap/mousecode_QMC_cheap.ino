#include <Wire.h> //I2C Arduino Library

// I2C 7-Bit Address Qmc5883L
#define QMC5883_ADDR 0x0D

#include "Mouse.h"
#include "Keyboard.h"

#define MLEFT 9
#define MMIDD 8
#define MRIGH 7

float x, y;

int mx,my,mz;

float hx = 0.0;
float last_hx = 0.0;
float hy = 0.0;
float last_hy = 0.0;

void setup() {
  pinMode(MLEFT, INPUT_PULLUP);
  pinMode(MMIDD, INPUT_PULLUP);
  pinMode(MRIGH, INPUT_PULLUP);
  RXLED0;
  TXLED0;

  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(QMC5883_ADDR); //start talking
  Wire.write(0x0B);
  Wire.write(0x01);
  Wire.endTransmission();
  Wire.beginTransmission(QMC5883_ADDR); //start talking
  Wire.write(0x09);
  Wire.write(0x1D);
  Wire.endTransmission();
  
  delay(10);
  Mouse.begin();
  Keyboard.begin();
}

void loop() {
  Wire.beginTransmission(QMC5883_ADDR);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(QMC5883_ADDR, 6);
  if (6 <= Wire.available()) {
    mx = Wire.read() | (Wire.read() << 8);
    mz = Wire.read() | (Wire.read() << 8);
    my = Wire.read() | (Wire.read() << 8);
  }

  hx = atan2(my,mx) * 100;
  hy = atan2(my,mz) * 100;

  x += (hx - last_hx);
  y += (hy - last_hy);

  // auto slow down
  x = x * 0.99;
  y = y * 0.99;

  if(x >  50) x=50;
  if(y >  50) y=50;
  if(x < -50) x=-50;
  if(y < -50) y=-50;

  Mouse.move(x/10, y/10, 0);
  
  if (digitalRead(MLEFT) == LOW) {
    TXLED1;
    if (!Mouse.isPressed(MOUSE_LEFT)) {
      Mouse.press(MOUSE_LEFT);
    }
  } else {
    TXLED0;
    if (Mouse.isPressed(MOUSE_LEFT)) {
      Mouse.release(MOUSE_LEFT);
    }
  }

  if (digitalRead(MRIGH) == LOW) {
    RXLED1;
    if (!Mouse.isPressed(MOUSE_RIGHT)) {
      Mouse.press(MOUSE_RIGHT);
    }
  } else {
    RXLED0;
    if (Mouse.isPressed(MOUSE_RIGHT)) {
      Mouse.release(MOUSE_RIGHT);
    }
  }

  if (digitalRead(MMIDD) == LOW) {
    if (y < 10) {
      Keyboard.press(KEY_UP_ARROW);
      delay(200);
      Keyboard.release(KEY_UP_ARROW);
    }
    if (y > 10) {
      Keyboard.press(KEY_DOWN_ARROW);
      delay(200);
      Keyboard.release(KEY_DOWN_ARROW);
    }
  }

  last_hx = hx;
  last_hy = hy;
  delay(5);
}
