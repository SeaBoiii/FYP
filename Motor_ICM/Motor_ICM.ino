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
const char string_cali[] PROGMEM = "Move the joystick to \nthe extreme";
const char string_left[] PROGMEM = "  left";
const char string_right[] PROGMEM = "  right";

const char counttext_1[] PROGMEM = "Get Ready!";
const char counttext_2[] PROGMEM = "3";
const char counttext_3[] PROGMEM = "2";
const char counttext_4[] PROGMEM = "1";
const char counttext_5[] PROGMEM = "SNAP!";

const char adjust_zoom[] PROGMEM = "|--- Adjust Zoom ---|";
const char adjust_focus[] PROGMEM = "|---Adjust Focus ---|";
const char string_36[] PROGMEM = "Adjust [ZOOM] lens";
const char string_36_1[] PROGMEM = "Adjust [FOCUS] lens";
const char string_37[] PROGMEM = "to the desired Image";

const char string_38[] PROGMEM = "to desired Outcome";
const char string_39[] PROGMEM = "Zoom[MAX]&Focus[MIN]";
const char string_40[] PROGMEM = "Zoom[MIN]&Focus[MAX]";

/* String Table */
const char *const main_menu[] PROGMEM = {string_0, string_1, string_2, string_3};
const char *const recalibration_menu[] PROGMEM {string_4, string_5, string_6, string_7, back};
const char *const movement_menu[] PROGMEM = {string_8, string_9, string_10, string_11, string_11_1, back};
const char *const settings_menu[] PROGMEM = {string_12, string_13, string_14, back};
const char *const positioning_menu[] PROGMEM = {string_15, string_16, string_17, back};
const char *const focus_menu[] PROGMEM = {string_19, string_20, string_21, string_22, back};
const char *const zoom_menu[] PROGMEM = {string_23, string_24, string_25, string_26, back};
const char *const zoomfocus_menu[] PROGMEM = {string_27, string_28, string_29, string_39, string_40, string_30, back};
const char *const presets_menu[] PROGMEM = {string_31, string_32, string_33, string_34, string_35, back};
const char *const calizoom_left[] PROGMEM = {cali_zoom, string_cali, string_left};
const char *const calizoom_right[] PROGMEM = {cali_zoom, string_cali, string_right};
const char *const califocus_left[] PROGMEM = {cali_focus, string_cali, string_left};
const char *const califocus_right[] PROGMEM = {cali_focus, string_cali, string_right};
const char *const countdown[] PROGMEM = {counttext_1, counttext_2, counttext_3, counttext_4, counttext_5};
const char *const zoom_adjust[] PROGMEM = {adjust_zoom, string_36, string_37};
const char *const focus_adjust[] PROGMEM = {adjust_focus, string_36_1, string_37};
const char *const focus_dist[] PROGMEM = {string_22, string_36, string_38};
const char *const zoom_dist[] PROGMEM = {string_26, string_36, string_38};
const char *const zoomfocus_dist[] PROGMEM = {string_30, string_36, string_38};

// Object Declaration
/* Motor Objects */
AccelStepper rear_motor(AccelStepper::DRIVER, rear_STEP, rear_DIR);
AccelStepper front_motor(AccelStepper::DRIVER, front_STEP, front_DIR);

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
int menu(int array_size, const char *const string_table[], int option_selected, int header=0, int footer=2, uint16_t color=DEEPPINK);
void hotbar(char title[], int current, int max_range, int current_option=0, bool haveBack=false, int header=0, int footer=3, uint16_t color=WHITE, bool updateBar=false);
void caliMenu(const char *const string_table[], int current_step, int max_steps=200, uint16_t color=WHITE, bool updateBar=false);
void moveMotorMenu(int count, const char *const string_table[], int current_step, int max_steps, uint16_t color=WHITE, bool updateBar=false);
void updateScreen(float delay_ms=0);
int getUpDown(int option, int current_option, int delay_ms=0);
int getLeftRight(int range, int current, int low_limit=0, int delay_ms=0);
void moveMotor(int type, int pos_desired, int shutter_spd=0);
void moveMultiMotor(float zoom_value, float focus_value, float shutter_spd=0);
int calibrate(int type, const char *const string_table[], int upper_limit, int lower_limit, uint16_t color=WHITE);
int chooseDist(int type, int count, const char *const string_table[], bool goBack=false, uint16_t color=WHITE);
void goDist(int type, char title[], int pos_desired, uint16_t color=WHITE, float shutter_spd = shutter_speed/2);
void goMultiDist(char title[], int zoom_desired, int focus_desired, uint16_t color=WHITE, float shutter_spd = shutter_speed/2);
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
  rear_motor.setMaxSpeed(RPM);
  front_motor.setMaxSpeed(RPM);

  steppers.addStepper(rear_motor);
  steppers.addStepper(front_motor);
  
  // ***** Display *****
  // able to call out similar functions
  // in Adafruit GFX lib
  tft.begin();
  tft.setRotation(0);
  tft.background(0,0,0);
  tft.setTextSize(2);

  // This costs 2% (for aesthetic)
  for(int i=0; i<10; i++) { // just to know if initialised properly
    tft.stroke(random(0, 255), random(0, 255), random(0, 255));
    tft.text("MOTOR ICM",6,80);
    delay(200);
  }
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.background(0,0,0);

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
  } else if (focus_current != 255) {
    setCurrentPos(FOCUS, focus_current * MS_STEP);
  }
  if (zoom_current == 255) {
    zoom_current = 0;
  }
  else if (zoom_current != 255) {
    setCurrentPos(ZOOM, zoom_current * MS_STEP);
  }
  
  // ****** Setup Menu ******
  // Only run through if ranges are invalid
  // if range == 255

  // ** sets up shutter speed **
  if (shutter_speed == 255) {  
    shutter_speed = 1;
    hotbar(shutter_menu, shutter_speed, 40, 0, false, 0, 1);
    do {
      hotbar(shutter_menu, shutter_speed, 40, 0, false, 0, 1, GOLDENROD, true);
      shutter_speed = getLeftRight(40, shutter_speed,1, 0);
    } while(digitalRead(SET));
    EEPROM.write(5, shutter_speed);
    updateScreen(500);
  }

  // ** gets the motor orientation **
  if (orientation == 255 || zoom_range == 255 || focus_range == 255) {
    int choice = 0;
    do {
      orientation = choice ? 1 : 0;
      menu(2, positioning_menu, choice, -1);
      choice = getUpDown(2, choice, 0);
    } while (digitalRead(SET));
    updateScreen(500);
    EEPROM.write(4, orientation); 
  }
  
  // ** calibrate zoom ** 
  if (zoom_range == 255) {
    zoom_current = 0;
    setAccel(ZOOM, CALI_ACCEL);
    setCurrentPos(ZOOM, 0);
    
    // set to maximum right
    zoom_current = calibrate(ZOOM, calizoom_right, MOTOR_STEPS, 0, AQUA);
    int maxZoom = zoom_current;
    updateScreen(500);

    // set to minimum left
    zoom_current = calibrate(ZOOM, calizoom_left, maxZoom, maxZoom-MOTOR_STEPS, AQUA);
    zoom_range = maxZoom - zoom_current;
    updateScreen(500);
    EEPROM.write(1, zoom_range);
    
    zoom_current = 0; // minimum becomes absolute min pos
    setCurrentPos(ZOOM, zoom_current);
    EEPROM.write(3, zoom_current);
  }

  // ** calibrate focus **
  if (focus_range == 255) {
    focus_current = 0;
    setAccel(FOCUS, CALI_ACCEL);
    setCurrentPos(FOCUS, 0);

    // set to maximum right
    focus_current = calibrate(FOCUS, califocus_right, MOTOR_STEPS, 0, DEEPPINK);
    int maxFocus = focus_current;
    updateScreen(500);

    // set to minimum left
    focus_current = calibrate(FOCUS, califocus_left, maxFocus, maxFocus-MOTOR_STEPS, DEEPPINK);
    focus_range = maxFocus - focus_current;
    updateScreen();
    EEPROM.write(0, focus_range);
    
    focus_current = 0; // minimum becomes absolute min pos
    setCurrentPos(FOCUS, focus_current);
    EEPROM.write(2, focus_current);
  }

}

void loop() {
  switch(screen) {
    /* Recalibration */
    case 0:
      switch(sscreen) {
        // ** zoom calibration **
        case 0: {
          setCurrentPos(ZOOM, zoom_current * MS_STEP);
          setAccel(ZOOM, CALI_ACCEL);

          moveMotor(ZOOM, zoom_range);
          zoom_current = zoom_range;
          zoom_current = calibrate(ZOOM, calizoom_right, MOTOR_STEPS, 0);
          int maxZoom = zoom_current;
          updateScreen(500);

          moveMotor(ZOOM, 0);
          zoom_current = 0;
          zoom_current = calibrate(ZOOM, calizoom_left, maxZoom, maxZoom-MOTOR_STEPS);
          zoom_range = maxZoom - zoom_current;
          updateScreen();
          EEPROM.write(1, zoom_range);
          
          zoom_current = 0; // minimum becomes absolute min pos
          setCurrentPos(ZOOM, zoom_current);
          EEPROM.write(3, zoom_current);
          sscreen = resetScreen(sscreen);
          break;
        }
        
        // ** focus calibration **
        case 1: {
          setCurrentPos(FOCUS, focus_current * MS_STEP);
          setAccel(FOCUS, CALI_ACCEL);

          moveMotor(FOCUS, focus_range);
          focus_current = focus_range;
          focus_current = calibrate(FOCUS, califocus_right, MOTOR_STEPS, 0);
          int maxFocus = focus_current;
          updateScreen(500);

          moveMotor(FOCUS, 0);
          focus_current = 0;
          focus_current = calibrate(FOCUS, califocus_left, maxFocus, maxFocus-MOTOR_STEPS);
          focus_range = maxFocus - focus_current;
          updateScreen();
          EEPROM.write(0, focus_range);
          
          focus_current = 0; // minimum becomes absolute min pos
          setCurrentPos(ZOOM, focus_current);
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
          EEPROM.write(4,255);
          EEPROM.write(5,255);
          resetFunc();
          break; 
        }
          
        // ** back **
        case 3:
          screen = -1;
          sscreen = -1;
          break;
        default:
          max_option = menu(4, recalibration_menu, option, 2);
          sscreen = getUpdate(sscreen);
          break;
      }
      break;
      
    /* Movements */
    case 1:
      switch(sscreen) {
        case 0: // ** focus **
          switch(ssscreen) {
            case 0: { // focus to max
              countdownMenu();
              goDist(FOCUS, string_20, focus_range, SNOW);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 1: { // focus to min
              countdownMenu();
              goDist(FOCUS, string_21, 0, VIOLET);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 2: { // focus to dist
              int pos_desired;
              pos_desired = chooseDist(FOCUS, 3, focus_dist, true, YELLOWGREEN);
              updateScreen();
              delay(500);
              countdownMenu();
              goDist(FOCUS, string_22, pos_desired, YELLOWGREEN);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 3: // back
              sscreen = -1;
              ssscreen = -1;
              break;
            default:
              max_option = menu(4, focus_menu, option, 3);
              ssscreen = getUpdate(ssscreen);
          }
          break;
        case 1: // ** zoom **
          switch(ssscreen) {
            case 0: { // zoom to max
              countdownMenu();
              goDist(ZOOM, string_24, zoom_range, SNOW);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 1: { // zoom to min
              countdownMenu();
              goDist(ZOOM, string_25, 0, VIOLET);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 2: { // zoom to dist
              int pos_desired;
              pos_desired = chooseDist(ZOOM, 3, zoom_dist, true, YELLOWGREEN);
              updateScreen();
              delay(500);
              countdownMenu();
              goDist(ZOOM, string_26, pos_desired, YELLOWGREEN);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 3: // back
              sscreen = -1;
              ssscreen = -1;
              break;
            default:
              max_option = menu(4, zoom_menu, option, 4);
              ssscreen = getUpdate(ssscreen);
          }
          break;
        case 2: // ** focus & zoom **
          switch(ssscreen) {
            case 0: { // both to max
              countdownMenu();
              goMultiDist(string_28, zoom_range, focus_range, VIOLET);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 1: { // both to min
              countdownMenu();
              goMultiDist(string_29, 0, 0, AZURE);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 2: { // zoom to max, focus to min
              countdownMenu();
              goMultiDist(string_30, zoom_range, 0, CORAL);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 3: { // zoom to min, focus to max
              countdownMenu();
              goMultiDist(string_39, 0, focus_range, CADETBLUE);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 4: { // to certain dist
              int zoom_desired, focus_desired;
              zoom_desired = chooseDist(ZOOM, 3, zoom_dist, true, YELLOWGREEN);
              if (zoom_desired == zoom_current) {
                zoom_desired = -1;
              }
              updateScreen(500);
              
              focus_desired = chooseDist(FOCUS, 3, focus_dist, true, RED);
              if (focus_desired == focus_current) {
                focus_desired = -1;
              }
              updateScreen();
              
              delay(500);
              countdownMenu();
              goMultiDist(string_40, zoom_desired, focus_desired, LIME);
              ssscreen = resetScreen(ssscreen);
              break;
            }
            case 5: { // back
              sscreen = -1;
              ssscreen = -1;
              break;
            }
            default:
              max_option = menu(6, zoomfocus_menu, option, 2);
              ssscreen = getUpdate(ssscreen);
          }
          break;
        case 3: // ** presets **
          switch(ssscreen) {
            case 1:
            case 2:
            case 3:
            case 4: // back
              sscreen = -1;
              ssscreen = -1;
              break;
            default:
              max_option = menu(5, presets_menu, option, 2);
              ssscreen = getUpdate(ssscreen);
          }
          break;
        case 4: // back
          screen = -1;
          sscreen = -1;
          break;
        default: {  // [Movement Menu] 
          if (firstTime) {
            setAccel(ZOOM, CALI_ACCEL);
            setAccel(FOCUS, CALI_ACCEL);
            
            zoom_current = chooseDist(ZOOM, 3, zoom_adjust, false, AQUA);
            EEPROM.write(3, zoom_current);
            updateScreen(500);
            focus_current = chooseDist(FOCUS, 3, focus_adjust, false, DEEPPINK);
            EEPROM.write(2, focus_current);
            updateScreen(500);
            firstTime = false;
          }
          max_option = menu(5, movement_menu, option, 2);
          sscreen = getUpdate(sscreen);
        }
      }
      break;

    /* Settings */
    case 2:
      switch (sscreen) {
        case 0: // ** orientation menu **
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
              max_option = menu(3, positioning_menu, option, 1);
              ssscreen = getUpdate(ssscreen);
              break;
          }
          break;
   
        case 1: // ** shutter menu **
          option = 0;
          hotbar(shutter_menu, shutter_speed, 40, option, true);
          do {
            hotbar(shutter_menu, shutter_speed, 40, option, true, 0, 3, GOLDENROD, true);
            option = getUpDown(2, option, 0);
            if (!option) shutter_speed = getLeftRight(40, shutter_speed,1, 0);
          } while(!(!digitalRead(SET) && option));
          EEPROM.write(5, shutter_speed);
          sscreen = resetScreen(sscreen);
          break;
        
        case 2: // ** back **
          screen = -1;
          sscreen = -1;
          break;

        default: // [settings menu]
          max_option = menu(3, settings_menu, option, 1);
          sscreen = getUpdate(sscreen);
      }
      break;

    /* Main Menu */
    default:
      firstTime = true;
      max_option = menu(3, main_menu, option, 2);
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
