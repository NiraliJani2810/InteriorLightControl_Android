#include <WiFi.h>  // Include the WiFi library for networking functionality

// Hardware pin configuration
const int lightSensorPin = A1;   // Analog input pin for ambient light sensor
const int rotaryPin = A2;        // Analog input pin for rotary/potentiometer
const int relayPin = 4;          // Digital output pin to control relay

// State control variables
bool manualMode = false;         // Tracks whether manual mode is active
bool relayState = false;         // Current ON/OFF state of the relay
bool overrideActive = false;     // Indicates if override mode is active

// WiFi Access Point credentials
char ssid[] = "LightControlAP";     // WiFi SSID (network name)
char password[] = "12345678";       // WiFi password

WiFiServer server(80);              // Create an HTTP server on port 80

void setup() {
  Serial.begin(9600);                      // Initialize serial communication for debugging
  pinMode(relayPin, OUTPUT);               // Set the relay pin as output
  digitalWrite(relayPin, LOW);             // Ensure relay starts in OFF state

  WiFi.beginNetwork(ssid, password);       // Start WiFi in Access Point mode
  unsigned long startTime = millis();      // Timestamp for timeout handling

  // Wait for AP IP assignment (timeout after 10 seconds)
  while (WiFi.localIP() == IPAddress(0, 0, 0, 0) && millis() - startTime < 10000) {
    delay(1000);
  }

  // Start HTTP server if Access Point successfully initialized
  if (WiFi.localIP() != IPAddress(0, 0, 0, 0)) {
    Serial.print("AP started! IP Address: ");
    Serial.println(WiFi.localIP());
    server.begin();
  } else {
    Serial.println("Failed to start Access Point.");
  }
}

void loop() {
  // Read sensor values
  int lightVal = analogRead(lightSensorPin);     // Read light intensity
  int rotaryVal = analogRead(rotaryPin);         // Read rotary sensor value

  // Logic control based on mode and override status
  if (!overrideActive) {
    if (rotaryVal > 100) manualMode = true;      // Set manual mode if rotary is moved above threshold
    else if (rotaryVal < 50) manualMode = false; // Set auto mode if rotary is moved below threshold

    if (manualMode) {
      relayState = (rotaryVal > 1500);           // Manual mode: ON if rotary value is high
    } else {
      relayState = (lightVal < 1800);            // Auto mode: ON if ambient light is low
    }
  }

  digitalWrite(relayPin, relayState ? HIGH : LOW); // Apply relay ON/OFF state

  // Check for incoming HTTP client request
  WiFiClient client = server.available();
  if (client) {
    String req = client.readStringUntil('\r');   // Read HTTP request line
    client.flush();                              // Clear remaining input buffer

    // Handle sensor status request
    if (req.indexOf("/sensor") != -1) {
      String json = "{";
      json += "\"mode\": \"" + String(manualMode ? "manual" : "auto") + "\", ";
      json += "\"relay\": \"" + String(relayState ? "on" : "off") + "\", ";
      json += "\"light\": " + String(lightVal) + ", ";
      json += "\"rotary\": " + String(rotaryVal) + ", ";
      json += "\"override\": \"" + String(overrideActive ? "yes" : "no") + "\"";
      json += "}";

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Connection: close");
      client.println();
      client.println(json);
    }

    // Handle manual relay control (override ON)
    else if (req.indexOf("/relay?state=") != -1) {
      if (req.indexOf("on") != -1) relayState = true;
      else if (req.indexOf("off") != -1) relayState = false;
      overrideActive = true;                              // Activate override mode
      digitalWrite(relayPin, relayState ? HIGH : LOW);    // Update relay state
      Serial.println("Relay override set");
      client.println("HTTP/1.1 200 OK");
      client.println();
    }

    // Handle override exit (return to auto/manual logic)
    else if (req.indexOf("/override?state=exit") != -1) {
      overrideActive = false;                             // Disable override mode
      Serial.println("Override deactivated");
      client.println("HTTP/1.1 200 OK");
      client.println();
    }

    // Handle mode change via API
    else if (req.indexOf("/mode?state=") != -1) {
      if (req.indexOf("manual") != -1) manualMode = true;
      else if (req.indexOf("auto") != -1) manualMode = false;
      client.println("HTTP/1.1 200 OK");
      client.println();
    }

    // Serve a simple HTML page on root request
    else if (req.indexOf("GET / ") != -1 || req.startsWith("GET / HTTP")) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("Connection: close");
      client.println();
      client.println("<html><body><h2>Interior Light Control System</h2><p>System Online.</p></body></html>");
    }

    // Handle unknown routes
    else {
      client.println("HTTP/1.1 404 Not Found");
      client.println();
    }

    client.stop(); // End client connection
  }

  delay(500); // Delay before next loop iteration to reduce server load
}
