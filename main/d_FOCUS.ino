// ********* FOCUS Functions ***********

void focus_move(int orientation, int steps) {
  // focus in front
  if (orientation == 0) {
    front_motor.move(steps);
  } 
  // focus at back
  if (orientation == 1) {
    rear_motor.move(-steps);
  }
}

void focus_startMove(int orientation, long steps, long time=0) {
  // focus in front
  if (orientation == 0) {
    front_motor.startMove(steps, time);
  }
  // focus at back
  if (orientation == 1) {
    rear_motor.startMove(-steps, time);
  }  
}

unsigned focus_nextAction(int orientation) {
  unsigned wait_time;
  // focus in front
  if (orientation == 0) {
    wait_time = front_motor.nextAction();
  }
  // focus at back
  if (orientation == 1) {
    wait_time = rear_motor.nextAction();
  }    
  return wait_time;
}

void focus_setRPM(int orientation, int rpm) {
  if (orientation == 0) {
    front_motor.setRPM(rpm);
  }
  if (orientation == 1) {
    rear_motor.setRPM(rpm);
  }
}

void focus_setSpeedProfile(int orientation, bool linear, short accel=1000, short decel=1000) {
  if (orientation == 0) {
    if (linear) {
      front_motor.setSpeedProfile(front_motor.LINEAR_SPEED, accel, decel);
    } else {
      front_motor.setSpeedProfile(front_motor.CONSTANT_SPEED);
    }
  }
  if (orientation == 1) {
    if (linear) {
      rear_motor.setSpeedProfile(rear_motor.LINEAR_SPEED, accel, decel);      
    } else {
      rear_motor.setSpeedProfile(rear_motor.CONSTANT_SPEED);
    }
  }
}
