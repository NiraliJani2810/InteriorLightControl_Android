# Interior Light Control – Android Application

This Android application connects to a custom-built interior lighting system powered by a CC3200 LaunchPad. It allows real-time monitoring and manual control of a relay based on data from a light sensor and rotary dial. The system supports both auto and manual modes, with override capability, and provides sensor status via a local WiFi Access Point.

---

## Features

- Connects to the CC3200 via WiFi Access Point (SSID: `LightControlAP`)
- Displays real-time sensor data: mode, relay state, light intensity, rotary value
- Manual relay control and mode switching
- Responsive theme switching (Light/Dark mode)
- Connection status feedback
- Light condition visualization with color-coded card

---

## Prerequisites

- Android Studio (Electric Eel or newer)
- Physical Android device or emulator with WiFi capabilities
- Access to the CC3200 LaunchPad with the corresponding `.ino` firmware running

---

## Installation

### 1. CC3200 Hardware Setup

- Flash the `InteriorLightControl.ino` sketch to your CC3200 LaunchPad using Energia
- Ensure the AP starts with SSID: `LightControlAP`, password: `12345678`
- Relay, light sensor, and rotary should be wired as described in the hardware README

### 2. Android Studio Project Setup

- Open this Android project in Android Studio
- Ensure the following permissions are declared in `AndroidManifest.xml`:
  ```xml
  <uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" />
  ```
- Run the app on a device/emulator
- Connect the device to the `LightControlAP` WiFi network created by the CC3200

---

## Usage

1. Launch the app
2. Ensure connection to `LightControlAP` (WiFi icon will update)
3. View current mode, relay state, light value, and rotary position
4. Tap the switch or buttons to:
   - Manually turn the relay ON/OFF (activates override)
   - Switch between auto and manual modes
5. View light condition levels as:
   - Dark, Dim, Normal, Bright, Very Bright

---

## API Endpoints Accessed by the App

| Endpoint                  | Purpose                           |
|---------------------------|-----------------------------------|
| `/sensor`                 | Get full sensor data              |
| `/relay?state=on|off`     | Manually override relay           |
| `/mode?state=manual|auto` | Switch between control modes      |

---

## Project Structure

- `MainActivity.kt`: Launches the UI and manages top bar, theme toggle, and layout
- `LightSensorScreen()`: Fetches sensor data, displays real-time states, and handles all controls
- `res/drawable`: Contains icons for WiFi, sun/moon (theme), and the car image
- `res/layout` and `res/values`: Contains UI definitions, colors, themes, and spacing

---

## License

This project is intended for educational use in IoT and Android integration. All code and documentation are open-source and may be modified for personal or academic purposes.
=======
