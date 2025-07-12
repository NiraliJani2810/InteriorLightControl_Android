#include <WiFi.h>

const int lightSensorPin = A1;   // Light sensor (J6 A1)
const int rotaryPin = A2;        // Rotary angle sensor (J6 A2)
const int relayPin = 4;          // Relay on J14 D4

bool manualMode = false;
bool relayState = false;

char ssid[] = "LightControlAP";      // WiFi AP SSID
char password[] = "12345678";        // WiFi AP password

WiFiServer server(80);               // HTTP server

void setup() {
  Serial.begin(9600);
  Serial.println("System Booting...");

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  Serial.println("Relay initialized");

  // Check WiFi hardware
  if (WiFi.status() != WL_NO_SHIELD) {
    Serial.println("WiFi hardware ready.");
  } else {
    Serial.println("⚠️ WiFi hardware not detected!");
  }

  // Start WiFi AP
  Serial.println("Starting WiFi Access Point...");
  WiFi.beginNetwork(ssid, password);

  // Wait max 10s for IP assignment
  unsigned long startTime = millis();
  while (WiFi.localIP() == INADDR_NONE && millis() - startTime < 10000) {
    delay(1000);
    Serial.println("Waiting for AP IP...");
  }

  if (WiFi.localIP() == INADDR_NONE) {
    Serial.println("❌ Failed to start Access Point. Check board or WiFi library.");
  } else {
    Serial.print("✅ AP started! IP Address: ");
    Serial.println(WiFi.localIP());
    server.begin();
    Serial.println("HTTP Server Started");
  }
}

void loop() {
  int lightVal = analogRead(lightSensorPin);   // 0–4095
  int rotaryVal = analogRead(rotaryPin);       // 0–4095

  // Determine mode
  if (rotaryVal > 100) manualMode = true;
  else if (rotaryVal < 50) manualMode = false;

  // Control logic
  if (manualMode) {
    relayState = (rotaryVal > 1500);
  } else {
    relayState = (lightVal < 1800);
  }

  digitalWrite(relayPin, relayState ? HIGH : LOW);

  // Debug output
  Serial.print("Mode: ");
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
    Serial.println("Client connected");
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

      Serial.println("Sent /sensor JSON");
    } else {
      client.println("HTTP/1.1 404 Not Found");
      client.println("Connection: close");
      client.println();
    }

    client.stop();
    Serial.println("Client disconnected");
  }

  delay(500); // Polling interval
}
