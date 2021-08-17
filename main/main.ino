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

// Arduino Pins
/* Motor Pins */
#define focus_DIR 3
#define focus_STEP 4
#define zoom_DIR 5
#define zoom_STEP 6

/* Joystick Pins */
#define VRX A0
#define VRY A1
#define SW 12

/* Motor Movements*/
#define FOCUS_MOVE 1
#define ZOOM_MOVE -1

// A4988 stepper(MOTOR_STEPS, DIR, STEP, MS1, MS2, MS3)
// focus ring is infront compared to zoom ring
// motor objects
A4988 focus_motor(MOTOR_STEPS, 3, 4);
A4988 zoom_motor(MOTOR_STEPS, 5,6);

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

// variable declaration
int value = 0;    // debug value

int option = 0;
int focus_max = 0;
int zoom_max = 0;

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
  focus_motor.begin(1, 1);
  zoom_motor.begin(1, 1);
  

  // on serial if debug is true
  if (debug) {
    Serial.begin(9600);
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // initial display buffer (Adafruit splash screen)
  display.clearDisplay();
  display.display();
  delay(3000);


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
    diff_zoom =0;
  }
 
  // setting up
  /*
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(2, 5);
  display.println(F("Setting up"));
  display.setCursor(12, 22);
  display.println(F("Motors..."));
  display.drawFastHLine(0,40, display.width(), WHITE);
  display.setTextSize(1);
  display.setCursor(1, 45);
  display.println(F("Please follow instructions"));
  display.display();
  delay(3000); */

  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  button = 1;
  delay(500);
}

void(* resetFunc) (void) = 0;

void setFocusRange() {
  // set the focus_max
  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Max range(focus ring)\n"));
    display.println(F("Press to confirm."));
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
        focus_motor.move(FOCUS_MOVE);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (focus_max == 0) {
        focus_max = focus_max;
      }
      else {
        focus_max--;
        focus_motor.move(-FOCUS_MOVE);
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
  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Min range(focus ring)\n"));
    display.println(F("Press to confirm."));
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
        focus_motor.move(FOCUS_MOVE);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (focus_min == -focus_max) {
        focus_min = focus_min;
      }
      else {
        focus_min--;
        focus_motor.move(-FOCUS_MOVE);
      }
      delay(100);
    }

    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);
  }

  button = 1;
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  delay(500);
}

void setZoomRange() {
  // set the zoom_max
  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Max range (zoom ring)\n"));
    display.println(F("Press to confirm."));
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
        zoom_motor.move(ZOOM_MOVE);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (zoom_max == 0) {
        zoom_max = zoom_max;
      }
      else {
        zoom_max--;
        zoom_motor.move(-ZOOM_MOVE);
      }
      delay(100);
    }

    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);
  }
  
  display.clearDisplay();
  zoom_min = zoom_max;

  button = 1; // 0 for click
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  delay(500);

  // set the zoom_min
  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Min range (zoom ring)\n"));
    display.println(F("Press to confirm."));
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
        zoom_motor.move(ZOOM_MOVE);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (zoom_min == -zoom_max) {
        zoom_min = zoom_min;
      }
      else {
        zoom_min--;
        zoom_motor.move(-ZOOM_MOVE);
      }
      delay(100);
    }

    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);
  }
}

/* Main Loop */
void loop() {
  /* Main Menu
   * - Recalibration (Initialisation)
   * - Start
   * - Quit
   */
  while (button != 0) {
    // moving joystick down
    if (y_value>600 && 450<x_value<850) {
      if (option == 2) {
        option = 0;
      }
      else {
        option++;
      }
      delay(200);
    }

    // moving joystick up
    if (y_value<440 && 450<x_value<850) {
      if (option == 0) {
        option = 2;
      }
      else {
        option--;
      }
      delay(200);
    }

    // selected -> Recalibration or Initialisation 
    if (option == 0) {
      display.clearDisplay();
      display.setCursor(0,0);
      display.print(F("|---- Main Menu ----|\n"));
      display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,11);
      display.setTextColor(BLACK,WHITE);
      if (diff_zoom==0 && diff_focus==0) {
        display.print(F("Initialisation"));
      } else {
        display.print(F("Recalibration"));
      }
      display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
      display.setTextColor(WHITE);
      display.setCursor(2,22);
      display.print(F("Start.exe"));
      display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,33);
      display.print(F("Quit()"));
      display.display();
      display.setTextColor(WHITE);
    }
    // selected -> start
    if (option == 1) {
      display.clearDisplay();
      display.setCursor(0,0);
      display.print(F("|---- Main Menu ----|\n"));
      display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,11);
      if (diff_zoom==0 && diff_focus==0) {
        display.print(F("Initialisation"));
      } else {
        display.print(F("Recalibration"));
      }
      display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
      display.setTextColor(BLACK,WHITE);
      display.setCursor(2,22);
      display.print(F("Start.exe"));
      display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
      display.setTextColor(WHITE);
      display.setCursor(2,33);
      display.print(F("Quit()"));
      display.display();
      display.setTextColor(WHITE);
    }
    // selected -> quit
    if (option == 2) {
      display.clearDisplay();
      display.setCursor(0,0);
      display.print(F("|---- Main Menu ----|\n"));
      display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,11);
      if (diff_zoom==0 && diff_focus==0) {
        display.print(F("Initialisation"));
      } else {
        display.print(F("Recalibration"));
      }
      display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,22);
      display.print(F("Start.exe"));
      display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
      display.setTextColor(BLACK,WHITE);
      display.setCursor(2,33);
      display.print(F("Quit()"));
      display.display();
      display.setTextColor(WHITE);
    }

    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);
  }

  /* Main Option */
  int main_option = option;
  option = 0;
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  button = 1;
  delay(500);
  
  // Recalibration/Initialisation
  if (main_option == 0) {
      // initialisation (no memory was stored)
      if (diff_zoom==0 && diff_focus==0) {
        setFocusRange();
        setZoomRange();

        // calculating range
        diff_focus = focus_max - focus_min;
        diff_zoom = zoom_max - zoom_min;

        // saving of variables
        /*
        int focus_min;      // address 0
        int zoom_min;       // address 1
        int diff_focus;     // address 2
        int diff_zoom;      // address 3
        */
        EEPROM.write(2,diff_focus);
        EEPROM.write(3,diff_zoom);

        focus_max = diff_focus;
        focus_min = 0;
        zoom_max = diff_zoom;
        zoom_min = 0;

      }
      /* If user has initialised:
       *  - Zoom Recalibration
       *  - Focus Recalibration
       *  - Reset All
       *  - Back
       */
      else {
        bool sure = false;
        int re_option = 0;
        RECALMENU:
        while (button != 0) {
          // moving joystick down
          if (y_value>600 && 450<x_value<850) {
            if (re_option == 3) {
              re_option = 0;
            }
            else {
              re_option++;
            }
            delay(200);
          }
      
          // moving joystick up
          if (y_value<440 && 450<x_value<850) {
            if (re_option == 0) {
              re_option = 3;
            }
            else {
              re_option--;
            }
            delay(200);
          }
          // Zoom Recalibration
          if (re_option == 0) {
            display.clearDisplay();
            display.setCursor(0,0);
            display.print(F("|---Recalibration---|\n"));
            display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,11);
            display.setTextColor(BLACK,WHITE);
            display.print(F("Zoom Recalibration"));
            display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
            display.setTextColor(WHITE);
            display.setCursor(2,22);
            display.print(F("Focus Recalibration"));
            display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,33);
            display.print(F("RESET all values"));
            display.drawRect(0,42,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,44);
            display.print(F("Back"));
            display.display();
            display.setTextColor(WHITE);
          }
          // Foccus Recalibration
          if (re_option == 1) {
            display.clearDisplay();
            display.setCursor(0,0);
            display.print(F("|---Recalibration---|\n"));
            display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,11);
            display.print(F("Zoom Recalibration"));
            display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
            display.setTextColor(BLACK,WHITE);
            display.setCursor(2,22);
            display.print(F("Focus Recalibration"));
            display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,33);
            display.setTextColor(WHITE);
            display.print(F("RESET all values"));
            display.drawRect(0,42,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,44);
            display.print(F("Back"));
            display.display();
            display.setTextColor(WHITE);
          }
          // Reset all
          if (re_option == 2) {
            display.clearDisplay();
            display.setCursor(0,0);
            display.print(F("|---Recalibration---|\n"));
            display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,11);       
            display.print(F("Zoom Recalibration"));
            display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,22);
            display.print(F("Focus Recalibration"));
            display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,33);
            display.setTextColor(BLACK,WHITE);
            display.print(F("RESET all values"));
            display.drawRect(0,42,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,44);
            display.setTextColor(WHITE);
            display.print(F("Back"));
            display.display();
            display.setTextColor(WHITE);
          }
          // Back
          if (re_option == 3) {
            display.clearDisplay();
            display.setCursor(0,0);
            display.print(F("|---Recalibration---|\n"));
            display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,11);
            display.print(F("Zoom Recalibration"));
            display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,22);
            display.print(F("Focus Recalibration"));
            display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,33);
            display.print(F("RESET all values"));
            display.drawRect(0,42,SCREEN_WIDTH,12,WHITE);
            display.setCursor(2,44);
            display.setTextColor(BLACK,WHITE);
            display.print(F("Back"));
            display.display();
            display.setTextColor(WHITE);
          }

          x_value = analogRead(VRX);
          y_value = analogRead(VRY);
          button = digitalRead(SW);
        }

        // note that now both focus and zoom motor is at ideal 0 (min)
        // drawing of main menu
        // 0=zoom, 1=focus, 2=presets, 3=recalibration, 4=quit
        x_value = analogRead(VRX);
        y_value = analogRead(VRY);
        button = 1;
        display.clearDisplay();
        display.setTextSize(1);
        delay(500);
        
        /* Logic */
        // zoom recalibration
        if (re_option == 0) {
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

            if (sure) {
              display.clearDisplay();
              display.setCursor(27,22);
              display.println(F("Are you sure?"));
              display.drawRect(43,30,36,12,WHITE);
              display.setCursor(45,32);
              display.setTextColor(BLACK,WHITE);
              display.print(F("Yes"));
              display.setCursor(64,32);
              display.setTextColor(WHITE);
              display.print(F("No"));
              display.display();
              display.setTextColor(WHITE);
            } else {
              display.clearDisplay();
              display.setCursor(27,22);
              display.println(F("Are you sure?"));
              display.drawRect(43,30,36,12,WHITE);
              display.setCursor(45,32);
              display.print(F("Yes"));
              display.setCursor(64,32);
              display.setTextColor(BLACK,WHITE);
              display.print(F("No"));
              display.display();
              display.setTextColor(WHITE);
            }
            x_value = analogRead(VRX);
            y_value = analogRead(VRY);
            button = digitalRead(SW);
          }
          x_value = analogRead(VRX);
          y_value = analogRead(VRY);
          button = 1;
          delay(500);
          
          if (sure) {
            setZoomRange();
            
            diff_zoom = zoom_max - zoom_min;

            EEPROM.write(3,diff_zoom);
    
            zoom_max = diff_zoom;
            zoom_min = 0;

          } else {
            goto RECALMENU;
          }
          sure = false;
        }
        // focus recalibration
        if (re_option == 1) {
          // add are you sure?
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

            if (sure) {
              display.clearDisplay();
              display.setCursor(27,22);
              display.println(F("Are you sure?"));
              display.drawRect(43,30,36,12,WHITE);
              display.setCursor(45,32);
              display.setTextColor(BLACK,WHITE);
              display.print(F("Yes"));
              display.setCursor(64,32);
              display.setTextColor(WHITE);
              display.print(F("No"));
              display.display();
              display.setTextColor(WHITE);
            } else {
              display.clearDisplay();
              display.setCursor(27,22);
              display.println(F("Are you sure?"));
              display.drawRect(43,30,36,12,WHITE);
              display.setCursor(45,32);
              display.print(F("Yes"));
              display.setCursor(64,32);
              display.setTextColor(BLACK,WHITE);
              display.print(F("No"));
              display.display();
              display.setTextColor(WHITE);
            }
            x_value = analogRead(VRX);
            y_value = analogRead(VRY);
            button = digitalRead(SW);
          }
          x_value = analogRead(VRX);
          y_value = analogRead(VRY);
          button = 1;
          delay(500);

          if (sure) {
            setFocusRange();

            diff_focus = focus_max - focus_min;

            EEPROM.write(2,diff_focus);
    
            focus_max = diff_focus;
            focus_min = 0;
            
          } else {
            goto RECALMENU;
          }
          sure = false;
        }
        // reset all
        if (re_option == 2) {
          // add are you sure?
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

            if (sure) {
              display.clearDisplay();
              display.setCursor(27,22);
              display.println(F("Are you sure?"));
              display.drawRect(43,30,36,12,WHITE);
              display.setCursor(45,32);
              display.setTextColor(BLACK,WHITE);
              display.print(F("Yes"));
              display.setCursor(64,32);
              display.setTextColor(WHITE);
              display.print(F("No"));
              display.display();
              display.setTextColor(WHITE);
            } else {
              display.clearDisplay();
              display.setCursor(27,22);
              display.println(F("Are you sure?"));
              display.drawRect(43,30,36,12,WHITE);
              display.setCursor(45,32);
              display.print(F("Yes"));
              display.setCursor(64,32);
              display.setTextColor(BLACK,WHITE);
              display.print(F("No"));
              display.display();
              display.setTextColor(WHITE);
            }
            x_value = analogRead(VRX);
            y_value = analogRead(VRY);
            button = digitalRead(SW);
          }
          x_value = analogRead(VRX);
          y_value = analogRead(VRY);
          button = 1;
          delay(500);

          if (sure) {
            // reset of variables
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
          } 
          sure = false;
        }
        // back
        if (re_option == 3) {
          // {empty placeholder}
        }
      }
      // note that now both focus and zoom motor is at ideal 0 (min)
      // drawing of main menu
      // 0=zoom, 1=focus, 2=presets, 3=recalibration, 4=quit
      x_value = analogRead(VRX);
      y_value = analogRead(VRY);
      button = 1;
      display.clearDisplay();
      display.setTextSize(1);
      delay(500);
  }
  // Start.exe 
  /* Start Menu
   * - Zoom
   * - Focus
   * - Presets
   * - Back
   */
  if (main_option == 1) {
    // if calibration not done
    if (diff_zoom==0 && diff_focus==0) {
      bool sure = false;
      // add are you sure?
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

        if (sure) {
          display.clearDisplay();
          display.setCursor(0,13);
          display.println(F("Calibration not done."));
          display.setCursor(27,22);
          display.println(F("Are you sure?"));
          display.drawRect(43,30,36,12,WHITE);
          display.setCursor(45,32);
          display.setTextColor(BLACK,WHITE);
          display.print(F("Yes"));
          display.setCursor(64,32);
          display.setTextColor(WHITE);
          display.print(F("No"));
          display.display();
          display.setTextColor(WHITE);
        } else {
          display.clearDisplay();
          display.setCursor(0,13);
          display.println(F("Calibration not done."));
          display.setCursor(27,22);
          display.println(F("Are you sure?"));
          display.drawRect(43,30,36,12,WHITE);
          display.setCursor(45,32);
          display.print(F("Yes"));
          display.setCursor(64,32);
          display.setTextColor(BLACK,WHITE);
          display.print(F("No"));
          display.display();
          display.setTextColor(WHITE);
        }
        x_value = analogRead(VRX);
        y_value = analogRead(VRY);
        button = digitalRead(SW);
      }
      x_value = analogRead(VRX);
      y_value = analogRead(VRY);
      button = 1;
      delay(500);

      if (sure == false) {
        return;
      }
    }
    while (button != 0) {
      // down
      if (y_value>600 && 450<x_value<850) {
        if (option == 3) {
          option = 0;
        }
        else {
          option++;
        }
        delay(200);
      }
      // up
      if (y_value<440 && 450<x_value<850) {
        if (option == 0) {
          option = 3;
        }
        else {
          option--;
        }
        delay(200);
      }
      // zoom
      if (option == 0) {
        display.clearDisplay();
        display.setCursor(0,0);
        display.print(F("|---- Start Menu----|\n"));
        display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,11); 
        display.setTextColor(BLACK,WHITE);
        display.print(F("Adjust Zoom"));
        display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
        display.setTextColor(WHITE);
        display.setCursor(2,22);
        display.print(F("Adjust Focus"));
        display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,33);
        display.print(F("Presets (WIP)"));
        display.drawRect(0,42,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,44);
        display.print(F("Back"));
        display.display();
        display.setTextColor(WHITE);
      }
      // focus
      else if (option == 1) {
        display.clearDisplay();
        display.setCursor(0,0);
        display.print(F("|---- Start Menu----|\n"));
        display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,11); 
        display.print(F("Adjust Zoom"));
        display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
        display.setTextColor(BLACK,WHITE);
        display.setCursor(2,22);
        display.print(F("Adjust Focus"));
        display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,33);
        display.setTextColor(WHITE);
        display.print(F("Presets (WIP)"));
        display.drawRect(0,42,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,44);
        display.print(F("Back"));
        display.display();
        display.setTextColor(WHITE);
      }
      // presets
      else if (option == 2) {
        display.clearDisplay();
        display.setCursor(0,0);
        display.print(F("|---- Start Menu----|\n"));
        display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,11); 
        display.print(F("Adjust Zoom"));
        display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,22);
        display.print(F("Adjust Focus"));
        display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,33);
        display.setTextColor(BLACK,WHITE);
        display.print(F("Presets (WIP)"));
        display.drawRect(0,42,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,44);
        display.setTextColor(WHITE);
        display.print(F("Back"));
        display.display();
        display.setTextColor(WHITE);
      }
      // back
      else if (option == 3) {
        display.clearDisplay();
        display.setCursor(0,0);
        display.print(F("|---- Start Menu----|\n"));
        display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,11); 
        display.print(F("Adjust Zoom"));
        display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,22);
        display.print(F("Adjust Focus"));
        display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,33);
        display.print(F("Presets (WIP)"));
        display.drawRect(0,42,SCREEN_WIDTH,12,WHITE);
        display.setCursor(2,44);
        display.setTextColor(BLACK,WHITE);
        display.print(F("Back"));
        display.display();
        display.setTextColor(WHITE);
      }

      x_value = analogRead(VRX);
      y_value = analogRead(VRY);
      button = digitalRead(SW);
    }

    int start_option = option;
    option = 0;
    int sub_option = 0;
    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = 1;
    delay(500);

    // zoom ring
    if (start_option == 0) {
      int divs = (SCREEN_WIDTH-30)/diff_zoom;
      int zoom_divs = zoom_min; //temp variable
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextColor(BLACK,WHITE);
      display.print(F("|---- Zoom Ring ----|\n"));
      display.setTextColor(WHITE);
      display.print(F("\nRange:  "));
      display.print(diff_zoom);
      display.drawRect(0,33,SCREEN_WIDTH-26,14,WHITE);
      display.fillRect(2,35,SCREEN_WIDTH-30,10,BLACK);  /* Reset inner rectangle */
      display.fillRect(2,35,zoom_divs,10,WHITE);
      display.setCursor(108,36);
      display.print(zoom_min);

      display.drawRect(0,53,30,12,WHITE);
      display.setCursor(2,55);
      display.setTextColor(WHITE);
      display.print(F("Back"));
      display.display();
      while (true) {
        // down
        if (y_value>600 && 450<x_value<850) {
          if (sub_option == 1) {
            sub_option = 1;
          }
          else {
            sub_option++;
          }
          delay(200);
        }
        // up
        if (y_value<440 && 450<x_value<850) {
          if (option == 0) {
            sub_option = 0;
          }
          else {
            sub_option--;
          }
          delay(200);
        }

        if (sub_option == 0) {
          zoom_divs = zoom_min*divs;
          display.setCursor(0,0);
          display.setTextColor(BLACK,WHITE);
          display.print(F("|---- Zoom Ring ----|\n"));
          display.setTextColor(WHITE,BLACK);
          display.setCursor(2,55);
          display.print(F("Back"));
          display.fillRect(2,35,SCREEN_WIDTH-30,10,BLACK);  /* Reset inner rectangle */
          display.fillRect(2,35,zoom_divs,10,WHITE);
          display.setCursor(108,36);
          display.print(zoom_min);
          display.display();

          if (x_value>600 && 450<y_value<850) {
            if (zoom_min == diff_zoom) {
              zoom_min = zoom_min;
            }
            else {
              zoom_min++;
              zoom_motor.rotate(ZOOM_MOVE);
            }
            delay(200);
          }
          if (x_value<440 && 450<y_value<850) {
            if (zoom_min == 0) {
              zoom_min = 0;
            }
            else {
              zoom_min--;
              zoom_motor.rotate(-ZOOM_MOVE);
            }
            delay(200);
          }
        }

        if (sub_option == 1) {
          display.setCursor(0,0);
          display.setTextColor(WHITE,BLACK);
          display.print(F("|---- Zoom Ring ----|\n"));
          display.setTextColor(BLACK,WHITE);
          display.setCursor(2,55);
          display.print(F("Back"));
          display.display();
        }
        x_value = analogRead(VRX);
        y_value = analogRead(VRY);
        button = digitalRead(SW);

        if (sub_option == 1 && button == 0) {
          button = 1;
          delay(500);
          EEPROM.write(1,zoom_min);
          break;
        }
      }
      
    }
    // focus ring
    if (start_option == 1) {
      int divs = (SCREEN_WIDTH-30)/diff_focus;
      int focus_divs = focus_min; //temp variable
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextColor(BLACK,WHITE);
      display.print(F("|---- Focus Ring ----|\n"));
      display.setTextColor(WHITE);
      display.print(F("\nRange:  "));
      display.print(diff_focus);
      display.drawRect(0,33,SCREEN_WIDTH-26,14,WHITE);
      display.fillRect(2,35,SCREEN_WIDTH-30,10,BLACK);  /* Reset inner rectangle */
      display.fillRect(2,35,focus_divs,10,WHITE);
      display.setCursor(108,36);
      display.print(focus_min);

      display.drawRect(0,53,30,12,WHITE);
      display.setCursor(2,55);
      display.setTextColor(WHITE);
      display.print(F("Back"));
      display.display();
      while (true) {
        // down
        if (y_value>600 && 450<x_value<850) {
          if (sub_option == 1) {
            sub_option = 1;
          }
          else {
            sub_option++;
          }
          delay(200);
        }
        // up
        if (y_value<440 && 450<x_value<850) {
          if (option == 0) {
            sub_option = 0;
          }
          else {
            sub_option--;
          }
          delay(200);
        }

        if (sub_option == 0) {
          focus_divs = focus_min*divs;
          display.setCursor(0,0);
          display.setTextColor(BLACK,WHITE);
          display.print(F("|---- Focus Ring ----|\n"));
          display.setTextColor(WHITE,BLACK);
          display.setCursor(2,55);
          display.print(F("Back"));
          display.fillRect(2,35,SCREEN_WIDTH-30,10,BLACK);  /* Reset inner rectangle */
          display.fillRect(2,35,focus_divs,10,WHITE);
          display.setCursor(108,36);
          display.print(focus_min);
          display.display();

          if (x_value>600 && 450<y_value<850) {
            if (focus_min == diff_focus) {
              focus_min = focus_min;
            }
            else {
              focus_min++;
              focus_motor.rotate(FOCUS_MOVE);
            }
            delay(200);
          }
          if (x_value<440 && 450<y_value<850) {
            if (focus_min == 0) {
              focus_min = 0;
            }
            else {
              focus_min--;
              focus_motor.rotate(-FOCUS_MOVE);
            }
            delay(200);
          }
        }

        if (sub_option == 1) {
          display.setCursor(0,0);
          display.setTextColor(WHITE,BLACK);
          display.print(F("|---- Focus Ring ----|\n"));
          display.setTextColor(BLACK,WHITE);
          display.setCursor(2,55);
          display.print(F("Back"));
          display.display();
        }
        x_value = analogRead(VRX);
        y_value = analogRead(VRY);
        button = digitalRead(SW);

        if (sub_option == 1 && button == 0) {
          button = 1;
          delay(500);
          EEPROM.write(0,focus_min);
          break;
        }
      }
      
    }
    if (start_option == 2) {
      // execute 
    }
    // back
    if (start_option == 3) {
      return;
    }  
  }

  // Quit()
  if (main_option == 2) {
    // Power off the pin
  }
  
  // debug for joystick
  if (debug) {
    value = analogRead(A0);  // read X axis value [0..1023]
    Serial.print("X:");
    Serial.print(value, DEC);
  
    value = analogRead(A1); // read Y axis value [0..1023]
    Serial.print(" | Y:");
    Serial.print(value, DEC);
  
    value = digitalRead(12); // read Button state [0,1]
    Serial.print(" | Button:");
    Serial.println(value, DEC);
  }

}
