/*************************************************************
 *
 * COMP: SCIENCE ENERGY
 *
 * Date          : 05.11.2023
 * By            : Selim Öztürk
 * e-mail        : selim@iddef.org
 *
 * */
#include "adc_tasks.h"
#include "hardware_configs.h"
#include "rtc_tasks.h"

void adc_water_adder(void){
  if(t_timer < (telemetri.tp)/7){
    telemetri.water1++;
  }else if(t_timer < (telemetri.tp*2)/7){
    telemetri.water2++;
  }else if(t_timer < (telemetri.tp*3)/7){
    telemetri.water3++;
  }else if(t_timer < (telemetri.tp*4)/7){
    telemetri.water4++;
  }else if(t_timer < (telemetri.tp*5)/7){
    telemetri.water5++;
  }else if(t_timer < (telemetri.tp*6)/7){
    telemetri.water6++;
  }else if(t_timer <= (telemetri.tp)){
    telemetri.water7++;
  }
}


void telemetry_update(void){
    //telemetri.temparature = adc_read_ntc();     // Sicaklik
    telemetri.temparature = adc_read_temp();
    telemetri.battery_voltage = adc_read_batery();
        
    telemetri.checksum = telemetri.water1 + telemetri.water2
    + telemetri.water3 + telemetri.water4 + telemetri.water5
    + telemetri.water6 + telemetri.water7;    
}