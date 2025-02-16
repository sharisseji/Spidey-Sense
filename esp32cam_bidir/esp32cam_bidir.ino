#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>

const char* WIFI_SSID = "Sharisse";
const char* WIFI_PASS = "panictime";

WebServer server(80);

// Define GPIOs for communication with Arduino Uno
#define PERSON_PIN 12  // Can be any available GPIO
#define VEHICLE_PIN 13   // Another available GPIO
#define ANIMAL_PIN 15
#define OBST_PIN 14

static auto loRes = esp32cam::Resolution::find(320, 240);
static auto midRes = esp32cam::Resolution::find(350, 530);
static auto hiRes = esp32cam::Resolution::find(800, 600);

void serveJpg() {
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.printf("CAPTURE OK %dx%d %dbn", frame->getWidth(), frame->getHeight(),
                static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void handleJpgLo() {
  if (!esp32cam::Camera.changeResolution(loRes)) {
    Serial.println("SET-LO-RES FAIL");
  }
  serveJpg();
}

void handleJpgHi() {
  if (!esp32cam::Camera.changeResolution(hiRes)) {
    Serial.println("SET-HI-RES FAIL");
  }
  serveJpg();
}

void handleJpgMid() {
  if (!esp32cam::Camera.changeResolution(midRes)) {
    Serial.println("SET-MID-RES FAIL");
  }
  serveJpg();
}

// Handle AI response from laptop and update GPIO pins
void handleAIResponse() {
  String message = server.arg("data");  // Get AI result from HTTP request
  Serial.println("Received from laptop: " + message);

  if (message == "PERSON") {
    digitalWrite(PERSON_PIN, HIGH);
    digitalWrite(VEHICLE_PIN, LOW);
    digitalWrite(ANIMAL_PIN, LOW);
    digitalWrite(OBST_PIN, LOW);
  } else if (message == "VEHICLE") {
    digitalWrite(PERSON_PIN, LOW);
    digitalWrite(VEHICLE_PIN, HIGH);
    digitalWrite(ANIMAL_PIN, LOW);
    digitalWrite(OBST_PIN, LOW);
  } else if (message == "ANIMAL") {
    digitalWrite(PERSON_PIN, LOW);
    digitalWrite(VEHICLE_PIN, LOW);
    digitalWrite(ANIMAL_PIN, HIGH);
    digitalWrite(OBST_PIN, LOW);
  } 
  else if (message == "OBSTRUCTION") {
    digitalWrite(PERSON_PIN, LOW);
    digitalWrite(VEHICLE_PIN, LOW);
    digitalWrite(ANIMAL_PIN, LOW);
    digitalWrite(OBST_PIN, HIGH);
  } else {
    digitalWrite(PERSON_PIN, LOW);
    digitalWrite(VEHICLE_PIN, LOW);
    digitalWrite(ANIMAL_PIN, LOW);
    digitalWrite(OBST_PIN, LOW);
  }

  server.send(200, "text/plain", "Data Received: " + message);
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(PERSON_PIN, OUTPUT);
  pinMode(VEHICLE_PIN, OUTPUT);
  pinMode(ANIMAL_PIN, OUTPUT);
  pinMode(OBST_PIN, OUTPUT);
  digitalWrite(PERSON_PIN, LOW);
  digitalWrite(VEHICLE_PIN, LOW);
  digitalWrite(ANIMAL_PIN, LOW);
  digitalWrite(OBST_PIN, LOW);

  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(hiRes);
    cfg.setBufferCount(2);
    cfg.setJpeg(80);

    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
  }

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println();
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.println("  /cam-lo.jpg");
  Serial.println("  /cam-hi.jpg");
  Serial.println("  /cam-mid.jpg");

  server.on("/cam-lo.jpg", handleJpgLo);
  server.on("/cam-hi.jpg", handleJpgHi);
  server.on("/cam-mid.jpg", handleJpgMid);

  // New: Endpoint to receive AI identification data
  server.on("/send", HTTP_GET, handleAIResponse);

  server.begin();
}

void loop() {
  server.handleClient();
}
