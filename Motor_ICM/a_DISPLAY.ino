/* ******** DISPLAY Functions **********
 * 
 * Functions targeted primarily to the display
 * Libraries used:
 * - TFT.h
 * - Adafruit GFX Library
 * 
 * Functions available:
 * menu() - Creates a menu template
 * hotbar() - Creates a slider template
 * resetScreen() - Resets the current screen to the previous
 * updateScreen() - Updates the current screen
 * caliMenu() - Creates a menu used during calibration
 * moveMotorMenu() - Creates a menu used when choosing specific values
 * countdownMenu() - An easy 3, 2, 1, GO! display
 * printMoveSteps() - Displays information of the current status of the program
 * 
 */


/** 
 * ----- MENU ------
 * Creates a menu using a string array 
 * Display which option is being selected
 * 
 * [Top of screen]            {Configurable}
 * Shutter Speed: __
 * Front Motor: ____
 * Rear Motor: _____ 
 * 
 * [Middle of screen]
 * |----< Menu Name >----|
 * | Option 1
 * | Option 2
 * | ...
 * 
 * [Bottom of screen]
 * < > Move the lens          {Will be dependent on the menu}
 * ^v  Select the options
 *  o  Select current option
 *  
 * @param array_size        Maximum choices that the menu should have.
 * @param string_table[]    Character array of strings to be displayed.
 * @param option_selected   Current selected option.
 * @param header            Values from -2 to 4, to display varying headers. 
 * @param footer            Values from 0 to 2, to display varying footers.
 * @param color             uint16_t colors can be used for display.
 * @return int              The maximum choices of the menu.
 */

int menu(int array_size, const char *const string_table[], int option_selected, int header=0, int footer=2, uint16_t color=DEEPPINK) {
  int total_num = array_size; 
  if (!updateMenu) return total_num;
  updateMenu = false;
  tft.setCursor(0, 0);
  int rect_y = 9;
  
  if (header != 0) {
    if (header > 0) {
      tft.setTextColor(AQUA);
      tft.print(F("Shutter Time: "));
      tft.setTextColor(WHITE);
      tft.println(shutter_speed);
      tft.setTextColor(AQUA);
      tft.print(F("Motor Time: "));
      tft.setTextColor(WHITE);
      tft.println(motor_time);
    }
    switch (header) {
      case -1:
      case 1: {
        tft.setTextColor(AQUA);
        tft.print(F("Front Motor: "));
        tft.setTextColor(WHITE,BLACK);
        tft.println(orientation ? "Zoom " : "Focus");
        tft.setTextColor(AQUA);
        tft.print(F("Rear Motor: "));
        tft.setTextColor(WHITE,BLACK);
        tft.println(orientation ? "Focus" : "Zoom ");
        break;
      }
      case -2:
      case 2: {
        tft.setTextColor(AQUA);
        tft.print(F("Focus Range: "));
        tft.setTextColor(WHITE,BLACK);
        tft.println(focus_range);
        tft.setTextColor(AQUA);
        tft.print(F("Zoom Range: "));
        tft.setTextColor(WHITE,BLACK);
        tft.println(zoom_range);
        break;
      }
      case 3: { // focus
        tft.setTextColor(AQUA);
        tft.print(F("Focus Range: "));
        tft.setTextColor(WHITE,BLACK);
        tft.println(focus_range);
        tft.setTextColor(AQUA);
        tft.print(F("Focus Position: "));
        tft.setTextColor(WHITE,BLACK);
        tft.println(focus_current);
        break;
      }
      case 4: { //zoom
        tft.setTextColor(AQUA);
        tft.print(F("Zoom Range: "));
        tft.setTextColor(WHITE,BLACK);
        tft.println(zoom_range);
        tft.setTextColor(AQUA);
        tft.print(F("Zoom Position: "));
        tft.setTextColor(WHITE,BLACK);
        tft.println(zoom_current);
        break;
      }
      default:{ 
        break;
      }
    }
    tft.println(); 
    rect_y =52;
  }

  tft.setTextColor(color);
  strcpy_P(buffer, (char *)pgm_read_word(&(string_table[0])));
  tft.println(buffer);
  tft.setTextColor(WHITE,BLACK);
  
  // displaying the sub menus
  for (int i=0; i<total_num; i++) {
    tft.drawRect(0,rect_y,tft.width(),12,WHITE);
    tft.setCursor(2,rect_y+2);
    if (i == option_selected) {
      tft.setTextColor(BLACK,WHITE);
    } else {
      tft.setTextColor(WHITE,BLACK);
    }
    strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i+1])));
    tft.print(buffer);

    rect_y = rect_y+11;
  }
  tft.setTextColor(WHITE,BLACK);

  switch(footer) {
    case 1:
      tft.drawChar(2,130,LEFT_ARROW,WHITE,BLACK,2);
      tft.drawChar(17,130,RIGHT_ARROW,WHITE,BLACK,2);
      tft.setCursor(32,134);
      tft.println(F(": Adjust values"));
      tft.drawChar(2,145,SELECT,WHITE,BLACK,2);
      tft.setCursor(17,149);
      tft.println(F(": Press to set"));
      break;
    case 2:
      tft.drawChar(2,130,UP_ARROW,WHITE,BLACK,2);
      tft.drawChar(17,130,DOWN_ARROW,WHITE,BLACK,2);
      tft.setCursor(32,134);
      tft.println(F(": Navigate"));
      tft.drawChar(2,145,SELECT,WHITE,BLACK,2);
      tft.setCursor(17,149);
      tft.println(F(": Press to set"));
      break;
    default:
    break;
  }

  return total_num;
}

/** 
 * --- Hotbar Screen ---
 * Creates a template for a bar type screen
 * 
 * @param title           Title of the hotbar/screen.
 * @param current         Current value.
 * @param max_range       Maximum range allowed. (Always start from 0)
 * @param current_option  Current option selected. (Only used if `haveBack` is true)
 * @param haveBack        Insert a back button if needed.
 * @param header          Values from -2 to 4, to display varying headers. 
 * @param footer          Values from 0 to 2, to display varying footers.
 * @param color           uint16_t colors can be used for display.
 * @param updateBar       Only updates certain areas of the hotbar. (Save Time)
 */
void hotbar(char title[], int current, int max_range, int current_option=0, bool haveBack=false, int header=0, int footer=3, uint16_t color=WHITE, bool updateBar=false) {
  if (!updateMenu) return;
  updateMenu = false;
  
  int divs = (tft.width()-30)/(float)max_range * abs(current);
  tft.setCursor(0, 0);
  int rect_y = 37;

  if (header != 0) {
    if (!updateBar) {
      tft.setTextColor(AQUA);
      tft.print(F("Shutter Speed: "));
      tft.setTextColor(WHITE);
      tft.println(shutter_speed);
      switch (header) {
        case 1: {
          tft.setTextColor(AQUA);
          tft.print(F("Front Motor: "));
          tft.setTextColor(WHITE);
          tft.println(orientation ? "Zoom" : "Focus");
          tft.setTextColor(AQUA);
          tft.print(F("Rear Motor: "));
          tft.setTextColor(WHITE);
          tft.println(orientation ? "Focus" : "Zoom");
        }
        case 2: {
          tft.setTextColor(AQUA);
          tft.print(F("Focus Range: "));
          tft.setTextColor(WHITE);
          tft.println(focus_range);
          tft.setTextColor(AQUA);
          tft.print(F("Zoom Range: "));
          tft.setTextColor(WHITE);
          tft.println(zoom_range);
        }
      }
      tft.println(); 
    }
    rect_y =75;
  }

  option ? tft.setTextColor(WHITE,BLACK) : tft.setTextColor(BLACK,WHITE);
  if (title != NULL) {
    char myChar;
    for (byte k=0; k<strlen_P(title); k++) {
      myChar = pgm_read_byte_near(title+k);
      tft.print(myChar);
    }
  }
  tft.println();
  tft.setTextColor(WHITE);
  tft.println();
  tft.print(F("Range: "));
  tft.println(max_range);
  tft.setCursor(2, rect_y + 22);
  option ? tft.setTextColor(BLACK,WHITE) : tft.setTextColor(WHITE,BLACK);
  if (haveBack) tft.println(F("[Go Back]"));

  /* Draw Hotbar */
  tft.drawRect(0,rect_y,tft.width()-26,14,WHITE);
  tft.fillRect(2,rect_y+2,tft.width()-30,10,BLACK); // reset inner rectangle
  tft.fillRect(2,rect_y+2,divs,10,color);
  tft.setCursor(105,rect_y+3);
  tft.setTextColor(WHITE,BLACK);
  tft.print(current);
  tft.print(" ");

  if (!updateBar) {
    switch(footer) {
      case 1:
        tft.drawChar(2,130,LEFT_ARROW,WHITE,BLACK,2);
        tft.drawChar(17,130,RIGHT_ARROW,WHITE,BLACK,2);
        tft.setCursor(32,134);
        tft.println(F(": Adjust values"));
        tft.drawChar(2,145,SELECT,WHITE,BLACK,2);
        tft.setCursor(17,149);
        tft.println(F(": Press to set"));
        break;
      case 2:
        tft.drawChar(2,130,UP_ARROW,WHITE,BLACK,2);
        tft.drawChar(17,130,DOWN_ARROW,WHITE,BLACK,2);
        tft.setCursor(32,134);
        tft.println(F(": Navigate"));
        tft.drawChar(2,145,SELECT,WHITE,BLACK,2);
        tft.setCursor(17,149);
        tft.println(F(": Press to set"));
        break;
      case 3:
        tft.drawChar(2,115,LEFT_ARROW,WHITE,BLACK,2);
        tft.drawChar(17,115,RIGHT_ARROW,WHITE,BLACK,2);
        tft.setCursor(32,119);
        tft.println(F(": Adjust values"));
        tft.drawChar(2,130,UP_ARROW,WHITE,BLACK,2);
        tft.drawChar(17,130,DOWN_ARROW,WHITE,BLACK,2);
        tft.setCursor(32,134);
        tft.println(F(": Navigate"));
        tft.drawChar(2,145,SELECT,WHITE,BLACK,2);
        tft.setCursor(17,149);
        tft.println(F(": Press to set"));
        break;
      case 4: { // for zoom calibration
        tft.drawChar(2,115,RIGHT_ARROW,WHITE,BLACK,2);
        tft.setCursor(17,119);
        tft.println(": to tele lens");
        tft.drawChar(2,130,LEFT_ARROW,WHITE,BLACK,2);
        tft.setCursor(17,134);
        tft.println(": to widest lens");
        tft.drawChar(2,145,SELECT,WHITE,BLACK,2);
        tft.setCursor(17,149);
        tft.println(F(": Press to set"));
        break;
      }
      case 5: { // for focus calibration
        tft.drawChar(2,115,RIGHT_ARROW,WHITE,BLACK,2);
        tft.setCursor(17,119);
        tft.println(": to infinity");
        tft.drawChar(2,130,LEFT_ARROW,WHITE,BLACK,2);
        tft.setCursor(17,134);
        tft.println(": to min focal len");
        tft.drawChar(2,145,SELECT,WHITE,BLACK,2);
        tft.setCursor(17,149);
        tft.println(F(": Press to set"));
        break;
      }
      default:
      break;
    }
  }
  
  return;
}

/** 
 * --- Reset Screen ---
 * Sets option to 0,
 * updateMenu to true,
 * returns to previous screen.
 * 
 * @param s     Current screen of the display.
 * @return int  returns -1 to the previous screen.
 */
int resetScreen(int s) {
  tft.background(0,0,0);
  s = -1;
  updateMenu = true;
  //option = 0;
  return s;
}

/** 
 * --- Updates Screen ---
 * Updates the screen by:
 * - Reset background to BLACK
 * - updateMenu to true
 * - variable delay (important when includes joystick readings)
 * 
 * @param delay_ms  Configurable delay to be set. (Defaults to 0ms)
 */
void updateScreen(float delay_ms=0) {
  delay(delay_ms);
  tft.background(0,0,0);
  updateMenu = true;
}

/** 
 * --- Calibrate Screen ---
 * Calibration screen that is used during motor calibration.
 * Calls `hotbar` function.
 * 
 * @param type          Motor currently selected.
 * @param string_table  Character array of strings to be displayed.
 * @param current_step  Current step/position of the motor.
 * @param max_steps     Maximum steps allowed for the motor to take. (Defaults to 200 steps)
 * @param color         uint16_t colors can be used for display.
 * @param updateBar     Only updates certain areas of the hotbar. (Save Time)
 */
void caliMenu(int type, const char *const string_table[], int current_step, int max_steps=200, uint16_t color=WHITE, bool updateBar=false) {
  if (!updateMenu) return;
  hotbar(NULL, current_step, max_steps, 0, false, 0, type?4:5, color, updateBar);
  int i = 0;
  tft.setCursor(0,0);
  tft.setTextColor(color);
  strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i++])));
  tft.println(buffer);
  tft.setCursor(0, 59);
  tft.setTextColor(WHITE);
  strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i++])));
  tft.print(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i++])));
  tft.setTextColor(DARKGREEN);
  tft.println(buffer);
  tft.setTextColor(WHITE);
  
  return;
}

/** 
 * --- Move Motor Screen ---
 * Similarly to cali Menu ...
 * ... move motor menu is only used when getting a specific distance required.
 * Calls `hotbar` function.
 * 
 * @param count         Maximum strings to be printed.
 * @param string_table  Character array of strings to be displayed.
 * @param current_step  Current step/position of the motor.
 * @param max_steps     Maximum steps allowed for the motor to take.
 * @param color         uint16_t colors can be used for display.
 * @param updateBar     Only updates certain areas of the hotbar. (Save Time)
 */
void moveMotorMenu(int count, const char *const string_table[], int current_step, int max_steps, uint16_t color=WHITE, bool updateBar=false) {
  if (!updateMenu) return;
  int i=0;
  hotbar(NULL, current_step, max_steps, 0, false, 0, 1, color, updateBar);
  tft.setCursor(0,0);
  strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i])));
  tft.setTextColor(color);
  tft.println(buffer);
  tft.setTextColor(WHITE);
  tft.setCursor(0, 59);
  for (i=1; i<count; i++) {
    strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i])));
    tft.println(buffer);
  }
  tft.setTextColor(WHITE);
}

/**
 * ---- Countdown Screen ----
 * A simple way to print a ...
 * ... countdown menu
 */
void countdownMenu() {
  if (shutter_speed == 0) {
    updateScreen();
    buzz();
    return;
  }
  int i=0;
  tft.setCursor(0,0);
  tft.println("Get Ready!");
  delay(2000);
  for (i=3; i>0; i--) {
    tft.setTextSize(4);
    tft.setCursor(70,0);
    tft.setTextColor(RED,BLACK);
    tft.println(i);
    delay(1000);
  }
  tft.setTextSize(2);
  tft.println("SNAP!");
  tft.setTextSize(1);

  delay(100);
  nikonTime();
  buzz();
  tft.setTextColor(WHITE);
  tft.println("\n\n\nExposing camera to\nenvironment for...");
  tft.setTextSize(3);
  tft.setTextColor(AQUA);
  for (int i=(shutter_speed-motor_time); i>0; i--) {
    tft.setTextColor(AQUA,BLACK);
    tft.setCursor(20,120);
    tft.print(i);
    tft.print("s  ");
    delay(1000);
  }
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  updateScreen();
}

/*
 * ---- Moving Motor Screen ----
 * Displays when motor is currently moving.
 * `shutter_speed` will be displayed.
 * `motor_time` will be displayed.
 * Other headers will be displayed. (if needed)
 * Displays the current state of the sequence.
 * 
 * @param type    Motor currently selected. (Can be null)
 * @param title   Title of the screen.
 * @param color   uint16_t colors can be used for display.
 * @param goBack  Only updates certain areas of the hotbar. (Save Time)
 * 
 * @warning Currently a work in progress.
 */
void printMoveSteps(int type, char title[], uint16_t color, int goBack) {
  updateScreen();
  tft.setCursor(0,0);
  tft.setTextColor(AQUA);
  tft.print(F("Shutter Speed: "));
  tft.setTextColor(WHITE);
  tft.println(shutter_speed);
  tft.setTextColor(AQUA);
  tft.print(F("Motor Time: "));
  tft.setTextColor(WHITE);
  tft.println(motor_time);
  if (type == NULL) {
    // nothing
  } else if (type == 3) {
    tft.setTextColor(AQUA, BLACK);
    tft.print(F("Focus Range: "));
    tft.setTextColor(WHITE, BLACK);
    tft.println(focus_range);
    tft.setTextColor(AQUA, BLACK);
    tft.print(F("Zoom Range: "));
    tft.setTextColor(WHITE, BLACK);
    tft.println(zoom_range);
  } else {
    tft.setTextColor(AQUA, BLACK);
    tft.print(type ? "Zoom" : "Focus");
    tft.print(F(" Range: "));
    tft.setTextColor(WHITE, BLACK);
    tft.println(type ? zoom_range : focus_range);
  }
  tft.println();

  char myChar;
  tft.setTextColor(color);
  for (byte k=0; k<strlen_P(title); k++) {
    myChar = pgm_read_byte_near(title+k);
    tft.print(myChar);
  }
  
  tft.line(0, 75, tft.width(), 75);
  tft.setCursor(0, 85);
  tft.setTextColor(WHITE);
  switch(goBack) {
    case 0: {
      tft.println(F("Moving to "));
      tft.setTextColor(LIGHTSKYBLUE, BLACK);
      tft.print(F("DESIRED "));
      tft.setTextColor(WHITE, BLACK);
      tft.println(F("location"));
      break;
    }
    case 1: {
      tft.println(F("Returning to "));
      tft.setTextColor(RED, BLACK);
      tft.print(F("PREVIOUS "));
      tft.setTextColor(WHITE, BLACK);
      tft.println(F("POV location"));
      break;
    }
    case 2: {
      tft.println(F("Moving to "));
      tft.setTextColor(LIME, BLACK);
      tft.print(F("STARTING "));
      tft.setTextColor(WHITE, BLACK);
      tft.println(F("position"));
      break;
    }
    default: break;
  }
}
