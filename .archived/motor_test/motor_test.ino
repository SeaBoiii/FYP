/*
 * Using accelerated motion ("linear speed") in nonblocking mode
 *
 * Copyright (C)2015-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
// Target RPM for cruise speed
#define RPM 120
// Acceleration and deceleration values are always in FULL steps / s^2
#define MOTOR_ACCEL 500
#define MOTOR_DECEL 0

// Microstepping mode. If you hardwired it to save pins, set to the same value here.
#define MICROSTEPS 1

#define DIR 3
#define STEP 4
#define SLEEP 13 // optional (just delete SLEEP from everywhere if not used)

/*
 * Choose one of the sections below that match your board
 */

//#include "DRV8834.h"
//#define M0 10
//#define M1 11
//DRV8834 stepper(MOTOR_STEPS, DIR, STEP, SLEEP, M0, M1);

 #include "A4988.h"
 A4988 stepper(MOTOR_STEPS, DIR, STEP);

// #include "DRV8825.h"
// #define MODE0 10
// #define MODE1 11
// #define MODE2 12
// DRV8825 stepper(MOTOR_STEPS, DIR, STEP, SLEEP, MODE0, MODE1, MODE2);

// #include "DRV8880.h"
// #define M0 10
// #define M1 11
// #define TRQ0 6
// #define TRQ1 7
// DRV8880 stepper(MOTOR_STEPS, DIR, STEP, SLEEP, M0, M1, TRQ0, TRQ1);

// #include "BasicStepperDriver.h" // generic
// BasicStepperDriver stepper(DIR, STEP);

void setup() {
    Serial.begin(115200);

    stepper.begin(RPM, MICROSTEPS);
    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    // stepper.setEnableActiveState(LOW);
    stepper.enable();
    // set current level (for DRV8880 only). Valid percent values are 25, 50, 75 or 100.
    // stepper.setCurrent(100);

    /*
     * Set LINEAR_SPEED (accelerated) profile.
     */
    //stepper.setSpeedProfile(stepper.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);

    Serial.println("START");
    /*
     * Using non-blocking mode to print out the step intervals.
     * We could have just as easily replace everything below this line with 
     * stepper.rotate(360);
     */
     stepper.startMove(200,500000);
     unsigned wait_time = stepper.nextAction();
     while(wait_time > 0) {
      wait_time = stepper.nextAction();
     }
     //delay(5000);
     //stepper.rotate(360);
}

void loop() {
    //unsigned wait_time = stepper.nextAction();
}
