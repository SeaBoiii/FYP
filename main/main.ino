#include "A4988.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

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

// A4988 stepper(MOTOR_STEPS, DIR, STEP, MS1, MS2, MS3)
// focus ring is infront compared to zoom ring
A4988 focus_motor(MOTOR_STEPS, 3, 4);
A4988 zoom_motor(MOTOR_STEPS, 5,6);

// display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// EEPROM declaration (0-255 int)

// variable declaration
int value = 0;

int option = 0;
int focus_max = 0;
int focus_min = 0;
int zoom_max = 0;
int zoom_min = 0;

int button = 1; // 0 for click
int x_value = 0;
int y_value = 0;

int diff_focus = 0;
int diff_zoom = 0;

void setup() {
  // setting up the joystick
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(12, INPUT_PULLUP);

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
  //display.clearDisplay();
  //display.display();
  //delay(2000);

  // setting up
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
  delay(3000);

  // set the focus_max
  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Max range(focus ring)\n"));
    display.println(F("Press joystick to \nconfirm."));
    display.drawFastHLine(0,40, display.width(), WHITE);
    display.setTextSize(2);
    display.setCursor(28, 49);
    display.print(F("Max:"));
    display.print(focus_max);
    display.display();
    if (x_value>600 && 450<y_value<850) {
      if (focus_max == 360) {
        focus_max = focus_max;
      }
      else {
        focus_max++;
        focus_motor.rotate(1);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (focus_max == 0) {
        focus_max = focus_max;
      }
      else {
        focus_max--;
        focus_motor.rotate(-1);
      }
      delay(100);
    }

    x_value = analogRead(A0);
    y_value = analogRead(A1);
    button = digitalRead(12);
  }

  delay(500);
  
  // set the focus_min
  display.clearDisplay();
  focus_min = focus_max;

  button = 1; // 0 for click
  x_value = analogRead(A0);
  y_value = analogRead(A1);

  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Min range(focus ring)\n"));
    display.println(F("Press joystick to \nconfirm."));
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
        focus_motor.rotate(1);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (focus_min == 0) {
        focus_min = focus_min;
      }
      else {
        focus_min--;
        focus_motor.rotate(-1);
      }
      delay(100);
    }

    x_value = analogRead(A0);
    y_value = analogRead(A1);
    button = digitalRead(12);
  }

  delay(500);

  // set the zoom_max
  button = 1;
  x_value = analogRead(A0);
  y_value = analogRead(A1);
  
  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Max range (zoom ring)\n"));
    display.println(F("Press joystick to \nconfirm."));
    display.drawFastHLine(0,40, display.width(), WHITE);
    display.setTextSize(2);
    display.setCursor(28, 49);
    display.print(F("Max:"));
    display.print(zoom_max);
    display.display();
    if (x_value>600 && 450<y_value<850) {
      if (zoom_max == 360) {
        zoom_max = zoom_max;
      }
      else {
        zoom_max++;
        zoom_motor.rotate(-1);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (zoom_max == -zoom_max) {
        zoom_max = zoom_max;
      }
      else {
        zoom_max--;
        zoom_motor.rotate(1);
      }
      delay(100);
    }

    x_value = analogRead(A0);
    y_value = analogRead(A1);
    button = digitalRead(12);
  }

  delay(500);
  
  // set the zoom_min
  display.clearDisplay();
  zoom_min = zoom_max;

  button = 1; // 0 for click
  x_value = analogRead(A0);
  y_value = analogRead(A1);

  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Min range (zoom ring)\n"));
    display.println(F("Press joystick to \nconfirm."));
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
        zoom_motor.rotate(-1);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (zoom_min == 0) {
        zoom_min = zoom_min;
      }
      else {
        zoom_min--;
        zoom_motor.rotate(1);
      }
      delay(100);
    }

    x_value = analogRead(A0);
    y_value = analogRead(A1);
    button = digitalRead(12);
  }
  
  diff_focus = focus_max - focus_min;
  diff_zoom = zoom_max - zoom_min;
  focus_max = diff_focus;
  focus_min = 0;
  zoom_max = diff_zoom;
  zoom_min = 0;

  // note that now both focus and zoom motor is at ideal 0 (min)
  // drawing of main menu
  // 0=zoom, 1=focus, 2=presets, 3=recalibration, 4=quit
  delay(500);
  x_value = analogRead(A0);
  y_value = analogRead(A1);
  button = 1;
  display.clearDisplay();
  display.setTextSize(1);

}

void(* resetFunc) (void) = 0;

void loop() {
  while (button != 0) {
    // down
    if (y_value>600 && 450<x_value<850) {
      if (option == 4) {
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
        option = 4;
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
      display.print(F("|---- Main Menu ----|\n"));
      display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,11); 
      display.setTextColor(BLACK,WHITE);
      display.print(F("Zoom Ring"));
      display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
      display.setTextColor(WHITE);
      display.setCursor(2,22);
      display.print(F("Focus Ring"));
      display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,33);
      display.print(F("Presets (WIP)"));
      display.drawRect(0,42,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,44);
      display.print(F("Recalibration [RST]"));
      display.drawRect(0,53,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,55);
      display.print(F("Quit()"));
      display.display();
      display.setTextColor(WHITE);
    }
    // focus
    else if (option == 1) {
      display.clearDisplay();
      display.setCursor(0,0);
      display.print(F("|---- Main Menu ----|\n"));
      display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,11); 
      display.print(F("Zoom Ring"));
      display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
      display.setTextColor(BLACK,WHITE);
      display.setCursor(2,22);
      display.print(F("Focus Ring"));
      display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,33);
      display.setTextColor(WHITE);
      display.print(F("Presets (WIP)"));
      display.drawRect(0,42,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,44);
      display.print(F("Recalibration [RST]"));
      display.drawRect(0,53,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,55);
      display.print(F("Quit()"));
      display.display();
      display.setTextColor(WHITE);
    }
    // presets
    else if (option == 2) {
      display.clearDisplay();
      display.setCursor(0,0);
      display.print(F("|---- Main Menu ----|\n"));
      display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,11); 
      display.print(F("Zoom Ring"));
      display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,22);
      display.print(F("Focus Ring"));
      display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,33);
      display.setTextColor(BLACK,WHITE);
      display.print(F("Presets (WIP)"));
      display.drawRect(0,42,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,44);
      display.setTextColor(WHITE);
      display.print(F("Recalibration [RST]"));
      display.drawRect(0,53,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,55);
      display.print(F("Quit()"));
      display.display();
      display.setTextColor(WHITE);
    }
    // recalibration
    else if (option == 3) {
      display.clearDisplay();
      display.setCursor(0,0);
      display.print(F("|---- Main Menu ----|\n"));
      display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,11); 
      display.print(F("Zoom Ring"));
      display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,22);
      display.print(F("Focus Ring"));
      display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,33);
      display.print(F("Presets (WIP)"));
      display.drawRect(0,42,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,44);
      display.setTextColor(BLACK,WHITE);
      display.print(F("Recalibration [RST]"));
      display.drawRect(0,53,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,55);
      display.setTextColor(WHITE);
      display.print(F("Quit()"));
      display.display();
      display.setTextColor(WHITE);
    }
    // quit
    else if (option == 4) {
      display.clearDisplay();
      display.setCursor(0,0);
      display.print(F("|---- Main Menu ----|\n"));
      display.drawRect(0,9,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,11); 
      display.print(F("Zoom Ring"));
      display.drawRect(0,20,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,22);
      display.print(F("Focus Ring"));
      display.drawRect(0,31,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,33);
      display.print(F("Presets (WIP)"));
      display.drawRect(0,42,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,44);
      display.print(F("Recalibration [RST]"));
      display.drawRect(0,53,SCREEN_WIDTH,12,WHITE);
      display.setCursor(2,55);
      display.setTextColor(BLACK,WHITE);
      display.print(F("Quit()"));
      display.display();
      display.setTextColor(WHITE);
    }

    x_value = analogRead(A0);
    y_value = analogRead(A1);
    button = digitalRead(12);
  }

  int sub_option = 0;
  x_value = analogRead(A0);
  y_value = analogRead(A1);
  button = 1;
  
  if (option == 4) {
    display.clearDisplay();
    display.display();
    return;
  }

  // zoom ring
  if (option == 0) {
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
            zoom_motor.rotate(-1);
          }
          delay(200);
        }
        if (x_value<440 && 450<y_value<850) {
          if (zoom_min == 0) {
            zoom_min = 0;
          }
          else {
            zoom_min--;
            zoom_motor.rotate(1);
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
      x_value = analogRead(A0);
      y_value = analogRead(A1);
      button = digitalRead(12);

      if (sub_option == 1 && button == 0) {
        button = 1;
        delay(500);
        break;
      }
    }
    
  }
  // focus ring
  if (option == 1) {
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
            focus_motor.rotate(-1);
          }
          delay(200);
        }
        if (x_value<440 && 450<y_value<850) {
          if (focus_min == 0) {
            focus_min = 0;
          }
          else {
            focus_min--;
            focus_motor.rotate(1);
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
      x_value = analogRead(A0);
      y_value = analogRead(A1);
      button = digitalRead(12);

      if (sub_option == 1 && button == 0) {
        button = 1;
        delay(500);
        break;
      }
    }
    
  }

  if (option == 3) {
    resetFunc(); //call reset
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
