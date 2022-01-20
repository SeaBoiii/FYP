// ICM Buddy for DSLR Cameras
// Uses 2 NEMA motors, 1 TFT 1.8 Screen and a joystick

// Library Used
#include <AccelStepper.h>
#include <TFT.h>
#include <SPI.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

// Motor Parameters
#define MOTOR_STEPS 200
#define RPM 1000
#define FOCUS 0
#define ZOOM 1
#define MS_STEP 8 // default 1/8 microstep
#define CALI_ACCEL 400

// Arduino Pins
/* TFT Display Pins */
#define CS  10
#define DC  9
#define RST 8

/* Motor Pins */
#define rear_DIR    5
#define rear_STEP   6
#define front_DIR   3
#define front_STEP  4

/* Joystick Pins */
#define UP    A4
#define DOWN  A3
#define LEFT  A2
#define RIGHT A1
#define SET   A0 

/* Camera Control Pins */
#define SHUTTER A5

/* Buzzer Signal Pin */
#define BUZZER 2

// Strings
/* Colour Strings */
#define WHITE     0xFFFF
#define BLACK     0x0000
#define AQUA      0xFEC0
#define DEEPPINK  0xF8B2  
#define DARKGREEN 0x0320
#define RED       0xF800 
#define LIGHTSKYBLUE 0x867F 
#define SNOW 0xFFDF
#define VIOLET 0xEC1D 
#define YELLOWGREEN 0x9E66
#define GOLDENROD 0xF81F
#define AZURE 0xF7FF
#define CORAL 0xFBEA 
#define CADETBLUE 0x5CF4
#define LIME 0x07E0

/* Symbols */
#define UP_ARROW    0x18
#define DOWN_ARROW  0x19
#define RIGHT_ARROW 0x1A
#define LEFT_ARROW  0x1B
#define SELECT      0x2A

/* PRGMEM Bugger */
char buffer[50];

/* Defining Strings */
const char back[] PROGMEM = "Back";

const char mm_0[] PROGMEM = "|---- Main Menu ----|";
const char mm_1[] PROGMEM = "Camera Settings";
const char mm_2[] PROGMEM = "Motor Calibration";
const char mm_3[] PROGMEM = "POV Calibration";
const char mm_4[] PROGMEM = "Focus Movements";
const char mm_5[] PROGMEM = "Zoom Movements";
const char mm_6[] PROGMEM = "ZoomFocus Movements";
const char mm_7[] PROGMEM = "Fixed Patterns";
const char mm_8[] PROGMEM = "Customise Patterns";

const char string_4[] PROGMEM = "|---Calibration---|";
const char string_5[] PROGMEM = "Zoom Calibration";
const char string_6[] PROGMEM = "Focus Calibration";
const char string_7[] PROGMEM = "RESET all values";

const char string_12[] PROGMEM = "|---- Settings -----|";
const char string_13[] PROGMEM = "Swap Motor Position";
const char string_14[] PROGMEM = "Camera Shutter Time";
const char string_15[] PROGMEM = "Motor Movement Time";

const char shutter_menu[] PROGMEM = "|Shutter Time(in s)|";
const char motor_time_menu[] PROGMEM = "Motor Move Time(in s)";

const char string_19[] PROGMEM = "|--Focus Movements--|";
const char string_20[] PROGMEM = "Move to infinity";
const char string_21[] PROGMEM = "Move to min distance";
const char string_22[] PROGMEM = "Move to a Value";
const char string_41[] PROGMEM = "To infinity & back";
const char string_42[] PROGMEM = "To min. & back";
const char string_45[] PROGMEM = "From inf. to POV-min";

const char string_23[] PROGMEM = "|--Zoom Movements --|";
const char string_24[] PROGMEM = "Move to tele";
const char string_25[] PROGMEM = "Move to widest";
const char string_43[] PROGMEM = "To tele & back";
const char string_44[] PROGMEM = "To widest & back";

const char string_27[] PROGMEM = "|Zoom&Focus Movement|";
const char string_28[] PROGMEM = "Move to Max [BOTH]";
const char string_29[] PROGMEM = "Move to Min [BOTH]";
const char string_39[] PROGMEM = "Zoom[MAX]&Focus[MIN]";
const char string_40[] PROGMEM = "Zoom[MIN]&Focus[MAX]";
const char string_46[] PROGMEM = "to Max[BOTH] & back";
const char string_47[] PROGMEM = "to Min[BOTH] & back";
const char string_48[] PROGMEM = "Z[MAX]F[MIN] & back";
const char string_49[] PROGMEM = "Z[MIN]F[MAX] & back";

const char string_31[] PROGMEM = "|----- Presets -----|";
const char string_32[] PROGMEM = "Bokeh Effect";
const char string_33[] PROGMEM = "Firework Effect";
const char string_34[] PROGMEM = "Zoom Blur Effect";
const char string_35[] PROGMEM = "Sine Wave Effect";
const char string_50[] PROGMEM = "ZigZagger Out Effect";

const char cali_zoom[] PROGMEM = "|--Calibrate Zoom --|";
const char cali_focus[] PROGMEM = "|--Calibrate Focus--|";
const char string_cali[] PROGMEM = "Move the joystick to \n";
const char zoom_left[] PROGMEM = "the widest lens";
const char zoom_right[] PROGMEM = "the tele lens";
const char focus_left[] PROGMEM = "min. focal length";
const char focus_right[] PROGMEM = "   infinity";

const char counttext_1[] PROGMEM = "Get Ready!";
const char counttext_2[] PROGMEM = "3";
const char counttext_3[] PROGMEM = "2";
const char counttext_4[] PROGMEM = "1";
const char counttext_5[] PROGMEM = "SNAP!";

const char adjust_zoom[] PROGMEM = "|--- Adjust Zoom ---|";
const char adjust_focus[] PROGMEM = "|---Adjust Focus ---|";
const char string_36[] PROGMEM = "Adjust [ZOOM] ring";
const char string_36_1[] PROGMEM = "Adjust [FOCUS] ring";
const char string_37[] PROGMEM = "to the desired POV";
const char string_38[] PROGMEM = "to desired Outcome";

const char custom_0[] PROGMEM = "|-CustomisedPattern-|";
const char custom_1[] PROGMEM = "Custom Pattern 1";
const char custom_2[] PROGMEM = "Custom Pattern 2";
const char custom_3[] PROGMEM = "Custom Pattern 3";

const char create_0[] PROGMEM = "Execute Pattern";
const char create_1[] PROGMEM = "Create New Pattern";

const char choose_motor_0[] PROGMEM = "|--Lens Selection--|";
const char choose_motor_1[] PROGMEM = "Move Zoom Lens";
const char choose_motor_2[] PROGMEM = "Move Focus Lens";

const char new_sequence_0[] PROGMEM = "|-Sequence Settings-|";
const char new_sequence_1[] PROGMEM = "Return to origin:false";
const char new_sequence_2[] PROGMEM = "Return to origin:true";
const char new_sequence_3[] PROGMEM = "Add new sequence";

/* String Table */
const char *const main_menu[] PROGMEM = {mm_0, mm_1, mm_2, mm_3, mm_4, mm_5, mm_6, mm_7, mm_8};
const char *const recalibration_menu[] PROGMEM = {string_4, string_5, string_6, string_7, back};
const char *const settings_menu[] PROGMEM = {string_12, string_13, string_14, string_15, back};
const char *const focus_menu[] PROGMEM = {string_19, string_20, string_21, string_22, string_41, string_42, string_45, back};
const char *const zoom_menu[] PROGMEM = {string_23, string_24, string_25, string_22, string_43, string_44, back};
const char *const zoomfocus_menu[] PROGMEM = {string_27, string_28, string_29, string_39, string_40, string_22, string_46, string_47, string_48, string_49, back};
const char *const presets_menu[] PROGMEM = {string_31, string_32, string_33, string_34, string_35, string_50, back};
const char *const calizoom_left[] PROGMEM = {cali_zoom, string_cali, zoom_left};
const char *const calizoom_right[] PROGMEM = {cali_zoom, string_cali, zoom_right};
const char *const califocus_left[] PROGMEM = {cali_focus, string_cali, focus_left};
const char *const califocus_right[] PROGMEM = {cali_focus, string_cali, focus_right};
const char *const countdown[] PROGMEM = {counttext_1, counttext_2, counttext_3, counttext_4, counttext_5};
const char *const zoom_adjust[] PROGMEM = {adjust_zoom, string_36, string_37};
const char *const focus_adjust[] PROGMEM = {adjust_focus, string_36_1, string_37};
const char *const focus_dist[] PROGMEM = {string_22, string_36_1, string_38};
const char *const zoom_dist[] PROGMEM = {string_22, string_36, string_38};
const char *const zoomfocus_dist[] PROGMEM = {string_22, string_36, string_38};
const char *const custom_menu[] PROGMEM = {custom_0, custom_1, custom_2, custom_3, back};
const char *const custom1_emptymenu[] PROGMEM = {custom_1, create_1, back};
const char *const custom1_menu[] PROGMEM = {custom_1, create_0, create_1, back};
const char *const custom2_emptymenu[] PROGMEM = {custom_2, create_1, back};
const char *const custom2_menu[] PROGMEM = {custom_2, create_0, create_1, back};
const char *const custom3_emptymenu[] PROGMEM = {custom_3, create_1, back};
const char *const custom3_menu[] PROGMEM = {custom_3, create_0, create_1, back};
const char *const lens_selection_menu[] PROGMEM = {choose_motor_0, choose_motor_1, choose_motor_2, back};
const char *const new_selection_truemenu[] PROGMEM = {new_sequence_0, new_sequence_2, new_sequence_3, back};
const char *const new_selection_falsemenu[] PROGMEM = {new_sequence_0, new_sequence_1, new_sequence_3, back};


// Object Declaration
/* Motor Objects */
AccelStepper rear_motor(AccelStepper::DRIVER, rear_STEP, rear_DIR);
AccelStepper front_motor(AccelStepper::DRIVER, front_STEP, front_DIR);

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
int focus_range;        // address 0
int zoom_range;         // address 1
int focus_current;      // address 2
int zoom_current;       // address 3
int orientation;        // address 4
int shutter_speed;      // address 5
int motor_time;         // address 6
int custom_itemcount1;  // address 8
int custom_itemcount2;  // address 9
int custom_itemcount3;  // address 10

// Custom Pattern Variables
char custom_buf1[24];
char custom_buf2[24];
char custom_buf3[24];

// Global Variables
int option = 0;
int updateMenu = true;
int screen = -1;    // main screen
int sscreen = -1;   // sub screen
int ssscreen = -1;  // super sub screen
int max_option = 0;

// Function Declaration
int menu(int array_size, const char *const string_table[], int option_selected, int header=0, int footer=2, uint16_t color=DEEPPINK);
void hotbar(char title[], int current, int max_range, int current_option=0, bool haveBack=false, int header=0, int footer=3, uint16_t color=WHITE, bool updateBar=false);
void caliMenu(int type, const char *const string_table[], int current_step, int max_steps=200, uint16_t color=WHITE, bool updateBar=false);
void moveMotorMenu(int count, const char *const string_table[], int current_step, int max_steps, uint16_t color=WHITE, bool updateBar=false);
void updateScreen(float delay_ms=0);
int getUpdate(int s, int offset=0);
int getUpDown(int option, int current_option, int delay_ms=0);
int getLeftRight(int range, int current, int low_limit=0, int delay_ms=0);
void moveMotor(int type, int pos_desired, int shutter_spd=0);
void moveMultiMotor(int zoom_value, int focus_value, float shutter_spd=0);
int calibrate(int type, const char *const string_table[], int upper_limit, int lower_limit, uint16_t color=WHITE);
int chooseDist(int type, int count, const char *const string_table[], bool goBack=false, uint16_t color=WHITE);
void goDist(int type, char title[], int pos_desired, uint16_t color=WHITE, bool goBack=true, float shutter_spd=motor_time, bool lastSequence=true);
void goMultiDist(char title[], int zoom_desired, int focus_desired, uint16_t color=WHITE, bool goBack=true, float shutter_spd=motor_time, bool lastSequence=true);
void(* resetFunc) (void) = 0;

/* Shutter Function */
void nikonTime() { // Controls the shutter of a Nikon camera
  digitalWrite(SHUTTER, LOW); // close shutter (fully pressed)
  delay(20);
  digitalWrite(SHUTTER, HIGH);
  delay(20);
}

/* Buzzer Function */
void buzz(int delay_ms=500, int freq=1000) {
  tone(BUZZER, freq);   // Send freq(Hz) sound signal...
  delay(delay_ms);      // ...for delay_ms
  noTone(BUZZER);       // Stop sound...
}

/* 
 * ~ SETUP ~
 */
void setup() {
  Serial.begin(9600);

  // **** Buzzer ****
  pinMode(BUZZER, OUTPUT);

  // **** Camera Controls ****
  // set the pins to output pins 
  // Using a Nikon D7100 shutter release
  // and an optocoupler
  // Currently in Time mode (2 presses)
  // HIGH = Open Position, LOW = Close
  pinMode(SHUTTER, OUTPUT);
  
  // ***** Joystick *****
  // change the pinmode for the joystick
  // 0 = pressed, 1 = not pressed
  pinMode(SET, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);

  // ***** Motor *****
  // motor using accelstepper lib
  rear_motor.setMaxSpeed(RPM);
  front_motor.setMaxSpeed(RPM);
  
  // ***** Display *****
  // able to call out similar functions
  // in Adafruit GFX lib
  tft.begin();
  tft.setRotation(0);
  tft.background(0,0,0);
  tft.setTextSize(2);

  // This costs 2% (for aesthetic)
  for(int i=0; i<5; i++) { // just to know if initialised properly
    tft.stroke(random(0, 255), random(0, 255), random(0, 255));
    tft.text("MOTOR ICM",6,80);
    delay(200);
  }
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.background(0,0,0);

  // ***** EEPROM Read *****
  // reads the stored memory
  
  focus_range = EEPROM.read(0);
  zoom_range = EEPROM.read(1);
  focus_current = EEPROM.read(2);
  zoom_current = EEPROM.read(3);
  orientation = EEPROM.read(4);
  shutter_speed = EEPROM.read(5);
  motor_time = EEPROM.read(6);
  custom_itemcount1 = EEPROM.read(8);
  custom_itemcount2 = EEPROM.read(9);
  custom_itemcount3 = EEPROM.read(10);

  // ***** Default Values *****
  // if empty (==255), setting default values to 0
  // for current positions -> Move the motor to stored current 
  if (focus_current == 255) {
    focus_current = 0;
  } else if (focus_current != 255) {
    setCurrentPos(FOCUS, focus_current * MS_STEP);
  }
  if (zoom_current == 255) {
    zoom_current = 0;
  } else if (zoom_current != 255) {
    setCurrentPos(ZOOM, zoom_current * MS_STEP);
  }
  if (focus_range == 255) {
    focus_range = 0;
  }
  if (zoom_range == 255) {
    zoom_range = 0;
  }
  if (orientation == 255) {
    orientation = 0;
  }
  if (shutter_speed == 255) {
    shutter_speed = 1;
  }
  if (motor_time == 255) {
    motor_time = 1;
  }

  // For Custom Profiles
  if (custom_itemcount1 == 255) {
    custom_itemcount1 = 0;
  }
  if (custom_itemcount2 == 255) {
    custom_itemcount2 = 0;
  }
  if (custom_itemcount3 == 255) {
    custom_itemcount3 = 0;
  }

  if (custom_itemcount1 > 0) {
    readStringFromMemory(25, custom_buf1);
    Serial.println(custom_buf1);
  }
  if (custom_itemcount2 > 0) {
    readStringFromMemory(50, custom_buf2);
    Serial.println(custom_buf2);
  }
  if (custom_itemcount3 > 0) {
    readStringFromMemory(75, custom_buf3);
    Serial.println(custom_buf3);
  }
}

/*
 * ~ LOOP ~
 */
void loop() {
  switch(screen) {
    /* Camera Settings */
    case 0: {
      switch (sscreen) {
        case 0: {// ** orientation menu **
          orientation = orientation ? 0 : 1;
          EEPROM.write(4,orientation);
          sscreen = resetScreen(sscreen);
          //sscreen = -1;
          break;
        }
        case 1: // ** set shutter time **
          hotbar(shutter_menu, shutter_speed, 40, 0, false, 0, 1, GOLDENROD);
          do {
            hotbar(shutter_menu, shutter_speed, 40, 0, false, 0, 1, GOLDENROD, true);
            shutter_speed = getLeftRight(40, shutter_speed, 0, 0);
          } while(digitalRead(SET));
          EEPROM.write(5, shutter_speed);
          updateScreen(500);
          sscreen = resetScreen(sscreen);
          break;
        case 2: { // ** set motor movement time **
          int shutter_time = 40;
          if (shutter_speed != 0) {
            shutter_time = shutter_speed;
          }
          hotbar(motor_time_menu, motor_time, shutter_time, 0, false, 0, 1, GOLDENROD);
          do {
            hotbar(motor_time_menu, motor_time, shutter_time, 0, false, 0, 1, GOLDENROD, true);
            motor_time = getLeftRight(shutter_time, motor_time, 1);
          } while(digitalRead(SET));
          EEPROM.write(6, motor_time);
          updateScreen(500);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 3: // ** back **
          screen = -1;
          sscreen = -1;
          break;

        default: // [settings menu]
          max_option = menu(4, settings_menu, option, 1);
          sscreen = getUpdate(sscreen);
      }
      break;
    }
    
    /* Recalibration */
    case 1: {
      switch(sscreen) {
        // ** zoom calibration **
        case 0: {
          zoom_current = 0;
          moveMotor(ZOOM, zoom_current);
          setCurrentPos(ZOOM, 0);
      
          // set to minimum left
          int minZoom = calibrate(ZOOM, calizoom_left, 50, -50, AQUA);
          setCurrentPos(ZOOM, 0); // set to 0
          updateScreen(500);
          
          // set to maximum right
          int maxZoom = calibrate(ZOOM, calizoom_right, MOTOR_STEPS, 0, AQUA);
          moveMotor(ZOOM, 0); // returns back to 0
          zoom_range = maxZoom - minZoom;
          updateScreen(500);
          EEPROM.write(1, zoom_range);
      
          // minimum becomes absolute min pos
          EEPROM.write(3, zoom_current);
          sscreen = resetScreen(sscreen);
          break;
        }
        
        // ** focus calibration **
        case 1: {
          focus_current = 0;
          moveMotor(FOCUS, focus_current);
          setCurrentPos(FOCUS, 0);
      
          // set to minimum left
          int minFocus = calibrate(FOCUS, califocus_left, 50, -50, DEEPPINK);
          setCurrentPos(FOCUS, 0); // set to 0
          updateScreen(500);
      
          // set to maximum right
          int maxFocus = calibrate(FOCUS, califocus_right, MOTOR_STEPS, 0, DEEPPINK);
          moveMotor(FOCUS, 0); // returns back to 0
          focus_range = maxFocus - minFocus; 
          updateScreen(500);
          EEPROM.write(0, focus_range);
      
          // minimum becomes absolute min pos
          EEPROM.write(2, focus_current);
          sscreen = resetScreen(sscreen);
          break;
        }
        
        // ** reset all calibration **
        case 2: {
          // TODO: Add are you sure?
          EEPROM.write(0,255);
          EEPROM.write(1,255);
          EEPROM.write(2,255);
          EEPROM.write(3,255);
          resetFunc();
          break; 
        }
          
        // ** back **
        case 3:
          screen = -1;
          sscreen = -1;
          break;
        default:
          max_option = menu(4, recalibration_menu, option, -2);
          sscreen = getUpdate(sscreen);
          break;
      }
      break;
    }
    
    /* POV Calibration */
    case 2: {
      updateScreen();
      zoom_current = 0;
      focus_current = 0;
      moveMotor(FOCUS, focus_current);
      moveMotor(ZOOM, zoom_current);
      //moveMultiMotor(zoom_current, focus_current);
      
      zoom_current = chooseDist(ZOOM, 3, zoom_adjust, false, AQUA);
      EEPROM.write(3, zoom_current);
      updateScreen(500);
      focus_current = chooseDist(FOCUS, 3, focus_adjust, false, DEEPPINK);
      EEPROM.write(2, focus_current);
      updateScreen(500);

      screen = resetScreen(screen);
      break;
    }

    /* Focus Movement */
    case 3: {
      switch(sscreen) { 
        case 0: { // focus to infinity
          countdownMenu();
          goDist(FOCUS, string_20, focus_range, SNOW);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 1: { // focus to min
          countdownMenu();
          goDist(FOCUS, string_21, 0, VIOLET);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 2: { // focus to dist
          int pos_desired;
          pos_desired = chooseDist(FOCUS, 3, focus_dist, true, YELLOWGREEN);
          updateScreen();
          delay(500);
          countdownMenu();
          goDist(FOCUS, string_22, pos_desired, YELLOWGREEN);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 3: { // focus to infinity & back
          countdownMenu();
          int previous_pos = focus_current;
          goDist(FOCUS, string_41, focus_range, AZURE, false, motor_time/2, false);
          goDist(FOCUS, string_41, previous_pos, AZURE, false, motor_time/2);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 4: { // focus to min & back
          countdownMenu();
          int previous_pos = focus_current;
          goDist(FOCUS, string_42, 0, CORAL, false, motor_time/2, false);
          goDist(FOCUS, string_42, previous_pos, CORAL, false, motor_time/2);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 5: { // MAX-POV-MIN
          int previous_pos = focus_current;
          printMoveSteps(NULL, string_32, CADETBLUE, 2); // setting lens to starting position
          moveMotor(FOCUS, focus_range);
          focus_current = focus_range;
          countdownMenu();
          goDist(FOCUS, string_45, previous_pos, LIME, false, motor_time/3, false);
          delay(toMS(motor_time/3));
          goDist(FOCUS, string_45, 0, LIME, true, motor_time/3);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 6: { // back
          screen = -1;
          sscreen = -1;
          break;
        }
        default: {
          max_option = menu(7, focus_menu, option, 0);
          sscreen = getUpdate(sscreen);
          break;
        }
      }
      break;
    }

    /* Zoom Movement */
    case 4: {
      switch(sscreen) {
        case 0: { // zoom to max
          countdownMenu();
          goDist(ZOOM, string_24, zoom_range, SNOW);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 1: { // zoom to min
          countdownMenu();
          goDist(ZOOM, string_25, 0, VIOLET);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 2: { // zoom to dist
          int pos_desired;
          pos_desired = chooseDist(ZOOM, 3, zoom_dist, true, YELLOWGREEN);
          updateScreen();
          delay(500);
          countdownMenu();
          goDist(ZOOM, string_22, pos_desired, YELLOWGREEN);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 3: { // zoom to max & back
          countdownMenu();
          int previous_pos = zoom_current;
          goDist(ZOOM, string_43, zoom_range, AZURE, false, motor_time/2, false);
          goDist(ZOOM, string_43, previous_pos, AZURE, false, motor_time/2);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 4: { // zoom to min & back
          countdownMenu();
          int previous_pos = zoom_current;
          goDist(ZOOM, string_44, 0, CORAL, false, motor_time/2, false);
          goDist(ZOOM, string_44, previous_pos, CORAL, false, motor_time/2);
          sscreen = resetScreen(sscreen);
        }
        case 5: { // back
          screen = -1;
          sscreen = -1;
          break;
        }
        default: {
          max_option = menu(6, zoom_menu, option, 0);
          sscreen = getUpdate(sscreen);
          break;
        }
      }
      break;
    }

    /* ZoomFocus Movements */
    case 5: {
      switch (sscreen) {
        case 0: { // both to max
          countdownMenu();
          goMultiDist(string_28, zoom_range, focus_range, VIOLET);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 1: { // both to min
          countdownMenu();
          goMultiDist(string_29, 0, 0, AZURE);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 2: { // zoom to max, focus to min
          countdownMenu();
          goMultiDist(string_39, zoom_range, 0, CORAL);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 3: { // zoom to min, focus to max
          countdownMenu();
          goMultiDist(string_40, 0, focus_range, CADETBLUE);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 4: { // to certain dist
          int zoom_desired, focus_desired;
          zoom_desired = chooseDist(ZOOM, 3, zoom_dist, true, YELLOWGREEN);
          if (zoom_desired == zoom_current) zoom_desired = -1; // failsafe
          updateScreen(500);
          
          focus_desired = chooseDist(FOCUS, 3, focus_dist, true, RED);
          if (focus_desired == focus_current) focus_desired = -1; // failsafe
          updateScreen();
          
          delay(500);
          countdownMenu();
          goMultiDist(string_22, zoom_desired, focus_desired, LIME);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 5: { // ZF[MAX] & back
          int previous_zoom, previous_focus;
          previous_zoom = zoom_current;
          previous_focus = focus_current;

          countdownMenu();
          goMultiDist(string_46, zoom_range, focus_range, VIOLET, false, motor_time/2, false);
          goMultiDist(string_46, previous_zoom, previous_focus, VIOLET, false, motor_time/2);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 6: { // ZF[MIN] & back
          int previous_zoom, previous_focus;
          previous_zoom = zoom_current;
          previous_focus = focus_current;

          countdownMenu();
          goMultiDist(string_47, 0, 0, CORAL, false, motor_time/2, false);
          goMultiDist(string_47, previous_zoom, previous_focus, CORAL, false, motor_time/2);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 7: { // Z[MAX]F[MIN] & back
          int previous_zoom, previous_focus;
          previous_zoom = zoom_current;
          previous_focus = focus_current;

          countdownMenu();
          goMultiDist(string_48, zoom_range, 0, AZURE, false, motor_time/2, false);
          goMultiDist(string_48, previous_zoom, previous_focus, AZURE, false, motor_time/2);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 8: { // Z[MIN]F[MAX] & back
          int previous_zoom, previous_focus;
          previous_zoom = zoom_current;
          previous_focus = focus_current;

          countdownMenu();
          goMultiDist(string_49, 0, focus_range, LIME, false, motor_time/2, false);
          goMultiDist(string_49, previous_zoom, previous_focus, LIME, false, motor_time/2);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 9: { // back
          screen = -1;
          sscreen = -1;
          break;
        }
        default: {
          max_option = menu(10, zoomfocus_menu, option, 0, 0);
          sscreen = getUpdate(sscreen);
          break;
        }
      }
      break;
    }
    
    /* Fixed Patterns */
    case 6: {
      switch (sscreen) {
        case 0: { // Bokeh Effect (S: Focus Max, Zoom Current. F: Focus Current, Zoom Widest)
          int previous_pos = focus_current;
          printMoveSteps(NULL, string_32, CADETBLUE, 2); // setting lens to starting position
          moveMotor(FOCUS, focus_range);
          focus_current = focus_range;
          updateScreen(2000);
          countdownMenu();
          goDist(FOCUS, string_32, previous_pos, VIOLET, false, motor_time/2, false);
          goDist(ZOOM, string_32, 0, VIOLET, true, motor_time/2);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 1: { // Firework Effect (Focus Max, then min, then return to original)
          int previous_pos = focus_current;
          printMoveSteps(NULL, string_33, CADETBLUE, 2); // setting lens to starting position
          moveMotor(FOCUS, focus_range);
          focus_current = focus_range;
          updateScreen(2000);
          countdownMenu();
          goDist(FOCUS, string_33, 0, AZURE, false, ((float)3/4)*motor_time, false);
          goDist(FOCUS, string_33, previous_pos, AZURE, false, ((float)1/4)*motor_time);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 2: { // Zoom Blur Effect (Focus & Zoom Max then back to original)
          int previous_zoom = zoom_current;
          int previous_focus = focus_current;
          printMoveSteps(NULL, string_33, CADETBLUE, 2); // setting lens to starting position
          moveMotor(FOCUS, focus_range); // will use multi motor if the timings issues are solved
          moveMotor(ZOOM, zoom_range);
          zoom_current = zoom_range;
          focus_current = focus_range;
          updateScreen(2000);
          countdownMenu();
          goDist(ZOOM, string_34, previous_zoom, LIME, false, motor_time/2, false);
          goDist(FOCUS, string_34, previous_focus, LIME, false, motor_time/2);
          sscreen = resetScreen(sscreen);
          break;
        }
        case 3: { // Sine Wave Effect
          countdownMenu();
          int previous_zoom = zoom_current;
          int previous_focus = focus_current;
          goDist(ZOOM, string_35, zoom_range, CORAL, false, motor_time/4, false);
          goDist(FOCUS, string_35, focus_range, CORAL, false, motor_time/4, false);
          goDist(ZOOM, string_35, 0, CORAL, false, motor_time/4,false);
          goDist(FOCUS, string_35, 0, CORAL, false, motor_time/4);
          printMoveSteps(NULL, string_35, CADETBLUE, 1); // return to initial position
          moveMotor(FOCUS, previous_focus); 
          moveMotor(ZOOM, previous_zoom);
          zoom_current = previous_zoom;
          focus_current = previous_focus;
          sscreen = resetScreen(sscreen);
          break;
        }
        case 4: { // The Zigzagger Out Effect
          int previous_pos = zoom_current;
          int minus = zoom_current/4;
          countdownMenu();
          for (int i=0; i<3; i++) {
            goDist(ZOOM, string_50, zoom_current-minus, CORAL, false, motor_time/7, false);
            goDist(ZOOM, string_50, zoom_current+(minus/2), CORAL, false, motor_time/7, false);
          }
          goDist(ZOOM, string_50, 0, CORAL, false, motor_time/7);
          printMoveSteps(NULL, string_35, CADETBLUE, 1); // return to initial position
          moveMotor(ZOOM, previous_pos);
          zoom_current = previous_pos;
          sscreen = resetScreen(sscreen);
          break;
        }
        case 5: // back
          screen = -1;
          sscreen = -1;
          break;
        default:
          max_option = menu(6, presets_menu, option, 0);
          sscreen = getUpdate(sscreen);
          break;
      }
      break;
    }

    /* Custom Patterns */
    case 7: {
      switch (sscreen) {
        case 0: { // Custom Profile 1
          switch (ssscreen) {
            case 0: { // Execute Sequence
              countdownMenu();
              splitStr(custom_buf1, custom_1, custom_itemcount1);
              EEPROM.write(3, zoom_current);
              EEPROM.write(4, focus_current);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 1: { // Create new Sequence
              custom_itemcount1 = createCustom(custom_buf1);
              if (custom_itemcount1 > 0) {
                writeStringToMemory(25, custom_buf1);
              }
              EEPROM.write(8, custom_itemcount1);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 2: { // Back
              sscreen = -1;
              ssscreen = -1;
              break;
            }
            default: {
              if (custom_itemcount1 == 0) { // no pattern stored
                max_option = menu(2, custom1_emptymenu, option); 
                ssscreen = getUpdate(ssscreen, 1);
              } else { // pattern stored, 3 options
                max_option = menu(3, custom1_menu, option);
                ssscreen = getUpdate(ssscreen);
              }
              break;
            }
          }
          break;
        }
        case 1: { // Custom Profile 2
          switch (ssscreen) {
            case 0: { // Execute Sequence
              countdownMenu();
              splitStr(custom_buf2, custom_2, custom_itemcount2);
              EEPROM.write(3, zoom_current);
              EEPROM.write(4, focus_current);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 1: { // Create new Sequence
              custom_itemcount2 = createCustom(custom_buf2);
              if (custom_itemcount2 > 0) {    
                writeStringToMemory(50, custom_buf2);
              }
              EEPROM.write(9, custom_itemcount2);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 2: { // Back
              sscreen = -1;
              ssscreen = -1;
              break;
            }
            default: {
              if (custom_itemcount2 == 0) { // no pattern stored
                max_option = menu(2, custom2_emptymenu, option); 
                ssscreen = getUpdate(ssscreen, 1);
              } else { // pattern stored, 3 options
                max_option = menu(3, custom2_menu, option);
                ssscreen = getUpdate(ssscreen);
              }
              break;
            }
          }
          break;
        }
        case 2: { // Custom Profile 3
          switch (ssscreen) {
            case 0: { // Execute Sequence
              countdownMenu();
              splitStr(custom_buf3, custom_3, custom_itemcount3);
              EEPROM.write(3, zoom_current);
              EEPROM.write(4, focus_current);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 1: { // Create new Sequence
              custom_itemcount3 = createCustom(custom_buf3);
              if (custom_itemcount3 > 0) {
                writeStringToMemory(75, custom_buf3);
              }
              EEPROM.write(10, custom_itemcount3);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 2: { // Back
              sscreen = -1;
              ssscreen = -1;
              break;
            }
            default: {
              if (custom_itemcount3 == 0) { // no pattern stored
                max_option = menu(2, custom3_emptymenu, option); 
                ssscreen = getUpdate(ssscreen, 1);
              } else { // pattern stored, 3 options
                max_option = menu(3, custom3_menu, option);
                ssscreen = getUpdate(ssscreen);
              }
              break;
            }
          }
          break;
        }
        case 3: { // back
          screen = -1;
          sscreen = -1;
          break;
        }
        default: {
          max_option = menu(4, custom_menu, option, 2);
          sscreen = getUpdate(sscreen);
          break;
        }
      }
      break;
    }

    /* Main Menu */
    default:
      max_option = menu(8, main_menu, option, 0);
      screen = getUpdate(screen);
      break;
  }
  option = getUpDown(max_option, option, 0);
}
