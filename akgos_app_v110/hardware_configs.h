/*************************************************************
 *
 * COMP: SCIENCE ENERGY
 *
 * Date          : 05.11.2023
 * By            : Selim Öztürk
 * e-mail        : selim@iddef.org
 *
 * */
#ifndef __HARDWARE_CONFIGS_H
#define __HARDWARE_CONFIGS_H
#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "flash_tasks.h"
#include "queue.h"
#include "gsm_modul_tasks.h"
#include "adc_tasks.h"
#include "rtc_tasks.h" 
#include "flash_tasks.h"  

/***************************************************************************************************************/
//#define APP_NAME               "SMARTWATER"


/** SET YOUR DEVICES ID **/   
#define GPS_OPEN                // GPS aktif - pasif
//#define DEVICE_ID               9000
#define FW_CURRENT_VERSION      108 
#define TELEMETRY_PERIOD        1440             //   1 saatte bir   Telemetri gönderme periyodu dakika cinsindan (7nin katlari)
#define GPS_PERIOD              (57823*10080)  //   7 günde bir    Konum alma süresi dakika cinsindan
#define FW_URL                  "http://api.waterhero.io/api/updates/fw"
#define TELEMETRY_URL           "http://api.waterhero.io/api/report/hit?token=8RX95hNlmzS2ieM777HE21VAv"

// "https://webhook.site/c1520be0-4e37-404c-a6b7-7f69ec31040f"

//"http://api.waterhero.io/api/report/hit?token=8RX95hNlmzS2ieM777HE21VAv"

// POST https://aquawatch-20243.firebaseio.com/data.json


#define GSM_DC_EN               BIT0       // Pin 1.0 Output
#define PULSE                   BIT2       // Pin 1.2 Input

#define NTC_ADC                 BIT0       // Pin 2.0 ADC
#define NTC_EN                  BIT1       // Pin 2.1 I/O
#define GSM_POWERKEY            BIT2       // Pin 2.2 Output
#define VBAT_ADC                BIT3       // Pin 2.3 ADC
#define GSM_STATUS              BIT5       // Pin 2.5 Input

#define GSM_RESET               BIT0       // Pin 3.0 Output
#define SDA                     BIT1       // Pin 3.1 Input
#define SCL                     BIT2       // Pin 3.2 Output
#define TXD                     BIT4       // Pin 3.4 Output
#define RXD                     BIT5       // Pin 3.5 Input


#define GSM_ON()                (P1OUT |= GSM_DC_EN)
#define GSM_OFF()               (P1OUT &= ~GSM_DC_EN)
#define NTC_ON()                (P2OUT |= NTC_EN)
#define NTC_OFF()               (P2OUT &= ~NTC_EN)
   
#define GSM_POWERKEY_ON()       (P2OUT |= GSM_POWERKEY)
#define GSM_POWERKEY_OFF()      (P2OUT &= ~GSM_POWERKEY)
#define GSM_RESET_ON()          (P3OUT |= GSM_RESET)
#define GSM_RESET_OFF()         (P3OUT &= ~GSM_RESET)
   
#define GSM_TIME_PWRKEY         900            //  0.7s < Pulldown < 1.0sn
#define GSM_TIME_RESET          400            //  0.3s < Pulldown < 1.0sn
/*
typedef enum{
    SLEEP_MODE_1,
    SLEEP_MODE_2,
    SLEEP_MODE_3,
    SLEEP_MODE_4,
    SLEEP_CPU_OFF
}sleeps_stds;

typedef enum{
    WDT_CLOSE_STD,
    WDT_32MS_STD,
    // ** add other stades **
}watchdog_stds;
*/
void hardware_config_init(void);

//void watchdog_config(watchdog_stds s);

void gsm_modul_uart_send(char *pStr);
bool gsm_modul_get_status(void);

//void gsm_modul_reset(void);

void clear_all_backup(void);

//void software_reset_and_record_all_datas(void);

float adc_read_batery(void);
void initADC_ntc(void);
float adc_read_ntc(void);
int adc_read_temp(void);

//void sleep_set(sleeps_stds s);

//void debug_print(char*data);

void give_boot_message(void);
void telemetry_set(void);

#endif 
