/***********************************************************************
*
*  FILE        : rx140_bootloader.c
*  DATE        : 2024-03-04
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
#include "r_smc_entry.h"
#include <stdio.h>
#include <string.h>
#include "rx_flash_helper.h"
#include "r_flash_rx_if.h"

/** boot config, should be move to single header file later */
#define BOOT_BUFFER_BASE_ADDR	FLASH_CF_BLOCK_127
#define CF_APP_ADDR			FLASH_CF_BLOCK_63
#define USER_PROG_RESET_VECTOR_ADDR	0xFFFFAFFC


#define BL_CMD_ERASE            0x10000001
#define BL_CMD_BOOT             0x10000020
#define BL_CMD_VERIFY           0x10000021
#define BL_CMD_IDN              0x10000022
#define BL_CMD_IMG_SIZE         0x10000023
#define BL_CMD_VERSION          0x10000024
#define BL_CMD_BOOT_INFO        0x10000025
#define BL_CAN_ID               0x12905900

#define BOOT_FLAG               0x53290921
#define BOOT_READY              0x12905329
typedef struct{
  uint32_t flag;
  uint32_t flag2;
  uint32_t imageSize;
  uint32_t product_id;
  uint32_t checksum;
  uint16_t version;
  uint8_t reserved[10];
}_boot_config_t;

_boot_config_t bootConfig;

void main(void);

void ExecuteApp()
{
	R_BSP_SET_PSW(0);
	volatile uint32_t addr;
	addr = *(uint32_t*)USER_PROG_RESET_VECTOR_ADDR;
	((void(*)())addr)();

	while(1);
}

void main(void)
{
	rx_flash_cf_read_config((void*)&bootConfig,sizeof(_boot_config_t));

	if(bootConfig.flag != BOOT_FLAG){
		ExecuteApp();
	}

	// valid image checksum
	if(rx_flash_cf_valid_checksum(bootConfig.imageSize,bootConfig.checksum) != 0x0){
		//
		rx_flash_cf_erase();
		ExecuteApp();
	}

	// copy image from buffer to app
	rx_flash_cf_erase_app();
	rx_flash_cf_buffer_copy(bootConfig.imageSize);

	ExecuteApp();

}
