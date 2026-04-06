float readDistanceCm() {
  // Make sure TRIG starts LOW
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);

  // Send 10 microsecond pulse
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);

  // Read echo pulse length
  long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 30000);

  // If no echo comes back, return -1
  if (duration == 0) {
    return -1.0;
  }

  // Convert time to distance in cm
  float distanceCm = duration * 0.0343 / 2.0;
  return distanceCm;
}

void sendDistanceData() {
  static unsigned long lastSend = 0;

  if (millis() - lastSend >= 200) {
    lastSend = millis();

    float distanceCm = readDistanceCm();
    lastDistanceCm = distanceCm;
    screenNeedsUpdate = true;

    if (client && client.connected()) {
      client.print("DIST,");
      client.println(distanceCm);
    }
  }
}

void updateObstacleState() {
  float distanceCm = readDistanceCm();

  // Ignore invalid reading
  if (distanceCm < 0) {
    return;
  }

  if (distanceCm <= STOP_DISTANCE_CM) {
    obstacleTooClose = true;

    // When moving forward, stop when an obstacle is too close if override is off.
    if (!safetyOverride && currentMotion == "forward") {
      Serial.println("Stopping: obstacle detected, override with override_on");
      setScreenStatus("STOP: obstacle too close");
      stopMotors();
    }
      
  } else if (distanceCm > CLEAR_DISTANCE_CM) {
    obstacleTooClose = false;
  }
} 

bool isForwardBlocked() {
  if (safetyOverride) {
    return false;
   }
  return obstacleTooClose;
}