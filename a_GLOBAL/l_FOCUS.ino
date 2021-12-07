// ******* FOCUS Function *******

void manual_focus() {
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