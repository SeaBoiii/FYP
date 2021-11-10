//***************** LOOP *************************
void loop() {
  /* Main Menu
   * - Recalibration (Initialisation)
   * - Basic Movement
   * - Advanced Movement
   * - Quit
   */
  while (button != 0) {
    option = getUpDown(4, option, 200);
    menu(4, main_menu, option);
    getJoyRead();
  }

  /* Main Option */
  int main_option = option;
  option = 0;
  resetJoy();
  delay(500);
  
  // Recalibration/Initialisation
  if (main_option == 0) {
      // calibration
      bool sure = false;
      int re_option = 0;
      RECALMENU:
      while (button != 0) {
        re_option = getUpDown(4, re_option, 200);
        menu(4, recalibration_menu, re_option);
        getJoyRead();
      }

      resetJoy();
      display.clearDisplay();
      display.setTextSize(1);
      delay(500);
        
      /* Logic */
      // zoom recalibration
      if (re_option == 0) {
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
      // focus recalibration
      if (re_option == 1) {
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
      // reset all
      if (re_option == 2) {
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
      // back
      if (re_option == 3) {
        // {empty placeholder}
      }
    // note that now both focus and zoom motor is at ideal 0 (min)
    // drawing of main menu
    // 0=zoom, 1=focus, 2=presets, 3=recalibration, 4=quit
    resetJoy();
    display.clearDisplay();
    display.setTextSize(1);
    delay(200);
  }
  // Primary Movements 
  /* Menu
   * - Zoom
   * - Focus
   * - Zoom to distance
   * - Back
   */
  if (main_option == 1) {
    // if calibration not done
    if (diff_zoom==0 && diff_focus==0) {
      bool sure = false;
      sure = areYouSure(0);
      resetJoy();
      delay(500);

      if (sure == false) {
        return;
      }
    }
    STARTMENU:
    // main function
    while (button != 0) {
      option = getUpDown(4, option);      
      menu(4, basic_menu, option);
      getJoyRead();
    }

    int start_option = option;
    option = 0;
    int sub_option = 0;
    resetJoy();
    delay(500);

    // manual zoom ring
    if (start_option == 0) {
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
    // manual focus ring
    if (start_option == 1) {
      float divs = (SCREEN_WIDTH-30)/(float)diff_focus;
      float focus_divs = focus_min; //temp variable
      display.clearDisplay();
      do {
        sub_option = getUpDown(2, sub_option);
        
        focus_divs = focus_min*divs;
        display.setCursor(0,0);
        if (sub_option == 0) {
          display.setTextColor(BLACK,WHITE);

          if (x_value>600 && 450<y_value<850) {
            if (focus_min == diff_focus) {
              focus_min = focus_min;
            }
            else {
              focus_min++;
              focus_move(orientation, 1);
            }
            delay(100);
          }
          if (x_value<440 && 450<y_value<850) {
            if (focus_min == 0) {
              focus_min = 0;
            }
            else {
              focus_min--;
              focus_move(orientation, -1);
            }
            delay(100);
          }
          
        } else {
          display.setTextColor(WHITE,BLACK);
        }
        display.print(F("|---- Focus Ring----|\n"));
        display.setTextColor(WHITE);
        strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[1])));
        display.print(buffer);
        display.print(diff_focus);
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
        display.fillRect(2,35,(int)focus_divs,10,WHITE);
        display.setCursor(105,36);
        display.setTextColor(WHITE,BLACK);
        display.print(focus_min);
        strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[4])));
        display.print(buffer);
        display.display();

        getJoyRead();

        if (sub_option == 1 && button == 0) {
          button = 1;
          delay(500);
          EEPROM.write(0,focus_min);
          goto STARTMENU;
        } 
      } while (true); 
    }
    // zoom to distance
    if (start_option == 2) {
      ZOOM_MENU:
      while (button != 0) {
        option = getUpDown(4, option);
        menu(4, movetodist_menu, option);
        getJoyRead();
      }
      int zoom_option = option;
      option = 0;
      resetJoy();
      delay(500);

      // zoom to max
      if (zoom_option == 0) {
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
      // zoom to min
      if (zoom_option == 1) {
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
      // zoom to specific distance
      if (zoom_option == 2) {
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
      // back
      if (zoom_option == 3) {
        goto STARTMENU;
      }
    }
    // back
    if (start_option == 3) {
      return;
    } 
  }

  // Advanced Movements
  // No menu, just presets
  if (main_option == 2) {
    // if calibration not done
    if (diff_zoom==0 && diff_focus==0) {
      bool sure = false;
      sure = areYouSure(0);
      if (sure == false) {
        return;
      }
    }
    ADVMENU:
    while (button != 0) {
      option = getUpDown(5, option);
      menu(5, adv_menu, option);
      getJoyRead();
    }

    int start_option = option;
    option = 0;
    int sub_option = 0;
    resetJoy();
    delay(500);

    // bokeh effect
    if (start_option == 0) {
      zoom_setSpeedProfile(orientation, true, 2000, 500);
      focus_setSpeedProfile(orientation, true, 500, 1000);
      int zoom_steps_to_move = diff_zoom - zoom_min;
      int focus_steps_to_move = diff_focus - focus_min;
      display.clearDisplay();
      display.setCursor(0,5);
      strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[3])));
      display.print(buffer);
      display.print(zoom_steps_to_move);
      display.display();
      zoom_startMove(orientation, zoom_steps_to_move/2, toMS(shutter_speed/4));
      focus_startMove(orientation, focus_steps_to_move, toMS(shutter_speed/4));
      unsigned wait_time;
      do {
        wait_time = zoom_nextAction(orientation);
        delay(toMilli(wait_time));
      } while(wait_time);
      do {
        wait_time = focus_nextAction(orientation);
        delay(toMilli(wait_time));
      } while(wait_time);
      zoom_startMove(orientation, zoom_steps_to_move/2, toMS(shutter_speed/4));
      focus_startMove(orientation, -focus_steps_to_move, toMS(shutter_speed/4));
      do {
        wait_time = zoom_nextAction(orientation);
        delay(toMilli(wait_time));        
      } while(wait_time);
      do {
        wait_time = focus_nextAction(orientation);
        delay(toMilli(wait_time));
      } while(wait_time);

      // return zoom to default
      zoom_startMove(orientation, -zoom_steps_to_move, toMS(1));
      do {
        wait_time = zoom_nextAction(orientation);
        delay(toMilli(wait_time));
      } while(wait_time);

      delay(3000);
      zoom_setSpeedProfile(orientation, false);
      focus_setSpeedProfile(orientation, false);
      EEPROM.write(0,focus_min);
      EEPROM.write(1,zoom_min);
      goto ADVMENU;
    }
    
    // firework effect
    if (start_option == 1) {
      int steps_to_move = diff_focus - focus_min;
      display.clearDisplay();
      display.setCursor(0,5);
      strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[3])));
      display.print(buffer);
      display.print(steps_to_move);
      display.display();
      focus_startMove(orientation, steps_to_move, toMS(shutter_speed/2));
      unsigned wait_time; 
      do {
        wait_time = focus_nextAction(orientation);
        delay(toMilli(wait_time));
      } while(wait_time);
      focus_startMove(orientation, -steps_to_move, toMS(shutter_speed/2));
      do {
        wait_time = focus_nextAction(orientation);
        delay(toMilli(wait_time));
      } while(wait_time);

      delay(3000);
      goto ADVMENU;
    }

    // zoom blur effect
    if (start_option == 2) {
      int zoom_steps_to_move = diff_zoom - zoom_min;
      int focus_steps_to_move = diff_focus - focus_min;
      display.clearDisplay();
      display.setCursor(0,5);
      strcpy_P(buffer, (char *)pgm_read_word(&(key_phrases[3])));
      display.print(buffer);
      display.print(zoom_steps_to_move);
      display.display();
      
      // focus to subject
      focus_startMove(orientation, focus_steps_to_move, toMS(1));
      unsigned wait_time;
      do {
        wait_time = focus_nextAction(orientation);
        delay(toMilli(wait_time));        
      } while(wait_time);
      zoom_startMove(orientation, zoom_steps_to_move, toMS(shutter_speed/2));
      focus_startMove(orientation, -focus_steps_to_move, toMS(shutter_speed/2));
      do {
        wait_time = focus_nextAction(orientation);
        delay(toMilli(wait_time));  
      } while(wait_time);  
      do {
        wait_time = zoom_nextAction(orientation);
        delay(toMilli(wait_time)); 
      } while(wait_time);

      delay(3000);
      // return zoom to default
      zoom_startMove(orientation, -zoom_steps_to_move, toMS(1));
      do {
        wait_time = zoom_nextAction(orientation);
      } while(wait_time);    

      EEPROM.write(0,focus_min);
      EEPROM.write(1,zoom_min);
      goto ADVMENU;
    }

    // sine wave
    if (start_option == 3) {
      zoom_setSpeedProfile(orientation, true, 1000, 1000);
      int steps_to_move = diff_zoom - zoom_min;
      display.clearDisplay();
      display.setCursor(0,5);
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
      delay(3000);

      // return to default
      zoom_startMove(orientation, steps_to_move, toMS(1));
      do {
        wait_time = zoom_nextAction(orientation);
        delay(toMilli(wait_time));
      } while(wait_time);
      
      zoom_setSpeedProfile(orientation, false);
      goto ADVMENU;
    }

    // back
    if (start_option == 4) {
      return;
    }
  }
  
  // Settings menu
  if (main_option == 3) {
    SETTINGS:
    while (button != 0) {
      option = getUpDown(3, option, 200);
      menu(3, settings_menu, option);
      getJoyRead();
    }

    int sub_option = option;
    option = 0;
    resetJoy();
    delay(500);
    
    // orientation
    if (sub_option == 0) {
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

    // Shutter Speed
    if (sub_option == 1) {
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

    // back    
    if (sub_option == 2) {
      return;
    }
  }
}
