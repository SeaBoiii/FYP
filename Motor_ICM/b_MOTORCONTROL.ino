// ******** MOTOR CONTROL Functions **********

/* 
 *  Calc RPM 
 * - Based on calculations made
 *   100 RPM = 12.5 steps/s
 *   
 * Calculate RPM by ratio
 * May not be needed since the speed
 * in AccelMotor is in steps/s
 */
float calcRPM(int steps, float seconds) {
  float requiredSpeed = steps/seconds;
  return 100 * (requiredSpeed / (float)12.5);
}

float calcAccel(int steps, float seconds) {
  float mid_speed = calcRPM(steps, seconds);
  return (float)(mid_speed - 0) / (seconds/2 * seconds/2);
}

long toMS(float seconds) {
  return seconds * 1000000;
}

/*
 * Using Serial to Movement
 * - Not implemented fully
 * - Z/F R/L Steps
 * - e.g. ZR300 = zoom right 300 steps
 */
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
}

void splitStr(char* data) {
  char* parameter;
  parameter = strtok(data, " ,");
  while (parameter != NULL) {
    setMotor(parameter);
    parameter = strtok(NULL, " ,");
  }
}

void setMotor(char* data) {
  if ((data[0] == 'F') || (data[0] == 'f')) {
    int steps = strtol(data+1, NULL, 10);
    // set focus motor steps to steps
  }

  if ((data[0] == 'F') || (data[0] == 'f')) {
    int steps = strtol(data+1, NULL, 10);
    // set zoom motor steps to steps
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
  Serial.print("Steps to move: ");
  Serial.println(steps_to_move);
  Serial.print("Current Position: ");
  Serial.println(stepper->currentPosition());
  if (shutter_spd != 0) {
    stepper->setAcceleration(calcAccel(abs(steps_to_move), (float)shutter_spd));
  }
  // if +ve, move clockwise
  // else -ve, move anti-clockwise
  if (steps_to_move == 0) return;
  //stepper->moveTo((steps_to_move > 0) ? pos_desired * MS_STEP : -pos_desired * MS_STEP);
  stepper->moveTo(pos_desired * MS_STEP);
  
  //blocking statement
  //delay(shutter_spd);
  while (stepper->distanceToGo() != 0) {
    stepper->run();
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
