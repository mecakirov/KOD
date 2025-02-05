#include "hardware_configs.h"
#include "rtc_tasks.h"
#include "adc_tasks.h"

telemetry_t telemetri;

static gsm_message_box_stades gsm_mailbox = GSM_EMPTY_ORDER;

static gsm_modul_steps gsm_task = GSM_EMPTY_STEP;

int main( void )
{    
  WDTCTL = WDTPW | WDTHOLD; 
  
//  WDTCTL = WDT_ADLY_250;                    // WDT 250ms, ACLK, interval timer
//  IE1 |= WDTIE; 
  
  hardware_config_init();

  telemetry_set();
  
  //debug_print((char *)"app_150_init\n");  
  char msg[20];
  sprintf((char*)msg,(char*)"Start %c%c0%u\n", flash_read_id_adress_char(TI_device_id_org_1), flash_read_id_adress_char(TI_device_id_org_2), (flash_read_id_adress(TI_device_id_zone) * 1000 + flash_read_id_adress(TI_device_id_no)));
  gsm_modul_uart_send(msg);
  
  gsm_mailbox = GSM_CONNECT_ORDER;
  
  //__bic_SR_register(LPM3_bits);
  
  
  while(1){
    
      gsm_task = gsm_modul_active_object(&gsm_mailbox);
      
      if(t_timer >= telemetri.tp){   
        t_timer = 0;   
        gsm_mailbox = GSM_CONNECT_ORDER;                
        telemetry_update();
        //__bic_SR_register_on_exit(LPM3_bits);
      }
      if(g_timer >= GPS_PERIOD){   
        g_timer = 0;
        gps_open = 0;
      }
  }
/*    
      char testt[20];
      sprintf((char*)testt,(char*)"Temparature 1 : %.2f Temparature 2 : %u\n", adc_read_ntc(), adc_read_temp());
      gsm_modul_uart_send(testt);
 */
  //return 0;
}

