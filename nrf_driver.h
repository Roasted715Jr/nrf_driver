#ifndef NRF_DRIVER_H
#define NRF_DRIVER_H

#include "pico/stdlib.h"
#include "hardware/spi.h"

//TODO: Implement higher level commands
//Change/check config
//Enable specific data pipe
//Set up retransmit
//Change/check RF channel
//Changecheck RF setup
//Change/check status register
//Change/check receive address for pipe
//Change/check transmit address
//Check number of bytes in RX data pipe
//Check FIFO status

//Lower level functions
void nrf_rd_cmd(uint8_t cmd, uint8_t* status_dst, uint8_t* data_dst, int len);
void nrf_wr_cmd(uint8_t cmd, uint8_t* status_dst, uint8_t* data_src, int len);
void nrf_cmd(uint8_t cmd, uint8_t* status_dst);
void set_csn(int val);
void normalize_data(uint8_t* data, int size);

#endif
