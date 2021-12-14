// ******** JOYSTICK Functions **********
int getUpDown(int option, int current_option, int delay_ms=200) {
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

int getLeftRight(int range, int current, int low_limit=0, int delay_ms=200) {
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

int getUpdate(int s) {
  if (!digitalRead(SET)) {
    s = option;
    option = 0;
    tft.background(0,0,0);
    updateMenu = true;
  }
  return s;
}
