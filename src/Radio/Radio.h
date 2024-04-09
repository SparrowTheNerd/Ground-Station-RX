#include <Arduino.h>
#ifndef RADIO_H
#define RADIO_H

class Radio {
public:
    void init();
    void rx();
    struct packet {
        float timeStamp, tankPrs, combnPrs, force;
        uint8_t status;
    } dataPacket;

private:
    // Private members
    void pinModeAF(int ulPin, uint32_t Alternate);
};

#endif // RADIO_H
