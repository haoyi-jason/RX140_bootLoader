/*
 * rx_flash_helper.c
 *
 *  Created on: 2024年3月4日
 *      Author: user
 */

#include <string.h>
#include "rx_flash_helper.h"
#include "r_flash_rx_if.h"
#include "platform.h"

void rx_flash_read(uint32_t address, uint8_t *src, uint16_t size)
{
	uint32_t addr = address;
	for(uint16_t i=0;i<size;i++){
		src[i] = *((uint8_t*)(addr + i));
	}
}

void rx_flash_write(uint32_t address, uint8_t *src, uint16_t size)
{
	uint32_t addr = address;
	uint32_t pgstart = (addr / FLASH_DF_BLOCK_SIZE)*FLASH_DF_BLOCK_SIZE;
	uint32_t pgend = pgstart + ((size)/FLASH_DF_BLOCK_SIZE +1)*FLASH_DF_BLOCK_SIZE;

	flash_err_t err;
	flash_res_t result;
	uint32_t wr_size;
	uint32_t wr_start;

	err = R_FLASH_Write((uint32_t)src,addr,size);
}
void rx_flash_erase(uint32_t pageStart)
{
	static flash_err_t err;
	flash_access_window_config_t config;

	while(R_FLASH_Control(FLASH_CMD_STATUS_GET,NULL) != FLASH_SUCCESS){

	}

	config.start_addr = pageStart;
	config.end_addr = pageStart + FLASH_DF_BLOCK_SIZE;
	//err = R_FLASH_Control(FLASH_CMD_ACCESSWINDOW_SET,(void*)&config);
	R_BSP_InterruptsDisable();
	err = R_FLASH_Erase(pageStart,1);
	R_BSP_InterruptsEnable();
	if(err != FLASH_SUCCESS){
		while(1);
	}
}
void rx_flash_open()
{
	flash_err_t err;
	err = R_FLASH_Open();
	if(err != FLASH_SUCCESS){
		while(1);
	}
}

void rx_flash_close()
{
	flash_err_t err;
	err = R_FLASH_Close();
	if(err != FLASH_SUCCESS){
		while(1);
	}
}
#define CF_APP_ADDR			FLASH_CF_LOWEST_VALID_BLOCK + FLASH_CF_BLOCK_SIZE*10
#define CF_APP_PAGES		54
#define CF_APP_SIZE			(CF_APP_PAGES*FLASH_CF_BLOCK_SIZE)
#define CF_CONFIG_ADDR		FLASH_CF_BLOCK_63
#define CF_USER_ADDR_START	FLASH_CF_BLOCK_64
#define CF_USER_PAGES		54
#define CF_USER_SIZE		(CF_USER_PAGES*FLASH_CF_BLOCK_SIZE)

uint8_t rx_flash_cf_program(uint32_t offset, uint8_t *buffer)
{
	if(offset > CF_USER_SIZE) return 0;
	uint8_t valid = 0;
	uint8_t buff[8];
	uint8_t buff_rb[8];
	memset(buff,0xff,8);
	memcpy(buff,buffer,4);
	rx_flash_open();

	flash_err_t err;
	err = R_FLASH_Write((uint32_t)buff, CF_USER_ADDR_START + offset,8);
	rx_flash_read(CF_USER_ADDR_START + offset,buff_rb,8);
	rx_flash_close();

	if(err == FLASH_SUCCESS){
		bool ok = true;
		for(uint8_t i=0;i<4;i++){
			ok &= (buff[i] == buff_rb[i]);
		}
		valid = ok?1:0;
	}

	return valid;
}

uint8_t rx_flash_cf_erase()
{
	flash_err_t err;
	rx_flash_open();
	err = R_FLASH_Erase(CF_USER_ADDR_START,CF_USER_PAGES );
	rx_flash_close();

	return err;
}

uint8_t rx_flash_cf_erase_app()
{
	flash_err_t err;
	rx_flash_open();
	err = R_FLASH_Erase(CF_APP_ADDR,CF_APP_PAGES );
	rx_flash_close();

	return err;
}

uint8_t rx_flash_cf_buffer_copy(uint32_t size)
{
	flash_err_t err = FLASH_SUCCESS;
	uint32_t szToWrite = (size/8)*8;	// align to 8 byte
	uint32_t szWritten = 0x0;
	uint8_t buff[8];
	uint32_t rdAddr = CF_USER_ADDR_START;
	uint32_t wrOffset = 0;
	rx_flash_open();

	while(wrOffset < szToWrite){
		rx_flash_read(rdAddr,buff,8);
		err = rx_flash_cf_program(wrOffset,buff);
		if(err != FLASH_SUCCESS){
			break;
		}
		rdAddr += 8;
		wrOffset += 8;
	}
	rx_flash_close();
	return err;
}

uint8_t rx_flash_cf_valid_checksum(uint32_t length, uint32_t checksum)
{
	uint8_t valid = 0xff;

	uint32_t img_checksum = 0x0;
	uint32_t addr = CF_USER_ADDR_START;
	for(uint32_t i=0;i<length;i++){
		img_checksum += *((uint8_t*)(addr + i));
	}

	if(img_checksum == checksum){
		valid = 0;
	}

	return valid;

}

uint8_t rx_flash_cf_write_config(uint8_t *buffer, uint16_t length)
{
	rx_flash_open();

	flash_err_t err;
	err = R_FLASH_Erase(CF_CONFIG_ADDR,1 );
	err = R_FLASH_Write((uint32_t)buffer,CF_CONFIG_ADDR,length);

	rx_flash_close();

	return err;
}

uint8_t rx_flash_cf_read_config(uint8_t *buffer, uint16_t size)
{
	rx_flash_open();

	flash_err_t err = FLASH_SUCCESS;
	for(uint16_t i=0;i<size;i++){
		buffer[i] = *((uint8_t*)(CF_CONFIG_ADDR + i));
	}
	rx_flash_close();

	return err;
}
