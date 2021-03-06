#include <inttypes.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "pulse.h"
#include "spi.h"

#define kStartPulse 0x0
#define kNumLeds 26

static void Handler (int signal);
static void ExitHandler(int signal);
static void RenderFrame(unsigned char *frame);

int dry_run = 0;
volatile int exiting = 0;

unsigned char pulse, r, g, b, start_led, end_led;

int main (int argc, char **argv) {
  struct itimerval timer;

  struct sched_param priority;
  priority.sched_priority = 10;

  setvbuf(stdin, NULL, _IOLBF, 256);
  setvbuf(stdout, NULL, _IOLBF, 256);

  if (argc == 8) {
    dry_run = 1;
  }

  if (argc == 7 || argc == 8) {
    pulse = strtoumax(argv[1], NULL, 10);
    r = strtoumax(argv[2], NULL, 16);
    g = strtoumax(argv[3], NULL, 16);
    b = strtoumax(argv[4], NULL, 16);
    start_led = strtoumax(argv[5], NULL, 10);
    end_led = strtoumax(argv[6], NULL, 10);
  } else {
    printf("Usage: a.out <pulse> <r> <g> <b> <start_led> <end_led> [-d]\n");
    exit(5);
  }

  sched_setscheduler(0, SCHED_FIFO, &priority);

  if (!dry_run) {
    OpenSpi("/dev/spidev1.0");
  }

  struct sigaction sa;
  memset((void *) &sa, 0, sizeof(sa));

  sa.sa_handler = ExitHandler;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction failure");
    exit(1);
  }

  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    perror("sigaction failure");
    exit(1);
  }

  if (pulse) {

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

    StartPulse(r, g, b, start_led, end_led);
  } else {
    unsigned char frame[kNumLeds*4];
    SolidFrame(r, g, b, start_led, end_led, frame);
    RenderFrame(frame);
  }

  while (1) {
    pause();
  }
}

int count = 0;

static void Handler (int signal) {
  unsigned char frame[26*4];
  FrameUpdate(frame);
  RenderFrame(frame);
}

static void ExitHandler(int signal) {
  exiting = 1;

  if (!pulse) {
    unsigned char frame[26*4];
    SolidFrame(0, 0, 0, start_led, end_led, frame);
    RenderFrame(frame);
    exit(0);
  }
}

static void RenderFrame(unsigned char *frame) {
  int is_off = frame[start_led*4] == 0xe0 &&
    frame[start_led*4 + 1] + frame[start_led*4 + 2] + frame[start_led*4 + 3] == 0x00;

  int do_exit = exiting && is_off;

  if (!dry_run) {
    Light(frame);
  } else {
    printf("\nDEBUG1: %d, %d\n", dry_run, count++);
    for (int i = 0; i < 26; i++) {
      printf("#%.2x%.2x%.2x%.2x\n", frame[i*4], frame[i*4+1], frame[i*4+2], frame[i*4+3]);
      printf("#%.2x%.2x%.2x\n", r, g, b);
    }

    if (do_exit) printf("Exiting.\n");
    printf("\n");
  }

  if (do_exit) exit(0);
}
