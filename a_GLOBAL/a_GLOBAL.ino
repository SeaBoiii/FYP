// ICM Buddy for DSLR cameras
// uses 2 NEMA motors, SSD1306 screen and a joystick


// All the libraries used
#include <A4988.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

// defining OLED display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define WHITE SSD1306_WHITE
#define BLACK SSD1306_BLACK

// using a 200-step motor
#define MOTOR_STEPS 200

// RPM
#define RPM 120

// Arduino Pins
/* Motor Pins */
#define rear_DIR 3
#define rear_STEP 4
#define front_DIR 5
#define front_STEP 6

/* Joystick Pins */
#define VRX A0
#define VRY A1
#define SW 12

/* PRGMEM Buffer */
char buffer[50];

/* defining strings */
const char back[] PROGMEM = "Back";
const char range[] PROGMEM = "\nRange:  ";
const char joystick_0[] PROGMEM = "Use the joystick for controls";
const char joystick_1[] PROGMEM = "Move joystick left and right to adjust the turn.";
const char joystick_2[] PROGMEM = "Click joystick to cfm";
const char set_txt[] PROGMEM = "Set";
const char steps_txt[] PROGMEM = "Steps to move: ";
const char blank_txt[] PROGMEM = " ";

const char string_0[] PROGMEM = "|---- Main Menu ----|";
const char string_1[] PROGMEM = "Calibration";
const char string_2[] PROGMEM = "Basic Movements";
const char string_3[] PROGMEM = "Advanced Movements";
const char string_3_1[] PROGMEM = "Settings";

const char string_4[] PROGMEM = "|---Recalibration---|";
const char string_5[] PROGMEM = "Zoom Recalibration";
const char string_6[] PROGMEM = "Focus Recalibration";
const char string_7[] PROGMEM = "RESET all values";

const char string_8[] PROGMEM = "|---Basic Movement--|";
const char string_9[] PROGMEM = "Manual Zoom";
const char string_10[] PROGMEM = "Manual Focus";
const char string_11[] PROGMEM = "Zoom to fixed Dist.";

const char string_12[] PROGMEM = "|-Move to distance--|";
const char string_13[] PROGMEM = "Zoom to Max";
const char string_14[] PROGMEM = "Zoom to Min";
const char string_15[] PROGMEM = "Zoom to a distance";

const char string_16[] PROGMEM = "|-Advance Movement--|";
const char string_17[] PROGMEM = "Bokeh Effect";
const char string_18[] PROGMEM = "Firework Effect";
const char string_19[] PROGMEM = "Zoom Blur Effect";
const char string_20[] PROGMEM = "Sine Wave Effect";

const char string_21[] PROGMEM = "Set the ring \nto focus on \na subject";
const char string_22[] PROGMEM = "Set the ring \nas blur as \npossible";
const char string_23[] PROGMEM = "You are setting \nthe maximum turn \nfor zoom ring.";
const char string_24[] PROGMEM = "You are setting \nthe mininum turn \nfor zoom ring.";

const char string_25[] PROGMEM = "|---- Settings -----|";
const char string_26[] PROGMEM = "Zoom/Focus Position";
const char string_27[] PROGMEM = "Camera Shutter Speed";

const char string_28[] PROGMEM = "|----Positioning----|";
const char string_29[] PROGMEM = "Zoom at the back";
const char string_30[] PROGMEM = "Zoom at the front";

const char string_31[] PROGMEM = "|---Shutter Speed---|";
const char string_32[] PROGMEM = "1s";
const char string_33[] PROGMEM = "4s";
const char string_34[] PROGMEM = "8s";
const char string_35[] PROGMEM = "16s";

/* Setting up string table */
const char *const main_menu[] PROGMEM = {string_0, string_1, string_2, string_3, string_3_1};
const char *const recalibration_menu[] PROGMEM {string_4, string_5, string_6, string_7, back};
const char *const basic_menu[] PROGMEM = {string_8, string_9, string_10, string_11, back};
const char *const movetodist_menu[] PROGMEM = {string_12, string_13, string_14, string_15, back};
const char *const adv_menu[] PROGMEM = {string_16, string_17, string_18, string_19, string_20, back};
const char *const key_phrases[] PROGMEM = {back, range, set_txt, steps_txt, blank_txt};
const char *const ring_phrases[] PROGMEM = {string_21, string_22, string_23, string_24};
const char *const joystick_phrases[] PROGMEM = {joystick_0, joystick_1, joystick_2};
const char *const settings_menu[] PROGMEM = {string_25, string_26, string_27, back};
const char *const positioning_menu[] PROGMEM = {string_28, string_29, string_30, back};
const char *const shutter_menu[] PROGMEM = {string_31, string_32, string_33, string_34, string_35, back};

// A4988 stepper(MOTOR_STEPS, DIR, STEP, MS1, MS2, MS3)
// focus ring is infront compared to zoom ring
// motor objects
A4988 rear_motor(MOTOR_STEPS, rear_DIR, rear_STEP);
A4988 front_motor(MOTOR_STEPS, front_DIR, front_STEP);

// display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// EEPROM declaration (0-255 int)
/* Things that are saved
 *  - diff_focus    (Max range of focus)
 *  - diff_zoom     (Max range of zoom)
 *  - focus_min     (Last focus call)
 *  - zoom_min      (Last zoom call)
 */
int focus_min;      // address 0
int zoom_min;       // address 1
int diff_focus;     // address 2
int diff_zoom;      // address 3
int orientation;    // address 4
int shutter_data;   // address 5

// global variable declaration
int option = 0;
int focus_max = 0;
int zoom_max = 0;
float shutter_speed = 0.5;

int button = 1; // 0 for click
int x_value = 0;
int y_value = 0;

// function prototypes