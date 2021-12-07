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