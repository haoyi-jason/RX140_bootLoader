/*
 * rx_flash_helper.h
 *
 *  Created on: 2024年3月4日
 *      Author: user
 */

#ifndef RX_FLASH_HELPER_H_
#define RX_FLASH_HELPER_H_

#include "platform.h"

void rx_flash_read(uint32_t address, uint8_t *src, uint16_t size);
void rx_flash_write(uint32_t address, uint8_t *src, uint16_t size);
void rx_flash_erase(uint32_t pageStart);
void rx_flash_test();
void rx_flash_open();
void rx_flash_close();

uint8_t rx_flash_cf_program(uint32_t offset, uint8_t *buffer);
uint8_t rx_flash_cf_erase();
uint8_t rx_flash_cf_valid_checksum(uint32_t length, uint32_t checksum);
uint8_t rx_flash_cf_write_config(uint8_t *buffer, uint16_t length);
uint8_t rx_flash_cf_erase_app();
uint8_t rx_flash_cf_buffer_copy(uint32_t size);
uint8_t rx_flash_cf_read_config(uint8_t *buffer, uint16_t size);
#endif /* RX_FLASH_HELPER_H_ */
