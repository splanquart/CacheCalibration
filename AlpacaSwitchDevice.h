#ifndef ALPACA_SWITCH_DEVICE_H
#define ALPACA_SWITCH_DEVICE_H

#include "AlpacaDevice.h"
#include "RelayController.h"

class AlpacaSwitchDevice : public AlpacaDevice {
public:
  AlpacaSwitchDevice(HttpHandler& server, int deviceNumber, RelayController* relayController);

  // Override generic methods from AlpacaDevice
  void begin() override;


  /*
   * Implement route about Alapaca setup
   * https://ascom-standards.org/api/?urls.primaryName=ASCOM%20Alpaca%20Management%20API#/HTML%20Browser%20User%20Interface
   */
  void handleSetupdevice();

  /*
   * Implement routes about Alpaca device API
   * https://ascom-standards.org/api/?urls.primaryName=ASCOM%20Alpaca%20Device%20API#/
   */
  void handleMaxswitchGet();
  void handleSupportedactionsGet();
  void handleCanwritesGet();
  void handleSwitchnameGet();
  void handleSwitchGet();
  void handleSwitchdescriptionGet();
  void handleSwitchvalueGet();
  void handleMaxswitchvalueGet();
  void handleMinswitchvalueGet();
  void handleSwitchstepGet();
  void handleSwitchvaluePut();
  void handleSwitchnamePut();
  void handleSwitchPut();

protected:
  RelayController* _controller;

  void _doConnect(bool connected) override;
  bool _isConnected() override;


};

#endif // ALPACA_SWITCH_DEVICE_H
