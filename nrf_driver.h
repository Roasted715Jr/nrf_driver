#ifndef NRF_DRIVER_H
#define NRF_DRIVER_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <memory.h>

#define SPI_ID spi_default
#define MISO_PIN PICO_DEFAULT_SPI_RX_PIN
#define MOSI_PIN PICO_DEFAULT_SPI_TX_PIN
#define SCK_PIN PICO_DEFAULT_SPI_SCK_PIN
#define CSN_PIN PICO_DEFAULT_SPI_CSN_PIN
#define CE_PIN 20

//*****************Look into INT pin functionality */
#define INT_IN 0

#define REG_RD_CMD 0b00000000
#define REG_WR_CMD 0b00100000
#define W_TX_PAYLOAD_CMD 0b10100000

#define REG_CONFIG 0x00
#define REG_EN_AA 0x01
#define REG_EN_RXADDR 0x02
#define REG_SETUP_AW 0x03
#define REG_SETUP_RETR 0x04
#define REG_RF_CH 0x05
#define REG_RF_SETUP 0x06
#define REG_STATUS 0x07
#define REG_OBSERVE_TX 0x08
#define REG_RPD 0x09
#define REG_RX_ADDR_P0 0x0A
#define REG_RX_ADDR_P1 0x0B
#define REG_RX_ADDR_P2 0x0C
#define REG_RX_ADDR_P3 0x0D
#define REG_RX_ADDR_P4 0x0E
#define REG_RX_ADDR_P5 0x0F
#define REG_TX_ADDR 0x10
#define REG_RX_PW_P0 0x11
#define REG_RX_PW_P1 0x12
#define REG_RX_PW_P2 0x13
#define REG_RX_PW_P3 0x14
#define REG_RX_PW_P4 0x15
#define REG_RX_PW_P5 0x16
#define REG_FIFO_STATUS 0x17
#define REG_DYNPD 0x1C
#define REG_FEATURE 0x1D

extern uint8_t nrf_status;
extern int addr_len;

//High level functions
//TODO: Implement
//TODO: Data validation so we don't break something :)

//Initialize
//Power on the module and enter Standby-I
void nrf_initialize();
//Send data
//Receive data

//Mid level functions
//TODO: Remove status_dst and just hardcode in nrf_status
//TODO: Change from pointers to values

//Power on/off
void nrf_power_on();
void nrf_power_off();
//Change/check config
void nrf_get_config(uint8_t* status_dst, uint8_t* config_dst);
void nrf_set_config(uint8_t* status_dst, uint8_t* config_src);
//Enable specific data pipe
//Set up retransmit
//Change/check RF channel
//Change/check RF setup
//Change/check status register
//Change/check receive address for pipe
void nrf_get_rx_addr(uint8_t* status_dst, uint8_t* addr_dst);
//Change/check transmit address
void nrf_get_tx_addr(uint8_t* status_dst, uint8_t* addr_dst);
void nrf_set_tx_addr(uint8_t* status_dst, uint8_t* addr_src, int len);
//Check/set number of bytes in RX data pipe
//Check FIFO status
void nrf_get_fifo_status(uint8_t* status_dst, uint8_t* fifo_status_dst);
//Read from RX FIFO
void nrf_pop_rx_fifo(uint8_t* data_dst);
//Write to TX FIFO (1 to 32 bytes)
void nrf_push_tx_fifo(uint8_t* data_src, int len);
//Assumes in Standby-I
void nrf_start_tx();
//Assumes in TX Mode
void nrf_stop_tx();
//Assumes in Standby-I
void nrf_start_rx();
//Assumes in RX Mode
void nrf_stop_rx();
void nrf_set_rx_bit(bool bit);

//Low level functions
void nrf_rd_cmd(uint8_t cmd, uint8_t* status_dst, uint8_t* data_dst, int len);
void nrf_wr_cmd(uint8_t cmd, uint8_t* status_dst, uint8_t* data_src, int len);
void nrf_cmd(uint8_t cmd, uint8_t* status_dst);
void set_csn(bool val);
void set_ce(bool val);
void normalize_data(uint8_t* data, int size);

#endif
