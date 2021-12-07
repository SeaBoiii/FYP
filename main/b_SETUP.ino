//***************** SETUP *************************

void setup() {
  // ***** Motor Setup *****
  // -> Lower RPM = Higher torque
  // -> Higher Microstepping = Higher holding torque
  rear_motor.begin(RPM, 1);
  front_motor.begin(RPM, 1);
  
  rear_motor.enable();
  front_motor.enable();

  // ***** Display Setup *****
  // if display fails = no proceed
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay(); // initial display buffer (Adafruit splash screen)
  display.display();

  // ***** Joystick Setup *****
  // change the pinmode for the joysticks
  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);
  pinMode(SW, INPUT_PULLUP);

  // setting up =default values
  x_value = analogRead(VRX);
  y_value = analogRead(VRY);
  button = 1;

  // ***** EEPROM Memory Read *****
  // -Reads the focus and zoom stored memory
  // if empty (==255), setting the default values to 0
  focus_min = EEPROM.read(0);
  zoom_min = EEPROM.read(1);
  diff_focus = EEPROM.read(2);
  diff_zoom = EEPROM.read(3);
  if (focus_min == 255) {
    focus_min = 0;
  }
  if (zoom_min == 255) {
    zoom_min = 0;
  }
  if (diff_focus == 255) {
    diff_focus = 0;
  }
  if (diff_zoom == 255) {
    diff_zoom = 0;
  }
 
  // -Reads the orientation memory
  orientation = EEPROM.read(4);
  /*
   * If orientation == 0
   * -> Zoom at the rear
   * If orientation == 1
   * -> Zoom at the front
   */
  if (orientation == 255) {
    orientation = 0;
    EEPROM.write(4,0);  
  }

  // -Reads the shutter data
  shutter_data = EEPROM.read(5);
  if (shutter_data == 255) {
    shutter_speed = 1;
    EEPROM.write(5, shutter_speed);
  } else {
    shutter_speed = shutter_data;    
  }
}