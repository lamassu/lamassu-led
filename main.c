#include <signal.h>
#include <sys/time.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "pulse.h"

static void Handler (void);

int main (int argc, char **argv) {
  struct itimerval timer;

  struct sched_param priority;
  priority.sched_priority = 10;

  // sched_setscheduler(0, SCHED_FIFO, &priority);

  if (signal(SIGALRM, (void (*)(int)) Handler) == SIG_ERR) {
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

  color pulse_color;
  pulse_color.type = COLOR_HSL;
  pulse_color.HSL.H = 4.9833;
  pulse_color.HSL.S = 0.84;
  pulse_color.HSL.L = 0.53;

  StartPulse(&pulse_color);

  while (1)
    pause();
}

static void Handler (void) {
  static int count = 0;

  led_color frame_color;
  FrameUpdate(&frame_color);

  count++;

  if (count == 3) {
    printf("transition color\n");

    color pulse_color;
    pulse_color.type = COLOR_HSL;
    pulse_color.HSL.H = 3.10000;
    pulse_color.HSL.S = 0.84;
    pulse_color.HSL.L = 0.53;

    StartPulse(&pulse_color);
  }

  printf("#%.2x%.2x%.2x%.2x\n", frame_color.r, frame_color.g, frame_color.b, frame_color.a);
}
