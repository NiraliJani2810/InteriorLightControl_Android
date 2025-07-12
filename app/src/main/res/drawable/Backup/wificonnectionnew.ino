#include <WiFi.h>

const int lightSensorPin = A1;
const int rotaryPin = A2;
const int relayPin = 4;

bool manualMode = false;
bool relayState = false;

char ssid[] = "NIRALIJANIPROJECT";
char password[] = "12345678";

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  Serial.println("🔧 System Booting...");

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  Serial.println("✅ Relay initialized");

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("❌ WiFi shield not detected. Halting.");
    while (true); // halt
  } else {
    Serial.println("✅ WiFi hardware ready");
  }

  // Start WiFi AP
  Serial.print("📶 Starting WiFi Access Point: ");
  Serial.println(ssid);

  WiFi.beginNetwork(ssid, password);

  unsigned long startTime = millis();
  while (WiFi.localIP() == INADDR_NONE) {
    delay(1000);
    Serial.print("⏳ Waiting for IP... ");
    Serial.println(WiFi.localIP());
    if (millis() - startTime > 15000) {
      Serial.println("⚠️ Retrying AP startup...");
      WiFi.beginNetwork(ssid, password);
      startTime = millis();
    }
  }

  Serial.print("✅ AP started at IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("🌐 HTTP Server Started");
}

void loop() {
  int lightVal = analogRead(lightSensorPin);
  int rotaryVal = analogRead(rotaryPin);

  // Mode selection
  if (rotaryVal > 100) manualMode = true;
  else if (rotaryVal < 50) manualMode = false;

  // Relay control
  if (manualMode) {
    relayState = (rotaryVal > 1500);
  } else {
    relayState = (lightVal < 1800);
  }

  digitalWrite(relayPin, relayState ? HIGH : LOW);

  // Debug
  Serial.print("📝 Mode: ");
  Serial.print(manualMode ? "Manual" : "Auto");
  Serial.print(" | Relay: ");
  Serial.print(relayState ? "ON" : "OFF");
  Serial.print(" | Light: ");
  Serial.print(lightVal);
  Serial.print(" | Rotary: ");
  Serial.println(rotaryVal);

  // Handle HTTP requests
  WiFiClient client = server.available();
  if (client) {
    Serial.println("🔗 Client connected");
    String req = client.readStringUntil('\r');
    client.flush();

    if (req.indexOf("/sensor") != -1) {
      String json = "{";
      json += "\"mode\": \"" + String(manualMode ? "manual" : "auto") + "\", ";
      json += "\"relay\": \"" + String(relayState ? "on" : "off") + "\", ";
      json += "\"light\": " + String(lightVal) + ", ";
      json += "\"rotary\": " + String(rotaryVal);
      json += "}";

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Connection: close");
      client.println();
      client.println(json);

      Serial.println("✅ Sent /sensor JSON");
    } else {
      client.println("HTTP/1.1 404 Not Found");
      client.println("Connection: close");
      client.println();
    }

    client.stop();
    Serial.println("🔌 Client disconnected");
  }

  delay(500);
}
