#ifndef ALPACA_DEVICE_H
#define ALPACA_DEVICE_H

#include <ArduinoJson.h>
#include "HttpHandler.h"

class AlpacaDevice {
public:
  AlpacaDevice(HttpHandler& server, int deviceNumber);

  virtual void begin();
  DynamicJsonDocument getDeviceInfo();
  /*
   * Implement routes about Alpaca device API
   * https://ascom-standards.org/api/?urls.primaryName=ASCOM%20Alpaca%20Device%20API#/
   */
  void handleConnected();
  void handleDescriptionGet();
  void handleNameGet();
  void handleDriverinfoGet();
  void handleDriverversionGet();
  void handleInterfaceVersionGet();
protected:
  HttpHandler& _server;
  int _deviceNumber;
  virtual void _doConnect(bool connected) = 0;
  virtual bool _isConnected() = 0;
  String _prefixSetupUri;
  String _prefixApiUri;

  String _description = "Controls a relay board for Cache Calibration";
  String _deviceName = "Cache Calibration Controller";
  String _driverVersion = "v1.0.0";
  const int _interfaceVersion = 1;
  String _alpacaDeviceType = "Unknown";


  //must be randomly generated and distinct for each devices
  String _uniqueId = "4431281c85604ad7982f5a6e507dda20-";

};

#endif // ALPACA_DEVICE_H
