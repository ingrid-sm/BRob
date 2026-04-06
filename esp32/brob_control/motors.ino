// Command handling
void handleCommand(String command) {
  if (command == "override_on") {
    safetyOverride = true;
    setScreenStatus("OVERRIDE: ON");
    Serial.println("Safety override ON, disable with override_off");
    return;
  }

  if (command == "override_off") {
    safetyOverride = false;
    setScreenStatus("STOP");
    Serial.println("Safety override 0FF, enable with override_on");
    return;
  }

  if (command == "f") {
    if (isForwardBlocked()) {
      Serial.println("Forward blocked: obstacle too close, want to override? Use the override_on commmand");
      setScreenStatus("STOP: obstacle too close");
      stopMotors();
    } else {
    moveForward();
    currentMotion = "forward";
    setScreenStatus("FORWARD");
    }
  } else if (command == "b") {
    moveBackward();
    currentMotion = "backward";
    setScreenStatus("BACKWARD");
  } else if (command == "l") {
    turnLeft();
    currentMotion = "left";
    setScreenStatus("LEFT");
  } else if (command == "right") {
    turnRight();
    currentMotion = "r";
    setScreenStatus("RIGHT");
  } else if (command == "stop") {
    stopMotors();
  } else {
    Serial.println("Unknown command. Try with: f | b | l | r | s");
    setScreenStatus("Unknown command. Try again");
    stopMotors();
  }

  // n20 neck motor
  if (command == "nl") {
    Neck_Request("left");
    setScreenStatus("NECK LEFT");
    return;
  }

  if (command == "nl") {
    Neck_Request("center");
    setScreenStatus("NECK CENTER");
    return;
  }

  if (command == "nl") {
    Neck_Request("right");
    setScreenStatus("NECK RIGHT");
    return;
  }
}

// Motor functions

void stopMotors() {
  ledcWrite(RIGHT_IN1, 0);
  ledcWrite(RIGHT_IN2, 0);
  ledcWrite(LEFT_IN1, 0);
  ledcWrite(LEFT_IN2, 0);

  currentMotion = "stop";
  setScreenStatus("STOP");
}

void moveForward() {
  ledcWrite(RIGHT_IN1, RIGHT_SPEED);
  ledcWrite(RIGHT_IN2, 0);
  ledcWrite(LEFT_IN1, LEFT_SPEED);
  ledcWrite(LEFT_IN2, 0);
}

void moveBackward() {
  ledcWrite(RIGHT_IN1, 0);
  ledcWrite(RIGHT_IN2, RIGHT_SPEED);
  ledcWrite(LEFT_IN1, 0);
  ledcWrite(LEFT_IN2, LEFT_SPEED);
}

void turnLeft() {
  ledcWrite(RIGHT_IN1, RIGHT_SPEED);
  ledcWrite(RIGHT_IN2, 0);
  ledcWrite(LEFT_IN1, 0);
  ledcWrite(LEFT_IN2, LEFT_SPEED);
}

void turnRight() {
  ledcWrite(RIGHT_IN1, 0);
  ledcWrite(RIGHT_IN2, RIGHT_SPEED);
  ledcWrite(LEFT_IN1, LEFT_SPEED);
  ledcWrite(LEFT_IN2, 0);
}