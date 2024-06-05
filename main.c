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

// #define BUF_LEN 100
// uint8_t out_buf[BUF_LEN] = {0};
// uint8_t in_buf[BUF_LEN];

uint8_t cmd;
uint8_t status_buf[1] = {0};
uint8_t in_buf[5] = {0};
uint8_t out_buf[5];


void print_byte(uint8_t byte);
void print_debug();

int main() {
	//Set up UART
	uart_init(uart_default, BAUD_RATE);
	uart_set_hw_flow(uart_default, false, false);
	uart_set_format(uart_default, DATA_BITS, STOP_BITS, PARITY);

	gpio_set_function(PICO_DEFAULT_UART_TX_PIN, GPIO_FUNC_UART);
	gpio_set_function(PICO_DEFAULT_UART_RX_PIN, GPIO_FUNC_UART);

	//Set up SPI
	spi_init(spi_default, 1000 * 1000); //1 MHz SCK

	gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
	gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
	gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
	//CSN needs to be software-controlled, otherwise it will go inactive between each byte (no bueno)
	// gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SIO);
	gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
	gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
	set_csn(1);

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

	while (true) {
		// cmd = 0b11111111; //NOP
		// cmd = 0b00000000; //Read reg 0x00 (CONFIG)
		cmd = 0b00001010; //Read reg 0x0A (RX_ADDR_P0)
		// cmd = 0b00000101; //Read reg 0x05 (RF_CH)
		// nrf_cmd(cmd, status_buf);
		// nrf_rd_cmd(cmd, status_buf, in_buf, 1);
		nrf_rd_cmd(cmd, status_buf, in_buf, 5);
		print_debug();

		cmd = 0b00101010; //Write to reg 0x0A (RX_ADDR_P0)
		out_buf[0] = 0b11111111; //Only change the LSB of the address
		nrf_wr_cmd(cmd, status_buf, out_buf, 5);
		print_debug();

		cmd = 0b00001010; //Read reg 0x0A (RX_ADDR_P0)
		nrf_rd_cmd(cmd, status_buf, in_buf, 5);
		print_debug();
		
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

void print_debug() {
	//Send data over UART
	uart_puts(uart_default, "Command: ");
	print_byte(cmd);
	uart_puts(uart_default, "\r\n");

	uart_puts(uart_default, "Status: ");
	print_byte(status_buf[0]);
	uart_puts(uart_default, "\r\n");

	uart_puts(uart_default, "Received Data: ");
	for (int i = 0; i < 5; i++)
		print_byte(in_buf[i]);
	uart_puts(uart_default, "\r\n");
}
