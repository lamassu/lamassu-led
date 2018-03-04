#include <signal.h>
#include <sys/time.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "pulse.h"

static void Handler (void);
static void ProcessCommand(void);

int main (int argc, char **argv) {
  struct itimerval timer;

  struct sched_param priority;
  priority.sched_priority = 10;

  setvbuf(stdin, NULL, _IOLBF, 256);
  setvbuf(stdout, NULL, _IOLBF, 256);

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

  while (1) {
    ProcessCommand();
  }
}

static const unsigned int kStartPulse = 0x0;

static void ProcessCommand(void) {
  char *line_ptr = NULL;
  size_t n = 0;

  unsigned int cmd, start_led, end_led, r, g, b;

  int res = scanf("%2x%2x%2x%2x%2x%2x", &cmd, &start_led, &end_led, &r, &g, &b);

  if (res != 6) return;

  switch (cmd) {
    case kStartPulse:
      StartPulse(r, g, b, start_led, end_led);
      break;
  }
}

static void Handler (void) {
  unsigned char frame[26*4];
  FrameUpdate(frame);

  for (int i = 0; i < 26; i++) {
    printf("#%.2x%.2x%.2x%.2x\n", frame[i*4], frame[i*4+1], frame[i*4+2], frame[i*4+3]);
  }

  printf("\n");
}
