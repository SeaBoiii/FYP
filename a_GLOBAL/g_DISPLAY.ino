// ********** DISPLAY **********
// Includes:
// - Create Menu
// - Create yes/no menu

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