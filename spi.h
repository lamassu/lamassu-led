#include <stdint.h>

const int kNumLeds = 26;

void open(const char *spi_dev);
void light(uint8_t r, uint8_t g, uint8_t b, uint8_t a, unsigned char leds_buf[]);
