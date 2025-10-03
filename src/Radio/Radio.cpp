#include "Radio.h"
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>

#define CS PC2
#define INT PC12

#define CLIENT_ADDRESS 17
#define SERVER_ADDRESS 41

RHHardwareSPI spi;
RH_RF95 rf95(CS, INT, spi);
RHReliableDatagram manager(rf95, SERVER_ADDRESS); //manager on address 2

uint32_t startTime;

void Radio::init() {
   pinMode(PC0, OUTPUT); pinMode(PC1, OUTPUT);
   digitalWrite(PC0, HIGH); digitalWrite(PC1, HIGH); //pull other chip selects high
   pinModeAF(PB4,GPIO_AF5_SPI1); SPI.setMISO(PB4);
   pinModeAF(PB5,GPIO_AF5_SPI1); SPI.setMOSI(PB5);
   pinModeAF(PB3,GPIO_AF5_SPI1); SPI.setSCLK(PB3);
   // spi.setFrequency(RHGenericSPI::Frequency8MHz);
   SPI.setClockDivider(SPI_CLOCK_DIV8);
   SPI.begin();
   

   // rf95.init();
   manager.init();
   rf95.setFrequency(924); //set frequency to 915MHz
   rf95.setTxPower(5,false); //set the transmit power to 20dBm using PA_BOOST
   rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);
   Serial.println("Radio initialized");
}

uint8_t data[] = "SERVER PING";
uint8_t buf[20];
uint8_t idx = 0;
void Radio::rx() {
   uint8_t len = sizeof(buf);
   uint8_t from;
   if (manager.available()) {
    // Wait for a message addressed to us from the client
    if (manager.recvfromAck(buf, &len, &from))
    {
      
      if (from != CLIENT_ADDRESS) return; //not the client we want
      memcpy(&dataPacket,buf,sizeof(dataPacket)-sizeof(int16_t));
      dataPacket.rssi = rf95.lastRssi();  
      Serial.write((const char*)&dataPacket,sizeof(dataPacket));
      // Serial.printf("RSSI: %d\n",dataPacket.rssi);
      // idx++;      
      // if(idx > 50) { //periodically maintain 2 way link
      if (Serial.available()) {
         uint8_t c = Serial.read();
         manager.sendtoWait(&c, 1, CLIENT_ADDRESS);
         
      }
      else manager.sendtoWait(data, sizeof(data), CLIENT_ADDRESS);
         // Serial.println("NACK");
      // }
      idx = 0;
         // Serial.println("PING" );
      // }
      // Serial.printf("%.3f,%.3f,%.3f,%.3f\n",dataPacket.timeStamp,dataPacket.tankPrs,dataPacket.combnPrs,dataPacket.force);
    }
  }
//   if(Serial.available()) {
//       char c = Serial.read();
//       if(c=='A') {
//          manager.sendtoWait((uint8_t *)c, 1, CLIENT_ADDRESS);
//       }
//   }
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