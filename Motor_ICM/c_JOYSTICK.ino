/* ******** JOYSTICK Functions **********
 *  
 * Functions involving the joystick hardware.
 * 
 * Functions available:
 * getUpDown() - Increases or decreases values based on up and down motion.
 * getLeftRight() - Increase or decreases values based on left and right motion.
 * getUpdate() - Updates the screen if the set button is pressed
 * 
 */


/*
 * returns the option either ++ or --
 * checks for up and down joystick movement
 * 
 * @param option          The maximum range of values.
 * @param current_option  The current value to be adjusted.
 * @param delay_ms        Delays so that reading the movement can be slowed down.
 * @return int            Returns `current_option` as it is.
 */
int getUpDown(int option, int current_option, int delay_ms=0) {
  if (!digitalRead(SET)) return current_option;
  if (!digitalRead(DOWN)) {
    delay(delay_ms);
    updateMenu = true;
    return (current_option == option-1) ? 0 : ++current_option;
  }

  if (!digitalRead(UP)) {
    delay(delay_ms);
    updateMenu = true;
    return (current_option == 0) ? option-1 : --current_option;
  }

  return current_option;
}

/*
 * returns the option either ++ or --
 * checks for left and right joystick
 * able to set to a lower limit if required
 * 
 * @param range       The maximum range of values.
 * @param current     The current value to be adjusted. 
 * @param low_limit   Lower limit of ranges. (If not 0)
 * @param delay_ms    Delays so that reading the movement can be slowed down.
 * @return int        Returns `current_option` as it is.
 */
int getLeftRight(int range, int current, int low_limit=0, int delay_ms=0) {
  if (!digitalRead(SET)) return current;
  if (!digitalRead(RIGHT)) {
    delay(delay_ms);
    updateMenu = true;
    return (current == range) ? current : ++current; 
  }

  if (!digitalRead(LEFT)) {
    delay(delay_ms);
    updateMenu = true;
    return (current == low_limit) ? low_limit : --current;
  }
  
  return current;
}

/*
 * Checks if SET is pressed
 * Clears the screen,
 * resets option
 * 
 * @param s       Current screen that it is on.
 * @param offset  Potential offset to be placed. (e.g. screen 1 will be screen 2 now if `offset` = 1)
 * @param reset   If the current option should be resetted or not.
 * @return int    Returns the screen.
 */
int getUpdate(int s, int offset=0, bool reset=true) {
  if (!digitalRead(SET)) {
    s = option+offset;
    if (reset) {
      option = 0;
    }
    tft.background(0,0,0);
    updateMenu = true;
  }
  return s;
}
