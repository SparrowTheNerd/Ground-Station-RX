#include <Arduino.h>
#ifndef RADIO_H
#define RADIO_H

class Radio {
public:
    void init();
    void rx();
    struct packet {
        float tankPrs, combnPrs, force, timeStamp;
    } dataPacket;

private:
    // Private members
    void pinModeAF(int ulPin, uint32_t Alternate);
};

#endif // RADIO_H
