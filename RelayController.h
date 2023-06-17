#ifndef RELAYCONTROLLER_H
#define RELAYCONTROLLER_H

#define EEPROM_SCHEMA_VERSION 1  // Increase this whenever you change the number of relays or the format of the stored data

#include <Arduino.h>
#include "ISwitch.h"

class RelayController : public ISwitch {
public:
  RelayController(int numRelays);
  ~RelayController();

  void setState(int relayNum, bool state);
  bool getState(int relayNum) const override;
  bool isConnected() const override;
  void connect(bool status=true) override;
  int getMaxSwitch() const override;
  String getName(int relay) const override;
  void setName(int relay, String value) override;

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
