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

## Wifi configuration

On first boot, the device will launch a WiFi Access Point named 'Cache_Calibration'.
Connect to this network using no password.
Once connected, open a web browser and navigate to 'http://192.168.4.1'.
Here you will find a WiFi configuration interface. Select your WiFi network from the list
of available networks, enter your network password, then click 'Save'.
The device will disconnect from the Access Point and attempt to connect to your WiFi network
using the details you provided. Once connected to your WiFi network, the device will be accessible
at the IP address assigned to it by your router.

The WiFi Access Point named 'Cache_Calibration' is only open if the wifi configuration is invalid
or not available. But if you want to change your wifi configuration 
you can go to 'http://<the_ip_of_cache_calibration>/reset' and wait 5 second.
After that 'Cache_Calibration' is available like first boot.

## Contributions

This project is open-source and we welcome contributions. Feel free to 
fork the project and submit pull requests. Please ensure any added 
code follows the existing style in the project. 

## License

This project is licensed under the [MIT License](LICENSE).

