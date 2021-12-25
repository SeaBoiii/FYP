// *********** ZOOM Motor Functions ***************

void zoom_move(int orientation, int steps) {
  // zoom at back
  if (orientation == 0) {
    rear_motor.move(steps);    
  }
  if (orientation == 1) {
    front_motor.move(-steps);
  }
}

void zoom_startMove(int orientation, long steps, long time=0) {
  if (orientation == 0) {
    rear_motor.startMove(steps, time);    
  }
  if (orientation == 1) {
    front_motor.startMove(-steps, time);
  }
}

unsigned zoom_nextAction(int orientation) {
  unsigned wait_time;
  if (orientation == 0) {
    wait_time = rear_motor.nextAction();
  }
  if (orientation == 1) {
    wait_time = front_motor.nextAction();
  }
  return wait_time;
}

void zoom_setRPM(int orientation, int rpm) {
  if (orientation == 0) {
    rear_motor.setRPM(rpm);
  }
  if (orientation == 1) {
    front_motor.setRPM(rpm);    
  }
}

void zoom_setSpeedProfile(int orientation, bool linear, short accel=1000, short decel=1000) {
  if (orientation == 0) {
    if (linear) {
      rear_motor.setSpeedProfile(rear_motor.LINEAR_SPEED, accel, decel);
    } else {
      rear_motor.setSpeedProfile(rear_motor.CONSTANT_SPEED);
    }
  }
  if (orientation == 1) {
    if (linear) {
      front_motor.setSpeedProfile(front_motor.LINEAR_SPEED, accel, decel);      
    } else {
      front_motor.setSpeedProfile(front_motor.CONSTANT_SPEED);
    }
  }
}