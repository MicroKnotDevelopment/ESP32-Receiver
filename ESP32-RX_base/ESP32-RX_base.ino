// include the library
#include <RadioLib.h>

// include the config
#include "config.h"

SX1280 radio = new Module(SX1280_NSS, SX1280_DI01, SX1280_NRST, SX1280_BUSY);

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif
void setFlag(void) {
  // check if the interrupt is enabled
  if(!enableInterrupt) {
    return;
  }

  // we got a packet, set the flag
  receivedFlag = true;
}


void func_rxSetData(){
    if(receivedFlag) {
      enableInterrupt = false;
  
      // reset flag
      receivedFlag = false;
  
      byte byteArr[33];
      int state = radio.readData(byteArr, 33);
  
      if (state == RADIOLIB_ERR_NONE) {
          for(int i=0; i < 33; i++)
          {
            Serial.print(byteArr[i]);
            Serial.print("\t");
          }
          Serial.println();
      } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
        // packet was received, but is malformed
        Serial.println(F("CRC error!"));
      } else {
        // some other error occurred
        Serial.print(F("failed, code "));
        Serial.println(state);
      }
  
      radio.startReceive();
      enableInterrupt = true;
    }
}


void setup() {
  Serial.begin(115200);

  // initialize SX1280 with default settings
  Serial.print(F("[SX1280] Initializing ... "));

  int state = radio.beginFLRC();

  state = radio.setFrequency(frequency);
  state = radio.setBitRate(bitRate);
  state = radio.setCodingRate(codingRate);
  state = radio.setOutputPower(outputPower);
  state = radio.setDataShaping(dataShaping);
  state = radio.setSyncWord(syncWord, 4);
  state = radio.setCRC(crcValue);

  // set the function that will be called
  // when new packet is received
  radio.setDio1Action(setFlag);

  // start listening for LoRa packets
  Serial.print(F("[SX1280] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

}

void loop() {
  func_rxSetData();
}
