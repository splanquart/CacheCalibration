#include "AlpacaDevice.h"

AlpacaDevice::AlpacaDevice(HttpHandler& server, int deviceNumber) 
  : _server(server), _deviceNumber(deviceNumber) { 
}

void AlpacaDevice::begin() {
  _server.bind(_prefixApiUri + "/connected", HTTP_ANY, std::bind(&AlpacaDevice::handleConnected, this));
  _server.bind(_prefixApiUri + "/description", HTTP_GET, std::bind(&AlpacaDevice::handleDescriptionGet, this));
  _server.bind(_prefixApiUri + "/name", HTTP_GET, std::bind(&AlpacaDevice::handleNameGet, this));
  _server.bind(_prefixApiUri + "/driverinfo", HTTP_GET, std::bind(&AlpacaDevice::handleDriverinfoGet, this));
  _server.bind(_prefixApiUri + "/driverversion", HTTP_GET, std::bind(&AlpacaDevice::handleDriverversionGet, this));
  _server.bind(_prefixApiUri + "/interfaceversion", HTTP_GET, std::bind(&AlpacaDevice::handleInterfaceVersionGet, this));
}
DynamicJsonDocument AlpacaDevice::getDeviceInfo() {
    DynamicJsonDocument val(1024);
    val["DeviceName"] = _deviceName;
    val["DeviceType"] = _alpacaDeviceType;
    val["DeviceNumber"] = _deviceNumber;
    val["UniqueID"] = _uniqueId;
    return val;
}
void AlpacaDevice::handleConnected(){
  _server.logRequest(__func__);
  if (_server.method() == HTTP_GET) {
    _server.returnBoolValue(_isConnected(), "", 0);  
  }
  else if (_server.method() == HTTP_PUT) {
    bool connected = false;
    
    for(int i=0;i<_server.args();i++) {
      if(_server.argName(i) == "Connected") {
        connected = _server.arg(i) == "True" ? true : false;
      }
    }
    _doConnect(connected);
    _server.returnBoolValue(_isConnected(), "", 0); 
  }
  Serial.println("");
}


void AlpacaDevice::handleDescriptionGet() {
  _server.logRequest(__func__);
  _server.returnStringValue(_description, "", 0);
}
void AlpacaDevice::handleNameGet() {
  _server.logRequest(__func__);
  _server.returnStringValue(_deviceName, "", 0);
}
void AlpacaDevice::handleDriverinfoGet() {
  _server.logRequest(__func__);
  _server.returnStringValue(_description, "", 0);
}
void AlpacaDevice::handleDriverversionGet() {
  _server.logRequest(__func__);
  _server.returnStringValue(_driverVersion, "", 0);
}
void AlpacaDevice::handleInterfaceVersionGet() {
  _server.logRequest(__func__);
  _server.returnIntValue(_interfaceVersion, "", 0);
}
