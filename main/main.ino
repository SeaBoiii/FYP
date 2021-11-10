void focus_move(int orientation, int steps) {
  // focus in front
  if (orientation == 0) {
    front_motor.move(steps);
  } 
  // focus at back
  if (orientation == 1) {
    rear_motor.move(-steps);
  }
}

void focus_startMove(int orientation, long steps, long time=0) {
  // focus in front
  if (orientation == 0) {
    front_motor.startMove(steps, time);
  }
  // focus at back
  if (orientation == 1) {
    rear_motor.startMove(-steps, time);
  }  
}

unsigned focus_nextAction(int orientation) {
  unsigned wait_time;
  // focus in front
  if (orientation == 0) {
    wait_time = front_motor.nextAction();
  }
  // focus at back
  if (orientation == 1) {
    wait_time = rear_motor.nextAction();
  }    
  return wait_time;
}

void focus_setRPM(int orientation, int rpm) {
  if (orientation == 0) {
    front_motor.setRPM(rpm);
  }
  if (orientation == 1) {
    rear_motor.setRPM(rpm);
  }
}

void focus_setSpeedProfile(int orientation, bool linear, short accel=1000, short decel=1000) {
  if (orientation == 0) {
    if (linear) {
      front_motor.setSpeedProfile(front_motor.LINEAR_SPEED, accel, decel);
    } else {
      front_motor.setSpeedProfile(front_motor.CONSTANT_SPEED);
    }
  }
  if (orientation == 1) {
    if (linear) {
      rear_motor.setSpeedProfile(rear_motor.LINEAR_SPEED, accel, decel);      
    } else {
      rear_motor.setSpeedProfile(rear_motor.CONSTANT_SPEED);
    }
  }
}

void zoom_move(int orientation, int steps) {
  // zoom at back
  if (orientation == 0) {
    rear_motor.move(steps);    
  }
  if (orientation == 1) {
    front_motor.move(-steps);
  }
}

void zoom_startMove(int orientation, long steps, long time=0) {
  if (orientation == 0) {
    rear_motor.startMove(steps, time);    
  }
  if (orientation == 1) {
    front_motor.startMove(-steps, time);
  }
}

unsigned zoom_nextAction(int orientation) {
  unsigned wait_time;
  if (orientation == 0) {
    wait_time = rear_motor.nextAction();
  }
  if (orientation == 1) {
    wait_time = front_motor.nextAction();
  }
  return wait_time;
}

void zoom_setRPM(int orientation, int rpm) {
  if (orientation == 0) {
    rear_motor.setRPM(rpm);
  }
  if (orientation == 1) {
    front_motor.setRPM(rpm);    
  }
}

void zoom_setSpeedProfile(int orientation, bool linear, short accel=1000, short decel=1000) {
  if (orientation == 0) {
    if (linear) {
      rear_motor.setSpeedProfile(rear_motor.LINEAR_SPEED, accel, decel);
    } else {
      rear_motor.setSpeedProfile(rear_motor.CONSTANT_SPEED);
    }
  }
  if (orientation == 1) {
    if (linear) {
      front_motor.setSpeedProfile(front_motor.LINEAR_SPEED, accel, decel);      
    } else {
      front_motor.setSpeedProfile(front_motor.CONSTANT_SPEED);
    }
  }
}

void setFocusRange() {
  focus_setRPM(orientation, 1);
  
  // set the focus_max
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,5);
  strcpy_P(buffer, (char *)pgm_read_word(&(ring_phrases[0])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[0])));
  display.println(buffer);
  display.display();
  delay(4000);

  display.clearDisplay();
  display.setCursor(0,5);
  display.setTextSize(1);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[1])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
  display.println(buffer);
  display.display();
  delay(4000);
  
  while (button != 0) {
    display.clearDisplay();
    display.setCursor(0,5);
    display.setTextSize(1);
    display.println(F("Set to most focus\n"));
    strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
    display.println(buffer);
    display.drawFastHLine(0,40, display.width(), WHITE);
    display.setTextSize(2);
    display.setCursor(28, 49);
    display.print(F("Max:"));
    display.print(focus_max);
    display.display();
    if (x_value>600 && 450<y_value<850) {
      if (focus_max == 250) {
        focus_max = focus_max;
      }
      else {
        focus_max++;
        focus_move(orientation, 1);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (focus_max == 0) {
        focus_max = focus_max;
      }
      else {
        focus_max--;
        focus_move(orientation, -1);
      }
      delay(100);
    }      
    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);
  }
  
  display.clearDisplay();
  focus_min = focus_max;

  button = 1; // 0 for click
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  delay(500);

  // focus_min
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,5);
  strcpy_P(buffer, (char *)pgm_read_word(&(ring_phrases[1])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[0])));
  display.println(buffer);
  display.display();
  delay(4000);

  display.clearDisplay();
  display.setCursor(0,5);
  display.setTextSize(1);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[1])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
  display.println(buffer);
  display.display();
  delay(4000);
  
  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Set to most blur\n"));
    strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
    display.println(buffer);
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
        focus_move(orientation, 1);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (focus_min == -focus_max) {
        focus_min = focus_min;
      }
      else {
        focus_min--;
        focus_move(orientation, -1);
      }
      delay(100);
    }
    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);  
  }

  focus_setRPM(orientation, RPM);
  button = 1;
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  delay(500);
}

void setZoomRange() {
  zoom_setRPM(orientation, 1);
  // set the zoom_max
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,5);
  strcpy_P(buffer, (char *)pgm_read_word(&(ring_phrases[2])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[0])));
  display.println(buffer);
  display.display();
  delay(4000);

  display.clearDisplay();
  display.setCursor(0,5);
  display.setTextSize(1);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[1])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
  display.println(buffer);
  display.display();
  delay(4000);

  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Set to maximum zoom\n"));
    strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
    display.println(buffer);
    display.drawFastHLine(0,40, display.width(), WHITE);
    display.setTextSize(2);
    display.setCursor(28, 49);
    display.print(F("Max:"));
    display.print(zoom_max);
    display.display();
    if (x_value>600 && 450<y_value<850) {
      if (zoom_max == 250) {
        zoom_max = zoom_max;
      }
      else {
        zoom_max++;
        zoom_move(orientation, 1);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (zoom_max == 0) {
        zoom_max = zoom_max;
      }
      else {
        zoom_max--;
        zoom_move(orientation, -1);
      }
      delay(100);
    }

    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);
  }
  
  zoom_min = zoom_max;

  button = 1; // 0 for click
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  delay(500);

  // set the zoom_min
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,5);
  strcpy_P(buffer, (char *)pgm_read_word(&(ring_phrases[3])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[0])));
  display.println(buffer);
  display.display();
  delay(4000);

  display.clearDisplay();
  display.setCursor(0,5);
  display.setTextSize(1);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[1])));
  display.println(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
  display.println(buffer);
  display.display();
  delay(4000);

  while (button != 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,5);
    display.println(F("Set to minimum zoom\n"));
    strcpy_P(buffer, (char *)pgm_read_word(&(joystick_phrases[2])));
    display.println(buffer);
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
        zoom_move(orientation, 1);
      }
      delay(100);
    }
    if (x_value<440 && 450<y_value<850) {
      if (zoom_min == -zoom_max) {
        zoom_min = zoom_min;
      }
      else {
        zoom_min--;
        zoom_move(orientation, -1);
      }
      delay(100);
    }

    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);
  }

  zoom_setRPM(orientation, RPM);
  button = 1;
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  delay(500);
}

void menu(int num_option, const char *const string_table[], int option_selected) {
  int rect_y = 9;
  display.clearDisplay();
  display.setCursor(0, 0);
  strcpy_P(buffer, (char *)pgm_read_word(&(string_table[0])));
  display.println(buffer);
  
  // displaying the sub menus
  for (int i=0; i<num_option; i++) {
    display.drawRect(0,rect_y,SCREEN_WIDTH,12,WHITE);
    display.setCursor(2,rect_y+2);
    if (i == option_selected) {
      display.setTextColor(BLACK,WHITE);
    } else {
      display.setTextColor(WHITE);
    }
    strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i+1])));
    display.print(buffer);

    rect_y = rect_y+11;
  }
  display.display();
  display.setTextColor(WHITE);
}

int getUpDown(int num_option, int current_option, int delay_ms=200) {
  // joystick down
  if (y_value>600 && 450<x_value<850) {
    delay(delay_ms);
    if (current_option == num_option-1) {
      return 0;
    }
    else return ++current_option;
  }

  // joystick up
  if (y_value<440 && 450<x_value<850) {
    delay(delay_ms);
    if (current_option == 0) {
      return num_option-1;
    }
    else return --current_option;
  }

  return current_option;
}

/* Are You Sure textbox
 * 0 - Calibration not done
 * 1 - Resetting
 */
bool areYouSure(int pick=2) {
  bool sure = false;
  while (button != 0) {
    // moving joystick right
    if (x_value>600 && 450<y_value<850) {
      sure = false;
      delay(200);
    }

    // moving joystick left
    if (x_value<440 && 450<y_value<850) {
      sure = true;
      delay(200);
    }

    display.clearDisplay();
    if (pick == 0) {
      display.setCursor(2,13);
      display.println(F("Calibration not done"));
    }
    if (pick == 1) {
      display.setCursor(13,13);
      display.println(F("Resetting..."));
    }
    display.setCursor(27,22);
    display.println(F("Are you sure?"));
    display.drawRect(43,30,36,12,WHITE);
    display.setCursor(45,32);
    if (sure) {
      display.setTextColor(BLACK,WHITE);
    } else {
      display.setTextColor(WHITE);
    }
    display.print(F("Yes"));
    display.setCursor(64,32);
    if (sure) {
      display.setTextColor(WHITE);
    } else {
      display.setTextColor(BLACK,WHITE);
    }
    display.print(F("No"));
    display.display();
    display.setTextColor(WHITE);
    
    x_value = analogRead(VRX);
    y_value = analogRead(VRY);
    button = digitalRead(SW);
  }
  resetJoy();
  delay(500);
  return sure;
}

void getJoyRead(int delay_ms=0) {
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  button = digitalRead(SW);
  delay(delay_ms);
}

void resetJoy() {
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  button = 1;
}

void getJoyXRead() {
  x_value = analogRead(VRX);
}

void getJoyYRead() {
  y_value = analogRead(VRY);
}

void getJoyButton() {
  button = digitalRead(SW);
}

long toMS(float seconds) {
  return seconds * 1000000;
}

long toMilli(long micro_s) {
  return micro_s * 0.001;
}