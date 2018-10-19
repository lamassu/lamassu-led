#include "color.h"

struct pulse_info {
  unsigned char r, g, b, first_led, last_led;
  int pulse_period, fade_period, solid_period, off_period, transition_period
};

typedef struct pulse_info pulse_info;

void StartPulse(unsigned char r, unsigned char g, unsigned char b, unsigned char first_led, unsigned char last_led);
void FrameUpdate(unsigned char leds_buf[]);
