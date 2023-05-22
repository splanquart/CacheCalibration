#ifndef RELAYCONTROLLER_H
#define RELAYCONTROLLER_H

#define EEPROM_SCHEMA_VERSION 1  // Increase this whenever you change the number of relays or the format of the stored data

#include <Arduino.h>

class RelayController {
public:
  RelayController(int numRelays);
  ~RelayController();

  void setRelayState(int relayNum, bool state);
  bool getRelayState(int relayNum);
  bool is_connected();
  void do_connect(bool status=true);
  int getMaxSwitch();
  String getName(int relay);
  void setName(int relay, String value);

private:
  int _numRelays;
  bool* _relayStates;
  bool _connected;
  String* _relayNames;
  String* _relayDescriptions;

  void loadOrInitializeEEPROM();
  void loadStateFromEEPROM();
  void setAllRelayStates(bool state);
  void sendCommand(int relay, int value);
};

#endif // RELAYCONTROLLER_H
