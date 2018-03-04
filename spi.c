#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "spi.h"

#define ARRAY_SIZE(array) sizeof(array)/sizeof(array[0])

static uint8_t mode = 3;
static uint8_t bits = 8;
static uint32_t speed = 256000;
static uint16_t delay;

static void pabort(const char *s) {
	perror(s);
	abort();
}

void open(const char *spi_dev) {
	fd = open(spi_dev, O_RDWR);
	if (fd<=0) {
		printf("%s: Device %s not found\n", argv[0], argv[1]);
		exit(1);
	}

	// spi mode
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	// bits per word
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	// max speed hz
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");
}

void light(uint8_t r, uint8_t g, uint8_t b, uint8_t a, unsigned char leds_buf[]) {
	int i, fd, ret;

  const leds_buf_size = kNumLeds * 4 + 8;

  unsigned char wr_buf[leds_buf_size];
  unsigned char start_buf[4] = {0x0, 0x0, 0x0, 0x0};
  unsigned char end_buf[4] = {0xff, 0xff, 0xff, 0xff};

  memcpy(wr_buf, start_buf, 4);
  int end_offset = 4 + num_leds * 4;
  memcpy(wr_buf + end_offset, end_buf, 4);

	if (write(fd, wr_buf, ARRAY_SIZE(wr_buf)) != ARRAY_SIZE(wr_buf)) {
		perror("Write Error");
	}

	return 0;
}
