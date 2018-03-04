#include <stdint.h>

const int kNumLeds = 26;

void OpenSpi(const char *spi_dev);
void Light(uint8_t r, uint8_t g, uint8_t b, uint8_t a, unsigned char leds_buf[]);
