// ICM Buddy for DSLR Cameras
// Uses 2 NEMA motors, 1 TFT 1.8 Screen and a joystick

#include <AccelStepper.h>
#include <MultiStepper.h>
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

// Strings
/* Colour Strings */
#define WHITE     0xFFFF
#define BLACK     0x0000
#define AQUA      0xFEC0
#define DEEPPINK  0xF8B2  
#define DARKGREEN 0x0320
#define RED       0xF800 


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

const char string_0[] PROGMEM = "|---- Main Menu ----|";
const char string_1[] PROGMEM = "Calibration";
const char string_2[] PROGMEM = "Start Movements";
const char string_3[] PROGMEM = "Settings";

const char string_4[] PROGMEM = "|---Recalibration---|";
const char string_5[] PROGMEM = "Zoom Recalibration";
const char string_6[] PROGMEM = "Focus Recalibration";
const char string_7[] PROGMEM = "RESET all values";

const char string_8[] PROGMEM = "|---- Movements ----|";
const char string_9[] PROGMEM = "Adjust Focus";
const char string_10[] PROGMEM = "Adjust Zoom";
const char string_11[] PROGMEM = "Zoom & Focus";
const char string_11_1[] PROGMEM = "Presets (WIP)";

const char string_12[] PROGMEM = "|---- Settings -----|";
const char string_13[] PROGMEM = "Zoom/Focus Position";
const char string_14[] PROGMEM = "Camera Shutter Speed";

const char string_15[] PROGMEM = "|----Positioning----|";
const char string_16[] PROGMEM = "Zoom at the back";
const char string_17[] PROGMEM = "Zoom at the front";

const char shutter_menu[] PROGMEM = "|---Shutter Speed---|";

const char string_19[] PROGMEM = "|--Adjusting Focus--|";
const char string_20[] PROGMEM = "Focus to Max";
const char string_21[] PROGMEM = "Focus to Min";
const char string_22[] PROGMEM = "Focus to a Value";

const char string_23[] PROGMEM = "|--Adjusting Zoom --|";
const char string_24[] PROGMEM = "Zoom to Max";
const char string_25[] PROGMEM = "Zoom to Min";
const char string_26[] PROGMEM = "Zoom to a Value";

const char string_27[] PROGMEM = "|-Adjust Zoom&Focus-|";
const char string_28[] PROGMEM = "ZoomFocus to Max";
const char string_29[] PROGMEM = "ZoomFocus to Min";
const char string_30[] PROGMEM = "ZoomFocus to a Value";

const char string_31[] PROGMEM = "|----- Presets -----|";
const char string_32[] PROGMEM = "Bokeh Effect";
const char string_33[] PROGMEM = "Firework Effect";
const char string_34[] PROGMEM = "Zoom Blur Effect";
const char string_35[] PROGMEM = "Sine Wave Effect";

const char cali_zoom[] PROGMEM = "|--Calibrate Zoom --|";
const char cali_focus[] PROGMEM = "|--Calibrate Focus--|";
const char string_cali[] PROGMEM = "Move the joystick to the extreme";
const char string_left[] PROGMEM = "  left";
const char string_right[] PROGMEM = "  right";

const char counttext_1[] PROGMEM = "Get Ready!";
const char counttext_2[] PROGMEM = "3";
const char counttext_3[] PROGMEM = "2";
const char counttext_4[] PROGMEM = "1";
const char counttext_5[] PROGMEM = "SNAP!";

const char adjust_zoom[] PROGMEM = "|--- Adjust Zoom ---|";
const char adjust_focus[] PROGMEM = "|---Adjust Focus ---|";
const char string_36[] PROGMEM = "Adjust the lens to the";
const char string_37[] PROGMEM = " desired Image";

/* String Table */
const char *const main_menu[] PROGMEM = {string_0, string_1, string_2, string_3};
const char *const recalibration_menu[] PROGMEM {string_4, string_5, string_6, string_7, back};
const char *const movement_menu[] PROGMEM = {string_8, string_9, string_10, string_11, string_11_1, back};
const char *const settings_menu[] PROGMEM = {string_12, string_13, string_14, back};
const char *const positioning_menu[] PROGMEM = {string_15, string_16, string_17, back};
const char *const focus_menu[] PROGMEM = {string_19, string_20, string_21, string_22, back};
const char *const zoom_menu[] PROGMEM = {string_23, string_24, string_25, string_26, back};
const char *const zoomfocus_menu[] PROGMEM = {string_27, string_28, string_29, string_30, back};
const char *const presets_menu[] PROGMEM = {string_31, string_32, string_33, string_34, string_35, back};
const char *const calizoom_left[] PROGMEM = {cali_zoom, string_cali, string_left};
const char *const calizoom_right[] PROGMEM = {cali_zoom, string_cali, string_right};
const char *const califocus_left[] PROGMEM = {cali_focus, string_cali, string_left};
const char *const califocus_right[] PROGMEM = {cali_focus, string_cali, string_right};
const char *const countdown[] PROGMEM = {counttext_1, counttext_2, counttext_3, counttext_4, counttext_5};
const char *const zoom_adjust[] PROGMEM = {adjust_zoom, string_36, string_37};
const char *const focus_adjust[] PROGMEM = {adjust_focus, string_36, string_37};

// Object Declaration
/* Motor Objects */
AccelStepper zoom_motor(AccelStepper::DRIVER, rear_STEP, rear_DIR);
AccelStepper focus_motor(AccelStepper::DRIVER, front_STEP, front_DIR);

MultiStepper steppers;

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
int focus_range;    // address 0
int zoom_range;     // address 1
int focus_current;  // address 2
int zoom_current;   // address 3
int orientation;    // address 4
int shutter_speed;  // address 5

// Global Variables
int option = 0;
int updateMenu = true;
int screen = -1;    // main screen
int sscreen = -1;   // sub screen
int ssscreen = -1;
int max_option = 0;
bool firstTime;

// Function Declaration
int menu(int array_size, const char *const string_table[], int option_selected, bool header=false, int footer=2);
int hotbar(char title[], int current, int max_range, int current_option=0, bool haveBack=false, bool header=false, int footer=3);
int getUpDown(int option, int current_option, int delay_ms=200);
int getLeftRight(int range, int current, int low_limit=0, int delay_ms=200);
int caliMenu(const char *const string_table[], int current_step, int max_steps=200);
void(* resetFunc) (void) = 0;

void setup() {
  Serial.begin(9600);
  
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
  zoom_motor.setMaxSpeed(RPM);
  focus_motor.setMaxSpeed(RPM);

  steppers.addStepper(zoom_motor);
  steppers.addStepper(focus_motor);
  
  // ***** Display *****
  // able to call out similar functions
  // in Adafruit GFX lib
  tft.begin();
  tft.background(0,0,0);
  tft.setRotation(0);

  // ***** EEPROM Read *****
  // reads the stored memory
  // if empty (==255), setting default values to 0
  focus_range = EEPROM.read(0);
  zoom_range = EEPROM.read(1);
  focus_current = EEPROM.read(2);
  zoom_current = EEPROM.read(3);
  orientation = EEPROM.read(4);
  shutter_speed = EEPROM.read(5);
  
  if (focus_current == 255) {
    focus_current = 0;
  }
  if (zoom_current == 255) {
    zoom_current = 0;
  }
  if (orientation == 255) {
    orientation = 0;
  }
  // ****** Setup Menu ******
  // Only run through if ranges are invalid
  // if range == 255
  // calibrate zoom
  if (zoom_range == 255) {
    do {
      int temp, check;
      temp = zoom_current;
      do {
        check = zoom_current;
        zoom_current = caliMenu(calizoom_right, zoom_current, MOTOR_STEPS);
        zoom_current = getLeftRight(MOTOR_STEPS, zoom_current, 0, 0);
      } while(check != zoom_current);
      int steps_to_move = (zoom_current - temp) * MS_STEP;
      zoom_motor.move(steps_to_move);
      while (zoom_motor.distanceToGo() != 0) {
        zoom_motor.setSpeed((steps_to_move >= 0) ? 600 : -600); 
        zoom_motor.runSpeed();
      }
    } while(digitalRead(SET));
    int maxZoom = zoom_current;
    delay(500);
    tft.background(0,0,0);
    updateMenu = true;
    do {
      int temp, check;
      temp = zoom_current;
      do {
        check = zoom_current;
        zoom_current = caliMenu(calizoom_left, zoom_current, MOTOR_STEPS);
        zoom_current = getLeftRight(maxZoom, zoom_current, -(MOTOR_STEPS-maxZoom), 0);
      } while(check != zoom_current);
      int steps_to_move = (zoom_current - temp) * MS_STEP;
      zoom_motor.move(steps_to_move);
      while (zoom_motor.distanceToGo() != 0) {
        zoom_motor.setSpeed((steps_to_move >= 0) ? 600 : -600); 
        zoom_motor.runSpeed();
      }
    } while (digitalRead(SET));
    zoom_range = maxZoom - zoom_current;
    EEPROM.write(3, zoom_range);
    
    zoom_current = 0; // minimum becomes absolute min pos
    zoom_motor.setCurrentPosition(0);
  }
  if (focus_range == 255) {
    do {
      int temp, check;
      temp = focus_current;
      do {
        check = focus_current;
        focus_current = caliMenu(califocus_right, focus_current, MOTOR_STEPS);
        focus_current = getLeftRight(MOTOR_STEPS, focus_current, 0, 0);
      } while(check != focus_current);
      int steps_to_move = (focus_current - temp) * MS_STEP;
      focus_motor.move(steps_to_move);
      while (focus_motor.distanceToGo() != 0) {
        focus_motor.setSpeed((steps_to_move >= 0) ? 600 : -600); 
        focus_motor.runSpeed();
      }
    } while(digitalRead(SET));
    int maxFocus = focus_current;
    delay(500);
    tft.background(0,0,0);
    updateMenu = true;
    do {
      int temp, check;
      temp = focus_current;
      do {
        check = focus_current;
        focus_current = caliMenu(califocus_left, focus_current, MOTOR_STEPS);
        focus_current = getLeftRight(maxFocus, focus_current, -(MOTOR_STEPS-maxFocus), 0);
      } while(check != zoom_current);
      int steps_to_move = (focus_current - temp) * MS_STEP;
      focus_motor.move(steps_to_move);
      while (focus_motor.distanceToGo() != 0) {
        focus_motor.setSpeed((steps_to_move >= 0) ? 600 : -600); 
        focus_motor.runSpeed();
      }
    } while (digitalRead(SET));
    focus_range = maxFocus - focus_current;
    EEPROM.write(2, focus_range);
    
    focus_current = 0; // minimum becomes absolute min pos
    focus_motor.setCurrentPosition(0);
  }

  if (shutter_speed == 255) {
    screen = 2;
    sscreen = 1;
    // teleport to shutter menu
  }

}

void loop() {
  switch(screen) {
    /* Recalibration */
    case 0:
      switch(sscreen) {
        // zoom calibration
        case 0: {
          zoom_motor.setCurrentPosition(zoom_current * MS_STEP);
          do {
            int temp, check;
            temp = zoom_current;
            do {
              check = zoom_current;
              zoom_current = caliMenu(calizoom_right, zoom_current, MOTOR_STEPS);
              zoom_current = getLeftRight(MOTOR_STEPS, zoom_current, 0, 0);
            } while(check != zoom_current);
            int steps_to_move = (zoom_current - temp) * MS_STEP;
            zoom_motor.move(steps_to_move);
            while (zoom_motor.distanceToGo() != 0) {
              zoom_motor.setSpeed((steps_to_move >= 0) ? 600 : -600); 
              zoom_motor.runSpeed();
            }
          } while(digitalRead(SET));
          int maxZoom = zoom_current;
          delay(500);
          tft.background(0,0,0);
          updateMenu = true;
          do {
            int temp, check;
            temp = zoom_current;
            do {
              check = zoom_current;
              zoom_current = caliMenu(calizoom_left, zoom_current, MOTOR_STEPS);
              zoom_current = getLeftRight(maxZoom, zoom_current, -(MOTOR_STEPS-maxZoom), 0);
            } while(check != zoom_current);
            int steps_to_move = (zoom_current - temp) * MS_STEP;
            zoom_motor.move(steps_to_move);
            while (zoom_motor.distanceToGo() != 0) {
              zoom_motor.setSpeed((steps_to_move >= 0) ? 600 : -600); 
              zoom_motor.runSpeed();
            }
          } while (digitalRead(SET));
          zoom_range = maxZoom - zoom_current;
          EEPROM.write(3, zoom_range);
          
          zoom_current = 0; // minimum becomes absolute min pos
          zoom_motor.setCurrentPosition(0);
          sscreen = resetScreen(sscreen);
          break;
        }
        
        // focus calibration
        case 1: {
          focus_motor.setCurrentPosition(focus_current * MS_STEP);
          do {
            int temp, check;
            temp = focus_current;
            do {
              check = focus_current;
              focus_current = caliMenu(califocus_right, focus_current, MOTOR_STEPS);
              focus_current = getLeftRight(MOTOR_STEPS, focus_current, 0, 0);
            } while(check != focus_current);
            int steps_to_move = (focus_current - temp) * MS_STEP;
            focus_motor.move(steps_to_move);
            while (focus_motor.distanceToGo() != 0) {
              focus_motor.setSpeed((steps_to_move >= 0) ? 600 : -600); 
              focus_motor.runSpeed();
            }
          } while(digitalRead(SET));
          int maxFocus = focus_current;
          delay(500);
          tft.background(0,0,0);
          updateMenu = true;
          do {
            int temp, check;
            temp = focus_current;
            do {
              check = focus_current;
              focus_current = caliMenu(califocus_left, focus_current, MOTOR_STEPS);
              focus_current = getLeftRight(maxFocus, focus_current, -(MOTOR_STEPS-maxFocus), 0);
            } while(check != zoom_current);
            int steps_to_move = (focus_current - temp) * MS_STEP;
            focus_motor.move(steps_to_move);
            while (focus_motor.distanceToGo() != 0) {
              focus_motor.setSpeed((steps_to_move >= 0) ? 600 : -600); 
              focus_motor.runSpeed();
            }
          } while (digitalRead(SET));
          focus_range = maxFocus - focus_current;
          EEPROM.write(2, focus_range);
          
          focus_current = 0; // minimum becomes absolute min pos
          focus_motor.setCurrentPosition(0);
          sscreen = resetScreen(sscreen);
          break;
        }
        
        // reset all calibration
        case 2: {
          // TODO: Add are you sure?
          EEPROM.write(0,255);
          EEPROM.write(1,255);
          EEPROM.write(2,255);
          EEPROM.write(3,255);
          EEPROM.write(5,255);
          resetFunc();
          break; 
        }
          
        // back
        case 3:
          screen = -1;
          sscreen = -1;
          break;
        default:
          max_option = menu(4, recalibration_menu, option);
          sscreen = getUpdate(sscreen);
          break;
      }
      break;
      
    /* Movements */
    case 1:
      switch(sscreen) {
        case 0: // focus
          switch(ssscreen) {
            case 0: { // focus to max
              int steps_to_move = focus_range - focus_current;
              for (int i=0; i<5; i++) {
                hotbar(string_20, 0, steps_to_move, 0, false, false, 0);
                tft.setTextSize(2);
                tft.setCursor(0, 59);
                tft.setTextColor(RED,BLACK);
                strcpy_P(buffer, (char *)pgm_read_word(&(countdown[i])));
                tft.print(buffer);
                delay(1000);
              }
              tft.setTextSize(1);
              tft.setTextColor(WHITE);
              delay(toMS(shutter_speed/2));
              focus_motor.move(steps_to_move * MS_STEP);
              focus_motor.setSpeed(calcRPM(abs(steps_to_move * MS_STEP), shutter_speed/2));
              while (focus_motor.distanceToGo() != 0) {
                updateMenu = true;
                steps_to_move = hotbar(string_20, 0, --steps_to_move, 0, false, false, 0);
                focus_motor.runSpeed();
              }
              delay(5000);
              // return back to original
              focus_motor.move(-steps_to_move * MS_STEP);
              focus_motor.setSpeed(-400);
              while (focus_motor.distanceToGo() != 0) {
                focus_motor.runSpeed();
              }
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 1: { // focus to min
              int steps_to_move = focus_current - 0;
              for (int i=0; i<5; i++) {
                hotbar(string_21, 0, steps_to_move, 0, false, false, 0);
                tft.setTextSize(2);
                tft.setCursor(0, 59);
                tft.setTextColor(RED,BLACK);
                strcpy_P(buffer, (char *)pgm_read_word(&(countdown[i])));
                tft.print(buffer);
                delay(1000);
              }
              delay(toMS(shutter_speed/2));
              focus_motor.move(-steps_to_move * MS_STEP);
              focus_motor.setSpeed(-calcRPM(abs(steps_to_move * MS_STEP), shutter_speed/2));
              while (focus_motor.distanceToGo() != 0) {
                updateMenu = true;
                steps_to_move = hotbar(string_21, 0, --steps_to_move, 0, false, false, 0);
                focus_motor.runSpeed();
              }
              delay(5000);
              // return back to original
              focus_motor.move(steps_to_move * MS_STEP);
              focus_motor.setSpeed(400);
              while (focus_motor.distanceToGo() != 0) {
                focus_motor.runSpeed();
              }
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 2:
            case 3: // back
              sscreen = -1;
              ssscreen = -1;
              break;
            default:
              max_option = menu(4, focus_menu, option, true);
              ssscreen = getUpdate(ssscreen);
          }
          break;
        case 1: // zoom
          switch(ssscreen) {
            case 0: { // zoom to max
              int steps_to_move = zoom_range - zoom_current;
              for (int i=0; i<5; i++) {
                hotbar(string_24, 0, steps_to_move, 0, false, false, 0);
                tft.setTextSize(2);
                tft.setCursor(0, 59);
                tft.setTextColor(RED,BLACK);
                strcpy_P(buffer, (char *)pgm_read_word(&(countdown[i])));
                tft.print(buffer);
                delay(1000);
              }
              delay(toMS(shutter_speed/2));
              zoom_motor.move(steps_to_move * MS_STEP);
              zoom_motor.setSpeed(calcRPM(abs(steps_to_move * MS_STEP), shutter_speed/2));
              while (zoom_motor.distanceToGo() != 0) {
                updateMenu = true;
                steps_to_move = hotbar(string_24, 0, --steps_to_move, 0, false, false, 0);
                zoom_motor.runSpeed();
              }
              delay(5000);
              // return back to original
              zoom_motor.move(-steps_to_move * MS_STEP);
              zoom_motor.setSpeed(-400);
              while (zoom_motor.distanceToGo() != 0) {
                zoom_motor.runSpeed();
              }
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 1: { // zoom to min
              int steps_to_move = zoom_current - 0;
              for (int i=0; i<5; i++) {
                hotbar(string_25, 0, steps_to_move, 0, false, false, 0);
                tft.setTextSize(2);
                tft.setCursor(0, 59);
                tft.setTextColor(RED,BLACK);
                strcpy_P(buffer, (char *)pgm_read_word(&(countdown[i])));
                tft.print(buffer);
                delay(1000);
              }
              delay(toMS(shutter_speed/2));
              zoom_motor.move(-steps_to_move * MS_STEP);
              zoom_motor.setSpeed(-calcRPM(abs(steps_to_move * MS_STEP), shutter_speed/2));
              while (zoom_motor.distanceToGo() != 0) {
                updateMenu = true;
                steps_to_move = hotbar(string_24, 0, --steps_to_move, 0, false, false, 0);
                zoom_motor.runSpeed();
              }
              // return back to original
              zoom_motor.move(steps_to_move * MS_STEP);
              zoom_motor.setSpeed(400);
              while (zoom_motor.distanceToGo() != 0) {
                zoom_motor.runSpeed();
              }
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 2:
            case 3: // back
              sscreen = -1;
              ssscreen = -1;
              break;
            default:
              max_option = menu(4, zoom_menu, option, true);
              ssscreen = getUpdate(ssscreen);
          }
          break;
        case 2: // focus & zoom
          switch(ssscreen) {
            case 0: {
                // may cause broken bones
              long positions[2];
              positions[0] = zoom_range;
              positions[1] = focus_range;
              steppers.moveTo(positions);
              delay(toMS(shutter_speed/2));
              steppers.runSpeedToPosition();
            }
            case 1: {
              // not sure how to implement this
            }
            case 2:
            case 3: // back
              sscreen = -1;
              ssscreen = -1;
              break;
            default:
              max_option = menu(4, zoomfocus_menu, option, true);
              ssscreen = getUpdate(ssscreen);
          }
          break;
        case 3: // presets
          switch(ssscreen) {
            case 1:
            case 2:
            case 3:
            case 4: // back
              sscreen = -1;
              ssscreen = -1;
              break;
            default:
              max_option = menu(5, presets_menu, option, true);
              ssscreen = getUpdate(ssscreen);
          }
          break;
        case 4: // back
          screen = -1;
          sscreen = -1;
          break;
        default: {  // [Movement Menu] 
          if (firstTime = true) {
            // adjust zoom to desired image
            zoom_motor.setCurrentPosition(zoom_current * MS_STEP);
            do {
              int temp, check;
              temp = zoom_current;
              do {
                check = zoom_current;
                zoom_current = caliMenu(zoom_adjust, zoom_current, zoom_range);
                zoom_current = getLeftRight(zoom_range, zoom_current, 0, 0);
              } while(check != zoom_current);
              int steps_to_move = (zoom_current - temp) * MS_STEP;
              zoom_motor.move(steps_to_move);
              while (zoom_motor.distanceToGo() != 0) {
                zoom_motor.setSpeed((steps_to_move >= 0) ? 600 : -600); 
                zoom_motor.runSpeed();
              }
            } while(digitalRead(SET));

            // adjust focus to desired image
            focus_motor.setCurrentPosition(focus_current * MS_STEP);
            do {
              int temp, check;
              temp = focus_current;
              do {
                check = focus_current;
                focus_current = caliMenu(focus_adjust, focus_current, focus_range);
                focus_current = getLeftRight(focus_range, focus_current, 0, 0);
              } while(check != focus_current);
              int steps_to_move = (focus_current - temp) * MS_STEP;
              focus_motor.move(steps_to_move);
              while (focus_motor.distanceToGo() != 0) {
                focus_motor.setSpeed((steps_to_move >= 0) ? 600 : -600); 
                focus_motor.runSpeed();
              }
            } while(digitalRead(SET));
          }
          max_option = menu(5, movement_menu, option, true);
          sscreen = getUpdate(sscreen);
        }
      }
      break;

    /* Settings */
    case 2:
      switch (sscreen) {
        case 0: // positioning menu
          switch(ssscreen) {
            case 0: // zoom at the back
              orientation = 0;
              ssscreen = -1;
              break;
            case 1: // zoom at the front
              orientation = 1;
              ssscreen = -1;
              break;
            case 2: // back
              ssscreen = -1;
              sscreen = -1;
              EEPROM.write(4,orientation);
              break;
            default:
              max_option = menu(3, positioning_menu, option, true);
              ssscreen = getUpdate(ssscreen);
              break;
          }
          break;
   
        case 1: // shutter menu
          option = 0;
          do {
            shutter_speed = hotbar(shutter_menu, shutter_speed, 16, option, true);
            option = getUpDown(2, option, 0);
            if (!option) shutter_speed = getLeftRight(16, shutter_speed,1, 0);
          } while(!(!digitalRead(SET) && option));
          EEPROM.write(5, shutter_speed);
          sscreen = resetScreen(sscreen);
          break;
        
        case 2: // back
          screen = -1;
          sscreen = -1;
          break;

        default: // [settings menu]
          max_option = menu(3, settings_menu, option, true);
          sscreen = getUpdate(sscreen);
      }
      break;

    /* Main Menu */
    default:
      firstTime = true;
      max_option = menu(3, main_menu, option, true);
      screen = getUpdate(screen);
      break;
  }
  option = getUpDown(max_option, option, 0);
  /*
  Serial.print("Screen: ");
  Serial.println(screen);
  Serial.print("SScreen: ");
  Serial.println(sscreen);
  Serial.print("SSScreen: ");
  Serial.println(ssscreen); */
}
