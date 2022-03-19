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

/** 
 * Calculate RPM by ratio
 * May not be needed since the speed
 * in AccelMotor is in steps/s
 * 
 * @param steps     Total steps for the motor to take.
 * @param seconds   Time required for the motor to finish.
 * @return float    RPM required for motor to reach in time.
 *                  (Either slowing down or speeding up)
 */
float calcRPM(int steps, float seconds) {
  return (float)(steps * MS_STEP)/seconds;
}

/*
 * Calculating the acceleration
 * @warning Requires some finetuning
 * 
 * @param steps     Total steps for the motor to take.
 * @param seconds   Time required for the motor to finish.
 * @return float    Acceleration required for motor to reach in time.
 *                  (Either accelerating or decelerating)
 */
float calcAccel(int steps, float seconds) {
  float max_speed = calcRPM(steps, seconds);
  return (float)max_speed / seconds;
}

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
 * data[3++] - The number of steps to move.
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
  AccelStepper *stepper;
  int pos_current;
  
  if (type) { // ZOOM
    stepper = orientation ? &front_motor : &rear_motor;
    pos_current = zoom_current;
  } else { // FOCUS
    stepper = orientation ? &rear_motor : &front_motor;
    pos_current = focus_current;
  }
  
  int steps_to_move = (pos_desired - pos_current) * MS_STEP;

  if (shutter_spd != 0) {
    stepper->setAcceleration(calcAccel(abs(steps_to_move), (float)shutter_spd));
  } 
  if (shutter_spd == 0) {
    stepper->setAcceleration(CALI_ACCEL);
  }
  // if +ve, move clockwise
  // else -ve, move anti-clockwise
  stepper->moveTo(pos_desired * MS_STEP);
  
  //blocking statement
  while (stepper->distanceToGo() != 0) {
    stepper->run();
    if(shutter_spd != 0) {
      delay(toMS((float)shutter_spd/abs(steps_to_move)));
    }
  }
}

/* 
 * @Override AccelStepper methods
 * type = 0 [FOCUS], 1 [ZOOM]
 * orientation 0 [Focus Front, Zoom Rear]
 * orientation 1 [Zoom Front, Focus Rear]
 *  
 * @param type   Motor currently selected.
 * @param accel  Acceleration to be set.
 */

void setAccel(int type, float accel) {
  AccelStepper *stepper;
  if (type) {
    stepper = orientation ? &front_motor : &rear_motor;
  } else {
    stepper = orientation ? &rear_motor : &front_motor;
  }

  stepper->setAcceleration(accel);
}

/*
 * @Override AccelStepper methods
 * type = 0 [FOCUS], 1 [ZOOM]
 * orientation 0 [Focus Front, Zoom Rear]
 * orientation 1 [Zoom Front, Focus Rear]
 * 
 * @param type    Motor currently selected.
 * @param value   Current position to be set.
 */
void setCurrentPos(int type, float value) {
  AccelStepper *stepper;
  if (type) {
    stepper = orientation ? &front_motor : &rear_motor;
  } else {
    stepper = orientation ? &rear_motor : &front_motor;
  }

  stepper->setCurrentPosition(value);
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

  int focus_steps = abs(focus_value-focus_current) * MS_STEP;
  int zoom_steps = abs(zoom_value-zoom_current) * MS_STEP;
  int average_steps = (focus_steps+zoom_steps)/2;

  //adjust speed accordingly
  if (shutter_spd != 0) {
    float zoom_RPM = 0;
    float focus_RPM = 0;
    if(zoom_value != -1) {
      zoom_RPM = calcAccel(zoom_steps, (float)shutter_spd);
    }
    if (focus_value != -1) {
      focus_RPM = calcAccel(focus_steps, (float)shutter_spd);
    }
    rear_motor.setAcceleration(orientation ? focus_RPM : zoom_RPM);
    front_motor.setAcceleration(orientation ? zoom_RPM : focus_RPM);
  } else if (shutter_spd == 0) {
    rear_motor.setAcceleration(CALI_ACCEL);
    front_motor.setAcceleration(CALI_ACCEL);
  }

  rear_motor.moveTo(rear_position * MS_STEP);
  front_motor.moveTo(front_position * MS_STEP);

  while (rear_motor.distanceToGo() != 0 || front_motor.distanceToGo() != 0) {
    rear_motor.run();
    front_motor.run();
    if (shutter_speed != 0) {
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
