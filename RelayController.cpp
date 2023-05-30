#include "RelayController.h"
#include <EEPROM.h>

RelayController::RelayController(int numRelays) {
  _numRelays = numRelays;
  _relayStates = new bool[_numRelays];
  _relayNames = new String[_numRelays];
  _relayDescriptions = new String[_numRelays];
  setAllRelayStates(false),
  _connected = false;

  EEPROM.begin(_numRelays * 50 + 2);  // +2 for the config version and number of relays bytes
  loadOrInitializeEEPROM();
}
void RelayController::loadOrInitializeEEPROM() {
  if (EEPROM.read(0) != EEPROM_SCHEMA_VERSION || EEPROM.read(1) != _numRelays) {
    // Config version or number of relays doesn't match, initialize with default values
    Serial.println("Initialize relay name and EEPROM");
    for (int i = 0; i < _numRelays; i++) {
      setRelayState(i, false);
      setName(i, String("Relay ") + (i+1));
      _relayDescriptions[i] = String("Relay ") + (i+1);
    }
    EEPROM.write(0, EEPROM_SCHEMA_VERSION);  // Write the current config version
    EEPROM.write(1, _numRelays);  // Write the current number of relays
    EEPROM.commit();
  } else {
    // Config version and number of relays match, load the states and names from EEPROM
    Serial.println("Load relay name and status from EEPROM");
    loadStateFromEEPROM();
  }
}
void RelayController::loadStateFromEEPROM() {
  Serial.println("Load state of relay");
  for (int i = 0; i < _numRelays; i++) {
    _relayStates[i] = EEPROM.read(2 + i) == 1;
    sendCommand(i, _relayStates[i]?1:0);
    Serial.print("  Relay ");
    Serial.print(i);
    Serial.print(" -> ");
    Serial.println(_relayStates[i]);
 

    _relayNames[i] = "";
    for (int j = 0; j < 50; j++) {
      char c = EEPROM.read(2 + _numRelays + i*50 + j);
      if (c == 0) {
        break;
      }
      _relayNames[i] += c;
    }
  }
}
bool RelayController::is_connected() {
  return _connected;
}
void RelayController::do_connect(bool status) {
  _connected = status;
}
RelayController::~RelayController() {
  delete[] _relayStates;
  delete[] _relayNames;
  delete[] _relayDescriptions;
  EEPROM.end();
}
int RelayController::getMaxSwitch() {
  return _numRelays;
}

void RelayController::setRelayState(int relayNum, bool state) {
  if (relayNum < 0 || relayNum >= _numRelays) {
    Serial.print("Invalid relay number: ");
    Serial.println(relayNum);
    return;
  }
  
  if (state == _relayStates[relayNum]) {
    // State hasn't changed, no need to send command
    return;
  }
  
  _relayStates[relayNum] = state;
  sendCommand(relayNum, state?1:0);
  EEPROM.write(2 + relayNum, state ? 1 : 0);
  EEPROM.commit();
}

bool RelayController::getRelayState(int relayNum) {
  if (relayNum < 0 || relayNum >= _numRelays) {
    Serial.print("Invalid relay number: ");
    Serial.println(relayNum);
    return false;
  }
  
  return _relayStates[relayNum];
}

void RelayController::setAllRelayStates(bool state) {
  for (int i = 0; i < _numRelays; i++) {
    _relayStates[i] = state;
    EEPROM.write(2 + i, state ? 1 : 0);
  }
  EEPROM.commit();
}

void RelayController::sendCommand(int relay, int value) {
  byte payload[4];
  payload[0] = 0xA0;
  payload[1] = relay + 1;
  payload[2] = value;
  payload[3] = payload[0] + payload[1] + payload[2];
  // Give some time to the watchdog
  ESP.wdtFeed();
  yield();
  
  Serial.write(payload, sizeof(payload));
}
String RelayController::getName(int relay) {
  if (relay < 0 || relay >= _numRelays) {
    Serial.print("Invalid relay number: ");
    Serial.println(relay);
    return "Invalid relay number";
  }
  return _relayNames[relay];
}
void RelayController::setName(int relay, String value) {
  if (relay < 0 || relay >= _numRelays) {
    Serial.print("Invalid relay number: ");
    Serial.println(relay);
    return ;
  }
  _relayNames[relay] = value;

  for (int i = 0; i < 50; i++) {
    if (i < value.length()) {
      EEPROM.write(2 + _numRelays + relay*50 + i, value[i]);
    } else {
      EEPROM.write(2 + _numRelays + relay*50 + i, 0);
    }
  }
  EEPROM.commit();
}
