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

void goDist(int type, char title[], int pos_desired, uint16_t color=WHITE, float shutter_spd = motor_time, bool goBack=true) {
  int pos_current, upper_limit;
  pos_current = type ? zoom_current : focus_current;
  upper_limit = type ? zoom_range : focus_range;

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

void goMultiDist(char title[], int zoom_desired, int focus_desired, uint16_t color=WHITE, float shutter_spd = motor_time, bool goBack=true) {
  printMoveSteps(NULL, title, color, false);
  nikonTime();
  moveMultiMotor(zoom_desired, focus_desired, shutter_spd);
  nikonTime();
  updateScreen(2000);

  // returns to original spot
  if (goBack) {
    printMoveSteps(NULL, title, color, true);
    moveMultiMotor(zoom_current, focus_current, shutter_spd);
  }
  updateScreen();
}
