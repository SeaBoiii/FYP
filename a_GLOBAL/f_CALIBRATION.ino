// ****************** CALIBRATION ********************
// Includes: 
// - Focus Calibration
// - Zoom Calibration

// set Focus Calibration
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


// set Zoom Calibration
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

void zoom_cali() {
  if (diff_zoom == 0) {
    sure = true;
  } else {
    sure = areYouSure();          
  }

  if (sure) {
    zoom_max = 0;
    setZoomRange();
    
    diff_zoom = zoom_max - zoom_min;

    EEPROM.write(3,diff_zoom);

    zoom_max = diff_zoom;
    zoom_min = 0;

  } else {
    sure = false;
    goto RECALMENU;
  }
  sure = false;
}

void focus_cali() {
  if (diff_focus == 0) {
      sure = true;          
    } else {
      sure = areYouSure();
    }

    if (sure) {
      focus_max = 0;
      setFocusRange();

      diff_focus = focus_max - focus_min;

      EEPROM.write(2,diff_focus);

      focus_max = diff_focus;
      focus_min = 0;
      
    } else {
      sure = false;
      goto RECALMENU;
    }
    sure = false;
}


void reset_cali() {
  sure = areYouSure();

  if (sure) {
    // reset of variables
    // setting to 255 will reset the memory
    /*
    int focus_min;      // address 0
    int zoom_min;       // address 1
    int diff_focus;     // address 2
    int diff_zoom;      // address 3
    */
    EEPROM.write(0,255);
    EEPROM.write(1,255);
    EEPROM.write(2,255);
    EEPROM.write(3,255);
    resetFunc(); //call reset
  } else {
    sure = false;
    goto RECALMENU;
  }
  sure = false;
}