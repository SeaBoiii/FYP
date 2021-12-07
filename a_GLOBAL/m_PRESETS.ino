// ******** PRESET Functions *********
// Here contains all the preset functions from the
// - advanced movements

void preset_bokeh() {
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

void preset_firework() {
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

void preset_zoomBlur() {
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

void preset_sineWave() {
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