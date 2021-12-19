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
