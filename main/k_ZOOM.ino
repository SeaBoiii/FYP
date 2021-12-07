// ******* ZOOM Functions ********

void manual_zoom() {
  float divs = (SCREEN_WIDTH-30)/(float)diff_zoom;
  float zoom_divs = zoom_min; //temp variable
  display.clearDisplay();
  do {
    sub_option = getUpDown(2, sub_option);

    zoom_divs = zoom_min*divs;
    display.setCursor(0,0);
    if (sub_option == 0) {
      display.setTextColor(BLACK,WHITE);

      if (x_value>600 && 450<y_value<850) {
        if (zoom_min == diff_zoom) {
          zoom_min = zoom_min;
        }
        else {
          zoom_min++;
          zoom_move(orientation, 1);
        }
        delay(100);
      }
      if (x_value<440 && 450<y_value<850) {
        if (zoom_min == 0) {
          zoom_min = 0;
        }
        else {
          zoom_min--;
          zoom_move(orientation, -1);
        }
        delay(100);
      }
      
    } else {
      display.setTextColor(WHITE,BLACK);
    }
    display.print(F("|---- Zoom Ring ----|\n"));
    display.setTextColor(WHITE);
    strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[1])));
    display.print(buffer);
    display.print(diff_zoom);
    display.setCursor(2,55);
    if (sub_option == 1) {
      display.setTextColor(BLACK,WHITE);
    } else {
      display.setTextColor(WHITE,BLACK);
    }
    strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[0])));
    display.print(buffer);
    display.drawRect(0,33,SCREEN_WIDTH-26,14,WHITE);
    display.fillRect(2,35,SCREEN_WIDTH-30,10,BLACK);  /* Reset inner rectangle */
    display.fillRect(2,35,(int)zoom_divs,10,WHITE);
    display.setCursor(105,36);
    display.setTextColor(WHITE,BLACK);
    display.print(zoom_min);
    strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[4])));
    display.print(buffer);
    display.display();

    getJoyRead();

    if (sub_option == 1 && button == 0) {
      button = 1;
      delay(500);
      EEPROM.write(1,zoom_min);
      goto STARTMENU;
    } 
  } while (true); 
}

void toMax_zoom() {
  int steps_to_move = diff_zoom - zoom_min;
  display.clearDisplay();
  display.setCursor(0,5);
  display.println(F("Moving to the \nmaximum zoom"));
  strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[3])));
  display.print(buffer);
  display.print(steps_to_move);
  display.display();
  zoom_startMove(orientation, steps_to_move, toMS(shutter_speed));
  unsigned wait_time;        
  do {
    wait_time = zoom_nextAction(orientation);
    delay(toMilli(wait_time));
  } while(wait_time);  

  // return to default
  zoom_startMove(orientation, -steps_to_move, toMS(1));
  do {
    wait_time = zoom_nextAction(orientation);
    delay(toMilli(wait_time));
  } while(wait_time);    

  delay(3000);
  goto ZOOM_MENU;
}

void toMin_zoom() {
  int steps_to_move = 0 - zoom_min;
  display.clearDisplay();
  display.setCursor(0,5);
  display.println(F("Moving to the \nminimum zoom"));
  strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[3])));
  display.print(buffer);
  display.print(steps_to_move);
  display.display();
  zoom_startMove(orientation, steps_to_move, toMS(shutter_speed));
  unsigned wait_time; 
  do {
    wait_time = zoom_nextAction(orientation); 
    delay(toMilli(wait_time));         
  } while (wait_time);

  // return to default
  zoom_startMove(orientation, -steps_to_move, toMS(1));
  do {
    wait_time = zoom_nextAction(orientation);
    delay(toMilli(wait_time));
  } while(wait_time); 

  delay(3000);
  EEPROM.write(1,zoom_min);
  goto ZOOM_MENU;
}

void toDist_zoom() {
  zoom_setRPM(orientation, 5);
  float divs = (SCREEN_WIDTH-30)/(float)diff_zoom;
  float zoom_divs = zoom_min; //temp variable
  int current_zoom_min = zoom_min;
  display.clearDisplay();
  do {
    sub_option = getUpDown(2, sub_option);

    zoom_divs = zoom_min*divs;
    display.setCursor(0,0);
    if (sub_option == 0) {
      display.setTextColor(BLACK,WHITE);

      if (x_value>600 && 450<y_value<850) {
        if (zoom_min == diff_zoom) {
          zoom_min = zoom_min;
        }
        else {
          zoom_min++;
          zoom_move(orientation, 1);
        }
      }
      if (x_value<440 && 450<y_value<850) {
        if (zoom_min == 0) {
          zoom_min = 0;
        }
        else {
          zoom_min--;
          zoom_move(orientation, -1);
        }
      }
      
    } else {
      display.setTextColor(WHITE,BLACK);
    }
    display.print(F("|-Set zoom distance-|\n"));
    display.setTextColor(WHITE);
    strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[1])));
    display.print(buffer);
    display.print(diff_zoom);
    display.setCursor(2,55);
    if (sub_option == 1) {
      display.setTextColor(BLACK,WHITE);
    } else {
      display.setTextColor(WHITE,BLACK);
    }
    strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[2])));
    display.print(buffer);
    display.drawRect(0,33,SCREEN_WIDTH-26,14,WHITE);
    display.fillRect(2,35,SCREEN_WIDTH-30,10,BLACK);  /* Reset inner rectangle */
    display.fillRect(2,35,(int)zoom_divs,10,WHITE);
    display.setCursor(105,36);
    display.setTextColor(WHITE,BLACK);
    display.print(zoom_min);
    strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[4])));
    display.print(buffer);
    display.display();

    getJoyRead();

    if (sub_option == 1 && button == 0) {
      zoom_setRPM(orientation, RPM);
      button = 1;
      delay(500);
      int steps_to_move = zoom_min - current_zoom_min;

      // going back to default
      zoom_startMove(orientation, -steps_to_move, toMS(1));
      unsigned wait_time;
      do {
        wait_time = zoom_nextAction(orientation);
        delay(toMilli(wait_time));
      } while(wait_time);

      delay(2000);
      display.clearDisplay();
      display.setCursor(0,5);
      display.println(F("Move to set distance"));
      strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[3])));
      display.print(buffer);
      display.print(steps_to_move);
      display.display();

      zoom_startMove(orientation, steps_to_move, toMS(shutter_speed));
      do {
        wait_time = zoom_nextAction(orientation);
        delay(toMilli(wait_time));
      } while(wait_time);        
      
      // return to default
      zoom_startMove(orientation, -steps_to_move, toMS(1));
      do {
        wait_time = zoom_nextAction(orientation);
        delay(toMilli(wait_time));
      } while(wait_time);
      
      delay(3000);
      zoom_min = current_zoom_min;
      EEPROM.write(1,zoom_min);
      goto STARTMENU;
    } 
  } while (true);
}