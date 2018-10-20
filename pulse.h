#include "color.h"

void SolidFrame(unsigned char r, unsigned char g, unsigned char b, unsigned char first_led, unsigned char last_led, unsigned char leds_buf[]);
void StartPulse(unsigned char r, unsigned char g, unsigned char b, unsigned char first_led, unsigned char last_led);
void FrameUpdate(unsigned char leds_buf[]);
