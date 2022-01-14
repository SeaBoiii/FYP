// ******** MOTOR CONTROL Functions **********

/* 
 *  -- Calc RPM --
 *   
 * Calculate RPM by ratio
 * May not be needed since the speed
 * in AccelMotor is in steps/s
 */
float calcRPM(int steps, float seconds) {
  return (float)(steps * MS_STEP)/seconds;
}

/*
 * Calculating the acceleration
 * - Requires some finetuning
 */
float calcAccel(int steps, float seconds) {
  float max_speed = calcRPM(steps, seconds);
  return (float)max_speed / seconds;
}

/*
 * to MilliSeconds converter
 */
long toMS(float seconds) {
  return seconds * 1000;
}

/*
 * Using Character Array to Movement
 * - Not implemented fully
 * - Z/F Steps
 * - e.g. Z300 = zoom to pos 300
 */
 /*
void checkSerial() {
  if (Serial.available() > 0) {
    int i = 0;
    delay(100);
    int numStr = Serial.available();
    if (numStr > 49) numStr = 49;
    while (numStr--) {
      buffer[i++] = Serial.read();
    }
    buffer[i]='\0';
    splitStr(buffer);
  }
} */

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

void setMotor(char* data, char title[], int custom_itemcount) {
  bool goBack = false;
  if ((data[1] == 'G' || data[1] == 'g')) {
    goBack = true;
  }
  
  if ((data[0] == 'F') || (data[0] == 'f')) {
    int steps = strtol(data+2, NULL, 10);
    // set focus motor steps to steps
    //moveMotor(FOCUS, steps, motor_time/custom_itemcount);
    goDist(FOCUS, title, steps, SNOW, goBack, motor_time/custom_itemcount); 
  }

  if ((data[0] == 'Z') || (data[0] == 'z')) {
    int steps = strtol(data+2, NULL, 10);
    // set zoom motor steps to steps
    //moveMotor(ZOOM, steps, motor_time/custom_itemcount);
    goDist(ZOOM, title, steps, AZURE, goBack, motor_time/custom_itemcount);
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

/* @Override AccelStepper methods
 *  type = 0 [FOCUS], 1 [ZOOM]
 *  orientation 0 [Focus Front, Zoom Rear]
 *  orientation 1 [Zoom Front, Focus Rear]
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
 * !!BEWARE!! Using multistepper motor, the speed cannot be adjusted. Hence, using this method is definitely not possible.
 * However, there is still a possibility in doing this using a single loop and wait till both are done.
 */
void moveMultiMotor(float zoom_value, float focus_value, float shutter_spd=0) {
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
