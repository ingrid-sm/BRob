void Neck_Init() {
  pinMode(NECK_IN1, OUTPUT);
  pinMode(NECK_IN2, OUTPUT);

  ledcAttach(NECK_IN1, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(NECK_IN2, PWM_FREQ, PWM_RESOLUTION);

  Neck_Stop();

  neckPrefs.begin("neck", false);
  Neck_LoadSavedPosition();
}

void Neck_LoadSavedPosition() {
  String saved = neckPrefs.getString("pos", "center");

  if (saved == "left" || saved == "center" || saved == "right") {
    neckPosition = saved;
    neckTarget = saved;
  } else {
    neckPosition = "center";
    neckTarget = "center";
  }
}

void Neck_SavePosition() {
  neckPrefs.putString("pos", neckPosition);
}

void Neck_Stop() {
  ledcWrite(NECK_IN1, 0);
  ledcWrite(NECK_IN2, 0);
  neckBusy = false;
}

void Neck_MoveLeft() {
  ledcWrite(NECK_IN1, 0);
  ledcWrite(NECK_IN2, NECK_SPEED);
}

void Neck_MoveRight() {
  ledcWrite(NECK_IN1, NECK_SPEED);
  ledcWrite(NECK_IN2, 0);
}

void Neck_Request(String targetPos) {
  if (neckBusy) {
    return;
  }

  if (targetPos != "left" && targetPos != "center" && targetPos != "right") {
    return;
  }

  if (targetPos == neckPosition) {
    neckTarget = targetPos;
    return;
  }

  neckTarget = targetPos;

  if (neckPosition == "left" && neckTarget == "center") {
    Neck_MoveRight();
    neckMoveEndTime = millis() + NECK_STEP_MS;
    neckBusy = true;
  } else if (neckPosition == "center" && neckTarget == "right") {
    Neck_MoveRight();
    neckMoveEndTime = millis() + NECK_STEP_MS;
    neckBusy = true;
  } else if (neckPosition == "right" && neckTarget == "center") {
    Neck_MoveLeft();
    neckMoveEndTime = millis() + NECK_STEP_MS;
    neckBusy = true;
  } else if (neckPosition == "center" && neckTarget == "left") {
    Neck_MoveLeft();
    neckMoveEndTime = millis() + NECK_STEP_MS;
    neckBusy = true;
  } else if (neckPosition == "left" && neckTarget == "right") {
    Neck_MoveRight();
    neckMoveEndTime = millis() + (2 * NECK_STEP_MS);
    neckBusy = true;
  } else if (neckPosition == "right" && neckTarget == "left") {
    Neck_MoveLeft();
    neckMoveEndTime = millis() + (2 * NECK_STEP_MS);
    neckBusy = true;
  }
}

void Neck_Update() {
  if (!neckBusy) {
    return;
  }

  if ((long)(millis() - neckMoveEndTime) >= 0) {
    Neck_Stop();
    neckPosition = neckTarget;
    Neck_SavePosition();
  }
}