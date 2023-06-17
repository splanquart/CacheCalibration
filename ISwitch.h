#ifndef ISWITCH_H
#define ISWITCH_H

class ISwitch {
public:
    virtual ~ISwitch() {}

    virtual bool getState(int relayNum) const = 0;
    virtual void setState(int relayNum, bool state) = 0;
    virtual bool isConnected() const = 0;
    virtual void connect(bool status = true) = 0;
    virtual String getName(int relayNum) const = 0;
    virtual void setName(int relayNum, const String value) = 0;
    virtual int getMaxSwitch() const = 0;
};
#endif // ISWITCH_H
