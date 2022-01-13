// ******** INTERFACE Functions **********
/*
 * Functions here integrates all the 3 modules before this
 * This allows creation of certain patterns or outcomes
 * much easier without a messy code in the main file
 */

int calibrate(int type, const char *const string_table[], int upper_limit, int lower_limit, uint16_t color=WHITE) {
  int pos_current;
  pos_current = type ? zoom_current : focus_current;
  caliMenu(type, string_table, pos_current, upper_limit, color);
  do {
    caliMenu(type, string_table, pos_current, upper_limit, color, true);
    pos_current = getLeftRight(upper_limit, pos_current, lower_limit, 0);
    Serial.print("Pos current: ");
    Serial.println(pos_current);
    moveMotor(type, pos_current);
  } while(digitalRead(SET));
  
  return pos_current;
}

int chooseDist(int type, int count, const char *const string_table[], bool goBack=false, uint16_t color=WHITE) {
  int pos_current, upper_limit;
  pos_current = type ? zoom_current : focus_current;
  upper_limit = type ? zoom_range : focus_range;
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
  }
  
  return pos_current;
}

void goDist(int type, char title[], int pos_desired, uint16_t color=WHITE, bool goBack=true, float shutter_spd=motor_time) {
  int pos_current;
  pos_current = type ? zoom_current : focus_current;

  printMoveSteps(type, title, color, false); 
  nikonTime();
  moveMotor(type, pos_desired, shutter_spd);
  nikonTime();
  updateScreen(4000);

  // returns to original spot
  if (goBack) {
    printMoveSteps(type, title, color, true);
    moveMotor(type, pos_current);
  }
  updateScreen();
}

void goMultiDist(char title[], int zoom_desired, int focus_desired, uint16_t color=WHITE, bool goBack=true, float shutter_spd=motor_time) {
  int prev_zoom, prev_focus;
  prev_zoom = zoom_current;
  prev_focus = focus_current;
  printMoveSteps(3, title, color, false);
  nikonTime();
  moveMultiMotor(zoom_desired, focus_desired, shutter_spd);
  nikonTime();
  updateScreen(2000);

  // returns to original spot
  if (goBack) {
    printMoveSteps(3, title, color, true);
    moveMultiMotor(prev_zoom, prev_focus, 3);
    //moveMultiMotor(prev_zoom, prev_focus);
  }
  updateScreen();
}

int writeStringToMemory(int addrOffset, const char strToWrite[]) {
  int len = strlen(strToWrite);
  EEPROM.write(addrOffset, len);

  for (int i=0; i<len; i++) {
    EEPROM.write(addrOffset+1+i, strToWrite[i]);
  }

  return addrOffset+1+len;
}

int readStringFromMemory(int addrOffset, char* strToRead) {
  int newStrLen = EEPROM.read(addrOffset);

  for (int i=0; i<newStrLen; i++) {
    strToRead[i] = EEPROM.read(addrOffset+1+i);
  }
  strToRead[newStrLen] = '\0';
  return addrOffset+1+newStrLen;
}

int createCustom(char* buf) {
  int itemcount=0;
  buf[0] = '\0';
  const char *zoom = "Z";
  const char *focus = "F";
  const char *goBackT = "G";
  const char *goBackF = "N";
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
    Serial.println(buf);
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
        char cstr[5];
        itoa(position_acquired, cstr, 10);
        strcat(buf, cstr);
        Serial.println(buf);
        return itemcount++;
      } else {
        goBack ? menu(3, new_selection_truemenu, option) : menu(3, new_selection_falsemenu, option);
        selection = getUpdate(selection);
      }
      option = getUpDown(max_option, option, 0);
    } while(selection != 1);
    strcat(buf, goBack ? goBackT : goBackF);
    char cstr[5];
    itoa(position_acquired, cstr, 10);
    strcat(buf, cstr);
    itemcount++;
    if (itemcount == 4) {
      break;
    }
    strcat(buf, ",");
    Serial.println(buf);
  } while(true);
  
  return itemcount;
} 
