// ******** INTERFACE Functions **********
/*
 * Functions here integrates all the 3 modules before this
 * This allows creation of certain patterns or outcomes
 * much easier without a messy code in the main file
 */

int calibrate(int type, const char *const string_table[], int upper_limit, int lower_limit, uint16_t color=WHITE) {
  int pos_current;
  pos_current = type ? zoom_current : focus_current;
  caliMenu(string_table, pos_current, upper_limit, color);
  do {
    caliMenu(string_table, pos_current, upper_limit, color, true);
    pos_current = getLeftRight(upper_limit, pos_current, lower_limit, 0);
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
    tft.println(F("Returning to "));
    tft.setTextColor(RED);
    tft.print(F("PREVIOUS "));
    tft.setTextColor(WHITE);
    tft.println(F("location"));
    for (int i=2; i<count; i++) {
      tft.println("                  ");
    }
    moveMotor(type, type ? zoom_current : focus_current);
  }
  
  return pos_current;
}

void goDist(int type, char title[], int pos_desired, uint16_t color=WHITE) {
  int pos_current, upper_limit;
  pos_current = type ? zoom_current : focus_current;
  upper_limit = type ? zoom_range : focus_range;

  printMoveSteps(type, title, false, color); 
  moveMotor(type, pos_desired, shutter_speed);
  delay(4000);
  printMoveSteps(type, title, true, color);
  moveMotor(type, pos_current);
}
