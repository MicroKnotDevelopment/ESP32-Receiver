/**
 * SX1280
 */
#define SX1280_NSS 17 // (D10)
#define SX1280_DI01 A3
#define SX1280_NRST 16 // (D11)
#define SX1280_BUSY A4

float frequency     = 2400.0;
int bitRate         = 520;
int codingRate      = 2;
int outputPower     = 8;
float dataShaping   = 1.0;
uint8_t syncWord[]  = {0x01, 0x23, 0x45, 0x67};
int crcValue        = 0;

/**
 * Servo & DC Motor
 */

#define SERVO_PIN 13 //D7
#define DC_MOTOR_PIN 2 //D9

// Acceleration
int zeroThrottle = 90, // From this value up going forward
    maxThrottle = 135, // Max throttle

// Reverse
    maxThrottleReverse = 85,  //from this value down is going into reverse
    minThrottleReverse = 50; // this is as low as we could go with the reverse

// Default Values
int defaultValue = 127, // default value
    rcDirection = 1; // 0 reverse, 1 forward

int currentDcValue = zeroThrottle,
    dcMoveVal = zeroThrottle;

// Servo Control
int servoPos,
    defaultServoPosition = 90,
    maxServoPosition = 145,
    minServoPosition = 35;
