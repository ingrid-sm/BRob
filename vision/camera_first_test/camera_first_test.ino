#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include "wifi_secrets.h"

// This sketch is for the common ESP32-CAM board that uses the AI Thinker pin layout.
// Your product photos match that layout, and your existing example uses the same one.

const int PWDN_GPIO_NUM = 32;
const int RESET_GPIO_NUM = -1;
const int XCLK_GPIO_NUM = 0;
const int SIOD_GPIO_NUM = 26;
const int SIOC_GPIO_NUM = 27;

const int Y9_GPIO_NUM = 35;
const int Y8_GPIO_NUM = 34;
const int Y7_GPIO_NUM = 39;
const int Y6_GPIO_NUM = 36;
const int Y5_GPIO_NUM = 21;
const int Y4_GPIO_NUM = 19;
const int Y3_GPIO_NUM = 18;
const int Y2_GPIO_NUM = 5;
const int VSYNC_GPIO_NUM = 25;
const int HREF_GPIO_NUM = 23;
const int PCLK_GPIO_NUM = 22;
const int FLASH_GPIO_NUM = 4;

const framesize_t FAST_FRAME_SIZE = FRAMESIZE_QVGA;
const framesize_t QUALITY_FRAME_SIZE = FRAMESIZE_VGA;

const int FAST_JPEG_QUALITY = 16;
const int QUALITY_JPEG_QUALITY = 10;

WebServer server(80);

bool startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  // Use a conservative clock to make the first camera test more stable.
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 14;
  config.fb_count = 1;
  config.fb_location = psramFound() ? CAMERA_FB_IN_PSRAM : CAMERA_FB_IN_DRAM;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

  esp_err_t result = esp_camera_init(&config);

  if (result != ESP_OK) {
    Serial.print("Camera start failed. Error code: 0x");
    Serial.println(result, HEX);
    return false;
  }

  sensor_t *sensor = esp_camera_sensor_get();
  if (sensor != nullptr) {
    sensor->set_framesize(sensor, FAST_FRAME_SIZE);
    sensor->set_quality(sensor, FAST_JPEG_QUALITY);
    sensor->set_brightness(sensor, 1);
    sensor->set_saturation(sensor, 0);
  }

  return true;
}

void setupFlash() {
  pinMode(FLASH_GPIO_NUM, OUTPUT);
  digitalWrite(FLASH_GPIO_NUM, LOW);
}

void setFlash(bool enabled) {
  digitalWrite(FLASH_GPIO_NUM, enabled ? HIGH : LOW);
}

void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected");
  Serial.print("Camera page: http://");
  Serial.println(WiFi.localIP());
}

String buildHomePage() {
  return R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>BRob Camera Test</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 24px;
      background: #f4f6f8;
      color: #1f2933;
    }
    button {
      font-size: 16px;
      padding: 10px 14px;
      margin: 0 10px 10px 0;
      cursor: pointer;
    }
    .note {
      margin-top: 12px;
      color: #52606d;
    }
    img {
      max-width: 100%;
      border: 2px solid #cbd2d9;
      background: white;
    }
  </style>
</head>
<body>
  <h1>BRob Camera Test</h1>
  <p>If you can see a photo below, the camera is working.</p>
  <button onclick="takePhoto('fast', 0)">Fast photo</button>
  <button onclick="takePhoto('quality', 0)">Better quality</button>
  <button onclick="takePhoto('quality', 1)">Better quality + flash</button>
  <p><a href="/photo?mode=quality" target="_blank">Open only the JPEG photo</a></p>
  <p class="note">Fast mode is quicker. Quality mode is sharper but slower. Flash helps in a dark room.</p>
  <img id="camera-photo" src="/photo?mode=fast&time=0" alt="Camera photo">

  <script>
    function takePhoto(mode, flash) {
      document.getElementById("camera-photo").src =
        "/photo?mode=" + mode + "&flash=" + flash + "&time=" + Date.now();
    }
  </script>
</body>
</html>
)HTML";
}

void handleHomePage() {
  server.send(200, "text/html", buildHomePage());
}

camera_fb_t *capturePhoto(String mode, bool useFlash) {
  sensor_t *sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    return nullptr;
  }

  if (mode == "quality") {
    sensor->set_framesize(sensor, QUALITY_FRAME_SIZE);
    sensor->set_quality(sensor, QUALITY_JPEG_QUALITY);
  } else {
    sensor->set_framesize(sensor, FAST_FRAME_SIZE);
    sensor->set_quality(sensor, FAST_JPEG_QUALITY);
  }

  delay(120);

  if (useFlash) {
    setFlash(true);
    delay(120);
  }

  camera_fb_t *frame = esp_camera_fb_get();

  if (useFlash) {
    setFlash(false);
  }

  return frame;
}

void handlePhoto() {
  String mode = server.hasArg("mode") ? server.arg("mode") : "fast";
  bool useFlash = server.hasArg("flash") && server.arg("flash") == "1";

  Serial.print("Photo requested. mode=");
  Serial.print(mode);
  Serial.print(" flash=");
  Serial.println(useFlash ? "on" : "off");

  camera_fb_t *frame = capturePhoto(mode, useFlash);

  if (frame == nullptr) {
    Serial.println("Photo capture failed");
    server.send(500, "text/plain", "Could not capture a photo.");
    return;
  }

  Serial.print("Photo captured. Bytes: ");
  Serial.println(frame->len);

  WiFiClient client = server.client();
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: image/jpeg");
  client.print("Content-Length: ");
  client.println(frame->len);
  client.println("Cache-Control: no-store");
  client.println("Connection: close");
  client.println();

  size_t totalWritten = 0;
  while (totalWritten < frame->len) {
    size_t written = client.write(frame->buf + totalWritten, frame->len - totalWritten);
    if (written == 0) {
      Serial.println("Photo send failed before all bytes were written");
      break;
    }
    totalWritten += written;
  }

  Serial.print("Photo bytes sent: ");
  Serial.println(totalWritten);

  client.flush();
  delay(20);
  client.stop();

  esp_camera_fb_return(frame);
}

void setupServer() {
  server.on("/", handleHomePage);
  server.on("/photo", handlePhoto);
  server.begin();
  Serial.println("Web server started");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("Starting ESP32-CAM test...");

  if (!startCamera()) {
    Serial.println("Stop here and fix the camera before moving on.");
    return;
  }

  setupFlash();
  connectToWiFi();
  setupServer();
}

void loop() {
  server.handleClient();
}
