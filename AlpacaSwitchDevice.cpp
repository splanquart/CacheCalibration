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
        ul.relaylist {
            list-style-type: none;
            padding-left: 0;
            border: none;
        }
    )");
   _setup.addStyle(R"(
        .switch-label {
            width: 150px;
        }
        
        .switch-label i {
            margin-right: 5px;
        }
        
        .switch-toggle {
            height: 40px;
        }
        
        .switch-toggle input[type="checkbox"] {
            position: absolute;
            opacity: 0;
            z-index: -2;
        }
        
        .switch-toggle input[type="checkbox"] + label {
            position: relative;
            display: inline-block;
            width: 100px;
            height: 40px;
            border-radius: 20px;
            margin: 0;
            cursor: pointer;
            box-shadow: inset -4px -4px 4px rgb(93 91 91 / 73%),
                        inset 10px 10px 10px rgba(0,0,0, .25);
            
        }
        
        .switch-toggle input[type="checkbox"] + label::before {
            position: absolute;
            content: 'OFF';
            font-size: 13px;
            text-align: center;
            line-height: 25px;
            top: 8px;
            left: 8px;
            width: 45px;
            height: 25px;
            border-radius: 20px;
            background-color: #d1dad3;
            box-shadow: -3px -3px 5px rgba(255,255,255,.5),
                        3px 3px 5px rgba(0,0,0, .25);
            transition: .3s ease-in-out;
        }
    )");
   _setup.addStyle(R"(
        @media (prefers-color-scheme: dark) {
          .switch-toggle input[type="checkbox"] + label::before {
            background-color: #2a2a2a;
          }
          .switch-toggle input[type="checkbox"]:checked + label::before {
            background: linear-gradient(0, #074600, #00b33c);
          }
        }
        .switch-toggle input[type="checkbox"]:disabled + label::before {
          display: none;
        }
        .switch-toggle input[type="checkbox"]:checked + label::before {
            left: 50%;
            content: 'ON';
            color: #fff;
            background-color: #00b33c;
            box-shadow: -3px -3px 5px rgba(255,255,255,.5),
                        3px 3px 5px #00b33c;
            background: linear-gradient(0, #0e8d00, #01e84e);
        }
  )");
  String urlSetSwithName = _prefixApiUri + "/setswitchname";
  String urlSetSwith = _prefixApiUri + "/setswitch";
  String urlGetSwith = _prefixApiUri + "/getswitch";
  _setup.addScript(R"(
      async function updateRelayState(relayNumber) {
          const response = await fetch(')" + urlGetSwith + R"(?Id=' + relayNumber, { method: 'GET' });
          if (!response.ok) {
              alert('Error fetching relay state');
              return;
          }
          const relayElement = document.getElementById('relay-state-' + relayNumber);
          relayElement.disabled = false;
          const responseData = await response.json();
          relayElement.checked = responseData.Value;
      }
      document.addEventListener('DOMContentLoaded', (event) => {
          // Update states for all relays on page load
          for(let i = 0; i < )" + String(_controller->getMaxSwitch()) + R"(; i++) {
              updateRelayState(i);
              setInterval(() => updateRelayState(i), 5000); // Update every 5 seconds
          }
      });
  )");
  
  _setup.addScript(R"(
      async function toggleRelayState(checkbox, relayNumber) {
          const state = checkbox.checked ? 'true' : 'false';
          const response = await fetch(')" + urlSetSwith + R"(', {
              method: 'PUT',
              headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
              body: 'Id=' + encodeURIComponent(relayNumber) + '&State=' + encodeURIComponent(state)
          });
          if (!response.ok) {
              alert('Error updating relay state');
          }
      }
  )");
  _setup.addScript(R"--(
      async function updateRelayName(form, relayNumber) {
        const relayName = form.relayName.value;
        const response = await fetch(')--" + urlSetSwithName + R"--(', {
          method: 'PUT',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: 'ID=' + encodeURIComponent(relayNumber) + '&Value=' + encodeURIComponent(relayName)
        });
        if (!response.ok) {
          alert('Error updating relay name');
        }
      }
  )--");

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
        server.sendContent(R"(
            <li class='relaylist block-holder'>
                <div class='switch-label'>
                <form onsubmit='event.preventDefault(); updateRelayName(this, )" + String(i) + R"()'>
                    <label for='relayName'>Relay )" + String(i+1) + R"(: </label>
                    <input type='text' id='relayName' name='relayName' value=')" + _controller->getName(i) + R"('>
                    <input type='submit' value='Update'>
                </form>
                </div>
                <div class='switch-toggle'>
                    <input type='checkbox' id='relay-state-)" + String(i) + R"(' onchange='toggleRelayState(this, )" + String(i) + R"()' disabled>
                    <label for='relay-state-)" + String(i) + R"('></label>
                </div>
            </li>
        )");  
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
