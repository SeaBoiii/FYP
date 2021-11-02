#include <A4988.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

// for debugging
bool debug = false;

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

// Joystick joy(VRX, VRY, SW)
// creating Joystick object
/* will be done when cleaning up code */

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

// variable declaration
int value = 0;    // debug value

int option = 0;
int focus_max = 0;
int zoom_max = 0;
float shutter_speed = 0.5;

int button = 1; // 0 for click
int x_value = 0;
int y_value = 0;


/* Setting up the Arduino
 * [When first powering up]
 */
void setup() {
  // setting up the joystick
  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);
  pinMode(SW, INPUT_PULLUP);

  // setting up motor (RPM to 1 and Microstepping to 1)
  // -> Lower RPM = Higher torque
  // -> Higher Microstepping = Higher holding torque
  rear_motor.begin(RPM, 1);
  front_motor.begin(RPM, 1);
  
  rear_motor.enable();
  front_motor.enable();

  // on serial if debug is true
  if (debug) {
    Serial.begin(9600);
  }

  // if display fails = no proceed
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // initial display buffer (Adafruit splash screen)
  display.clearDisplay();
  display.display();


  // trys to reads the stored values in memory
  // if empty (==255), setting the default values to 0
  focus_min = EEPROM.read(0);
  zoom_min = EEPROM.read(1);
  diff_focus = EEPROM.read(2);
  diff_zoom = EEPROM.read(3);
  if (focus_min == 255) {
    focus_min = 0;
  }
  if (zoom_min == 255) {
    zoom_min = 0;
  }
  if (diff_focus == 255) {
    diff_focus = 0;
  }
  if (diff_zoom == 255) {
    diff_zoom = 0;
  }
 
  // reads the orientation
  orientation = EEPROM.read(4);
  /*
   * If orientation == 0
   * -> Zoom at the back
   * If orientation == 1
   * -> Zoom at the front
   */
  if (orientation == 255) {
    orientation = 0;
    EEPROM.write(4,0);  
  }

  // reads the shutter data
  shutter_data = EEPROM.read(5);
  if (shutter_data == 255) {
    shutter_speed = 1;
    EEPROM.write(5, shutter_speed);
  } else {
    shutter_speed = shutter_data;    
  }

  // setting up analog stick
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  button = 1;
  delay(500);
}

void(* resetFunc) (void) = 0;

void focus_move(int orientation, int steps) {
  // focus in front
  if (orientation == 0) {
    front_motor.move(steps);
  } 
  // focus at back
  if (orientation == 1) {
    rear_motor.move(-steps);
  }
}

void focus_startMove(int orientation, long steps, long time=0) {
  // focus in front
  if (orientation == 0) {
    front_motor.startMove(steps, time);
  }
  // focus at back
  if (orientation == 1) {
    rear_motor.startMove(-steps, time);
  }  
}

unsigned focus_nextAction(int orientation) {
  unsigned wait_time;
  // focus in front
  if (orientation == 0) {
    wait_time = front_motor.nextAction();
  }
  // focus at back
  if (orientation == 1) {
    wait_time = rear_motor.nextAction();
  }    
  return wait_time;
}

void zoom_move(int orientation, int steps) {
  // zoom at back
  if (orientation == 0) {
    rear_motor.move(-steps);    
  }
  if (orientation == 1) {
    front_motor.move(steps);
  }
}

void zoom_startMove(int orientation, long steps, long time=0) {
  if (orientation == 0) {
    rear_motor.move(-steps, time);    
  }
  if (orientation == 1) {
    front_motor.move(steps, time);
  }
}

unsigned zoom_nextAction(int orientation) {
  unsigned wait_time;
  if (orientation == 0) {
    wait_time = rear_motor.nextAction();
  }
  if (orientation == 1) {
    wait_time = front_motor.nextAction();
  }
  return wait_time;
}

void setFocusRange() {
  focus_motor.setRPM(1);
  
  // set the focus_max
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,5);
  strcpy_P(buffer, (char *)pgm_read_word(&(ring_phrases[0])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[0])));
  display.println(buffer);
  display.display();
  delay(4000);

  display.clearDisplay();
  display.setCursor(0,5);
  display.setTextSize(1);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[1])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
  display.println(buffer);
  display.display();
  delay(4000);
  
  while (button != 0) {
    display.clearDisplay();
    display.setCursor(0,5);
    display.setTextSize(1);
    display.println(F("Set to most focus\n"));
    strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
    display.println(buffer);
    display.drawFastHLine(0,40, display.width(), WHITE);
    display.setTextSize(2);
    display.setCursor(28, 49);
    display.print(F("Max:"));
    display.print(focus_max);
    display.display();
    if (x_value>600 && 450<y_value<850) {
      if (focus_max == 250) {
        focus_max = focus_max;
      }
      else {
        focus_max++;
        focus_move(orientation, 1);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (focus_max == 0) {
        focus_max = focus_max;
      }
      else {
        focus_max--;
        focus_move(orientation, -1);
      }
      delay(100);
    }      
    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);
  }
  
  display.clearDisplay();
  focus_min = focus_max;

  button = 1; // 0 for click
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  delay(500);

  // focus_min
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,5);
  strcpy_P(buffer, (char *)pgm_read_word(&(ring_phrases[1])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[0])));
  display.println(buffer);
  display.display();
  delay(4000);

  display.clearDisplay();
  display.setCursor(0,5);
  display.setTextSize(1);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[1])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
  display.println(buffer);
  display.display();
  delay(4000);
  
  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Set to most blur\n"));
    strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
    display.println(buffer);
    display.drawFastHLine(0,40, display.width(), WHITE);
    display.setTextSize(2);
    display.setCursor(28, 49);
    display.print(F("Min:"));
    display.print(focus_min);
    display.display();
    if (x_value>600 && 450<y_value<850) {
      if (focus_min == focus_max) {
        focus_min = focus_min;
      }
      else {
        focus_min++;
        focus_move(orientation, 1);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (focus_min == -focus_max) {
        focus_min = focus_min;
      }
      else {
        focus_min--;
        focus_move(orientation, -1);
      }
      delay(100);
    }
    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);  
  }

  focus_motor.setRPM(RPM);
  button = 1;
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  delay(500);
}

void setZoomRange() {
  zoom_motor.setRPM(1);
  // set the zoom_max
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,5);
  strcpy_P(buffer, (char *)pgm_read_word(&(ring_phrases[2])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[0])));
  display.println(buffer);
  display.display();
  delay(4000);

  display.clearDisplay();
  display.setCursor(0,5);
  display.setTextSize(1);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[1])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
  display.println(buffer);
  display.display();
  delay(4000);

  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Set to maximum zoom\n"));
    strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
    display.println(buffer);
    display.drawFastHLine(0,40, display.width(), WHITE);
    display.setTextSize(2);
    display.setCursor(28, 49);
    display.print(F("Max:"));
    display.print(zoom_max);
    display.display();
    if (x_value>600 && 450<y_value<850) {
      if (zoom_max == 250) {
        zoom_max = zoom_max;
      }
      else {
        zoom_max++;
        zoom_move(orientation, 1);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (zoom_max == 0) {
        zoom_max = zoom_max;
      }
      else {
        zoom_max--;
        zoom_move(orientation, -1);
      }
      delay(100);
    }

    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);
  }
  
  zoom_min = zoom_max;

  button = 1; // 0 for click
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  delay(500);

  // set the zoom_min
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,5);
  strcpy_P(buffer, (char *)pgm_read_word(&(ring_phrases[3])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[0])));
  display.println(buffer);
  display.display();
  delay(4000);

  display.clearDisplay();
  display.setCursor(0,5);
  display.setTextSize(1);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[1])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
  display.println(buffer);
  display.display();
  delay(4000);

  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Set to minimum zoom\n"));
    strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
    display.println(buffer);
    display.drawFastHLine(0,40, display.width(), WHITE);
    display.setTextSize(2);
    display.setCursor(28, 49);
    display.print(F("Min:"));
    display.print(zoom_min);
    display.display();
    if (x_value>600 && 450<y_value<850) {
      if (zoom_min == zoom_max) {
        zoom_min = zoom_min;
      }
      else {
        zoom_min++;
        zoom_move(orientation, 1);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (zoom_min == -zoom_max) {
        zoom_min = zoom_min;
      }
      else {
        zoom_min--;
        zoom_move(orientation, -1);
      }
      delay(100);
    }

    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);
  }

  zoom_motor.setRPM(RPM);
  button = 1;
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  delay(500);
}

void menu(int num_option, const char *const string_table[], int option_selected) {
  int rect_y = 9;
  display.clearDisplay();
  display.setCursor(0, 0);
  strcpy_P(buffer, (char *)pgm_read_word(&(string_table[0])));
  display.println(buffer);
  
  // displaying the sub menus
  for (int i=0; i<num_option; i++) {
    display.drawRect(0,rect_y,SCREEN_WIDTH,12,WHITE);
    display.setCursor(2,rect_y+2);
    if (i == option_selected) {
      display.setTextColor(BLACK,WHITE);
    } else {
      display.setTextColor(WHITE);
    }
    strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i+1])));
    display.print(buffer);

    rect_y = rect_y+11;
  }
  display.display();
  display.setTextColor(WHITE);
}

int getUpDown(int num_option, int current_option, int delay_ms=200) {
  // joystick down
  if (y_value>600 && 450<x_value<850) {
    delay(delay_ms);
    if (current_option == num_option-1) {
      return 0;
    }
    else return ++current_option;
  }

  // joystick up
  if (y_value<440 && 450<x_value<850) {
    delay(delay_ms);
    if (current_option == 0) {
      return num_option-1;
    }
    else return --current_option;
  }

  return current_option;
}

/* Are You Sure textbox
 * 0 - Calibration not done
 * 1 - Resetting
 */
bool areYouSure(int pick=2) {
  bool sure = false;
  while (button != 0) {
    // moving joystick right
    if (x_value>600 && 450<y_value<850) {
      sure = false;
      delay(200);
    }

    // moving joystick left
    if (x_value<440 && 450<y_value<850) {
      sure = true;
      delay(200);
    }

    display.clearDisplay();
    if (pick == 0) {
      display.setCursor(2,13);
      display.println(F("Calibration not done"));
    }
    if (pick == 1) {
      display.setCursor(13,13);
      display.println(F("Resetting..."));
    }
    display.setCursor(27,22);
    display.println(F("Are you sure?"));
    display.drawRect(43,30,36,12,WHITE);
    display.setCursor(45,32);
    if (sure) {
      display.setTextColor(BLACK,WHITE);
    } else {
      display.setTextColor(WHITE);
    }
    display.print(F("Yes"));
    display.setCursor(64,32);
    if (sure) {
      display.setTextColor(WHITE);
    } else {
      display.setTextColor(BLACK,WHITE);
    }
    display.print(F("No"));
    display.display();
    display.setTextColor(WHITE);
    
    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);
  }
  resetJoy();
  delay(500);
  return sure;
}

void getJoyRead(int delay_ms=0) {
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  button = digitalRead(SW);
  delay(delay_ms);
}

void resetJoy() {
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  button = 1;
}

void getJoyXRead() {
  x_value = analogRead(VRX);
}

void getJoyYRead() {
  y_value = analogRead(VRY);
}

void getJoyButton() {
  button = digitalRead(SW);
}

String wrap(String s, int limit){
  int space = 0;
  int i = 0;
  int line = 0;
  while(i<s.length()){
    
    if(s.substring(i, i+1) == " ") {
      space=i; 
      }
    if(line > limit-1) {
      s=s.substring(0,space)+"~"+s.substring(space+1);line = 0;
      }
    i++;line++;
    }
  s.replace("~","\n");
    return s;
}

/*
void setDisplay(const char *const string_table[]) {
  display.clearDisplay();
  display.setCursor(0, 0);
  for (int i=0; i<string_table.length(); i++) {
    strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i])));
    display.println(buffer);
  }
  display.display();
} */

long toMS(float seconds) {
  return seconds * 1000000;
}

/* Main Loop */
void loop() {
  /* Main Menu
   * - Recalibration (Initialisation)
   * - Basic Movement
   * - Advanced Movement
   * - Quit
   */
  while (button != 0) {
    option = getUpDown(4, option, 200);
    menu(4, main_menu, option);
    getJoyRead();
  }

  /* Main Option */
  int main_option = option;
  option = 0;
  resetJoy();
  delay(500);
  
  // Recalibration/Initialisation
  if (main_option == 0) {
      // calibration
      bool sure = false;
      int re_option = 0;
      RECALMENU:
      while (button != 0) {
        re_option = getUpDown(4, re_option, 200);
        menu(4, recalibration_menu, re_option);
        getJoyRead();
      }

      resetJoy();
      display.clearDisplay();
      display.setTextSize(1);
      delay(500);
        
      /* Logic */
      // zoom recalibration
      if (re_option == 0) {
        if (diff_zoom == 0) {
          sure = true;
        } else {
          sure = areYouSure();          
        }

        if (sure) {
          zoom_max = 0;
          setZoomRange();
          
          diff_zoom = zoom_max - zoom_min;

          EEPROM.write(3,diff_zoom);
  
          zoom_max = diff_zoom;
          zoom_min = 0;

        } else {
          sure = false;
          goto RECALMENU;
        }
        sure = false;
      }
      // focus recalibration
      if (re_option == 1) {
        if (diff_focus == 0) {
          sure = true;          
        } else {
          sure = areYouSure();
        }

        if (sure) {
          focus_max = 0;
          setFocusRange();

          diff_focus = focus_max - focus_min;

          EEPROM.write(2,diff_focus);
  
          focus_max = diff_focus;
          focus_min = 0;
          
        } else {
          sure = false;
          goto RECALMENU;
        }
        sure = false;
      }
      // reset all
      if (re_option == 2) {
        sure = areYouSure();

        if (sure) {
          // reset of variables
          // setting to 255 will reset the memory
          /*
          int focus_min;      // address 0
          int zoom_min;       // address 1
          int diff_focus;     // address 2
          int diff_zoom;      // address 3
          */
          EEPROM.write(0,255);
          EEPROM.write(1,255);
          EEPROM.write(2,255);
          EEPROM.write(3,255);
          resetFunc(); //call reset
        } else {
          sure = false;
          goto RECALMENU;
        }
        sure = false;
      }
      // back
      if (re_option == 3) {
        // {empty placeholder}
      }
    // note that now both focus and zoom motor is at ideal 0 (min)
    // drawing of main menu
    // 0=zoom, 1=focus, 2=presets, 3=recalibration, 4=quit
    resetJoy();
    display.clearDisplay();
    display.setTextSize(1);
    delay(200);
  }
  // Primary Movements 
  /* Menu
   * - Zoom
   * - Focus
   * - Zoom to distance
   * - Back
   */
  if (main_option == 1) {
    // if calibration not done
    if (diff_zoom==0 && diff_focus==0) {
      bool sure = false;
      sure = areYouSure(0);
      resetJoy();
      delay(500);

      if (sure == false) {
        return;
      }
    }
    STARTMENU:
    // main function
    while (button != 0) {
      option = getUpDown(4, option);      
      menu(4, basic_menu, option);
      getJoyRead();
    }

    int start_option = option;
    option = 0;
    int sub_option = 0;
    resetJoy();
    delay(500);

    // manual zoom ring
    if (start_option == 0) {
      float divs = (SCREEN_WIDTH-30)/(float)diff_zoom;
      float zoom_divs = zoom_min; //temp variable
      display.clearDisplay();
      do {
        sub_option = getUpDown(2, sub_option);

        zoom_divs = zoom_min*divs;
        display.setCursor(0,0);
        if (sub_option == 0) {
          display.setTextColor(BLACK,WHITE);

          if (x_value>600 && 450<y_value<850) {
            if (zoom_min == diff_zoom) {
              zoom_min = zoom_min;
            }
            else {
              zoom_min++;
              zoom_motor.move(ZOOM_MOVE);
            }
            delay(100);
          }
          if (x_value<440 && 450<y_value<850) {
            if (zoom_min == 0) {
              zoom_min = 0;
            }
            else {
              zoom_min--;
              zoom_motor.move(-ZOOM_MOVE);
            }
            delay(100);
          }
          
        } else {
          display.setTextColor(WHITE,BLACK);
        }
        display.print(F("|---- Zoom Ring ----|\n"));
        display.setTextColor(WHITE);
        strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[1])));
        display.print(buffer);
        display.print(diff_zoom);
        display.setCursor(2,55);
        if (sub_option == 1) {
          display.setTextColor(BLACK,WHITE);
        } else {
          display.setTextColor(WHITE,BLACK);
        }
        strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[0])));
        display.print(buffer);
        display.drawRect(0,33,SCREEN_WIDTH-26,14,WHITE);
        display.fillRect(2,35,SCREEN_WIDTH-30,10,BLACK);  /* Reset inner rectangle */
        display.fillRect(2,35,(int)zoom_divs,10,WHITE);
        display.setCursor(105,36);
        display.setTextColor(WHITE,BLACK);
        display.print(zoom_min);
        strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[4])));
        display.print(buffer);
        display.display();

        getJoyRead();

        if (sub_option == 1 && button == 0) {
          button = 1;
          delay(500);
          EEPROM.write(1,zoom_min);
          goto STARTMENU;
        } 
      } while (true); 
    }
    // manual focus ring
    if (start_option == 1) {
      float divs = (SCREEN_WIDTH-30)/(float)diff_focus;
      float focus_divs = focus_min; //temp variable
      display.clearDisplay();
      do {
        sub_option = getUpDown(2, sub_option);
        
        focus_divs = focus_min*divs;
        display.setCursor(0,0);
        if (sub_option == 0) {
          display.setTextColor(BLACK,WHITE);

          if (x_value>600 && 450<y_value<850) {
            if (focus_min == diff_focus) {
              focus_min = focus_min;
            }
            else {
              focus_min++;
              focus_motor.move(FOCUS_MOVE);
            }
            delay(100);
          }
          if (x_value<440 && 450<y_value<850) {
            if (focus_min == 0) {
              focus_min = 0;
            }
            else {
              focus_min--;
              focus_motor.move(-FOCUS_MOVE);
            }
            delay(100);
          }
          
        } else {
          display.setTextColor(WHITE,BLACK);
        }
        display.print(F("|---- Focus Ring----|\n"));
        display.setTextColor(WHITE);
        strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[1])));
        display.print(buffer);
        display.print(diff_focus);
        display.setCursor(2,55);
        if (sub_option == 1) {
          display.setTextColor(BLACK,WHITE);
        } else {
          display.setTextColor(WHITE,BLACK);
        }
        strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[0])));
        display.print(buffer);
        display.drawRect(0,33,SCREEN_WIDTH-26,14,WHITE);
        display.fillRect(2,35,SCREEN_WIDTH-30,10,BLACK);  /* Reset inner rectangle */
        display.fillRect(2,35,(int)focus_divs,10,WHITE);
        display.setCursor(105,36);
        display.setTextColor(WHITE,BLACK);
        display.print(focus_min);
        strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[4])));
        display.print(buffer);
        display.display();

        getJoyRead();

        if (sub_option == 1 && button == 0) {
          button = 1;
          delay(500);
          EEPROM.write(0,focus_min);
          goto STARTMENU;
        } 
      } while (true); 
    }
    // zoom to distance
    if (start_option == 2) {
      ZOOM_MENU:
      while (button != 0) {
        option = getUpDown(4, option);
        menu(4, movetodist_menu, option);
        getJoyRead();
      }
      int zoom_option = option;
      option = 0;
      resetJoy();
      delay(500);

      // zoom to max
      if (zoom_option == 0) {
        int steps_to_move = diff_zoom - zoom_min;
        display.clearDisplay();
        display.setCursor(0,5);
        display.println(F("Moving to the \nmaximum zoom"));
        strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[3])));
        display.print(buffer);
        display.print(steps_to_move);
        display.display();
        zoom_motor.startMove((ZOOM_MOVE)*steps_to_move, toMS(shutter_speed));
        unsigned wait_time;        
        do {
          wait_time = zoom_motor.nextAction();
        } while(wait_time);        
        zoom_min = diff_zoom;
        delay(3000);
        EEPROM.write(1,zoom_min);
        goto ZOOM_MENU;
      }
      // zoom to min
      if (zoom_option == 1) {
        int steps_to_move = zoom_min - 0;
        display.clearDisplay();
        display.setCursor(0,5);
        display.println(F("Moving to the \nminimum zoom"));
        strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[3])));
        display.print(buffer);
        display.print(steps_to_move);
        display.display();
        zoom_motor.startMove((-ZOOM_MOVE)*steps_to_move, toMS(shutter_speed));
        unsigned wait_time; 
        do {
          wait_time = zoom_motor.nextAction();          
        } while (wait_time);
        zoom_min = 0;
        delay(3000);
        EEPROM.write(1,zoom_min);
        goto ZOOM_MENU;
      }
      // zoom to specific distance
      if (zoom_option == 2) {
        zoom_motor.setRPM(5);
        float divs = (SCREEN_WIDTH-30)/(float)diff_zoom;
        float zoom_divs = zoom_min; //temp variable
        display.clearDisplay();
        do {
          sub_option = getUpDown(2, sub_option);

          zoom_divs = zoom_min*divs;
          display.setCursor(0,0);
          if (sub_option == 0) {
            display.setTextColor(BLACK,WHITE);

            if (x_value>600 && 450<y_value<850) {
              if (zoom_min == diff_zoom) {
                zoom_min = zoom_min;
              }
              else {
                zoom_min++;
                zoom_motor.move(ZOOM_MOVE);
              }
            }
            if (x_value<440 && 450<y_value<850) {
              if (zoom_min == 0) {
                zoom_min = 0;
              }
              else {
                zoom_min--;
                zoom_motor.move(-ZOOM_MOVE);
              }
            }
            
          } else {
            display.setTextColor(WHITE,BLACK);
          }
          display.print(F("|-Set zoom distance-|\n"));
          display.setTextColor(WHITE);
          strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[1])));
          display.print(buffer);
          display.print(diff_zoom);
          display.setCursor(2,55);
          if (sub_option == 1) {
            display.setTextColor(BLACK,WHITE);
          } else {
            display.setTextColor(WHITE,BLACK);
          }
          strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[2])));
          display.print(buffer);
          display.drawRect(0,33,SCREEN_WIDTH-26,14,WHITE);
          display.fillRect(2,35,SCREEN_WIDTH-30,10,BLACK);  /* Reset inner rectangle */
          display.fillRect(2,35,(int)zoom_divs,10,WHITE);
          display.setCursor(105,36);
          display.setTextColor(WHITE,BLACK);
          display.print(zoom_min);
          strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[4])));
          display.print(buffer);
          display.display();

          getJoyRead();

          if (sub_option == 1 && button == 0) {
            zoom_motor.setRPM(RPM);
            button = 1;
            delay(500);
            int steps_to_move = zoom_min;
            zoom_motor.startMove((-ZOOM_MOVE)*steps_to_move, toMS(shutter_speed/2));
            unsigned wait_time;
            do {
              wait_time = zoom_motor.nextAction();
            } while(wait_time);
            delay(5000);
            display.clearDisplay();
            display.setCursor(0,5);
            display.println(F("Moving to the \nset distance"));
            strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[3])));
            display.print(buffer);
            display.print(steps_to_move);
            display.display();
            zoom_motor.startMove((-ZOOM_MOVE)*steps_to_move, toMS(shutter_speed));
            do {
              wait_time = zoom_motor.nextAction();
            } while(wait_time);        
            delay(3000);
            EEPROM.write(1,zoom_min);
            goto STARTMENU;
          } 
        } while (true);
      }
      // back
      if (zoom_option == 3) {
        goto STARTMENU;
      }
    }
    // back
    if (start_option == 3) {
      return;
    } 
  }

  // Advanced Movements
  // No menu, just presets
  if (main_option == 2) {
    // if calibration not done
    if (diff_zoom==0 && diff_focus==0) {
      bool sure = false;
      sure = areYouSure(0);
      if (sure == false) {
        return;
      }
    }
    ADVMENU:
    while (button != 0) {
      option = getUpDown(5, option);
      menu(5, adv_menu, option);
      getJoyRead();
    }

    int start_option = option;
    option = 0;
    int sub_option = 0;
    resetJoy();
    delay(500);

    // bokeh effect
    if (start_option == 0) {
      zoom_motor.setSpeedProfile(zoom_motor.LINEAR_SPEED, 2000, 500);
      focus_motor.setSpeedProfile(zoom_motor.LINEAR_SPEED, 500, 1000);
      int zoom_steps_to_move = diff_zoom - zoom_min;
      int focus_steps_to_move = diff_focus - focus_min;
      display.clearDisplay();
      display.setCursor(0,5);
      strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[3])));
      display.print(buffer);
      display.print(zoom_steps_to_move);
      display.display();
      zoom_motor.startMove((ZOOM_MOVE)*zoom_steps_to_move, toMS(shutter_speed));
      focus_motor.startMove((FOCUS_MOVE)*focus_steps_to_move, toMS(shutter_speed/2));
      unsigned wait_time;
      unsigned focus_wait_time;
      do {
        wait_time = zoom_motor.nextAction();
        focus_wait_time = focus_motor.nextAction();
        if (!focus_wait_time) {
          focus_motor.startMove((-FOCUS_MOVE)*focus_steps_to_move, toMS(shutter_speed/2));
        }
      } while(wait_time);        
      zoom_min = diff_zoom;
      delay(3000);
      EEPROM.write(0,focus_min);
      EEPROM.write(1,zoom_min);
      goto ADVMENU;
    }
    
    // firework effect
    if (start_option == 1) {
      int steps_to_move = diff_focus - focus_min;
      display.clearDisplay();
      display.setCursor(0,5);
      strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[3])));
      display.print(buffer);
      display.print(steps_to_move);
      display.display();
      focus_motor.startMove((FOCUS_MOVE)*steps_to_move, toMS(shutter_speed/2));
      unsigned wait_time; 
      do {
        wait_time = focus_motor.nextAction();
      } while(wait_time);
      focus_motor.startMove((-FOCUS_MOVE)*steps_to_move, toMS(shutter_speed/2));
      do {
        wait_time = focus_motor.nextAction();
      } while(wait_time);
      delay(3000);
      goto ADVMENU;
    }

    // zoom blur effect
    if (start_option == 2) {
      int zoom_steps_to_move = diff_zoom - zoom_min;
      int focus_steps_to_move = diff_focus - focus_min;
      display.clearDisplay();
      display.setCursor(0,5);
      strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[3])));
      display.print(buffer);
      display.print(zoom_steps_to_move);
      display.display();
      zoom_motor.startMove((ZOOM_MOVE)*zoom_steps_to_move, toMS(shutter_speed));
      focus_motor.startMove((FOCUS_MOVE)*focus_steps_to_move, toMS(shutter_speed/2));
      unsigned wait_time;
      unsigned focus_wait_time;
      do {
        wait_time = zoom_motor.nextAction();
        focus_wait_time = focus_motor.nextAction();
        if (!focus_wait_time) {
          focus_motor.startMove((-FOCUS_MOVE)*focus_steps_to_move, toMS(shutter_speed/2));
        }
      } while(wait_time);        
      zoom_min = diff_zoom;
      delay(3000);
      EEPROM.write(0,focus_min);
      EEPROM.write(1,zoom_min);
      goto ADVMENU;
    }

    // sine wave
    if (start_option == 3) {
      zoom_motor.setSpeedProfile(zoom_motor.LINEAR_SPEED, 1000, 1000);
      int steps_to_move = diff_zoom - zoom_min;
      display.clearDisplay();
      display.setCursor(0,5);
      strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[3])));
      display.print(buffer);
      display.print(steps_to_move);
      display.display();
      zoom_motor.startMove((ZOOM_MOVE)*steps_to_move, toMS(shutter_speed));
      unsigned wait_time;        
      do {
        wait_time = zoom_motor.nextAction();
      } while(wait_time);        
      zoom_min = diff_zoom;
      delay(3000);
      EEPROM.write(1,zoom_min);
      zoom_motor.setSpeedProfile(zoom_motor.CONSTANT_SPEED);
      goto ADVMENU;
    }

    // back
    if (start_option == 4) {
      return;
    }
  }
  
  // Settings menu
  if (main_option == 3) {
    SETTINGS:
    while (button != 0) {
      option = getUpDown(3, option, 200);
      menu(3, settings_menu, option);
      getJoyRead();
    }

    int sub_option = option;
    option = 0;
    resetJoy();
    delay(500);
    
    // orientation
    if (sub_option == 0) {
      while (button != 0) {
        option = getUpDown(3, option, 200);
        menu(3, positioning_menu, option);
        getJoyRead();             
      }

      int position_option = option;
      option = 0;
      resetJoy();
      delay(500);

      if (position_option == 0) {
        orientation = 0;
        EEPROM.write(4,orientation);
      }

      if (position_option == 1) {
        orientation = 1;
        EEPROM.write(4,orientation);
      }
      
      goto SETTINGS;   
    }

    // Shutter Speed
    if (sub_option == 1) {
      if (shutter_speed == 1) {
        option = 0;
      } else if (shutter_speed == 4) {
        option = 1;
      } else if (shutter_speed == 8) {
        option = 2;
      } else if (shutter_speed == 16) {
        option = 3;
      }
      while (button != 0) {
        option = getUpDown(5, option, 200);
        menu(5, shutter_menu, option);
        getJoyRead();
      }

      int shutter_option = option;
      option = 0;
      resetJoy();
      delay(500);

      if (shutter_option == 0) {
        shutter_speed = 1;        
      }
      if (shutter_option == 1) {
        shutter_speed = 4;
      }      
      if (shutter_option == 2) {
        shutter_speed = 8;
      }
      if (shutter_option == 3) {
        shutter_speed = 16;
      }

      EEPROM.write(5, shutter_speed);

      goto SETTINGS;
    }

    // back    
    if (sub_option == 2) {
      return;
    }
  }
}
