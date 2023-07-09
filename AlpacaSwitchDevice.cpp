#include "AlpacaSwitchDevice.h"

AlpacaSwitchDevice::AlpacaSwitchDevice(HttpHandler& server, int deviceNumber, ISwitch* switchController)
  : AlpacaDevice(server, deviceNumber, "Switch", {}), _controller(switchController) {
  _prefixSetupUri = "/setup/v1/switch/" + String(_deviceNumber);
  _prefixApiUri = "/api/v1/switch/" + String(_deviceNumber);
}

void AlpacaSwitchDevice::begin() {
  AlpacaDevice::begin();
  _server.bind(_prefixSetupUri + "/setup", HTTP_GET, std::bind(&AlpacaSwitchDevice::handleSetupdevice, this));
  
  _server.bind(_prefixApiUri + "/maxswitch", HTTP_GET, std::bind(&AlpacaSwitchDevice::handleMaxswitchGet, this));
  _server.bind(_prefixApiUri + "/supportedactions", HTTP_GET, std::bind(&AlpacaSwitchDevice::handleSupportedactionsGet, this));
  _server.bind(_prefixApiUri + "/canwrite", HTTP_GET, std::bind(&AlpacaSwitchDevice::handleCanwritesGet, this));
  _server.bind(_prefixApiUri + "/getswitchname", HTTP_GET, std::bind(&AlpacaSwitchDevice::handleSwitchnameGet, this));
  _server.bind(_prefixApiUri + "/getswitchdescription", HTTP_GET, std::bind(&AlpacaSwitchDevice::handleSwitchdescriptionGet, this));
  _server.bind(_prefixApiUri + "/getswitch", HTTP_GET, std::bind(&AlpacaSwitchDevice::handleSwitchGet, this));

  _server.bind(_prefixApiUri + "/getswitchvalue", HTTP_GET, std::bind(&AlpacaSwitchDevice::handleSwitchvalueGet, this));
  _server.bind(_prefixApiUri + "/maxswitchvalue", HTTP_GET, std::bind(&AlpacaSwitchDevice::handleMaxswitchvalueGet, this));
  _server.bind(_prefixApiUri + "/minswitchvalue", HTTP_GET, std::bind(&AlpacaSwitchDevice::handleMinswitchvalueGet, this));
  _server.bind(_prefixApiUri + "/switchstep", HTTP_GET, std::bind(&AlpacaSwitchDevice::handleSwitchstepGet, this));
  _server.bind(_prefixApiUri + "/setswitchvalue", HTTP_PUT, std::bind(&AlpacaSwitchDevice::handleSwitchvaluePut, this));
  _server.bind(_prefixApiUri + "/setswitchname", HTTP_PUT, std::bind(&AlpacaSwitchDevice::handleSwitchnamePut, this));
  _server.bind(_prefixApiUri + "/setswitch", HTTP_PUT, std::bind(&AlpacaSwitchDevice::handleSwitchPut, this));

   _setup.addStyle(R"(
          ul.relaylist li{
            list-style-type: none;
          }
  )");
  String urlSetSwithName = _prefixApiUri + "/setswitchname";
  String urlSetSwith = _prefixApiUri + "/setswitch";
  _setup.addScript("async function updateRelayName(form, relayNumber) {"
                   "  const relayName = form.relayName.value;"
                   "  const response = await fetch('" + urlSetSwithName + "', {"
                   "    method: 'PUT',"
                   "    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },"
                   "    body: 'ID=' + encodeURIComponent(relayNumber) + '&Value=' + encodeURIComponent(relayName)"
                   "  });"
                   "  if (!response.ok) {"
                   "    alert('Error updating relay name');"
                   "  }"
                   "}");
  _setup.addScript("function setState(relayNum, state) {"
                   "  var xhr = new XMLHttpRequest();"
                   "  xhr.open('PUT', '" + urlSetSwith + "', true);"
                   "  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');"
                   "  xhr.send('Id=' + relayNum + '&State=' + state);"
                   "}");
}
void AlpacaSwitchDevice::_doConnect(bool connected) {
  _controller->connect(connected);
}
bool AlpacaSwitchDevice::_isConnected() {
  return _controller->isConnected();
}
void AlpacaSwitchDevice::handleSetupdevice() {
  _server.logRequest(__func__);
 
  _setup.render([this](HttpHandler& server) {
      server.sendContent("<ul class=\"relaylist\">");
      for(int i=0;i<_controller->getMaxSwitch();i++) {
        server.sendContent("<li class=\"relaylist\">");
        server.sendContent("<form onsubmit='event.preventDefault(); updateRelayName(this, " + String(i) + ");'>");
        server.sendContent("<label for='relayName'>Relay " + String(i+1) + ": </label>");
        server.sendContent("<input type='text' id='relayName' name='relayName' value='" + _controller->getName(i) + "'>");
        server.sendContent("<input type='submit' value='Update'>");
        server.sendContent("</form>");
    
        server.sendContent("<button type='button' onclick='setState("
            + String(i) + ", \"true\")'>On</button> ");
        server.sendContent("<button type='button' onclick='setState("
            + String(i) + ", \"false\")'>Off</button> ");
        server.sendContent("</li>");    
      }
      server.sendContent("</ul>");
  });
}

void AlpacaSwitchDevice::handleMaxswitchGet() {
  _server.logRequest(__func__);
  _server.returnIntValue(_controller->getMaxSwitch(), "", 0);
}
void AlpacaSwitchDevice::handleSupportedactionsGet() {
  _server.logRequest(__func__);
  const size_t CAPACITY = JSON_ARRAY_SIZE(5);
  StaticJsonDocument<CAPACITY> devices;
  JsonArray array = devices.to<JsonArray>();
  //array.add(1);
  array.add("SetSwitchName");
  _server.returnResponse(array);
}
void AlpacaSwitchDevice::handleCanwritesGet() {
  _server.logRequest(__func__);
  _server.returnBoolValue(true, "", 0); 
}
void AlpacaSwitchDevice::handleSwitchnameGet() {
  _server.logRequest(__func__);
  int id = _server.getArgCaseInsensitive("id").toInt();

  _server.logArg("Id", id); 
  _server.returnStringValue(_controller->getName(id), "", 0);
}

void AlpacaSwitchDevice::handleSwitchGet() {
  _server.logRequest(__func__);
  int id = _server.getArgCaseInsensitive("id").toInt();
  _server.logArg("Id", id); 
  _server.returnBoolValue(_controller->getState(id), "", 0);
}

void AlpacaSwitchDevice::handleSwitchdescriptionGet() { 
  _server.logRequest(__func__);
  int id = _server.getArgCaseInsensitive("id").toInt();
  _server.logArg("Id", id); 
  _server.returnStringValue(_controller->getName(id) + " description", "", 0);
}
void AlpacaSwitchDevice::handleSwitchvalueGet() {
  _server.logRequest(__func__);
  int id = _server.getArgCaseInsensitive("id").toInt();
  _server.logArg("Id", id); 
  _server.returnIntValue(_controller->getState(id)?1:0, "", 0);
}
void AlpacaSwitchDevice::handleMaxswitchvalueGet() {
  _server.logRequest(__func__);
  int id = _server.getArgCaseInsensitive("id").toInt();
  _server.logArg("Id", id); 
  _server.returnIntValue(1, "", 0);
}
void AlpacaSwitchDevice::handleMinswitchvalueGet() {
  _server.logRequest(__func__);
  int id = _server.getArgCaseInsensitive("id").toInt();
  _server.logArg("Id", id); 
  _server.returnIntValue(0, "", 0);
}
void AlpacaSwitchDevice::handleSwitchstepGet() {  
  _server.logRequest(__func__);
  int id = _server.getArgCaseInsensitive("id").toInt();
  _server.logArg("Id", id); 
  _server.returnIntValue(1, "", 0);
}
void AlpacaSwitchDevice::handleSwitchvaluePut() {
  _server.logRequest(__func__);
  if (_server.method() == HTTP_PUT) {
    int id = _server.getArgCaseInsensitive("id").toInt();
    int value = (uint32_t)_server.arg("Value").toInt();
    _server.logArg("Id", id); 
    _controller->setState(id, value==1);
    _server.returnNothing("", 0);
  }
}
void AlpacaSwitchDevice::handleSwitchPut() {
  _server.logRequest(__func__);
  if (_server.method() == HTTP_PUT) {
    int id = _server.getArgCaseInsensitive("id").toInt();
    String state = _server.getArgCaseInsensitive("State");
    state.toLowerCase();
    Serial.print(id);
    Serial.print("  State: ");
    Serial.println(state);
    if(state=="true") {
      _controller->setState(id, 1);
    } else {
      _controller->setState(id, 0);
    }
    _server.returnNothing("", 0);
  }
}
void AlpacaSwitchDevice::handleSwitchnamePut() {
  _server.logRequest(__func__);
  if (_server.method() == HTTP_PUT) {
    int id = _server.getArgCaseInsensitive("id").toInt();
    String value = _server.arg("Value");
    _server.logArg("Id", id); 
    _server.logArg("Value", value);
    _controller->setName(id, value);
    _server.returnNothing("", 0);
  }
}
