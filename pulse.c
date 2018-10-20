#include <sys/time.h>
#include <stdio.h>
#include <string.h>

#include "pulse.h"

#define kSecond 1000000

struct timeval pulse_start_time, transition_start_time;
color color_a = {COLOR_NONE}, color_b = {COLOR_NONE};
color color_a_rgb = {COLOR_NONE}, color_b_rgb = {COLOR_NONE};
int range_first_led = -1, range_last_led = -1;

const int kPulsePeriod = 1 * kSecond;
const int kFadePeriod = kSecond / 8;
const int kSolidPeriod = kSecond / 4;
const int kOffPeriod = kSecond / 2;
const int kTransitionPeriod = kSecond / 2;
const int kNumLeds = 26;

const led_color kOffLedColor = {0, 0, 0, 0};

static void CreateRGB(unsigned char r, unsigned char g, unsigned char b, color *c) {
  c->type = COLOR_RGB;
  c->RGB.R = (double)r / 0xff;
  c->RGB.G = (double)g / 0xff;
  c->RGB.B = (double)b / 0xff;
}

static void SetColorA(color *c) {
  color_a = *c;
  color_a_rgb = *c;
  color_HSL_to_RGB(&color_a_rgb);
}

static void SetColorB(color *c) {
  color_b = *c;
  color_b_rgb = *c;
  color_HSL_to_RGB(&color_b_rgb);
}

static int TimevalSubtract (struct timeval *result, struct timeval *x, struct timeval *y) {
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }

  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

static float ComputeTransitionFraction(struct timeval *t1) {
  struct timeval result;

  if (color_b.type == COLOR_NONE) return 0.0;

  TimevalSubtract(&result, t1, &transition_start_time);
  long micros = result.tv_sec * 1000000 + result.tv_usec;

  double fraction = (double)micros / (double)kTransitionPeriod;
  fraction = fraction > 1.0 ? 1.0 : fraction;

  return (float)fraction;
}

static float MixNumber (float fraction, double value_a, double value_b) {
  float inverse_fraction = 1.0 - fraction;

  return inverse_fraction * value_a + fraction * value_b;
}

static void MixColors(float transition_fraction, color *color_result) {
  color_result->type = COLOR_RGB;
  color_result->RGB.R = MixNumber(transition_fraction, color_a_rgb.RGB.R, color_b_rgb.RGB.R);
  color_result->RGB.G = MixNumber(transition_fraction, color_a_rgb.RGB.G, color_b_rgb.RGB.G);
  color_result->RGB.B = MixNumber(transition_fraction, color_a_rgb.RGB.B, color_b_rgb.RGB.B);

  color_RGB_to_HSL(color_result);
}

static void ComputeColor(float pulse_fraction, float transition_fraction, led_color *led_result) {
  color color_result;

  if (color_b.type == COLOR_HSL) {
    MixColors(transition_fraction, &color_result);
  } else {
    color_result = color_a;
  }

  color_result.HSL.L = color_result.HSL.L * pulse_fraction;
  color_HSL_to_RGB(&color_result);

  led_result->r = color_result.RGB.R * 0xff;
  led_result->g = color_result.RGB.G * 0xff;
  led_result->b = color_result.RGB.B * 0xff;
  led_result->a = pulse_fraction * 0xff;
}

static void ComputePulse(struct timeval *t1, led_color *led_color) {
  struct timeval result;

  if (color_a.type == COLOR_NONE) {
    return;
  }

  TimevalSubtract(&result, t1, &pulse_start_time);
  long micros = result.tv_sec * 1000000 + result.tv_usec;
  long period = micros % kPulsePeriod;
  float transition_fraction = ComputeTransitionFraction(t1);

  if (period < kFadePeriod) {
    float pulse_fraction = (float)period / kFadePeriod;
    ComputeColor(pulse_fraction, transition_fraction, led_color);
    return;
  }

  period -= kFadePeriod;

  if (period < kSolidPeriod) {
    ComputeColor(1.0, transition_fraction, led_color);
    return;
  }

  period -= kSolidPeriod;

  if (period < kFadePeriod) {
    float pulse_fraction = 1.0 - (float)period / kFadePeriod;
    ComputeColor(pulse_fraction, transition_fraction, led_color);
    return;
  }

  *led_color = kOffLedColor;

  if (color_b.type == COLOR_HSL) {
    SetColorA(&color_b);
    color_b.type = COLOR_NONE;
  }
}

void StartPulse(unsigned char r, unsigned char g, unsigned char b, unsigned char first_led, unsigned char last_led) {
  range_first_led = first_led;
  range_last_led = last_led;

  color new_color;
  CreateRGB(r, g, b, &new_color);
  color_RGB_to_HSL(&new_color);

  if (color_a.type == COLOR_NONE) {
    SetColorA(&new_color);
    gettimeofday(&pulse_start_time, NULL);
  } else {
    SetColorB(&new_color);
    gettimeofday(&transition_start_time, NULL);
  }
}

void SolidFrame(unsigned char r, unsigned char g, unsigned char b, unsigned char first_led, unsigned char last_led, unsigned char leds_buf[]) {
  const unsigned char color_buf[4] = {0xff, b, g, r};
  static const unsigned char off_buf[4] = {0xe0, 0x0, 0x0, 0x0};

  for (int i = 0; i < kNumLeds; i++) {
    int offset = i * 4;
    if (i < first_led || i > last_led) {
      memcpy(leds_buf + offset, off_buf, 4);
    } else {
      memcpy(leds_buf + offset, color_buf, 4);
    }
  }
}

void FrameUpdate(unsigned char leds_buf[]) {
  struct timeval t1, elapsedPulse, elapsedTransition;
  led_color pulse_color;
  int leds_buf_size = kNumLeds * 4;

  gettimeofday(&t1, NULL);
  ComputePulse(&t1, &pulse_color);

  static const unsigned char off_buf[4] = {0xe0, 0x0, 0x0, 0x0};
  const unsigned char brightness = 0xe0 | (unsigned char)(((float)pulse_color.a / 255) * 31);
  const unsigned char color_buf[4] = {brightness, pulse_color.b, pulse_color.g, pulse_color.r};

  for (int i = 0; i < kNumLeds; i++) {
    int offset = i * 4;
    if (i < range_first_led || i > range_last_led) {
      memcpy(leds_buf + offset, off_buf, 4);
    } else {
      memcpy(leds_buf + offset, color_buf, 4);
    }
  }
}
