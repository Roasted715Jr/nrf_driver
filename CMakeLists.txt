cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(nrf_driver)

pico_sdk_init()

add_executable(nrf_driver
	main.c
	nrf_driver.c
)

target_link_libraries(nrf_driver pico_stdlib hardware_uart hardware_spi pico_bootrom)

pico_add_extra_outputs(nrf_driver)
