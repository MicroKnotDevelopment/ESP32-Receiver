/**
 * SX1280
 */

#define SX1280_NSS 17// (D10)
#define SX1280_DI01 A4
#define SX1280_NRST 16// (D11)
#define SX1280_BUSY A3

float frequency     = 2400.0;
int bitRate         = 520;
int codingRate      = 2;
int outputPower     = 8;
float dataShaping   = 1.0;
uint8_t syncWord[]  = {0x01, 0x23, 0x45, 0x67};
int crcValue        = 0;
