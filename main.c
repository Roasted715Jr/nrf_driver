#include <stdio.h>
#include "nrf_driver.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/spi.h"
#include "pico/bootrom.h"

#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define BTN_PIN 2
// #define MODE_PIN 

// #define BUF_LEN 100
// uint8_t out_buf[BUF_LEN] = {0};
// uint8_t in_buf[BUF_LEN];

// uint8_t cmd;
// uint8_t status = 0;
uint8_t in_buf[5] = {0};
uint8_t out_buf[5];


void print_byte(uint8_t byte);
void print_debug(int len);

int main() {
	//Set up UART
	uart_init(uart_default, BAUD_RATE);
	uart_set_hw_flow(uart_default, false, false);
	uart_set_format(uart_default, DATA_BITS, STOP_BITS, PARITY);

	gpio_set_function(PICO_DEFAULT_UART_TX_PIN, GPIO_FUNC_UART);
	gpio_set_function(PICO_DEFAULT_UART_RX_PIN, GPIO_FUNC_UART);

	//Set up SPI
	spi_init(SPI_ID, 1000 * 1000); //1 MHz SCK

	gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
	gpio_set_function(MISO_PIN, GPIO_FUNC_SPI);
	gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
	//CSN needs to be software-controlled, otherwise it will go inactive between each byte (no bueno)
	// gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SIO);
	gpio_init(CSN_PIN);
	gpio_set_dir(CSN_PIN, GPIO_OUT);
	nrf_csn(1);
	gpio_init(CE_PIN);
	gpio_set_dir(CE_PIN, GPIO_OUT);
	nrf_ce(0);

	//Set up LED pin
	const uint LED_PIN = PICO_DEFAULT_LED_PIN;
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	//Set up button go to into boot mode without unplugging
	gpio_init(BTN_PIN);
	gpio_set_dir(BTN_PIN, GPIO_IN);
	gpio_pull_up(BTN_PIN);

	//Initialize array
	// for (int i = 0; i < BUF_LEN; i++)
	// 	out_buf[i] = i;

	// uart_puts(uart_default, "\n------------------------------------\r\n");
	// uart_puts(uart_default, "Retrieving config\r\n");
	// nrf_get_config(&nrf_status, in_buf);
	// print_debug(1);

	// uart_puts(uart_default, "\nWriting config\r\n");
	// uint8_t new_config = 0b00001010;
	// nrf_set_config(&nrf_status, &new_config);

	// uart_puts(uart_default, "\nVerifying config\r\n");
	// nrf_get_config(&nrf_status, in_buf);
	// print_debug(1);

	// uart_puts(uart_default, "\nPrinting FIFO status\r\n");
	// nrf_get_fifo_status(&nrf_status, in_buf);
	// print_debug(1);

	// uint8_t val = 0b00111111;
	// uart_puts(uart_default, "\nRX_PW_P0 value\r\n");
	// nrf_rd_cmd(REG_RD_CMD | REG_RX_PW_P0, &nrf_status, in_buf, 1);
	// print_debug(1);
	// nrf_wr_cmd(REG_WR_CMD | REG_RX_PW_P0, &nrf_status, &val, 1);
	// uart_puts(uart_default, "\nRX_PW_P0 value\r\n");
	// nrf_rd_cmd(REG_RD_CMD | REG_RX_PW_P0, &nrf_status, in_buf, 1);
	// print_debug(1);

	// uart_puts(uart_default, "\nStarting RX\r\n");
	// nrf_start_rx();

	// sleep_ms(5);

	// uart_puts(uart_default, "\nPrinting FIFO status\r\n");
	// nrf_get_fifo_status(&nrf_status, in_buf);
	// print_debug(1);

	// uart_puts(uart_default, "\nAdding to TX FIFO\r\n");
	// nrf_push_tx_fifo(69420);

	// uart_puts(uart_default, "\nPrinting FIFO status\r\n");
	// nrf_get_fifo_status(&nrf_status, in_buf);
	// print_debug(1);

	// uart_puts(uart_default, "\nTransmitting\r\n");
	// nrf_start_tx();
	// sleep_ms(1);
	// nrf_stop_tx();

	// uart_puts(uart_default, "\nPrinting FIFO status\r\n");
	// nrf_get_fifo_status(&nrf_status, in_buf);
	// print_debug(1);

	// uart_puts(uart_default, "\nRetrieving config\r\n");
	// nrf_get_config(&nrf_status, in_buf);
	// print_debug(1);

	// //To Transmit
	// //Power on
	// nrf_power_on();
	// //Turn off auto retransmit (SETUP_RETR) and acknowledgement (EN_AA) ???
	// //Verify TX address
	// //Put data to transmit into FIFO (keep at 1 byte for now)
	// uint8_t val = 0b10101010;
	// nrf_wr_cmd(W_TX_PAYLOAD_CMD, &nrf_status, &val, 1);
	// //Clear RX bit
	// nrf_set_rx_bit(false);
	// //Chip enable
	// nrf_ce(true);
	// //Check TX FIFO (or STATUS TX_DS)

	//To Receive:
	//Power on
	nrf_power_on();
	//Ensure that the RX pipe is enabled (0 and 1 are by default)
	//Verify RX pipe address
	// Don't change for now
	//Ensure the data pipe has a number set for payload bytes (RX_PW_Px) (keep at 1 byte for now)
	uint8_t val = 0b00000001;
	nrf_wr_cmd(REG_WR_CMD | REG_RX_PW_P0, &nrf_status, &val, 1);
	//Enable RX bit
	nrf_set_rx_bit(true);
	//Chip enable
	nrf_ce(true);

	while (true) {
		uart_puts(uart_default, "\nPrinting FIFO status\r\n");
		nrf_get_fifo_status(&nrf_status, in_buf);
		print_debug(1);

		//Toggle LED
		gpio_put(LED_PIN, !gpio_get(LED_PIN));

		//Go into the boot mode if we press the button
		if (!gpio_get(BTN_PIN))
			reset_usb_boot(0, 0);

		sleep_ms(1000);
	}

	return 0;
}

void print_byte(uint8_t byte) {
	//Iterate through each bit received
	for (int i = 7; i >= 0; i--) {
		bool bit = byte & (1 << i);
		uart_putc_raw(uart_default, bit ? '1' : '0');
	}
}

void print_debug(int len) {
	//Send data over UART
	// uart_puts(uart_default, "Command: ");
	// print_byte(cmd);
	// uart_puts(uart_default, "\r\n");

	uart_puts(uart_default, "Chip Status: ");
	print_byte(nrf_status);
	uart_puts(uart_default, "\r\n");

	uart_puts(uart_default, "Received Data: ");
	for (int i = 0; i < len; i++)
		print_byte(in_buf[i]);
	uart_puts(uart_default, "\r\n");
}
