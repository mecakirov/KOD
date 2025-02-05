/*************************************************************
 *
 * COMP: SCIENCE ENERGY
 *
 * Date          : 05.11.2023
 * By            : Selim Öztürk
 * e-mail        : selim@iddef.org
 *
 * */
#include <msp430.h>
#include "flash_tasks.h"

/****   FLASH READ WRITE TEST ***********************************

void flash_tester2(void){

    uint32_t  dt[64],i=0,boot_message = 0;
    for(i=0; i<64; i++){ dt[i] = 0x12345678; }

    flash_clear_sector(MSP430_SELECT_SECTOR(28)  );
    flash_write_sector(MSP430_SELECT_SECTOR(28), (uint32_t  *)dt , 64);

    boot_message = flash_read_adress(MSP430_SELECT_SECTOR(28));

    flash_clear_sector(MSP430_SELECT_SECTOR(29)  );
    flash_write_sector(MSP430_SELECT_SECTOR(29), (uint32_t  *)dt , 64);

    boot_message = flash_read_adress(MSP430_SELECT_SECTOR(29));
}

 void flash_tester(void){   // not dt[64] değilde 128 yapınca patlıo dikkatli ol !!!!!!!!!!!!!!


     uint32_t  dt[64],i=0;

     for(i=0; i<64; i++){ dt[i] = 0x12345678; }

     flash_clear_sector(MSP430_SELECT_SECTOR(28)  );
     flash_clear_sector(MSP430_SELECT_SECTOR(29) );
     flash_clear_sector(MSP430_SELECT_SECTOR(30) );

     // BU AŞAĞIDAKİ GİBİ YAZINCA TUM SECTORE YAZILIR
     flash_write_sector(MSP430_SELECT_SECTOR(28), (uint32_t  *)dt , 64);
     flash_write_sector(MSP430_SELECT_SECTOR(28) + (MSP430_SECTOR_SIZE/2), (uint32_t  *)dt , 64);

     flash_write_sector(MSP430_SELECT_SECTOR(29), (uint32_t  *)dt , 64);
     flash_write_sector(MSP430_SELECT_SECTOR(29) + (MSP430_SECTOR_SIZE/2), (uint32_t  *)dt , 64);

     flash_write_sector(MSP430_SELECT_SECTOR(30), (uint32_t  *)dt , 64);
     flash_write_sector(MSP430_SELECT_SECTOR(30) + (MSP430_SECTOR_SIZE/2), (uint32_t  *)dt , 64);

     flash_clear_sector(MSP430_SELECT_SECTOR(28)  );
     flash_clear_sector(MSP430_SELECT_SECTOR(29) );
     flash_clear_sector(MSP430_SELECT_SECTOR(30) ); // for msp430g2553 sector 30 0xFE00

 }

*****/


uint32_t flash_read_adress(uint16_t addr)
{
    uint32_t mem_read;
    FCTL2 = FWKEY;
    FCTL3 = FWKEY;
    FCTL1 = FWKEY;
    mem_read = *((uint32_t *)addr);
    return mem_read;
}

uint16_t flash_read_id_adress(uint16_t addr)
{
    uint16_t mem_read;
    FCTL2 = FWKEY;
    FCTL3 = FWKEY;
    FCTL1 = FWKEY;
    mem_read = *((uint16_t *)addr);
    return mem_read;
}
uint8_t flash_read_id_adress_char(uint16_t addr)
{
    uint8_t mem_read;
    FCTL2 = FWKEY;
    FCTL3 = FWKEY;
    FCTL1 = FWKEY;
    mem_read = *((uint8_t *)addr);
    return mem_read;
}
/***
 * data max length should be 64 -> data_size
 *
 *  ***/
void flash_write_adress(uint16_t  adress, uint32_t  *data , uint16_t data_size)
{
  uint16_t _data_size   = data_size;
  if(_data_size>64){_data_size = 64;}
  uint32_t *addr = (uint32_t *)adress;
  uint16_t i = 0;
  
  _DINT();
  _NOP();
  while (FCTL3 & BUSY);
  FCTL2 = FWKEY + FSSEL_1 + FN0;
  FCTL3 = FWKEY;
  FCTL1 = FWKEY + WRT;
  for(i = 0 ; i<_data_size ; i++){
     while (!(FCTL3 & WAIT));
     *(addr+i) = data[i];    
  }
  while (FCTL3 & BUSY);
  FCTL1 = FWKEY;
  FCTL3 = FWKEY + LOCK;
  _EINT();
  
/*  _DINT();
  while ((FCTL3 & BUSY) != 0);
  FCTL2 = FWKEY + FSSEL_1 + FN0;
  FCTL3 = FWKEY;
  FCTL1 = FWKEY + ERASE;
  FCTL1 = FWKEY + WRT;
  while ((FCTL3 & BUSY) != 0);
  for(i = 0 ; i<_data_size ; i++){
      *(addr+i)   =  data[i] ;
  }
  while ((FCTL3 & BUSY) != 0);
  FCTL1 = FWKEY;
  FCTL3 = FWKEY + LOCK;
  _EINT();*/
}

/***
 * sector_adress must be -> MSP430_SELECT_SECTOR(x)
 *
 *  ***/
void flash_clear_sector(uint16_t  sector_adress )
{
  uint8_t *addr = (uint8_t *)sector_adress;
  _DINT();
  _NOP();
  while (FCTL3 & BUSY);
  FCTL2 = FWKEY + FSSEL_1 + FN0;
  FCTL3 = FWKEY;
  FCTL1 = FWKEY + ERASE;
  while (FCTL3 & BUSY);
  *addr = 0;
  while (FCTL3 & BUSY);
  FCTL1 = FWKEY;
  FCTL3 = FWKEY + LOCK;
  _EINT();
  
 /*uint32_t *addr = (uint32_t *)(sector_adress );
  _DINT();
  while ((FCTL3 & BUSY) != 0);
  FCTL2 = FWKEY + FSSEL_1 + FN0;
  FCTL3 = FWKEY;
  FCTL1 = FWKEY + ERASE;
  while ((FCTL3 & BUSY) != 0);
  *(addr+0)   =  0 ;
  while ((FCTL3 & BUSY) != 0);
  FCTL1 = FWKEY;
  FCTL3 = FWKEY + LOCK;
  _EINT();*/
}




/******************************/
/*
void FlashWriteint(int value , int addr)
{
    FlashWrite(value, addr + 1);
    FlashWrite(value >> 8, addr);
}

void FlashWrite(char value , int addr)
{
    uint8_t syc = 0;
    char part;
    part = value;
    int mod = addr % 0x0040; // Segmentin ba�lang�c�n� bulmak i�in modu al�n�yor.
    int inaddr = addr - mod;
    char *segment;
    segment = (char *)inaddr;
    char flashBuffer[64];  // Segmenteki verinin silinmeden �nce yaz�laca�� alan
    char *Flash_ptr;                          // Flash pointer
    Flash_ptr = (char *)segment;                 // Initialize Flash pointer

    //Value int gelirse 2 defa buffer da oynama yap�lacak.

    for(syc = 0; syc < 64; syc++)
    {
        if(syc == mod){
            flashBuffer[syc] = part;
        }else{
            segment = inaddr + (char *)syc;
            flashBuffer[syc] = *segment;
        }
    }

    _DINT();

    FCTL3 = FWKEY;                            // Clear Lock bit
    FCTL1 = FWKEY | ERASE;                    // Set Erase bit
    // adres 40 l�k bloklar halinde ERASE yap�l�r 0x1023 0x1000 ve 0x1040 aras�n� siler
    *Flash_ptr = 0;                           // Dummy write to erase Flash

    FCTL1 = FWKEY | WRT;                      // Set WRT bit for write operation

    for (syc = 0; syc < 64; syc++)
    {
        *Flash_ptr++ = flashBuffer[syc];
    }

    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY | LOCK;

    _EINT();
}

float FlashReadfloat(int addr)
{
    float *pvalue;
    float value;
    
    pvalue = &value;
        
    *pvalue = FlashRead(addr);
    *(pvalue + 1) = FlashRead(addr + 1);
    *(pvalue + 2) = FlashRead(addr + 2);
    *(pvalue + 3) = FlashRead(addr + 3);
    
    return value;
}

int FlashReadint(int addr)
{
    int value = 0;
    value = (FlashRead(addr) << 8) + FlashRead(addr + 1);
    return value;
}

char FlashRead(int addr)
{
    char *ptr;
    ptr = (char *)addr;
    return *ptr;
}





static void saveData()
{
    FlashWriteint(temp_water1, w1_adr);
    FlashWriteint(temp_water2, w2_adr);
    FlashWriteint(temp_water3, w3_adr);
    FlashWriteint(temp_water4, w4_adr);
    FlashWriteint(temp_water5, w5_adr);
    FlashWriteint(temp_water6, w6_adr);
    FlashWriteint(water_l, wl_adr);
}
void saveDataGPS(void)
{

    FlashWrite(TI_second, TI_second_adr);
    FlashWrite(TI_minute, TI_minute_adr);
    FlashWrite(TI_hour, TI_hour_adr);
    FlashWrite(TI_day, TI_day_adr);
    FlashWrite(TI_month, TI_month_adr);
    FlashWrite(TI_year, TI_year_adr);
    FlashWritePtr(lat_adr, (uint32_t *)&telemetri.latitude , 4);
    FlashWritePtr(long_adr, (uint32_t *)&telemetri.longitude , 4);
    
}

static void readData()
{
    temp_water1 = FlashReadint(w1_adr);             // 1. gÃ¼nÃ¼n su verisi
    temp_water2 = FlashReadint(w2_adr);            // 2. gÃ¼nÃ¼n su verisi
    temp_water3 = FlashReadint(w3_adr);             // 3. gÃ¼nÃ¼n su verisi
    temp_water4 = FlashReadint(w4_adr);             // 4. gÃ¼nÃ¼n su verisi
    temp_water5 = FlashReadint(w5_adr);             // 5. gÃ¼nÃ¼n su verisi
    temp_water6 = FlashReadint(w6_adr);             // 6. gÃ¼nÃ¼n su verisi
    water_l = FlashReadint(wl_adr);
}

void readDataGPS(void)
{
    TI_second = FlashRead(TI_second_adr);
    TI_minute = FlashRead(TI_minute_adr);
    TI_hour = FlashRead(TI_hour_adr);
    TI_day = FlashRead(TI_day_adr);
    TI_month = FlashRead(TI_month_adr);
    TI_year = FlashRead(TI_year_adr);
    FlashWritePtr((uint32_t)&(telemetri.latitude), (uint32_t *)lat_adr , 4);
    FlashWritePtr((uint32_t)&(telemetri.longitude), (uint32_t *)long_adr , 4);    
}

void read_all_flash_data_init(void)
{
    readData();
}

void save_all_datas_to_flash(void)
{
    saveData();
}

void FlashWritePtr(uint32_t  adress, uint32_t *data , uint8_t data_size)
{
    int mod = adress % 0x0040; // Segmentin ba�lang�c�n� bulmak i�in modu al�n�yor.
    int inaddr = adress - mod;

    char flashBuffer[64];  // Segmenteki verinin silinmeden �nce yaz�laca�� alan
    char *Flash_ptr;   
    Flash_ptr = (char *)inaddr;    // Flash pointer

    //Value int gelirse 2 defa buffer da oynama yapilacak.
    uint32_t syc = 0;
    for(syc = 0; syc < 64; syc++)
    {    
      flashBuffer[syc] = FlashRead(inaddr + syc);
    }
  
    for(syc = 0; syc < data_size; syc++)
    {       
       flashBuffer[mod + syc] = FlashRead((int)data + syc); //  *(data + syc);        
    }

    _DINT();

    FCTL3 = FWKEY;                            // Clear Lock bit
    FCTL1 = FWKEY | ERASE;                    // Set Erase bit
    // adres 40 l�k bloklar halinde ERASE yap�l�r 0x1023 0x1000 ve 0x1040 aras�n� siler
    *Flash_ptr = 0;                           // Dummy write to erase Flash

    FCTL1 = FWKEY | WRT;                      // Set WRT bit for write operation

    for (syc = 0; syc < 64; syc++)
    {
        *Flash_ptr++ = flashBuffer[syc];
    }

    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY | LOCK;

    _EINT();
}

*/
/*
static void saveData()
{
    flash_write_adress(w1_adr, (uint32_t  *)temp_water1, 1);
    flash_write_adress(w2_adr, (uint32_t  *)temp_water2, 1);
    flash_write_adress(w3_adr, (uint32_t  *)temp_water3, 1);
    flash_write_adress(w4_adr, (uint32_t  *)temp_water4, 1);
    flash_write_adress(w5_adr, (uint32_t  *)temp_water5, 1);
    flash_write_adress(w6_adr, (uint32_t  *)temp_water6, 1);
    flash_write_adress(wl_adr, (uint32_t  *)water_l,1);
}
void saveDataGPS(void)
{

    flash_write_adress(TI_second_adr, (uint32_t  *)TI_second, 1);
    flash_write_adress(TI_minute_adr, (uint32_t  *)TI_minute, 1);
    flash_write_adress(TI_hour_adr, (uint32_t  *)TI_hour, 1);
    flash_write_adress(TI_day_adr, (uint32_t  *)TI_day, 1);
    flash_write_adress(TI_month_adr, (uint32_t  *)TI_month, 1);
    flash_write_adress(TI_year_adr, (uint32_t  *)TI_year, 1);
    int i;
    for(i = 0; i < 4; i++)
    {
      flash_write_adress(lat_adr+i, (uint32_t  *)((char)telemetri.latitude << (8*i)), 1);        
      flash_write_adress(long_adr+i, (uint32_t  *)((char)telemetri.longitude << (8*i)), 1);
    }
}

static void readData()
{
    temp_water1 = flash_read_adress(w1_adr);             // 1. gÃ¼nÃ¼n su verisi
    temp_water2 = flash_read_adress(w2_adr);            // 2. gÃ¼nÃ¼n su verisi
    temp_water3 = flash_read_adress(w3_adr);             // 3. gÃ¼nÃ¼n su verisi
    temp_water4 = flash_read_adress(w4_adr);             // 4. gÃ¼nÃ¼n su verisi
    temp_water5 = flash_read_adress(w5_adr);             // 5. gÃ¼nÃ¼n su verisi
    temp_water6 = flash_read_adress(w6_adr);             // 6. gÃ¼nÃ¼n su verisi
    water_l = flash_read_adress(wl_adr);
}

void readDataGPS(void)
{
    TI_second = flash_read_adress(TI_second_adr);
    TI_minute = flash_read_adress(TI_minute_adr);
    TI_hour = flash_read_adress(TI_hour_adr);
    TI_day = flash_read_adress(TI_day_adr);
    TI_month = flash_read_adress(TI_month_adr);
    TI_year = flash_read_adress(TI_year_adr);
    int i;
    for(i = 0; i < 4; i++)
    {
      telemetri.latitude += (flash_read_adress(lat_adr+i) << (8*i));
      telemetri.longitude += (flash_read_adress(long_adr+i) << (8*i));
    }
}
*/
