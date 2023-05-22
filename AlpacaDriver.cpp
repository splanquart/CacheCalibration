#include "AlpacaDriver.h"
#include "RelayController.h"


AlpacaDriver::AlpacaDriver(RelayController* controller, int deviceId)
    : _controller(controller),
      _deviceId(deviceId),
      _server(ALPACA_PORT) {}

void AlpacaDriver::begin() {
  _udp.begin(DISCOVERY_PORT);
 
  // Print the IP address
  Serial.println("");
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  _server.onNotFound(std::bind(&AlpacaDriver::handlerNotFound, this)); 
  
  _server.on("/api/v1.0/devices", HTTP_GET, std::bind(&AlpacaDriver::handleGetDevices, this));
  
  _server.on("/management/apiversions", HTTP_GET, std::bind(&AlpacaDriver::handleAPIVersions, this));
  _server.on("/management/v1/description", HTTP_GET, std::bind(&AlpacaDriver::handleDescription, this));
  _server.on("/management/v1/configureddevices", HTTP_GET, std::bind(&AlpacaDriver::handleConfiguredDevices, this));

  _server.on("/api/v1/switch/0/connected", HTTP_ANY, std::bind(&AlpacaDriver::handleConnected, this));
  _server.on("/api/v1/switch/0/description", HTTP_GET, std::bind(&AlpacaDriver::handleDescriptionGet, this));
  _server.on("/api/v1/switch/0/name", HTTP_GET, std::bind(&AlpacaDriver::handleNameGet, this));
  _server.on("/api/v1/switch/0/driverinfo", HTTP_GET, std::bind(&AlpacaDriver::handleDriverinfoGet, this));
  _server.on("/api/v1/switch/0/driverversion", HTTP_GET, std::bind(&AlpacaDriver::handleDriverversionGet, this));
  _server.on("/api/v1/switch/0/interfaceversion", HTTP_GET, std::bind(&AlpacaDriver::handleInterfaceVersionGet, this));

  _server.on("/api/v1/switch/0/maxswitch", HTTP_GET, std::bind(&AlpacaDriver::handleMaxswitchGet, this));
  _server.on("/api/v1/switch/0/supportedactions", HTTP_GET, std::bind(&AlpacaDriver::handleSupportedactionsGet, this));
  _server.on("/api/v1/switch/0/canwrite", HTTP_GET, std::bind(&AlpacaDriver::handleCanwritesGet, this));
  _server.on("/api/v1/switch/0/getswitchname", HTTP_GET, std::bind(&AlpacaDriver::handleSwitchnameGet, this));
  _server.on("/api/v1/switch/0/getswitchdescription", HTTP_GET, std::bind(&AlpacaDriver::handleSwitchdescriptionGet, this));

  _server.on("/api/v1/switch/0/getswitchvalue", HTTP_GET, std::bind(&AlpacaDriver::handleSwitchvalueGet, this));
  _server.on("/api/v1/switch/0/maxswitchvalue", HTTP_GET, std::bind(&AlpacaDriver::handleMaxswitchvalueGet, this));
  _server.on("/api/v1/switch/0/minswitchvalue", HTTP_GET, std::bind(&AlpacaDriver::handleMinswitchvalueGet, this));
  _server.on("/api/v1/switch/0/switchstep", HTTP_GET, std::bind(&AlpacaDriver::handleSwitchstepGet, this));
  _server.on("/api/v1/switch/0/setswitchvalue", HTTP_PUT, std::bind(&AlpacaDriver::handleSwitchvaluePut, this));
  _server.on("/api/v1/switch/0/setswitchname", HTTP_PUT, std::bind(&AlpacaDriver::handleSwitchnamePut, this));

  _server.on("/setup", HTTP_GET, std::bind(&AlpacaDriver::handleSetup, this));
  _server.on("/setup/v1/switch/0/setup", HTTP_GET, std::bind(&AlpacaDriver::handleSetupdevice, this));
  
  _server.begin();
  Serial.println("Server started");
}
void AlpacaDriver::handleClient() {
  // handle GET and PUT requests from clients
  _server.handleClient();
}
void AlpacaDriver::handleDiscovery() {
  // if there's data available, read a packet
  int packetSize = _udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size: ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = _udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", on port ");
    Serial.println(_udp.remotePort());

    // read the packet into packetBufffer
    int len = _udp.read(_packetBuffer, 255);
    if (len > 0) {
      //Ensure that it is null terminated
      _packetBuffer[len] = 0;
    }
    Serial.print("Contents: ");
    Serial.println(_packetBuffer);

    // No undersized packets allowed
    if (len < 16)
    {
      return;
    }

    if (strncmp("alpacadiscovery1", _packetBuffer, 16) != 0)
    {
      return;
    }
    // prepare response
    char response[36] = {0};
    sprintf(response, "{\"AlpacaPort\": %d}", ALPACA_PORT);
    // send a reply, to the IP address and port that sent us the packet we received
    _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
    _udp.write(response);
    _udp.endPacket();
    Serial.print("Response: ");
    Serial.println(response);
  }
}
void AlpacaDriver::handlerNotFound(){
  Serial.println("----------");
  Serial.print("URL not found: ");
  Serial.println(_server.uri());
  Serial.println("----------");
  _server.send(404, "text/plain", "Not found");
}
void AlpacaDriver::returnResponse(DynamicJsonDocument val) {
  int clientID = (uint32_t)_server.arg("ClientID").toInt();
  int transID = (uint32_t)_server.arg("ClientTransactionID").toInt();
  for(int i=0;i<_server.args();i++) {
    if (_server.argName(i) == "clientId") {
      clientID = (uint32_t)_server.arg(i).toInt();  
    } else if (_server.argName(i) == "ClientTransactionID") {
      transID = (uint32_t)_server.arg(i).toInt();
    }
  }
  Serial.print("    | clientId: ");
  Serial.println(clientID);
  Serial.print("    | clientTransactionId: ");
  Serial.println(transID);
  
  DynamicJsonDocument doc(1024);
  doc["Value"] = val;
  doc["ErrorMessage"] = "";
  doc["ErrorNumber"] = 0;
  doc["ClientTransactionID"] = transID;
  doc["ServerTransactionID"] = ++_serverTransactionID;

  String response;
  serializeJson(doc, response);
  _server.send(200, _content_type, response); // envoie la réponse JSON au client Alpaca
  Serial.print("    | ");
  Serial.println(response);
}
void AlpacaDriver::returnBoolValue(bool val, String errMsg, int errNr) {
  int clientID = (uint32_t)_server.arg("ClientID").toInt();
  int transID = (uint32_t)_server.arg("ClientTransactionID").toInt();
  for(int i=0;i<_server.args();i++) {
    if (_server.argName(i) == "clientId") {
      clientID = (uint32_t)_server.arg(i).toInt();  
    } else if (_server.argName(i) == "ClientTransactionID") {
      transID = (uint32_t)_server.arg(i).toInt();
    }
  }
  Serial.print("    | clientId: ");
  Serial.println(clientID);
  Serial.print("    | clientTransactionId: ");
  Serial.println(transID);
  
  DynamicJsonDocument doc(1024);
  
  doc["Value"] = val;
  doc["ErrorMessage"] = errMsg;
  doc["ErrorNumber"] = errNr;
  doc["ClientTransactionID"] = transID;
  doc["ServerTransactionID"] = ++_serverTransactionID;
  
  String response;
  serializeJson(doc, response);
  
  _server.send(200, _content_type, response);
  Serial.print("    | ");
  Serial.println(response);
}
void AlpacaDriver::returnNothing(String errMsg, int errNr) {
  int clientID = (uint32_t)_server.arg("ClientID").toInt();
  int transID = (uint32_t)_server.arg("ClientTransactionID").toInt();
  for(int i=0;i<_server.args();i++) {
    if (_server.argName(i) == "clientId") {
      clientID = (uint32_t)_server.arg(i).toInt();  
    } else if (_server.argName(i) == "ClientTransactionID") {
      transID = (uint32_t)_server.arg(i).toInt();
    }
  }
  Serial.print("    | clientId: ");
  Serial.println(clientID);
  Serial.print("    | clientTransactionId: ");
  Serial.println(transID);
  
  DynamicJsonDocument doc(1024);
  
  doc["ErrorMessage"] = errMsg;
  doc["ErrorNumber"] = errNr;
  doc["ClientTransactionID"] = transID;
  doc["ServerTransactionID"] = ++_serverTransactionID;
  
  String response;
  serializeJson(doc, response);
  
  _server.send(200, _content_type, response);
  Serial.print("    | ");
  Serial.println(response);
}
void AlpacaDriver::returnStringValue(String val, String errMsg, int errNr) {
  int clientID = (uint32_t)_server.arg("ClientID").toInt();
  int transID = (uint32_t)_server.arg("ClientTransactionID").toInt();
  for(int i=0;i<_server.args();i++) {
    if (_server.argName(i) == "clientId") {
      clientID = (uint32_t)_server.arg(i).toInt();  
    } else if (_server.argName(i) == "ClientTransactionID") {
      transID = (uint32_t)_server.arg(i).toInt();
    }
  }
  Serial.print("    | clientId: ");
  Serial.println(clientID);
  Serial.print("    | clientTransactionId: ");
  Serial.println(transID);
  
  DynamicJsonDocument doc(1024);
  
  doc["Value"] = val;
  doc["ErrorMessage"] = errMsg;
  doc["ErrorNumber"] = errNr;
  doc["ClientTransactionID"] = transID;
  doc["ServerTransactionID"] = ++_serverTransactionID;
  
  String response;
  serializeJson(doc, response);
  
  _server.send(200, _content_type, response);
  Serial.print("    | ");
  Serial.println(response);
}
void AlpacaDriver::returnIntValue(int val, String errMsg, int errNr) {
  int clientID = (uint32_t)_server.arg("ClientID").toInt();
  int transID = (uint32_t)_server.arg("ClientTransactionID").toInt();
  for(int i=0;i<_server.args();i++) {
    if (_server.argName(i) == "clientId") {
      clientID = (uint32_t)_server.arg(i).toInt();  
    } else if (_server.argName(i) == "ClientTransactionID") {
      transID = (uint32_t)_server.arg(i).toInt();
    }
  }
  Serial.print("    | clientId: ");
  Serial.println(clientID);
  Serial.print("    | clientTransactionId: ");
  Serial.println(transID);
  
  DynamicJsonDocument doc(1024);
  
  doc["Value"] = val;
  doc["ErrorMessage"] = errMsg;
  doc["ErrorNumber"] = errNr;
  doc["ClientTransactionID"] = transID;
  doc["ServerTransactionID"] = ++_serverTransactionID;
  
  String response;
  serializeJson(doc, response);
  
  _server.send(200, _content_type, response);
  Serial.print("    | ");
  Serial.println(response);
}

void AlpacaDriver::handleGetDevices(){
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.println("handleGetDevices");
  StaticJsonDocument<512> doc;
  JsonObject root = doc.to<JsonObject>();
  root["DeviceType"] = _alpacaDeviceType;
  root["Name"] = _deviceName;
  root["UniqueId"] = _uniqueId;
  root["DriverVersion"] = _driverVersion;

  String response;
  serializeJson(doc, response);
  Serial.println(response);
  _server.send(200, _content_type, response);
}

void AlpacaDriver::handleAPIVersions() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.println("handleAPIVersions");
  //String responseJson = "{\"ServerVersion\":\"1.0\",\"SupportedVersions\":[1]}";
  //_server.send(200, _content_type, responseJson);
  const size_t CAPACITY = JSON_ARRAY_SIZE(5);
  StaticJsonDocument<CAPACITY> devices;
  JsonArray array = devices.to<JsonArray>();
  array.add(1);
  returnResponse(array);

}
void AlpacaDriver::handleDescription() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.println("handleDescription");

  
  DynamicJsonDocument val(1024);

  val["ServerName"] = _description;
  val["Manufacturer"] = _manufacturer;
  val["ManufacturerVersion"] = _driverVersion; //"1.0";
  val["Location"] = "FR";

  const size_t CAPACITY = JSON_ARRAY_SIZE(5);
  StaticJsonDocument<CAPACITY> devices;
  JsonArray array = devices.to<JsonArray>();
  array.add(val);
  returnResponse(val);

}
void AlpacaDriver::handleConfiguredDevices() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.println("handleConfiguredDevices");

  DynamicJsonDocument val(1024);
  val["DeviceName"] = _deviceName;
  val["DeviceType"] = _alpacaDeviceType;
  val["DeviceNumber"] = 0;
  val["UniqueID"] = _uniqueId;

  const size_t CAPACITY = JSON_ARRAY_SIZE(5);
  StaticJsonDocument<CAPACITY> devices;
  JsonArray array = devices.to<JsonArray>();
  array.add(val);
  returnResponse(array);
}

void AlpacaDriver::handleConnected(){
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleConnected ");
  if (_server.method() == HTTP_GET) {
    Serial.print("GET");
    returnBoolValue(_controller->is_connected(), "", 0);  
  }
  else if (_server.method() == HTTP_PUT) {
    bool connected = false;
    
    for(int i=0;i<_server.args();i++) {
      if(_server.argName(i) == "Connected") {
        connected = _server.arg(i) == "True" ? true : false;
      }
    }
    _controller->do_connect(connected);
    Serial.print("PUT");
    returnBoolValue(_controller->is_connected(), "", 0); 
  }
  Serial.println("");
}
void AlpacaDriver::handleDescriptionGet() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleDescriptionGet ");
  returnStringValue(_description, "", 0);
}
void AlpacaDriver::handleNameGet() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleDescriptionGet ");
  returnStringValue(_deviceName, "", 0);
}
void AlpacaDriver::handleDriverinfoGet() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleDriverinfoGet ");
  returnStringValue(_description, "", 0);
}
void AlpacaDriver::handleDriverversionGet() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleDriverversionGet ");
  returnStringValue(_driverVersion, "", 0);
}
void AlpacaDriver::handleInterfaceVersionGet() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleInterfaceVersionGet ");
  returnIntValue(_interfaceVersion, "", 0);
}
void AlpacaDriver::handleMaxswitchGet() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleMaxswitchGet ");
  returnIntValue(_controller->getMaxSwitch(), "", 0);
}
void AlpacaDriver::handleSupportedactionsGet() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleMaxswitchGet ");
  const size_t CAPACITY = JSON_ARRAY_SIZE(5);
  StaticJsonDocument<CAPACITY> devices;
  JsonArray array = devices.to<JsonArray>();
  //array.add(1);
  array.add("SetSwitchName");
  returnResponse(array);
}
void AlpacaDriver::handleCanwritesGet() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.println("handleCanwritesGet ");
  returnBoolValue(true, "", 0); 

}
void AlpacaDriver::handleSwitchnameGet() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleSwitchnameGet ");
  int id = (uint32_t)_server.arg("ID").toInt();

  Serial.print("Id: ");
  Serial.println(id);
  returnStringValue(_controller->getName(id), "", 0);
  Serial.println();
}
void AlpacaDriver::handleSwitchdescriptionGet() { 

  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleSwitchdescriptionGet ");
  int id = (uint32_t)_server.arg("ID").toInt();

  Serial.print("Id: ");
  Serial.println(id);
  returnStringValue(_controller->getName(id) + " description", "", 0);
  Serial.println();
}
void AlpacaDriver::handleSwitchvalueGet() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleSwitchvalueGet ");
  int id = (uint32_t)_server.arg("ID").toInt();

  Serial.print("Id: ");
  Serial.println(id);
  returnIntValue(_controller->getRelayState(id)?1:0, "", 0);
  Serial.println();
}
void AlpacaDriver::handleMaxswitchvalueGet() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleMaxswitchvalueGet ");
  int id = (uint32_t)_server.arg("ID").toInt();

  Serial.print("Id: ");
  Serial.println(id);
  returnIntValue(1, "", 0);
  Serial.println();
}
void AlpacaDriver::handleMinswitchvalueGet() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleMinswitchvalueGet ");
  int id = (uint32_t)_server.arg("ID").toInt();

  Serial.print("Id: ");
  Serial.println(id);
  returnIntValue(0, "", 0);
  Serial.println();
}
void AlpacaDriver::handleSwitchstepGet() {  

  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleSwitchstepGet ");

  int id = (uint32_t)_server.arg("ID").toInt();

  Serial.print("Id: ");
  Serial.println(id);
  returnIntValue(1, "", 0);
  Serial.println();
}
void AlpacaDriver::handleSwitchvaluePut() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleSwitchvaluePut ");

  if (_server.method() == HTTP_PUT) {
    Serial.println("PUT");
    int id = (uint32_t)_server.arg("ID").toInt();
    int value = (uint32_t)_server.arg("Value").toInt();
    Serial.print("Id: ");
    Serial.println(id);
    _controller->setRelayState(id, value==1);
    returnNothing("", 0);
  }
}
void AlpacaDriver::handleSwitchnamePut() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleSwitchnamePut ");

  if (_server.method() == HTTP_PUT) {
    Serial.println("PUT");
    int id = (uint32_t)_server.arg("ID").toInt();
    String value = _server.arg("Value");
    Serial.print("Id: ");
    Serial.println(id);
    Serial.print("Value: ");
    Serial.println(value);
    _controller->setName(id, value);
    returnNothing("", 0);
  }
}
void AlpacaDriver::handleSetup() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleSetup ");
  String response = "<html><body>Setup main page<br/><a href=\"/setup/v1/switch/0/setup\">Configuration Switch 0</a></body></html>";
  _server.send(200, _content_type_html, response); // envoie la réponse JSON au client Alpaca

}
void AlpacaDriver::handleSetupdevice() {
  Serial.print(_server.uri());
  Serial.print(" -> ");
  Serial.print("handleSetupdevice ");
  String response = "<html><body>Setup Switch 0<br/></body></html>";
  _server.sendContent("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
  _server.sendContent("<meta charset=\"UTF-8\"><html><body><ul>");
  _server.sendContent("<h1>Configuration of Switch 0</h1>");
  for(int i=0;i<_controller->getMaxSwitch();i++) {
    _server.sendContent("<li>");
    _server.sendContent("<form onsubmit='event.preventDefault(); updateRelayName(this, " + String(i) + ");'>");
    _server.sendContent("<label for='relayName'>Relay " + String(i+1) + ": </label>");
    _server.sendContent("<input type='text' id='relayName' name='relayName' value='" + _controller->getName(i) + "'>");
    _server.sendContent("<input type='submit' value='Update'>");
    _server.sendContent("</form>");
    _server.sendContent("</li>");    

  }
  _server.sendContent("<script>"
                   "async function updateRelayName(form, relayNumber) {"
                   "  const relayName = form.relayName.value;"
                   "  const response = await fetch('/api/v1/switch/0/setswitchname', {"
                   "    method: 'PUT',"
                   "    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },"
                   "    body: 'ID=' + encodeURIComponent(relayNumber) + '&Value=' + encodeURIComponent(relayName)"
                   "  });"
                   "  if (!response.ok) {"
                   "    alert('Error updating relay name');"
                   "  }"
                   "}"
                   "</script>");
  _server.sendContent("</ul></body></html>");
}
