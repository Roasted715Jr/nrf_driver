cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(nrf_driver)

pico_sdk_init()

add_executable(nrf_driver
	main.c
	nrf_driver.c
)

# pico_enable_stdio_uart(nrf_driver 1)
# pico_enable_stdio_usb(nrf_driver 0)

target_link_libraries(nrf_driver pico_stdlib hardware_uart hardware_spi pico_bootrom)

pico_add_extra_outputs(nrf_driver)
