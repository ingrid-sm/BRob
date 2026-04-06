#include <WiFi.h>
#include "wifi_secrets.h"
#include <U8g2lib.h>
#include <SPI.h>
#include <Preferences.h>

// TCP motor control
const int SERVER_PORT = 9001;
WiFiServer server(SERVER_PORT);
WiFiClient client;

// Right track driver GPIO pin placement
const int RIGHT_IN1 = 4;
const int RIGHT_IN2 = 5;

// Left track driver GPIO pin placement
const int LEFT_IN1 = 15;
const int LEFT_IN2 = 16;

// Encoder GPIO pin placement
const int RIGHT_ENC_A = 17;
const int RIGHT_ENC_B = 18;

const int LEFT_ENC_A = 38;
const int LEFT_ENC_B = 39;

// n20 neck motor GPIO pin placement
const int NECK_IN1 = 12;
const int NECK_IN2 = 14;

// Encoder tick counters
volatile long rightTicks = 0;
volatile long leftTicks = 0;

const int PWM_FREQ = 1000;
const int PWM_RESOLUTION = 8;

const int ULTRASONIC_TRIG = 35;
const int ULTRASONIC_ECHO = 36;

int RIGHT_SPEED = 128;
int LEFT_SPEED = 115;

bool obstacleTooClose = false;
bool safetyOverride = false;

const float STOP_DISTANCE_CM = 8.0;
const float CLEAR_DISTANCE_CM = 10.0;

// Screen GPIO pin placement
const int OLED_CS = 10;
const int OLED_DC = 11;
const int OLED_RES = 21;

String screenStatus = "READY";
float lastDistanceCm = -1.0;
bool screenNeedsUpdate = true;

String currentMotion = "stop";

Preferences neckPrefs;

String neckPosition = "center";
String neckTarget = "center";

bool neckBusy = false;
unsigned long neckMoveEndTime = 0;

const int NECK_SPEED = 140;
const unsigned long NECK_STEP_MS = 260;

// Function declarations 
void stopMotors();
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();
void handleCommand(String command);

void rightEncoderISR();
void leftEncoderISR();
void printEncoderTicks();
void sendEncoderData();

float readDistanceCm();
void sendDistanceData();

void updateObstacleState();
bool isForwardBlocked();

void initScreen();
void updateScreen();
void setScreenStatus(String newStatus);

void Neck_Init();
void Neck_Update();
void Neck_Stop();
void Neck_MoveLeft();
void Neck_MoveRight();
void Neck_Request(String targetPos);
void Neck_LoadSavedPosition();
void Neck_SavePosition();


// setup()
void setup() {
  Serial.begin(115200);
  delay(500);
  delay(2000);
  Serial.println("SETUP STARTED");

  // Set motor pins as outputs
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);

  //Attach PWM channels to motor pins
  ledcAttach(RIGHT_IN1, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(RIGHT_IN2, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(LEFT_IN1, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(LEFT_IN2, PWM_FREQ, PWM_RESOLUTION);

  // Set encoder pins as inputs
  pinMode(RIGHT_ENC_A, INPUT_PULLUP);
  pinMode(RIGHT_ENC_B, INPUT_PULLUP);
  pinMode(LEFT_ENC_A, INPUT_PULLUP);
  pinMode(LEFT_ENC_B, INPUT_PULLUP);

  // Sensor pins
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);

  // Stop motors on boot for safety
  stopMotors();

  // Neck
  Neck_Init();

  // Attach encoder interrupts
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENC_A), rightEncoderISR, RISING);
  attachInterrupt(digitalPinToInterrupt(LEFT_ENC_A), leftEncoderISR, RISING);

  // Connect to wifi
  Serial.println("Connecting to Wifi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay (500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wifi connected");
  Serial.println("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // Start TCP server
  server.begin();
  Serial.println("TCP server started on port ");
  Serial.println(SERVER_PORT);


  initScreen();
  updateScreen();
}

// loop()
void loop() {
  // If no connected client, look for one
  if (!client || !client.connected()) {
    client = server.available();

    if (client) {
      Serial.println("Client connected");
    } else {
      // Keep motors stopped when no client connected
      stopMotors();
      delay(10);
      return;
    }
  }

  // Update ultrasonic safety state
  updateObstacleState();

  // If client sent data, read one line
  if (client.available()) {
    String command = client.readStringUntil('\n');
    command.trim(); 

    Serial.print("Received command: ");
    Serial.println(command);

    handleCommand(command);
  }

  // Send encoder data to ROS
  sendEncoderData();

  // Send distance data
  sendDistanceData();

  // Print encoder values for TESTING 
  // printEncoderTicks();

  updateScreen();

  // Neck
  Neck_Update();

  // If client disconnected, stop for safety
  if (!client.connected()) {
    Serial.println("Client disconnected");
    stopMotors();
  }
}

