/*************************************************************
 *
 * COMP: SCIENCE ENERGY
 *
 * Date          : 05.11.2023
 * By            : Selim Öztürk
 * e-mail        : selim@iddef.org
 *
 * */
#ifndef __ADC_TASKS_H
#define __ADC_TASKS_H

#include "hardware_configs.h"

typedef struct
{
    uint16_t id;                  // ID - Cihaz numarasi
    uint16_t signal_quality;      // GSM Sinyal Kuvveti
    uint8_t hour;                // Saat
    uint8_t minute;              // Dakika
    uint8_t second;              // Saniye
    uint8_t day;                 // G�n
    uint8_t month;               // Ay
    uint8_t year;                // Yil
    float temparature;          // Sicaklik
    float battery_voltage;      // Pil Gerilimi
    float battery_voltage2;     // Pil Gerilimi modem ac�k
    uint16_t water1;              // 1. gunun su verisi
    uint16_t water2;              // 2. gunun su verisi
    uint16_t water3;              // 3. gunun su verisi
    uint16_t water4;              // 4. gunun su verisi
    uint16_t water5;              // 5. gunun su verisi
    uint16_t water6;              // 6. gunun su verisi
    uint16_t water7;              // 7. gunun su verisi
    float latitude;               // Enlem
    float longitude;              // Boylamss
    uint16_t checksum;             // Saglama
    uint32_t tp;                  // Telemetri Periyodu
    uint16_t time_modem_open;     // ID - Cihaz numarasi
    unsigned char telemetry_url[100];
    unsigned char update_url[100];
} telemetry_t;

extern telemetry_t telemetri;

void telemetry_update(void);

void adc_water_adder(void);

#endif 
