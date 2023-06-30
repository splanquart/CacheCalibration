#ifndef ALPACACOVERCALIBRATORDEVICE_H
#define ALPACACOVERCALIBRATORDEVICE_H

#include "AlpacaDevice.h"
#include "ISwitch.h"

enum class CoverStatus {
  NotPresent = 0, 
  Closed     = 1, 
  Moving     = 2, 
  Open       = 3, 
  Unknown    = 4,
  Error      = 5
};

enum class CalibratorStatus {
    NotPresent = 0,
    Off = 1,
    NotReady = 2,
    Ready = 3,
    Unknown = 4,
    Error = 5
};

class AlpacaCoverCalibratorDevice : public AlpacaDevice {
public:
  AlpacaCoverCalibratorDevice(HttpHandler& httpHandler, int deviceId, ISwitch* coverController, ISwitch* calibrationLampController);

  void begin() override;
  void _doConnect(bool connected) override;
  bool _isConnected() override;
 
  /*
   * Implement route about Alapaca setup
   * https://ascom-standards.org/api/?urls.primaryName=ASCOM%20Alpaca%20Management%20API#/HTML%20Browser%20User%20Interface
   */
  void handleSetupdevice();
  
  void handleSupportedActionsGet();

  /*
   * Implemenre route about Alpaca CoverCalibrator
   * https://ascom-standards.org/api/?urls.primaryName=ASCOM%20Alpaca%20Device%20API#/CoverCalibrator%20Specific%20Methods
   */
  void handleCloseCoverPut();
  void handleOpenCoverPut();
  void handleCalibratorStateGet();
  void handleCoverStateGet();

  void handleMaxBrightnessGet();
  void handleBrightnessGet();
  void handleCalibratorOffPut();
  void handleCalibratorOnPut();

protected:
  ISwitch* _cover;
  ISwitch* _calibration;
  int _maxBrightness;
};

#endif // ALPACACOVERCALIBRATORDEVICE_H
