#include <signal.h>
#include <sys/time.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "pulse.h"
#include "spi.h"

#define kStartPulse 0x0

static void Handler (int signal);
static void ProcessCommand(void);

int dry_run = 0;

int main (int argc, char **argv) {
  struct itimerval timer;

  struct sched_param priority;
  priority.sched_priority = 10;

  setvbuf(stdin, NULL, _IOLBF, 256);
  setvbuf(stdout, NULL, _IOLBF, 256);

  if (argc == 2) {
    dry_run = 1;
  }

  printf("%d\n", argc);
  sched_setscheduler(0, SCHED_FIFO, &priority);

  if (!dry_run) {
    OpenSpi("/dev/spidev1.0");
  }

  struct sigaction sa;
  sa.sa_handler = Handler;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGALRM, &sa, NULL) == -1) {
    perror("sigaction failure");
    exit(1);
  }

  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 10000;
  timer.it_interval = timer.it_value;

  if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
    perror("error calling setitimer()");
    exit(1);
  }

  StartPulse(0xff, 0x0, 0x0, 0x0, 0x30);

  while (1) {
    sleep(1);
  }
}

int count = 0;

static void Handler (int signal) {
  unsigned char frame[26*4];
  printf("DEBUG1: %d, %d\n", dry_run, count++);
  FrameUpdate(frame);

  if (!dry_run) {
    Light(frame);
  }

  for (int i = 0; i < 26; i++) {
    printf("#%.2x%.2x%.2x%.2x\n", frame[i*4], frame[i*4+1], frame[i*4+2], frame[i*4+3]);
  }

  printf("\n");
}
