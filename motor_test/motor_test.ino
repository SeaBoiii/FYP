#include <A4988.h>

#define MOTOR_STEPS 200

A4988 focus_motor(MOTOR_STEPS, 3, 4, 11, 10, 9);

void setup() {
  // put your setup code here, to run once:
  focus_motor.begin(1, 1);
  focus_motor.rotate(45);
  delay(1000);
  focus_motor.setMicrostep(1/2);
  focus_motor.rotate(45);
  delay(1000);
  focus_motor.setMicrostep(1/4);
  focus_motor.rotate(45);
  delay(1000);
  focus_motor.setMicrostep(1/8);
  focus_motor.rotate(45);
  delay(1000);
  focus_motor.setMicrostep(1/16);
  focus_motor.rotate(45);
  delay(5000);
  focus_motor.setMicrostep(1);
  focus_motor.move(10);
  delay(1000);
  focus_motor.setMicrostep(1/2);
  focus_motor.move(10);
  delay(1000);
  focus_motor.setMicrostep(1/4);
  focus_motor.move(10);
  delay(1000);
  focus_motor.setMicrostep(1/8);
  focus_motor.move(10);
  delay(1000);
  focus_motor.setMicrostep(1/16);
  focus_motor.move(10);
}

void loop() {
  // put your main code here, to run repeatedly:
}
