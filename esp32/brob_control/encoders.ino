void rightEncoderISR() {
  int bState = digitalRead(RIGHT_ENC_B);

  if (bState == HIGH) {
    rightTicks++;
  } else {
    rightTicks--;
  }
}

void leftEncoderISR() {
  int bState = digitalRead(LEFT_ENC_B);

  if (bState == HIGH) {
    leftTicks--;
  } else {
    leftTicks++;
  }
}

// Prints encoder ticks
void printEncoderTicks() {
  static unsigned long lastPrint = 0;

  if (millis() - lastPrint >= 500) {
    lastPrint = millis();

    long leftCopy = leftTicks;
    long rightCopy = rightTicks;

    Serial.print("Left ticks: ");
    Serial.print(leftCopy);
    Serial.print(" | Right ticks: ");
    Serial.println(rightCopy);
  }
}

// Sends encoder data to ROS
void sendEncoderData() {
  static unsigned long lastSend = 0;

  if (millis() - lastSend >= 100) {
    lastSend = millis();

    long leftCopy;
    long rightCopy; 

    noInterrupts();
    leftCopy = leftTicks;
    rightCopy = rightTicks;
    interrupts();

    if (client && client.connected()) {
      client.print("ENC,");
      client.print(leftCopy);
      client.print(",");
      client.println(rightCopy);
    }
  }
}