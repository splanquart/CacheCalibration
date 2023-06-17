#ifndef RELAY_ADAPTER_H
#define RELAY_ADAPTER_H

#include <Arduino.h>
#include "ISwitch.h"
#include "RelayController.h"

class RelayAdapter : public ISwitch {
public:
  RelayAdapter(RelayController* controller, std::vector<int> relayIndices);

  bool getState(int relayNum) const override;
  void setState(int relayNum, bool state) override;
  bool isConnected() const override;
  void connect(bool status = true) override;
  String getName(int relayNum) const override;
  void setName(int relayNum, const String value) override;
  int getMaxSwitch() const override;

private:
  RelayController* _controller;
  std::vector<int> _relayIndices;
};

#endif // RELAY_ADAPTER_H
