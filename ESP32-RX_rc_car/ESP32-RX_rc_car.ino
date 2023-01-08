// include the library
#include <RadioLib.h>
#include <ESP32Servo.h>
#include "config.h"

Servo rcServo;
Servo dcMotor;

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

int servoPosition = 127,
    dcPosition = 127;

/**
 * SX1280
 */
SX1280 radio = new Module(SX1280_NSS, SX1280_DI01, SX1280_NRST, SX1280_BUSY);

static int taskCore1 = 0;
static int taskCore2 = 1;
TaskHandle_t task_rxSetData;
TaskHandle_t task_rxControl;

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

void func_rxSetData( void * pvParameters ){
  while(true){
    if(receivedFlag) {
      enableInterrupt = false;
  
      // reset flag
      receivedFlag = false;
  
      byte byteArr[33];
      int state = radio.readData(byteArr, 33);
  
      if (state == RADIOLIB_ERR_NONE) {
        //  Servo Move
        // Left Joystick
        // byteArr[0] = left-right
        // byteArr[1] = up-down
      
        //  Right Joystick
        // byteArr[2] = left-right
        // byteArr[3] = up-down

        servoPosition = byteArr[2];
        dcPosition = byteArr[1];
      } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
        // packet was received, but is malformed
        //Serial.println(F("CRC error!"));
      } else {
        servoPosition = 127;
        dcPosition = 127;
      }
  
      radio.startReceive();
      enableInterrupt = true;
    }
  }
}

void func_rxControl( void * pvParameters ){
    while(true){
      servoMove(servoPosition);
      dcMove(dcPosition);
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

  // Attach the motor
  dcMotor.attach(DC_MOTOR_PIN);
  
  // Set the output to the middle or "zero" position. CONNECT THE ESC DURING THIS DELAY!!
  dcMotor.write(zeroThrottle);

  // attach the servo
  rcServo.attach(SERVO_PIN);

  xTaskCreatePinnedToCore(
    func_rxSetData,   // Function to implement the task
    "task_rxSetData", // Name of the task
    10000,            // Stack size in words
    NULL,             // Task input parameter
    0,                // Priority of the task
    &task_rxSetData,  // Task handle
    taskCore1         // Core where the task should run
  );
 
  xTaskCreatePinnedToCore(
    func_rxControl,    // Function to implement the task
    "task_rxControl",  // Name of the task 
    10000,              // Stack size in words 
    NULL,               // Task input parameter
    1,                  // Priority of the task
    &task_rxControl,   // Task handle
    taskCore2           // Core where the task should run
  );
  
}

/**
 * DC Motor move
 */
void dcMove(int acceleration) {
  if (acceleration > defaultValue) {
    dcMoveVal = map(acceleration, 127, 255, zeroThrottle, maxThrottle);
    rcDirection = 1;
  }
  else if (acceleration < (defaultValue-13)) {
    dcMoveVal = map(acceleration, 0, defaultValue, minThrottleReverse, maxThrottleReverse);
    
    if(rcDirection==1) {
      dcMotor.write(zeroThrottle-10);
      delay(45);
      dcMotor.write(zeroThrottle);
      delay(35);
    }
    
    rcDirection = 0;

  } else {
    dcMoveVal = zeroThrottle;
    rcDirection = 1;
  }

  if(currentDcValue!=dcMoveVal) {
    currentDcValue = dcMoveVal;
    dcMotor.write(dcMoveVal);
  }
}

// Servo Move
void servoMove(int servoDirection) {
  /**
   * Default Value
   */
  servoPos = 90;

  /**
   * Right
   */
  if (servoDirection > defaultValue) {
    servoPos = map(servoDirection, defaultValue, 0, defaultServoPosition, minServoPosition);
  }

  /**
   * Left
   */
  if (servoDirection < defaultValue) {
    servoPos = map(servoDirection, defaultValue, 255, defaultServoPosition, maxServoPosition);
  }

  rcServo.write(servoPos);
}

void loop() {
}
