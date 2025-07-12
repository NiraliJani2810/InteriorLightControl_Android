# Interior Light Control System – CC3200 (Relay-Only Version)

This project implements an automatic and manual interior light control system using the CC3200 LaunchPad and Energia. The system toggles a relay based on ambient light intensity or user input from a rotary sensor. A built-in WiFi Access Point allows remote control and monitoring through standard HTTP requests.

## Features

- Relay control based on ambient light or manual setting
- WiFi Access Point functionality for mobile or PC access
- HTTP server with multiple API endpoints
- Real-time sensor monitoring and control
- Override mode for manual relay switching

## Hardware Requirements

- CC3200 LaunchPad XL
- Light Sensor connected to analog pin A1
- Rotary Sensor connected to analog pin A2
- Relay Module connected to digital pin 4
- USB Cable for power and programming

## Wiring

| Component      | CC3200 Pin |
|----------------|-------------|
| Light Sensor   | A1          |
| Rotary Sensor  | A2          |
| Relay Module   | GPIO4       |

## Software Setup

1. Install Energia IDE (version 1.8.7E21 recommended)
2. Install CC3200 board support from the board manager
3. Load the `.ino` file in Energia
4. Select the appropriate board and COM port
5. Upload the sketch to the CC3200

## How It Works

The system reads ambient light and rotary sensor values to determine whether to activate the relay. In auto mode, the relay is activated when the light value falls below a threshold. In manual mode, the relay state is determined by the rotary sensor value. Manual override is also available through HTTP commands.

## WiFi Access Point

- SSID: LightControlAP
- Password: 12345678

After connecting a device to the AP, access the system at `http://192.168.1.1/`.

## API Endpoints

| Endpoint                      | Function                               |
|------------------------------|----------------------------------------|
| `/sensor`                    | Returns current system status in JSON  |
| `/relay?state=on`            | Manually turn relay ON (override)      |
| `/relay?state=off`           | Manually turn relay OFF (override)     |
| `/override?state=exit`       | Exit override mode                     |
| `/mode?state=manual`         | Set system to manual mode              |
| `/mode?state=auto`           | Set system to auto mode                |

## System Behavior

- Auto Mode: Relay turns ON when ambient light is below threshold
- Manual Mode: Relay turns ON when rotary value is high
- Override Mode: Triggered when relay is manually controlled, cancelable via `/override?state=exit`

## License

This project is open-source and intended for academic and prototyping use.
