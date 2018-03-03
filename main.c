#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sched.h>

#include "color.h"

struct timeval t0;
struct color col;

void DoStuff(void);

int main (int argc, char **argv) {
  struct itimerval timer;

  struct sched_param priority;
  priority.sched_priority = 10;

  col.HSL.H = 0.361;
  col.HSL.S = 1.0;
  col.HSL.L = 0.64;
  col.type = COLOR_HSL;

  // sched_setscheduler(0, SCHED_FIFO, &priority);
  gettimeofday(&t0, NULL);

  if (signal(SIGALRM, (void (*)(int)) DoStuff) == SIG_ERR) {
    perror("Unable to catch SIGALRM");
    exit(1);
  }

  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 10000;
  timer.it_interval = timer.it_value;

  if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
    perror("error calling setitimer()");
    exit(1);
  }

  while (1)
    pause();
}

int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y) {
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

const int SECOND = 1000000;
const int PULSE_PERIOD = 4 * SECOND;
const int FADE_PERIOD = 1 * SECOND;
const int SOLID_PERIOD = 1 * SECOND;
const int OFF_PERIOD = 1 * SECOND;

void computeColor (struct color *c, float fraction) {
  c->HSL.L = c->HSL.L * fraction;
  color_HSL_to_RGB(c);
}

void DoStuff (void) {
  struct timeval t1;
  struct timeval result;

  gettimeofday(&t1, NULL);
  timeval_subtract(&result, &t1, &t0);

  long micros = result.tv_sec * 1000000 + result.tv_usec;
  long period = micros % PULSE_PERIOD;

  if (period < FADE_PERIOD) {
    float fraction = (float)period / FADE_PERIOD;
    struct color c = col;
    computeColor(&c, fraction);
    printf("fade-in: %.3f: #%02x%02x%02x\n", fraction, (int)(0xff * c.RGB.R), (int)(0xff * c.RGB.G), (int)(0xff * c.RGB.B));
    return;
  }

  period -= FADE_PERIOD;

  if (period < SOLID_PERIOD) {
    printf("solid\n");
    return;
  }

  period -= SOLID_PERIOD;

  if (period < FADE_PERIOD) {
    float fraction = 1.0 - (float)period / FADE_PERIOD;
    struct color c = col;
    computeColor(&c, fraction);
    printf("fade-out: %.3f: #%02x%02x%02x\n", fraction, (int)(0xff * c.RGB.R), (int)(0xff * c.RGB.G), (int)(0xff * c.RGB.B));
    return;
  }

  printf("off\n");
}
