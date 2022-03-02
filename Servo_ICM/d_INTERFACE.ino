/* ******** INTERFACE Functions **********
 *  
 * Functions here integrates all the 3 modules before this
 * This allows creation of certain patterns or outcomes
 * much easier without a messy code in the main file
 * 
 * Functions available:
 * calibrate() - Calibration of the motors, to set the upper and lower limit of the motor.
 * choostDist() - Getting a specific distance of the motor to be used for other manipulation.
 * goDist() - Move the motor to a distance, includes the display.
 * goMultiDist() - Move both motors to their specific distances, includes the display.
 * createCustom() - Get a custom string of sequences by the motor, set by the user.
 * 
 */


/**
 * Calibration of motors,
 * involves the display, joystick and motorcontrol module.
 * 
 * @param type          Motor currently selected. (Can be null)
 * @param string_table  Character array of strings to be displayed.
 * @param upper_limit   Upper limit of the calibration range.
 * @param lower_limit   Lower limit of the calibration range.
 * @param color         uint16_t colors can be used for display.
 * @return int          Current position of the motor.
 */
int calibrate(int type, const char *const string_table[], int upper_limit, int lower_limit, uint16_t color=WHITE) {
  int pos_current;
  pos_current = type ? zoom_current : focus_current;
  caliMenu(type, string_table, pos_current, upper_limit, color);
  do {
    caliMenu(type, string_table, pos_current, upper_limit, color, true);
    pos_current = getLeftRight(upper_limit, pos_current, lower_limit, 0);
    moveMotor(type, pos_current);
  } while(digitalRead(SET));
  
  return pos_current;
}

/**
 * Similar to the calibration interface,
 * function allows user to move the motors to ...
 * ... a specific distance and returns the value.
 * 
 * @param type          Motor currently selected. (Can be null)
 * @param count         Max size of the `string_table`.
 * @param string_table  Character array of strings to be displayed.
 * @param goBack        Set to `true`  if motor has to return to previous position.
 * @param color         uint16_t colors can be used for display.
 * @return int          Current position of the motor.
 */
int chooseDist(int type, int count, const char *const string_table[], bool goBack=false, uint16_t color=WHITE) {
  int pos_current, upper_limit, lower_limit;
  pos_current = type ? zoom_current : focus_current;
  upper_limit = type ? zoom_range : focus_range;
  //lower_limit = type ? zoom_min : focus_min;
  moveMotorMenu(count, string_table, pos_current, upper_limit, color);
  do {
    moveMotorMenu(count, string_table, pos_current, upper_limit, color, true);
    pos_current = getLeftRight(upper_limit, pos_current, 0, 0);
    moveMotor(type, pos_current);
  } while(digitalRead(SET));

  if (goBack) {
    // needs to go back to original spot
    delay(500);
    tft.setCursor(0, 59);
    tft.setTextColor(WHITE,BLACK);
    tft.println(F("Returning to         "));
    tft.setTextColor(RED,BLACK);
    tft.print(F("PREVIOUS "));
    tft.setTextColor(WHITE,BLACK);
    tft.println(F("location          "));
    for (int i=2; i<count; i++) {
      tft.println("                  ");
    }
    moveMotor(type, type ? zoom_current : focus_current);
  } else {
    type ? zoom_current = pos_current : focus_current = pos_current;
  }

  EEPROM.write(3, zoom_current);
  EEPROM.write(2, focus_current);
  
  return pos_current;
}

/**
 * Moves the motor to a specific distance,
 * also displays the relevant information of ... 
 * ... what the sequence is to the user.
 * 
 * @param type          Motor currently selected. (Can be null)
 * @param title         Title of the hotbar/screen.
 * @param pos_desired   Number of steps for the motor to move.
 * @param color         uint16_t colors can be used for display.
 * @param goBack        Set to `true`  if motor has to return to previous position.
 * @param shutter_spd   Time allowed for the motor to move to the desired position.
 * @param lastSequence  If this is the last sequence, it will play the buzzer and click the camera shutter.
 */
void goDist(int type, char title[], int pos_desired, uint16_t color=WHITE, bool goBack=true, float shutter_spd=motor_time, bool lastSequence=true) {
  int pos_current;
  pos_current = type ? zoom_current : focus_current;
  
  printMoveSteps(type, title, color, false); 
  moveMotor(type, pos_desired, shutter_spd);
  if (lastSequence) {
    nikonTime();
    buzz();
  }

  // returns to original spot
  if (goBack) {
    updateScreen(4000);
    printMoveSteps(type, title, color, true);
    moveMotor(type, pos_current);
  } 
  updateScreen();
}

/**
 * Similar to `goDist` function, this does the same.
 * But, it involves moving both motors (rear & front) ...
 * ... at the same time.
 * 
 * @param title           Title of the hotbar/screen.
 * @param zoom_desired    Specified position for zoom motor to move.
 * @param focus_desired   Specified position for focus motor to move.
 * @param color           uint16_t colors can be used for display.
 * @param goBack          Set to `true`  if motor has to return to previous position.
 * @param shutter_spd     Time allowed for the motor to move to the desired position.
 * @param lastSequence    If this is the last sequence, it will play the buzzer and click the camera shutter.
 */
void goMultiDist(char title[], int zoom_desired, int focus_desired, uint16_t color=WHITE, bool goBack=true, float shutter_spd=motor_time, bool lastSequence=true) {
  int prev_zoom, prev_focus;
  prev_zoom = zoom_current;
  prev_focus = focus_current;
  
  printMoveSteps(3, title, color, false);
  moveMultiMotor(zoom_desired, focus_desired, shutter_spd);
  if (lastSequence) {
    nikonTime();
    buzz();
  }

  // returns to original spot
  if (goBack) {
    updateScreen(4000);
    printMoveSteps(3, title, color, true);
    moveMultiMotor(prev_zoom, prev_focus);
    //moveMultiMotor(prev_zoom, prev_focus);
  }
  updateScreen();
}

/**
 * Stores a string to the ...
 * ... arduino EEPROM memory.
 * 
 * @param addrOffset  Starting address of the memory address to write to.
 * @param strToWrite  The string to be written to memory.
 * @return int        Ending address of the stored string.
 */
int writeStringToMemory(int addrOffset, const char strToWrite[]) {
  int len = strlen(strToWrite);
  EEPROM.write(addrOffset, len);

  for (int i=0; i<len; i++) {
    EEPROM.write(addrOffset+1+i, strToWrite[i]);
  }

  return addrOffset+1+len;
}

/**
 * Returns a string from the ...
 * ... arduino EEPROM memory
 * 
 * @param addrOffset
 * @param strToRead
 * @return int
 */
int readStringFromMemory(int addrOffset, char* strToRead) {
  int newStrLen = EEPROM.read(addrOffset);

  for (int i=0; i<newStrLen; i++) {
    strToRead[i] = EEPROM.read(addrOffset+1+i);
  }
  strToRead[newStrLen] = '\0';
  return addrOffset+1+newStrLen;
}

/**
 * Create a custom sequence formed to a character array.
 * All the necessary information is stored in the array.
 * @notice Currently, multi motor is not available or to be made
 *         available unless the overall efficiency of the program
 *         can be improved.
 *         
 * @param buf   Character array to store the sequence.
 * @return int  Total number of sequences. 
 *              Maximum of 8 sequences per 1 pattern.
 */
int createCustom(char* buf) {
  int itemcount=0;
  buf[0] = '\0';
  const char *zoom = "Z";
  const char *focus = "F";
  const char *goBackT = "T";
  const char *goBackF = "F";
  const char *lastSequenceT = "T";
  const char *lastSequenceF = "F";
  do {
    int position_acquired;
    do { // Choosing Zoom or Focus
      max_option = menu(3, lens_selection_menu, option);
      option = getUpDown(max_option, option, 0);
    } while (digitalRead(SET));
    delay(500);
    if (option == 2) {
      return itemcount;
    }
    strcat(buf, option ? focus : zoom);
    updateScreen();
    if (option) {
      position_acquired = chooseDist(FOCUS, 3, focus_dist, false, YELLOWGREEN); 
    } else {
      position_acquired = chooseDist(ZOOM, 3, zoom_dist, false, YELLOWGREEN);
    }

    bool goBack=false;
    int selection=-1;
    option = 0;
    updateScreen();
    do { // choosing goBack, add another, back
      if (selection == 0) {
        goBack = goBack ? false : true;
        selection = resetScreen(selection);
      } else if (selection == 2) { // back
        strcat(buf, goBack ? goBackT : goBackF);
        strcat(buf, lastSequenceT);
        char cstr[5];
        itoa(position_acquired, cstr, 10);
        strcat(buf, cstr);
        return itemcount++;
      } else {
        goBack ? menu(3, new_selection_truemenu, option) : menu(3, new_selection_falsemenu, option);
        selection = getUpdate(selection);
      }
      option = getUpDown(max_option, option, 0);
    } while(selection != 1);
    strcat(buf, goBack ? goBackT : goBackF);
    strcat(buf, lastSequenceF);
    char cstr[5];
    itoa(position_acquired, cstr, 10);
    strcat(buf, cstr);
    itemcount++;
    if (itemcount == 8) {
      break;
    }
    strcat(buf, ",");
  } while(true);
  
  return itemcount;
} 
