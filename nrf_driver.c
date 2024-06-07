#include "nrf_driver.h"

uint8_t nrf_status;
int addr_len;

//TODO: Addr width init
//TODO: Data width inti
//TODO: Channel init
void nrf_initialize() {
	nrf_power_on();
}

void nrf_power_on() {
	uint8_t config;
	nrf_get_config(&nrf_status, &config);

	//Set the PWR_UP bit
	config |= 0b00000010;
	nrf_set_config(&nrf_status, &config);
}

void nrf_power_off() {
	uint8_t config;
	nrf_get_config(&nrf_status, &config);

	//Clear the PWR_UP bit
	config &= 0b11111101;
	nrf_set_config(&nrf_status, &config);
}

void nrf_start_tx() {
	nrf_set_rx_bit(false);
	set_ce(true);
}

void nrf_stop_tx() {
	//Turn off CE so that we can perform register writes if still in TX Mode
	set_ce(false);

	// //Go to power down state to ensure we can exit both TX Mode and Standy-II
	// nrf_power_off();
}

void nrf_start_rx() {
	nrf_set_rx_bit(true);
	set_ce(true);
}

void nrf_stop_rx() {
	//Turn off CE and go into Stanby-I
	set_ce(false);
}

void nrf_set_rx_bit(bool bit) {
	uint8_t config;
	nrf_get_config(&nrf_status, &config);

	config |= bit ? 0b00000001 : 0b00000000;
	nrf_set_config(&nrf_status, &config);
}

void nrf_get_config(uint8_t* status_dst, uint8_t* config_dst) {
	nrf_rd_cmd(REG_RD_CMD | REG_CONFIG, status_dst, config_dst, 1);
}

void nrf_set_config(uint8_t* status_dst, uint8_t* config_src) {
	nrf_wr_cmd(REG_WR_CMD | REG_CONFIG, status_dst, config_src, 1);
}

void nrf_get_fifo_status(uint8_t* status_dst, uint8_t* fifo_status_dst) {
	nrf_rd_cmd(REG_RD_CMD | REG_FIFO_STATUS, status_dst, fifo_status_dst, 1);
}

void nrf_push_tx_fifo(uint8_t* data_src, int len) {
	nrf_wr_cmd(W_TX_PAYLOAD_CMD, &nrf_status, data_src, len);
}

//TODO: Utilize actual address width set in SETUP_AW
void nrf_get_tx_addr(uint8_t* status_dst, uint8_t* addr_dst) {
	nrf_rd_cmd(REG_RD_CMD | REG_TX_ADDR, status_dst, addr_dst, 5);
}

//TODO: Utilize actual address width set in SETUP_AW
void nrf_set_tx_addr(uint8_t* status_dst, uint8_t* addr_src, int len) {
	nrf_wr_cmd(REG_WR_CMD | REG_TX_ADDR, status_dst, addr_src, len);
}

void nrf_rd_cmd(uint8_t cmd, uint8_t* status_dst, uint8_t* data_dst, int len) {
	//Create a buffer for the command so the user doesn't have to
	// uint8_t cmd_buf[1];
	// cmd_buf[0] = cmd;

	set_csn(false);

	// spi_write_read_blocking(spi_default, cmd_buf, status_dst, 1);
	spi_write_read_blocking(SPI_ID, &cmd, status_dst, 1);
	spi_read_blocking(SPI_ID, 0, data_dst, len);

	set_csn(true);

	//Flip the bytes
	// normalize_data(data_dst, len);
}

void nrf_wr_cmd(uint8_t cmd, uint8_t* status_dst, uint8_t* data_src, int len) {
	set_csn(false);

	spi_write_read_blocking(SPI_ID, &cmd, status_dst, 1);
	spi_write_blocking(SPI_ID, data_src, len);

	set_csn(true);
}

void nrf_cmd(uint8_t cmd, uint8_t* status_dst) {
	//Create a buffer for the command so the user doesn't have to
	// uint8_t cmd_buf[1];
	// cmd_buf[0] = cmd;

	set_csn(false);

	// spi_write_read_blocking(spi_default, cmd_buf, status_dst, 1);
	spi_write_read_blocking(SPI_ID, &cmd, status_dst, 1);

	set_csn(true);
}

void set_csn(bool val) {
	gpio_put(CSN_PIN, val);
}

void set_ce(bool val) {
	gpio_put(CE_PIN, val);
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
