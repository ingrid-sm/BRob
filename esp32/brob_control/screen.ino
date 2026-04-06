U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI u8g2(
  U8G2_R0,
  OLED_CS,
  OLED_DC,
  OLED_RES
);

void initScreen() {
  SPI.begin(47, -1, 48, OLED_CS);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.drawStr(0, 12, "BRob starting...");
  u8g2.sendBuffer();
}

void setScreenStatus(String newStatus) {
  if (screenStatus != newStatus) {
    screenStatus = newStatus;
    screenNeedsUpdate = true;
  }
}

void updateScreen() {
  static unsigned long lastDraw = 0;

  if (!screenNeedsUpdate && millis() - lastDraw < 200) {
    return;
  }

  lastDraw = millis();
  screenNeedsUpdate = false;

  char distanceText[24];

  if (lastDistanceCm < 0) {
    sprintf(distanceText, "Dist: no echo");
  } else {
    sprintf(distanceText, "Dist: %.1f cm", lastDistanceCm);
  }

  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.drawStr(0, 12, "BRob");

  u8g2.setFont(u8g2_font_10x20_tf);
  u8g2.drawStr(0, 36, screenStatus.c_str());

  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.drawStr(0, 52, distanceText);

  if (safetyOverride) {
    u8g2.drawStr(0, 64, "OVERRIDE: ON");
  } else {
    u8g2.drawStr(0, 64, "OVERRIDE: OFF");
  }

  u8g2.sendBuffer();
}