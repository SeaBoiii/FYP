// ******** DISPLAY Functions **********
/* ----- MENU ------
 * Creates a menu using a string array 
 * Display which option is being selected
 * 
 * [Top of screen]
 * Shutter Speed: __
 * Rear Motor: ____ Front Motor: ____
 * 
 * [Middle of screen]
 * |----< Menu Name >----|
 * | Option 1
 * | Option 2
 * | ...
 * 
 * [Bottom of screen]
 * < > Move the lens          {Will be dependent on the menu}
 * ^v  Select the options
 *  o  Select current option
 */

int menu(int array_size, const char *const string_table[], int option_selected, bool header=false, int footer=2) {
  int total_num = array_size; 
  if (!updateMenu) return total_num;
  updateMenu = false;
  tft.setCursor(0, 0);
  int rect_y = 9;
  if (header) {
    tft.setTextColor(AQUA);
    tft.print(F("Shutter Speed: "));
    tft.setTextColor(WHITE);
    tft.println(shutter_speed);
    tft.setTextColor(AQUA);
    tft.print(F("Front Motor: "));
    tft.setTextColor(WHITE);
    tft.println(orientation ? "Zoom" : "Focus");
    tft.setTextColor(AQUA);
    tft.print(F("Rear Motor: "));
    tft.setTextColor(WHITE);
    tft.println(orientation ? "Focus" : "Zoom");
    tft.println();
    rect_y =42;
  }
  tft.setTextColor(DEEPPINK);
  strcpy_P(buffer, (char *)pgm_read_word(&(string_table[0])));
  tft.println(buffer);
  tft.setTextColor(WHITE,BLACK);
  
  // displaying the sub menus
  for (int i=0; i<total_num; i++) {
    tft.drawRect(0,rect_y,tft.width(),12,WHITE);
    tft.setCursor(2,rect_y+2);
    if (i == option_selected) {
      tft.setTextColor(BLACK,WHITE);
    } else {
      tft.setTextColor(WHITE,BLACK);
    }
    strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i+1])));
    tft.print(buffer);

    rect_y = rect_y+11;
  }
  tft.setTextColor(WHITE,BLACK);

  switch(footer) {
    case 1:
      tft.drawChar(2,130,LEFT_ARROW,WHITE,BLACK,2);
      tft.drawChar(17,130,RIGHT_ARROW,WHITE,BLACK,2);
      tft.setCursor(32,134);
      tft.println(F(": Adjust values"));
      tft.drawChar(2,145,SELECT,WHITE,BLACK,2);
      tft.setCursor(17,149);
      tft.println(F(": Press to set"));
      break;
    case 2:
      tft.drawChar(2,130,UP_ARROW,WHITE,BLACK,2);
      tft.drawChar(17,130,DOWN_ARROW,WHITE,BLACK,2);
      tft.setCursor(32,134);
      tft.println(F(": Navigate"));
      tft.drawChar(2,145,SELECT,WHITE,BLACK,2);
      tft.setCursor(17,149);
      tft.println(F(": Press to set"));
      break;
    default:
    break;
  }

  return total_num;
}

/* --- Hotbar Screen ---
 *  Creates a template for a bar type screen
 *  Calculates the necessary parameters:
 *  char title - The title of the screen
 *  int current - Current value
 *  int max_range - The maximum range of the bar
 *  bool header - Header for the hotbar [false] 
 *  int footer - Type of footer to display [1]
 */

int hotbar(char title[], int current, int max_range, int current_option=0, bool haveBack=false, bool header=false, int footer=3) {
  if (!updateMenu) return current;
  updateMenu = false;
  
  int divs = (tft.width()-30)/(float)max_range * current;
  tft.setCursor(0, 0);
  int rect_y = 37;
  if (header) {
    tft.setTextColor(AQUA);
    tft.print(F("Shutter Speed: "));
    tft.setTextColor(WHITE);
    tft.println(shutter_speed);
    tft.setTextColor(AQUA);
    tft.print(F("Front Motor: "));
    tft.setTextColor(WHITE);
    tft.println(orientation ? "Zoom" : "Focus");
    tft.setTextColor(AQUA);
    tft.print(F("Rear Motor: "));
    tft.setTextColor(WHITE);
    tft.println(orientation ? "Focus" : "Zoom");
    tft.println();
    rect_y = 75;
  }
  option ? tft.setTextColor(WHITE,BLACK) : tft.setTextColor(BLACK,WHITE);
  if (title != NULL) {
    char myChar;
    for (byte k=0; k<strlen_P(title); k++) {
      myChar = pgm_read_byte_near(title+k);
      tft.print(myChar);
    }
  }
  tft.println();
  tft.setTextColor(WHITE);
  tft.println();
  tft.print(F("Range: "));
  tft.println(max_range);
  tft.setCursor(2, rect_y + 22);
  option ? tft.setTextColor(BLACK,WHITE) : tft.setTextColor(WHITE,BLACK);
  if (haveBack) tft.println(F("[Go Back]"));

  /* Draw Hotbar */
  tft.drawRect(0,rect_y,tft.width()-26,14,WHITE);
  tft.fillRect(2,rect_y+2,tft.width()-30,10,BLACK); // reset inner rectangle
  tft.fillRect(2,rect_y+2,divs,10,WHITE);
  tft.setCursor(105,rect_y+3);
  tft.setTextColor(WHITE,BLACK);
  tft.print(current);
  tft.print(" ");

  switch(footer) {
    case 1:
      tft.drawChar(2,130,LEFT_ARROW,WHITE,BLACK,2);
      tft.drawChar(17,130,RIGHT_ARROW,WHITE,BLACK,2);
      tft.setCursor(32,134);
      tft.println(F(": Adjust values"));
      tft.drawChar(2,145,SELECT,WHITE,BLACK,2);
      tft.setCursor(17,149);
      tft.println(F(": Press to set"));
      break;
    case 2:
      tft.drawChar(2,130,UP_ARROW,WHITE,BLACK,2);
      tft.drawChar(17,130,DOWN_ARROW,WHITE,BLACK,2);
      tft.setCursor(32,134);
      tft.println(F(": Navigate"));
      tft.drawChar(2,145,SELECT,WHITE,BLACK,2);
      tft.setCursor(17,149);
      tft.println(F(": Press to set"));
      break;
    case 3:
      tft.drawChar(2,115,LEFT_ARROW,WHITE,BLACK,2);
      tft.drawChar(17,115,RIGHT_ARROW,WHITE,BLACK,2);
      tft.setCursor(32,119);
      tft.println(F(": Adjust values"));
      tft.drawChar(2,130,UP_ARROW,WHITE,BLACK,2);
      tft.drawChar(17,130,DOWN_ARROW,WHITE,BLACK,2);
      tft.setCursor(32,134);
      tft.println(F(": Navigate"));
      tft.drawChar(2,145,SELECT,WHITE,BLACK,2);
      tft.setCursor(17,149);
      tft.println(F(": Press to set"));
      break;
    default:
    break;
  }
  
  return current;
}

/* Reset Screen */
int resetScreen(int s) {
  tft.background(0,0,0);
  s = -1;
  updateMenu = true;
  option = 0;
  return s;
}

/* --- Calibrate Screen ---
 * String_table will determine if zoom/focus
 * return calibration distance
 */
int caliMenu(const char *const string_table[], int current_step, int max_steps=200) {
  if (!updateMenu) return current_step;
  current_step = hotbar(NULL, current_step, max_steps, 0, false, false, 1);
  int i = 0;
  tft.setCursor(0,0);
  strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i++])));
  tft.println(buffer);
  tft.setCursor(0, 59);
  strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i++])));
  tft.print(buffer);
  strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i++])));
  tft.setTextColor(DARKGREEN);
  tft.println(buffer);
  tft.setTextColor(WHITE);
  
  return current_step;
}
