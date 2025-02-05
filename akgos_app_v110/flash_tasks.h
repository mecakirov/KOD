/*************************************************************
 *
 * COMP: SCIENCE ENERGY
 *
 * Date          : 05.11.2023
 * By            : Selim Öztürk
 * e-mail        : selim@iddef.org
 *
 * */
#ifndef __FLASH_TASKS_H
#define __FLASH_TASKS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


/******************* CONFIG *********************/
/** SELECT JUST YOUR MCUS CONFIGS **/
/*#define MSP430_START_ADRESS                 (uint32_t)(0x2000)
#define MSP430_END_ADRESS                   (uint32_t)(0xFcff + 1)
#define MSP430_SECTOR_SIZE                  (uint32_t)(0x200)
#define MSP430_MAX_SECTOR_NUM               ((MSP430_END_ADRESS - MSP430_START_ADRESS)/MSP430_SECTOR_SIZE)      // FOR MSP430G2553 -> 32, FOR MSP430G2955 -> 110
#define MSP430_SELECT_SECTOR(sector)        sector>(MSP430_MAX_SECTOR_NUM-1) ? (MSP430_END_ADRESS-MSP430_SECTOR_SIZE) :( MSP430_START_ADRESS + (sector * MSP430_SECTOR_SIZE))
#define MSP430_FIND_SECTOR(adress)          adress < MSP430_START_ADRESS ? 0 : ((adress - MSP430_START_ADRESS)/MSP430_SECTOR_SIZE)
*/
/** SELECT JUST YOUR SECTORS **/
#define APP1_ADRESS                         0x2E00
#define APP2_ADRESS                         0x9200
#define BOOT_MESSAGE_ADRESS                 0xF600
#define BOOT_PSW                            0xAAAAAAAA

#define BACKUP_VECTORS_FLASH_AREA           0xFA00  
#define BACKUP_INTERRPT_FLASH_AREA          0xFBE0  
/**************************************************/

#define TI_device_id_org_1      0x1000
#define TI_device_id_org_2      0x1001
#define TI_device_id_zone       0x1002
#define TI_device_id_no         0x1004
/*#define TI_second_adr           0x1002
#define TI_minute_adr           0x1003
#define TI_hour_adr             0x1004
#define TI_day_adr              0x1005
#define TI_month_adr            0x1006
#define TI_year_adr             0x1007
#define w1_adr                  0x1008
#define w2_adr                  0x100A
#define w3_adr                  0x100C
#define w4_adr                  0x100E
#define w5_adr                  0x1010
#define w6_adr                  0x1012
#define w7_adr                  0x1014
#define wl_adr                  0x1016
#define lat_adr                 0x1018
#define long_adr                0x101C
   */
uint8_t flash_read_id_adress_char(uint16_t addr);   
uint16_t flash_read_id_adress(uint16_t addr);
uint32_t flash_read_adress(uint16_t addr);
void flash_write_adress(uint16_t  adress, uint32_t *data , uint16_t data_size);
void flash_clear_sector(uint16_t  sector_adress );

/*
extern void FlashWritePtr(uint32_t  adress, uint32_t *data , uint8_t data_size);

extern void FlashWriteint(int value , int addr);
extern void FlashWrite(char value , int addr);

extern float FlashReadfloat(int addr);

extern int FlashReadint(int addr);
extern char FlashRead(int addr);


void read_all_flash_data_init(void);
void save_all_datas_to_flash(void);
void saveDataGPS(void);
void readDataGPS(void);*/
#endif 
