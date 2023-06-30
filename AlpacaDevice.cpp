#include "AlpacaDevice.h"

AlpacaDevice::AlpacaDevice(HttpHandler& server, int deviceNumber, String deviceType, std::vector<String> supportedActions) 
  : _server(server), _deviceNumber(deviceNumber), _alpacaDeviceType(deviceType), _supportedActions(supportedActions) { 
    _uniqueId = _generateUniqueId(_alpacaDeviceType, _deviceNumber);
    _deviceName = _deviceName + "_" + _uniqueId;
}

void AlpacaDevice::begin() {
  _server.bind(_prefixApiUri + "/connected", HTTP_ANY, std::bind(&AlpacaDevice::handleConnected, this));
  _server.bind(_prefixApiUri + "/description", HTTP_GET, std::bind(&AlpacaDevice::handleDescriptionGet, this));
  _server.bind(_prefixApiUri + "/name", HTTP_GET, std::bind(&AlpacaDevice::handleNameGet, this));
  _server.bind(_prefixApiUri + "/driverinfo", HTTP_GET, std::bind(&AlpacaDevice::handleDriverinfoGet, this));
  _server.bind(_prefixApiUri + "/driverversion", HTTP_GET, std::bind(&AlpacaDevice::handleDriverversionGet, this));
  _server.bind(_prefixApiUri + "/interfaceversion", HTTP_GET, std::bind(&AlpacaDevice::handleInterfaceVersionGet, this));
  _server.bind(_prefixApiUri + "/supportedactions", HTTP_GET, std::bind(&AlpacaDevice::handleSupportedActionsGet, this));

}
void AlpacaDevice::handleSupportedActionsGet() {
  _server.logRequest(__func__);
  DynamicJsonDocument doc(1024);
  JsonArray actions = doc.to<JsonArray>();
  for(const auto& action : _supportedActions) {
    actions.add(action);
  }
  Serial.println(_getSupportedActionsLog());
  _server.returnResponse(doc);
}
String AlpacaDevice::_getSupportedActionsLog() {
  String logOutput = "List of supported actions: ";
  for(const auto& action : _supportedActions) {
    logOutput += action;
    logOutput += ", ";
  }
  return logOutput.substring(0, logOutput.length()-2);
}
String AlpacaDevice::_base36_encode(unsigned long long number) {
  const char ALPHABET[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  const int BASE = sizeof(ALPHABET) - 1;
  String result = "";
  
  while (number > 0) {
    result = String(ALPHABET[number % BASE]) + result;
    number /= BASE;
  }
  
  return result;
}
String AlpacaDevice::_generateUniqueId(String hardwareName, int deviceId) {
  /**
   * produce a name like CoverCalibrator-S6VQ8TW9A-0
   */
  String mac = WiFi.macAddress();
  mac.replace(":", ""); // Supprimer les deux-points dans l'adresse MAC
  unsigned long long macValue = strtoull(mac.c_str(), NULL, 16);
  String uniqueId = hardwareName + "-" + _base36_encode(macValue) + "-" + String(deviceId);
  return uniqueId;
}
String AlpacaDevice::getDeviceType() {
  return _alpacaDeviceType;
}
String AlpacaDevice::getDeviceName() {
  return _alpacaDeviceType + " " + String(_deviceNumber);
}
String AlpacaDevice::getDeviceSetupUrl() {
  return _prefixSetupUri + "/setup";
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
