// ******** INTERFACE Functions **********
/*
 * Functions here integrates all the 3 modules before this
 * This allows creation of certain patterns or outcomes
 * much easier without a messy code in the main file
 */

int calibrate(int type, const char *const string_table[], int upper_limit, int lower_limit, uint16_t color=WHITE) {
  int pos_current;
  pos_current = type ? zoom_current : focus_current;
  do {
    caliMenu(string_table, pos_current, upper_limit, color);
    pos_current = getLeftRight(upper_limit, pos_current, lower_limit, 0);
    moveMotor(type, pos_desired = pos_current);
  } while(digitalRead(SET));
  
  return pos_current;
}
