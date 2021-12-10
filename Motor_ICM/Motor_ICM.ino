// ICM Buddy for DSLR Cameras
// Uses 2 NEMA motors, 1 TFT 1.8 Screen and a joystick

#include <AccelStepper.h>
#include <TFT.h>
#include <SPI.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

// Motor Parameters
#define MOTOR_STEPS 200
#define RPM 120

// Arduino Pins
/* TFT Display Pins */
#define CS 10
#define DC 9
#define RST 8

/* Motor Pins */
#define rear_DIR 2
#define rear_STEP 3
#define front_DIR 4
#define front_STEP 5

/* Joystick Pins */
#define UP    A0
#define DOWN  A1
#define LEFT  A2
#define RIGHT A3
#define SET   A4 

// Strings
/* PRGMEM Bugger */
char buffer[50];

/* Defining Strings */


// Object Declaration
/* Motor Objects */
AccelStepper rear_motor(AccelStepper::FULL2WIRE, rear_DIR, rear_STEP);
AccelStepper front_motor(AccelStepper::FULL2WIRE, front_DIR, front_STEP);

/* Display Object */
TFT tft(CS, DC, RST);

// EEPROM declaration (0-255 int)
/* Things that are saved
 *  - focus_range     (Max range of focus)
 *  - zoom_range      (Max range of zoom)
 *  - focus_current   (Current focus value)
 *  - zoom_current    (Current zoom value)
 *  - orientation     (Orientation of motors)
 *  - shutter_speed   (Shutter speed)
 */

void setup() {
  

}

void loop() {
  // put your main code here, to run repeatedly:

}
