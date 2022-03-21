/* ******** MOTOR CONTROL Functions **********
 *  
 * Functions involving the stepper driver and movement of motors.
 * Libraries used:
 * - AccelStepper.h
 * 
 * Functions available:
 * calcRPM() - Calculates the RPM required for the motor to move 
 *             to position in a fixed time.
 * calcAccel() - Calculates the acceleration required for the motor to move 
 *               to position in a fixed time.
 * toMS() - Easy seconds to milliseconds converter.
 * splitStr() - Splits a string by ` ` or `,` and works hand in hand with 
 *              `setMotor()` to move a motor based on the string fed.
 * setMotor() - Moves the motor according to the instructions set on the string.
 * moveMotor() - Moves the motor based on the desired position in `x` time.
 * setAccel() - Sets the acceleration of a motor.
 * setCurrentPos() - Sets the current position of a motor.
 * moveMultiMotor() - Moves 2 motors in unison based on the desired position in `x` time.
 * 
 */

/*
 * to MilliSeconds converter
 * 
 * @param seconds   Time to be converted to milliseconds. (in seconds)
 * @return long     Converted seconds to milliseconds.
 */
long toMS(float seconds) {
  return seconds * 1000;
}

/*
 * Using Character Array to Movement
 * - Splits the character array by ` ` or `,`
 * - Gives the required parameters for `setMotor()`
 * 
 * @param data              Character array of the custom pattern.
 * @param title             Title of the screen.
 * @param custom_itemcount  The total amount of sequences in the character array.
 */
void splitStr(char* data, char title[], int custom_itemcount) {
  char int_buf[25];
  strcpy(int_buf, data);
  char* parameter;
  parameter = strtok(int_buf, " ,");
  while (parameter != NULL) {
    setMotor(parameter, title, custom_itemcount);
    parameter = strtok(NULL, " ,");
  }
}

/*
 * Works hand in hand with `splitStr()` to move the motor
 * according to the desired sequence.
 * 
 * How it is read:
 * Z/F T/F T/F int  [min 4 char, max 6 char]
 * data[0] - Either zoom or focus motor.
 * data[1] - If motor has to go back or not.
 * data[2] - If it is the last sequence.
 * data[4++] - The number of steps to move.
 * 
 * @param data              Character array of a sequence/
 * @param title             Title of the screen.
 * @param custom_itemcount  The total amount of sequences in the character array.
 */
void setMotor(char* data, char title[], int custom_itemcount) {
  bool goBack = false;
  if ((data[1] == 'T' || data[1] == 't')) {
    goBack = true;
  }

  bool lastSequence = false;
  if ((data[2] == 'T' || data[2] == 't')) {
    lastSequence = true;
  }
  
  if ((data[0] == 'F') || (data[0] == 'f')) {
    int steps = strtol(data+3, NULL, 10);
    // set focus motor steps to steps
    //moveMotor(FOCUS, steps, motor_time/custom_itemcount);
    goDist(FOCUS, title, steps, SNOW, goBack, motor_time/custom_itemcount, lastSequence); 
  }

  if ((data[0] == 'Z') || (data[0] == 'z')) {
    int steps = strtol(data+3, NULL, 10);
    // set zoom motor steps to steps
    //moveMotor(ZOOM, steps, motor_time/custom_itemcount);
    goDist(ZOOM, title, steps, AZURE, goBack, motor_time/custom_itemcount, lastSequence);
  }
}

/*
 * Move motor to desired location
 * - Given the position
 * - Move the motor to pos
 * - Return motor to pos
 * type = 0 [FOCUS], 1 [ZOOM]
 *  orientation 0 [Focus Front, Zoom Rear]
 *  orientation 1 [Zoom Front, Focus Rear]
 *  
 * @param type          Motor currently selected.
 * @param pos_desired   Number of steps for the motor to move.
 * @param shutter_spd   Time allowed for the motor to move to the desired position.
 */

void moveMotor(int type, int pos_desired, int shutter_spd=0) {
  // zoom_range, focus_range, zoom_current, focus_current, 
  // zoom_min, focus_min, shutter_speed
  Servo *motor;
  int pos_current;
  int min_pos;
  
  if (type) { // ZOOM
    motor = orientation ? &front_motor : &rear_motor;
    pos_current = zoom_current;
    min_pos = zoom_min;
  } else { // FOCUS
    motor = orientation ? &rear_motor : &front_motor;
    pos_current = focus_current;
    min_pos = focus_min;
  }

  int steps_to_move = (pos_desired - pos_current);
  
  // if +ve, move clockwise
  // else -ve, move anti-clockwise
  if (shutter_spd != 0) {
    if (steps_to_move > 0) {  // +ve
      for (int pos=pos_current; pos<=pos_desired; pos+=1) {
        if ((type == 1 && orientation == 0) || (type == 0 && orientation == 1)) { // if rear motor
          motor->write(MOTOR_STEPS - (pos + min_pos));
        } else {
          motor->write(pos + min_pos);
        }
        delay(toMS((float)shutter_spd/abs(steps_to_move)));
      }
    }

    if (steps_to_move < 0) {  // -ve
      for (int pos=pos_current; pos>=pos_desired; pos-=1) {
        if ((type == 1 && orientation == 0) || (type == 0 && orientation == 1)) { // if rear motor
          motor->write(MOTOR_STEPS - (pos + min_pos));
        } else {
          motor->write(pos + min_pos);
        }
        delay(toMS((float)shutter_spd/abs(steps_to_move)));
      }
    }
    
  } else {
    if ((type == 1 && orientation == 0) || (type == 0 && orientation == 1)) { // if rear motor
      motor->write(MOTOR_STEPS - (pos_desired + min_pos));
    } else {
      motor->write(pos_desired + min_pos);
    }
  }

  type ? zoom_current = pos_desired : focus_current = pos_desired;
}

/*
 * MultiStepper methods
 * Functions available: moveTo & runSpeedToPosition
 * To not move a motor, put '-1' in the value
 * 
 * @param zoom_value    Steps to move the zoom motor.
 * @param focus_value   Steps to move the focus motor.
 * @param shutter_spd   Time allowed for the motor to move to the desired position.
 */
void moveMultiMotor(int zoom_value, int focus_value, float shutter_spd=0) {
  int rear_position;
  int front_position;
  int rear_current = orientation ? focus_current : rear_current;
  int front_current = orientation ? zoom_current : focus_current;
  int rear_min;
  int front_min;
  
  if (zoom_value == -1) {
    rear_position = orientation ? focus_value : zoom_current;
    front_position = orientation ? zoom_current : focus_value; 
  } else if (focus_value == -1) {
    rear_position = orientation ? focus_current : zoom_value;
    front_position = orientation ? zoom_value : focus_current;
  } else if (focus_value == -1 && zoom_value == -1) {
    rear_position = orientation ? focus_current : zoom_current;
    front_position = orientation ? zoom_current : focus_current;
  } else {
    rear_position = orientation ? focus_value : zoom_value;
    front_position = orientation ? zoom_value : focus_value;
  }

  rear_min = orientation ? focus_min : zoom_min;
  front_min = orientation ? zoom_min : focus_min;

  int rear_steps = rear_position-rear_current;
  int front_steps = front_position-front_current;
  int average_steps = abs((rear_steps+front_steps)/2);

  while(rear_steps != 0 || front_steps != 0) {
    if (rear_steps > 0) {
      rear_current += 1;
      rear_steps--;
    } else if (rear_steps < 0) {
      rear_current -= 1;
      rear_steps++;
    }

    if (front_steps > 0) {
      front_current += 1;
      front_steps--;
    } else if (front_steps < 0) {
      front_current -= 1;
      front_steps++;
    }
    rear_motor.write(MOTOR_STEPS - (rear_current + rear_min));
    front_motor.write(front_current + front_min);
    if (shutter_spd != 0) {
      delay(toMS((float)shutter_spd/average_steps));
    }
  }

  if (zoom_value == -1) {
    focus_current = focus_value;
  } else if (focus_value == -1) {
    zoom_current = zoom_value;
  } else {
    focus_current = focus_value;
    zoom_current = zoom_value;
  }
}
