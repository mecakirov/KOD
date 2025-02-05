/*************************************************************
 *
 * COMP: SCIENCE ENERGY
 *
 * Date          : 05.11.2023
 * By            : Selim Öztürk
 * e-mail        : selim@iddef.org
 *
 * */
#include "hardware_configs.h"

void telemetry_set(void)
{
    telemetri.id = flash_read_id_adress(TI_device_id_zone) * 1000 + flash_read_id_adress(TI_device_id_no);              // ID - Cihaz numarasi
    telemetri.signal_quality = 0;     // GSM Sinyal Kuvveti
    telemetri.hour = 15;              // Saat
    telemetri.minute = 0;            // Dakika
    telemetri.second = 0;            // Saniye
    telemetri.day = 1;               // Gun
    telemetri.month = 7;              // Ay
    telemetri.year = 24;            // Yil
    //telemetri.temparature = adc_read_ntc();     // Sicaklik
    telemetri.temparature = adc_read_temp();
    telemetri.battery_voltage = adc_read_batery();          // Pil Gerilimi
    telemetri.battery_voltage2 = adc_read_batery();  // Pil Gerilimi modem acik
    telemetri.water1 = 0;             // 1. gunun su verisi
    telemetri.water2 = 0;             // 2. gunun su verisi
    telemetri.water3 = 0;             // 3. gunun su verisi
    telemetri.water4 = 0;             // 4. gunun su verisi
    telemetri.water5 = 0;             // 5. gunun su verisi
    telemetri.water6 = 0;             // 6. gunun su verisi
    telemetri.water7 = 0;             // 7. gunun su verisi
    telemetri.latitude = 0;  // Enlem 41.053469
    telemetri.longitude = 0;  // Boylam 28.943536
    telemetri.checksum = 0;           // Periyotlar arasi toplam su miktari
    telemetri.tp = 57823 * TELEMETRY_PERIOD;  // TP - Telemetri Periyodu
    telemetri.time_modem_open = 0;
    //_array_clear((unsigned char*)telemetri.telemetry_url,100);
    snprintf((char*)telemetri.telemetry_url,100, (char*)"%s" ,(char*)TELEMETRY_URL);
    //_array_clear((unsigned char*)telemetri.update_url,100);
    snprintf((char*)telemetri.update_url,100, (char*)"%s" ,(char*)FW_URL);    
}
