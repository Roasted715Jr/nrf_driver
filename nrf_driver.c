#include "nrf_driver.h"




void nrf_rd_cmd(uint8_t cmd, uint8_t* status_dst, uint8_t* data_dst, int len) {
	//Create a buffer for the command so the user doesn't have to
	// uint8_t cmd_buf[1];
	// cmd_buf[0] = cmd;

	set_csn(0);

	// spi_write_read_blocking(spi_default, cmd_buf, status_dst, 1);
	spi_write_read_blocking(spi_default, &cmd, status_dst, 1);
	spi_read_blocking(spi_default, 0, data_dst, len);

	set_csn(1);

	//Flip the bytes
	// normalize_data(data_dst, len);
}

void nrf_wr_cmd(uint8_t cmd, uint8_t* status_dst, uint8_t* data_src, int len) {
	set_csn(0);

	spi_write_read_blocking(spi_default, &cmd, status_dst, 1);
	spi_write_blocking(spi_default, data_src, len);

	set_csn(1);
}

void nrf_cmd(uint8_t cmd, uint8_t* status_dst) {
	//Create a buffer for the command so the user doesn't have to
	// uint8_t cmd_buf[1];
	// cmd_buf[0] = cmd;

	set_csn(0);

	// spi_write_read_blocking(spi_default, cmd_buf, status_dst, 1);
	spi_write_read_blocking(spi_default, &cmd, status_dst, 1);

	set_csn(1);
}

void set_csn(int val) {
	gpio_put(PICO_DEFAULT_SPI_CSN_PIN, val);
}

//Flip the data to be MSByte first. When given from NRF, it's LSByte first
void normalize_data(uint8_t* data, int size) {
	//Only continue if we have more than 1 byte
	// if (size > 1) {
		uint8_t tmp[size];

		//Store the flipped data
		for (int i = 0; i < size; i++) {
			tmp[i] = data[size - i - 1];
		}

		//Put the data back in
		for (int i = 0; i < size; i++) {
			data[i] = tmp[i];
		}
	// }
}
