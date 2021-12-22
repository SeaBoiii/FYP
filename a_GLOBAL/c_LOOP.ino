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
        zoom_cali();
      }

      if (re_option == 1) {
        focus_cali();
      }

      // reset all
      if (re_option == 2) {
        reset_cali();
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
    // checks calibration not done
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
      manual_zoom();
    }
    
    // manual focus ring
    if (start_option == 1) {
      manual_focus();
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
        toMax_zoom();
      }
      // zoom to min
      if (zoom_option == 1) {
        toMin_zoom();
      }
      // zoom to specific distance
      if (zoom_option == 2) {
        toDist_zoom();
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
  // Up to 4 presets
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
      preset_bokeh();
    }
    
    // firework effect
    if (start_option == 1) {
      preset_firework();
    }

    // zoom blur effect
    if (start_option == 2) {
      preset_zoomBlur();
    }

    // sine wave
    if (start_option == 3) {
      preset_sineWave();
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
      set_Orientation();  
    }

    // Shutter Speed
    if (sub_option == 1) {
      set_ShutterSpeed();
    }

    // back    
    if (sub_option == 2) {
      return;
    }
  }
}
