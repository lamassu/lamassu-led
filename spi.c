#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/spi/spidev.h>

#include "spi.h"

#define ARRAY_SIZE(array) sizeof(array)/sizeof(array[0])

static uint8_t mode = 3;
static uint8_t bits = 8;
static uint32_t speed = 256000;
static int fd;

static void pabort(const char *s, int exit_code) {
	perror(s);
	exit(exit_code);
}

void OpenSpi(const char *spi_dev) {
	int ret;
	fd = open(spi_dev, O_RDWR);

	if (fd<=0)
		pabort("can't open spi device", 1);

	// spi mode
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode", 2);

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode", 2);

	// bits per word
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word", 2);

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word", 2);

	// max speed hz
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz", 2);

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz", 2);
}

void Light(unsigned char leds_buf[]) {
	const int kNumLeds = 26;
  const int leds_buf_size = kNumLeds * 4 + 8;

  unsigned char wr_buf[leds_buf_size];
  unsigned char start_buf[4] = {0x0, 0x0, 0x0, 0x0};
  unsigned char end_buf[4] = {0xff, 0xff, 0xff, 0xff};

  memcpy(wr_buf, start_buf, 4);
  int end_offset = 4 + kNumLeds * 4;
	memcpy(wr_buf + 4, leds_buf, kNumLeds * 4);
  memcpy(wr_buf + end_offset, end_buf, 4);

	if (write(fd, wr_buf, ARRAY_SIZE(wr_buf)) != ARRAY_SIZE(wr_buf)) {
		perror("Write Error");
	}
}
