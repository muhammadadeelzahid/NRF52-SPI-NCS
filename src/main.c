/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <drivers/gpio.h>
#include <drivers/SPi.h>



#define SPI_BUFSIZE  8   //SPI Communication buffer size

uint8_t   spi_tx_buf[SPI_BUFSIZE]; // spi tx buffer 
uint8_t   spi_rx_buf[SPI_BUFSIZE]; // spi rx buffer


#define ADD_REG_WHO_AM_I				0x0F
#define UC_WHO_AM_I_DEFAULT_VALUE		0x3F

/* set read single command. Attention: command must be 0x3F at most */
#define SET_READ_SINGLE_CMD(x)			(x | 0x80)


void main(void)
{
	const struct device *spi_dev = DEVICE_DT_GET(DT_NODELABEL(spi1)) ;
	if (!device_is_ready(spi_dev)) {
		/* Device is not ready to use */
		printk("\r\nStop device not ready\r\n");
	}

	struct spi_cs_control chip = 
	{
		.gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0)),
		.gpio_pin = 17,
		.gpio_dt_flags = GPIO_ACTIVE_LOW,
    	.delay = 2
	};
	

	struct spi_config spi_cfg = {
				.frequency = 4000000,
				.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB |
				 SPI_MODE_CPOL | SPI_MODE_CPHA,
				.slave = 0,
				.cs = &chip 
	};





	struct spi_buf tx_buf_arr = {.buf = spi_tx_buf, .len=8};
	struct spi_buf_set tx = {.buffers = &tx_buf_arr, .count = 1};


	struct spi_buf rx_buf_arr = {.buf = spi_rx_buf, .len = 8};
	struct spi_buf_set rx  = {.buffers = &rx_buf_arr, .count = 1};


	spi_tx_buf[0] = SET_READ_SINGLE_CMD(ADD_REG_WHO_AM_I);
	int error = spi_transceive(spi_dev,&spi_cfg,&tx,&rx);
	if(error != 0){
		printk("SPI transceive error: %i\n", error);
	}

	if (spi_rx_buf[1] == UC_WHO_AM_I_DEFAULT_VALUE)
	{
		printk("LIS3DSH detected \r\n");
	}


}