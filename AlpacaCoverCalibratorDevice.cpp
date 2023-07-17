#include "AlpacaCoverCalibratorDevice.h"

AlpacaCoverCalibratorDevice::AlpacaCoverCalibratorDevice(HttpHandler& httpHandler, int deviceId, ISwitch* coverController, ISwitch* calibrationLampController)
  : AlpacaDevice(httpHandler, deviceId, "CoverCalibrator", 
  {"coverstate", "closecover", "opencover"}
  ),
  _cover(coverController),
  _maxBrightness(100),
  _calibration(calibrationLampController) {
  _prefixSetupUri = "/setup/v1/covercalibrator/" + String(_deviceNumber);
  _prefixApiUri = "/api/v1/covercalibrator/" + String(_deviceNumber);
}

void AlpacaCoverCalibratorDevice::begin() {
  AlpacaDevice::begin();
  _server.bind(_prefixSetupUri + "/setup", HTTP_GET, std::bind(&AlpacaCoverCalibratorDevice::handleSetupdevice, this));


  _server.bind(_prefixApiUri + "/coverstate", HTTP_GET, std::bind(&AlpacaCoverCalibratorDevice::handleCoverStateGet, this));
  if(_cover) {
    _server.bind(_prefixApiUri + "/closecover", HTTP_PUT, std::bind(&AlpacaCoverCalibratorDevice::handleCloseCoverPut, this));
    _server.bind(_prefixApiUri + "/opencover", HTTP_PUT, std::bind(&AlpacaCoverCalibratorDevice::handleOpenCoverPut, this));
    //  _server.bind(_prefixApiUri + "/haltcover", HTTP_PUT, std::bind(&AlpacaCoverCalibratorDevice::handleHaltCoverPut, this));  
  }

  _server.bind(_prefixApiUri + "/calibratorstate", HTTP_GET, std::bind(&AlpacaCoverCalibratorDevice::handleCalibratorStateGet, this));
  if(_calibration) {
    _server.bind(_prefixApiUri + "/brightness", HTTP_GET, std::bind(&AlpacaCoverCalibratorDevice::handleBrightnessGet, this));
    _server.bind(_prefixApiUri + "/maxbrightness", HTTP_GET, std::bind(&AlpacaCoverCalibratorDevice::handleMaxBrightnessGet, this));
    _server.bind(_prefixApiUri + "/calibratoroff", HTTP_PUT, std::bind(&AlpacaCoverCalibratorDevice::handleCalibratorOffPut, this));
    _server.bind(_prefixApiUri + "/calibratoron", HTTP_PUT, std::bind(&AlpacaCoverCalibratorDevice::handleCalibratorOnPut, this));    
  }

  _setup.addStyle(R"(
    li {
        margin-top: 1em;
    }
    ul form {
        margin-bottom: 0em;
    }
    #coverState {
        padding: 0 1em;
        background: linear-gradient(0deg, #00000091, #00000033);
        text-shadow: 1px 1px 2px #151515;
        color: white;
    }
    #coverState.open {
      background-color: #05a900;
    }
    #coverState.close {
      background-color: #a90000;
    }
    .hidden > * {
      display: none;
    }
    @media (prefers-color-scheme: dark) {
      #coverState {
          background: linear-gradient(0deg, #00000091, #00000033);
          text-shadow: none;
      }
    }
  )");


   _setup.addScript("var _prefixApiUri = '" + _prefixApiUri + "';\n" R"(
        var coverStateMapping = {
            0: "Cover not present",
            1: "Cover is closed",
            2: "Cover is moving",
            3: "Cover is open",
            4: "Cover status is unknown",
            5: "Cover error"
        };

        function updateCoverState() {
            fetch(_prefixApiUri + "/coverstate")
            .then(response => response.json())
            .then(response => {
                var coverStateElement = document.getElementById("coverState");
                coverStateElement.innerHTML = coverStateMapping[response.Value] || "Unexpected state";
                // Clear existing classes
                coverStateElement.className = '';
    
                // Add class based on cover state
                if (response.Value == 1) {
                    coverStateElement.className = 'close';
                } else if (response.Value == 3) {
                    coverStateElement.className = 'open';
                }
                var coverControls = document.getElementById('coverControls');
                if (response.Value === 0) {
                    coverControls.classList.add('hidden');
                } else {
                    coverControls.classList.remove('hidden');
                }
            });
        }
        setInterval(updateCoverState, 2000);
        updateCoverState();

        document.getElementById("openCover").addEventListener('click', function() {
            fetch(_prefixApiUri + "/opencover", {method: 'PUT'});
        });
        document.getElementById("closeCover").addEventListener('click', function() {
            fetch(_prefixApiUri + "/closecover", {method: 'PUT'});
        });
    )");

  _setup.addScript("var _prefixApiUri = '" + _prefixApiUri + "';\n" R"---(
        function updateCalibratorState() {
          var calibratorStateMapping = {
            0: "The device does not have a calibration capability",
            1: "The calibrator is off",
            2: "The calibrator is stabilizing or is not yet in the commanded state",
            3: "The calibrator is ready for use",
            4: "The calibrator state is unknown",
            5: "The calibrator encountered an error when changing state"
          };
        
          fetch(_prefixApiUri + '/calibratorstate')
            .then(response => response.json())
            .then(data => {
              var stateText = calibratorStateMapping[data.Value] || "Unknown";
              document.getElementById('state').innerText = stateText;
              
              var calibratorControls = document.getElementById('calibratorControls');
              if (data.Value === 0) {
                calibratorControls.classList.add('hidden');
              } else {
                calibratorControls.classList.remove('hidden');
              }
            });
        }
  
        function calibratorOn() {
          const params = new URLSearchParams({
            brightness: 100
          });
        
          fetch(_prefixApiUri + '/calibratoron', { 
            method: 'PUT',
            headers: {
              'Content-Type': 'application/x-www-form-urlencoded'
            },
            body: params
          })
          .then(() => updateCalibratorState());
        }
  
        function calibratorOff() {
          fetch(_prefixApiUri + '/calibratoroff', { method: 'PUT' })
            .then(() => updateCalibratorState());
        }
  
        // Update calibrator state every 2 seconds
        setInterval(updateCalibratorState, 2000);
        updateCalibratorState();  // Also update on page load
    )---");
    _setup.addScript("var _prefixApiUri = '" + _prefixApiUri + "';\n" R"(
    function updateBrightness() {
        let maxBrightness, currentBrightness;

        fetch(_prefixApiUri + '/maxbrightness')
        .then(response => response.json())
        .then(data => {
            maxBrightness = data.Value;

            return fetch(_prefixApiUri + '/brightness');
        })
        .then(response => response.json())
        .then(data => {
            currentBrightness = data.Value;

            const brightnessElement = document.getElementById('brightness');
            brightnessElement.max = maxBrightness;
            brightnessElement.value = currentBrightness;
            const brightnessValueElement = document.getElementById('brightnessValue');
            brightnessValueElement.innerText = currentBrightness;
        });
    }

    setInterval(updateBrightness, 2000);
    updateBrightness();
  )");
}

void AlpacaCoverCalibratorDevice::_doConnect(bool connected) {
  _cover->connect(connected);
}

bool AlpacaCoverCalibratorDevice::_isConnected() {
  // retourner ici l'état de connexion du périphérique CoverCalibrator
  return _cover->isConnected();
}

void AlpacaCoverCalibratorDevice::handleSetupdevice() {
  _server.logRequest(__func__);
  _setup.render([this](HttpHandler& server) {
      _server.sendContent(R"---(
        <div id="coverBlock" class="block-holder">
          <span id="coverState">Unknown</span><br/>
          <div id="coverControls">
            <button id="openCover">Open Cover</button>
            <button id="closeCover">Close Cover</button>
          </div>
        </div>

        <div id="calibratorState" class="block-holder">
          <div>
            <span id="state">Unknown</span>
            <div id="brightnessBlock">
              <label for="brightness">Brightness: </label>
              <progress id="brightness" max="100" value="0"></progress>
              <span id="brightnessValue">&nbsp;</span>
            </div>
          </div>
          <div id="calibratorControls">
            <button onclick="calibratorOff()">Turn Off</button>
            <button onclick="calibratorOn()">Turn On</button>
          </div>
        </div>


      )---");
  });
}

void AlpacaCoverCalibratorDevice::handleCloseCoverPut() {
  _server.logRequest(__func__);
  _cover->setState(0, false);
  Serial.println("      CloseCover");
  _server.returnNothing("", 0);
}

void AlpacaCoverCalibratorDevice::handleOpenCoverPut() {
  _server.logRequest(__func__);
  _cover->setState(0, true);
  Serial.println("      OpenCover");
  _server.returnNothing("", 0);
}
void AlpacaCoverCalibratorDevice::handleCoverStateGet() {
  _server.logRequest(__func__);
  if(!_cover) {
    _server.returnIntValue(static_cast<int>(CoverStatus::NotPresent), "", 0);
    return; 
  }
  bool isCoverOn = _cover->getState(0);
  CoverStatus coverState = isCoverOn ? CoverStatus::Open : CoverStatus::Closed;
  //Serial.println("CoverState: " + String(coverState));
  _server.returnIntValue(static_cast<int>(coverState), "", 0);
}
void AlpacaCoverCalibratorDevice::handleCalibratorStateGet() {
  _server.logRequest(__func__);
  if(!_calibration) {
    _server.returnIntValue(static_cast<int>(CalibratorStatus::NotPresent), "", 0);
    return;  
  }
  bool isCalibratorOn = _calibration->getState(0);
  CalibratorStatus calibratorState = isCalibratorOn ? CalibratorStatus::Ready : CalibratorStatus::Off;
  //Serial.println("ColibratorState: " + String(calibratorState));
  _server.returnIntValue(static_cast<int>(calibratorState), "", 0);
}

void AlpacaCoverCalibratorDevice::handleMaxBrightnessGet() {
  _server.logRequest(__func__);
  _server.returnIntValue(_maxBrightness, "", 0);
}
void AlpacaCoverCalibratorDevice::handleBrightnessGet() {
  _server.logRequest(__func__);
  int brightness = _calibration->getState(0)?100:0;
  _server.returnIntValue(brightness, "", 0);
}
void AlpacaCoverCalibratorDevice::handleCalibratorOffPut() {
  _server.logRequest(__func__);
  _calibration->setState(0, false);
  Serial.println("      CalibratorOff");
  _server.returnNothing("", 0);
}
void AlpacaCoverCalibratorDevice::handleCalibratorOnPut() {
  _server.logRequest(__func__);
  int brightness = _server.getArgCaseInsensitive("brightness").toInt();
  _server.logArg("Brightness", brightness); 
  _calibration->setState(0, brightness==100);
  Serial.println("      CalibratorOn");
  Serial.println("      brightness: "+String(brightness));
  _server.returnNothing("", 0);
}
