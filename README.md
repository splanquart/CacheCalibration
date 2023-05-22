# Cache Calibration Controller

The Cache Calibration Controller is a hardware and software project for 
astronomers. It provides an interface for controlling relay devices from 
popular astronomy software such as N.I.N.A. 

## Features

- Relay control via Alpaca 1.0 protocol
- WiFi connectivity with ESP8266 microcontroller
- User-friendly web interface for device setup and control
- Persistent relay names stored in EEPROM

## Hardware Requirements

- ESP8266 Microcontroller
- Relay module(s)

## Software Setup

1. Install the Arduino IDE.
2. Open the `cachecalibration.ino` file in the Arduino IDE.
3. Install the required libraries via the Arduino IDE's Library Manager:
    - ESP8266WiFi
    - ESP8266WebServer
    - ESP8266HTTPUpdateServer
    - EEPROM
4. Upload the sketch to your ESP8266 device.

## Usage

Once the software is uploaded to your device, you can access the web 
interface via a browser on any device connected to the same network. 
From there, you can control individual relays, change relay names, and 
set up WiFi credentials.

## Contributions

This project is open-source and we welcome contributions. Feel free to 
fork the project and submit pull requests. Please ensure any added 
code follows the existing style in the project. 

## License

This project is licensed under the [MIT License](LICENSE).

