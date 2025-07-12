const int lightSensorPin = A1;   // Light Sensor
const int rotaryPin = A2;        // Rotary angle sensor
const int relayPin = 4;          // Relay on J14 (Digital D4)

bool manualMode = false;
bool relayState = false;

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);  // Ensure relay is OFF at start
  Serial.println("Automatic Light Relay Control Started");
}

void loop() {
  int lightVal = analogRead(lightSensorPin);   // Range: 0 - 4095
  int rotaryVal = analogRead(rotaryPin);       // Range: 0 - 4095

  // Decide mode: auto or manual
  if (rotaryVal > 100) {
    manualMode = true;
  } else if (rotaryVal < 50) {
    manualMode = false;
  }

  if (manualMode) {
    // Manual mode: turn relay ON if knob turned enough
    if (rotaryVal > 1500) {  // Reduced from 2000 to 1500
    relayState = true;
    } else {
        relayState = false;
    }

    Serial.print("Mode: Manual | ");
  } else {
    // Auto mode: turn relay ON if light is below threshold
    if (lightVal < 1800) {
      relayState = true;
    } else {
      relayState = false;
    }
    Serial.print("Mode: Auto | ");
  }

  // Apply to relay
  digitalWrite(relayPin, relayState ? HIGH : LOW);

  // Serial output
  Serial.print("Relay: ");
  if (relayState) {
    Serial.print("Relay: ON  (Manual Override) | ");
  } else {
    Serial.print("Relay: OFF (Manual Override) | ");
  }

  Serial.print("| Light: ");
  Serial.print(lightVal);
  Serial.print(" | Rotary: ");
  Serial.println(rotaryVal);

  delay(500);
}
