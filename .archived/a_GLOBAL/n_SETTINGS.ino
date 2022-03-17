// ****** SETTINGS Function ********

void set_Orientation() {
  while (button != 0) {
    option = getUpDown(3, option, 200);
    menu(3, positioning_menu, option);
    getJoyRead();             
  }

  int position_option = option;
  option = 0;
  resetJoy();
  delay(500);

  if (position_option == 0) {
    orientation = 0;
    EEPROM.write(4,orientation);
  }

  if (position_option == 1) {
    orientation = 1;
    EEPROM.write(4,orientation);
  }
  
  goto SETTINGS;
}

void set_ShutterSpeed() {
  if (shutter_speed == 1) {
    option = 0;
  } else if (shutter_speed == 4) {
    option = 1;
  } else if (shutter_speed == 8) {
    option = 2;
  } else if (shutter_speed == 16) {
    option = 3;
  }
  while (button != 0) {
    option = getUpDown(5, option, 200);
    menu(5, shutter_menu, option);
    getJoyRead();
  }

  int shutter_option = option;
  option = 0;
  resetJoy();
  delay(500);

  if (shutter_option == 0) {
    shutter_speed = 1;        
  }
  if (shutter_option == 1) {
    shutter_speed = 4;
  }      
  if (shutter_option == 2) {
    shutter_speed = 8;
  }
  if (shutter_option == 3) {
    shutter_speed = 16;
  }

  EEPROM.write(5, shutter_speed);

  goto SETTINGS;
}