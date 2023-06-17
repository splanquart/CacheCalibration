#include "RelayAdapter.h"

RelayAdapter::RelayAdapter(RelayController* controller, std::vector<int> relayIndices) 
  : _controller(controller), _relayIndices(relayIndices) {}

bool RelayAdapter::getState(int relayNum) const {
  return _controller->getState(_relayIndices[relayNum]);
}

void RelayAdapter::setState(int relayNum, bool state) {
  _controller->setState(_relayIndices[relayNum], state);
}

bool RelayAdapter::isConnected() const {
  return _controller->isConnected();
}

void RelayAdapter::connect(bool status) {
  _controller->connect(status);
}

String RelayAdapter::getName(int relayNum) const {
  return _controller->getName(_relayIndices[relayNum]);
}

void RelayAdapter::setName(int relayNum, const String value) {
  _controller->setName(_relayIndices[relayNum], value);
}

int RelayAdapter::getMaxSwitch() const {
  return _relayIndices.size();
}
