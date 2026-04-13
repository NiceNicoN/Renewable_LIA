#include <Servo.h>

Servo horizontal;
Servo vertical;

int ldrlt = A0;  // top-left
int ldrrt = A3;  // top-right
int ldrld = A1;  // bottom-left
int ldrrd = A2;  // bottom-right

int tol = 15;
int servoh = 100;
int servov = 100;

void setup() {
  horizontal.attach(4);
  vertical.attach(13);
  horizontal.write(servoh);
  vertical.write(servov);
  Serial.begin(9600);
}

void loop() {
  int lt = readAverage(ldrlt);
  int rt = readAverage(ldrrt);
  int ld = readAverage(ldrld);
  int rd = readAverage(ldrrd);
  
  int avt = (lt + rt) / 2;
  int avd = (ld + rd) / 2;
  int avl = (lt + ld) / 2;
  int avr = (rt + rd) / 2;
  
  int avgLight = (lt + rt + ld + rd) / 4;
  
  if (avgLight < 200) {
    Serial.println("Too dark");
    delay(5000);
    return;
  }
  
  int dvert = avt - avd;
  int dhoriz = avl - avr;
  
  if (abs(dvert) > tol) {
    if (dvert > 0) {
      if (servov > 20) servov--;
    } else {
      if (servov < 160) servov++;
    }
    vertical.write(servov);
  }
  
  if (abs(dhoriz) > tol) {
    if (dhoriz > 0) {
      if (servoh > 20) servoh--;
    } else {
      if (servoh < 160) servoh++;
    }
    horizontal.write(servoh);
  }
  
  delay(100);
  // ======== Debug Output ========
 Serial.print("TL: "); Serial.print(lt);
 Serial.print("  TR: "); Serial.print(rt);
 Serial.print("  BL: "); Serial.print(ld);
 Serial.print("  BR: "); Serial.print(rd);
 Serial.print("  |  dV: "); Serial.print(dvert);
 Serial.print("  dH: "); Serial.println(dhoriz);
 //Serial.print("  |  H: "); Serial.print(servohori);
 //Serial.print("°  V: "); Serial.println(servovert);
 // ======== Small Delay for Stability ========
 delay(2);
}

int readAverage(int pin) {
  long total = 0;
  for (int i = 0; i < 10; i++) {
    total += analogRead(pin);
    delay(2);
  }
  return total / 10;
}