// ******** MOTOR CONTROL Functions **********

/* Calc RPM 
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

long toMS(float seconds) {
  return seconds * 1000000;
}

/*
 * Requirement for TMC driver
 * Nema 17 200 steps = 1600 steps
 * 1/8 microstepping
 */
long convertSteps(int steps) {
  
}

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

/* @Override AccelStepper methods
 *  type = 0 [FOCUS], 1 [ZOOM]
 *  orientation 0 [Focus Front, Zoom Rear]
 *  orientation 1 [Zoom Front, Focus Rear]
 *  // Shall be discontinued
 */
 /*
void motor_move(int type, long steps) {
  if (type) { // zoom
    orientation ? front_motor.move(-steps) : rear_motor.move(steps);
  } else {
    orientation ? rear_motor.move(-steps) : front_motor.move(steps);
  }
}
 
void motor_moveTo(int type, long pos) {
  if (type) { // zoom
    orientation ? front_motor.moveTo(-pos) : rear_motor.moveTo(pos);
  } else {
    orientation ? rear_motor.moveTo(-pos) : front_motor.moveTo(pos);
  }
}

void motor_runToNewPosition(int type, long pos) {
  if (type) { // zoom
    // if zoom in front
    orientation ? front_motor.runToNewPosition(-pos) : rear_motor.runToNewPosition(pos);
  } else {
    orientation ? rear_motor.runToNewPosition(-pos) : front_motor.runToNewPosition(pos);
  }
}

void motor_setSpeed(int type, int spd) {
  if (type) {
    orientation ? front_motor.setSpeed(spd) : rear_motor.setSpeed(spd);
  } else {
    orientation ? rear_motor.setSpeed(spd) : front_motor.setSpeed(spd);
  }
}

void motor_runSpeedToPosition(int type) {
  if (type) {
    orientation ? front_motor.runSpeedToPosition() : rear_motor.runSpeedToPosition();
  } else {
    orientation ? rear_motor.runSpeedToPosition() : front_motor.runSpeedToPosition();
  }
}

void motor_runToPosition(int type) {
  if (type) {
    orientation ? front_motor.runToPosition() : rear_motor.runToPosition();
  } else {
    orientation ? rear_motor.runToPosition() : front_motor.runToPosition();
  }
}

void motor_run(int type) {
  if (type) {
    orientation ? front_motor.run() : rear_motor.run();
  } else {
    orientation ? rear_motor.run() : front_motor.run();
  }
}

long motor_currentPosition(int type) {
  if (type) {
    return orientation ? front_motor.currentPosition() : rear_motor.currentPosition();
  } else {
    return orientation ? rear_motor.currentPosition() : front_motor.currentPosition();
  }
}

void motor_setAcceleration(int type, long accel) {
  if (type) {
    orientation ? front_motor.setAcceleration(accel) : rear_motor.setAcceleration(accel);
  } else {
    orientation ? rear_motor.setAcceleration(accel) : front_motor.setAcceleration(accel);
  }
}

void motor_setCurrentPosition(int type, long pos) {
  if (type) {
    orientation ? front_motor.setCurrentPosition(-pos) : rear_motor.setCurrentPosition(pos);
  } else {
    orientation ? rear_motor.setCurrentPosition(-pos) : front_motor.setCurrentPosition(pos);
  }
}
*/
