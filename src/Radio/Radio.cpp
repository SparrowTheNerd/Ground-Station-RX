#include "Radio.h"
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <RH_RF95.h>

#define CS PC2
#define INT PC12

RHHardwareSPI spi;
RH_RF95 rf95(CS, INT, spi);

void Radio::init() {
  pinMode(PC0, OUTPUT); pinMode(PC1, OUTPUT);
  digitalWrite(PC0, HIGH); digitalWrite(PC1, HIGH); //pull other chip selects high
  pinModeAF(PB4,GPIO_AF5_SPI1); SPI.setMISO(PB4);
  pinModeAF(PB5,GPIO_AF5_SPI1); SPI.setMOSI(PB5);
  pinModeAF(PB3,GPIO_AF5_SPI1); SPI.setSCLK(PB3);
  spi.begin();

  rf95.init();
  rf95.setFrequency(915.0); //set frequency to 915MHz
  rf95.setTxPower(20,false); //set the transmit power to 20dBm using PA_BOOST
  rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);
}

void Radio::rx() {
   if (rf95.available()) {
      // Should be a message for us now
      uint8_t buf[sizeof(dataPacket)];
      uint8_t len = sizeof(buf);
      if (rf95.recv(buf, &len)) {
      if (!len) return;
      buf[len] = 0;
      memcpy(&dataPacket,buf,sizeof(dataPacket));
      Serial.print(dataPacket.tankPrs,5); Serial.print(", "); Serial.print(dataPacket.combnPrs,5); Serial.print(", "); Serial.println(dataPacket.force,5);

      } else { Serial.println("Receive failed"); }
  }
}

void Radio::pinModeAF(int ulPin, uint32_t Alternate) {
   int pn = digitalPinToPinName(ulPin);

   if (STM_PIN(pn) < 8) {
      LL_GPIO_SetAFPin_0_7( get_GPIO_Port(STM_PORT(pn)), STM_LL_GPIO_PIN(pn), Alternate);
   } else {
      LL_GPIO_SetAFPin_8_15(get_GPIO_Port(STM_PORT(pn)), STM_LL_GPIO_PIN(pn), Alternate);
   }

   LL_GPIO_SetPinMode(get_GPIO_Port(STM_PORT(pn)), STM_LL_GPIO_PIN(pn), LL_GPIO_MODE_ALTERNATE);
}