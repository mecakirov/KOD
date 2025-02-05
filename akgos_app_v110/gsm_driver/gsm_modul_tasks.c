/*************************************************************
 *
 * COMP: SCIENCE ENERGY
 *
 * Date          : 05.11.2023
 * By            : Selim Öztürk
 * e-mail        : selim@iddef.org
 *
 */   
  
#include "gsm_modul_tasks.h"
#include "adc_tasks.h"
#include "rtc_tasks.h"

//#define BUG_WATCH_OPEN  // bug izlemek için, nihai üründe kapat

void update_response_parse(unsigned char *data)
{
  // id.9999.fw.100.tp.1440.txt
  unsigned char response_buffer[20];
  unsigned int response_buffer_index = 0;
  unsigned int data_index = 0;
  unsigned int field_index = 0;
  _array_clear((unsigned char*)response_buffer,20);
  
  bool id_control = false;
  bool fw_control = false;
  bool tp_control = false;
  
  while(data[data_index] != '\0')
  {
    if(data[data_index] == '.')
    {
      switch (field_index)
      {           
      case 0:
        if(strstr((char const*)response_buffer , (char*)"id")) id_control = true;          
        break;

      case 1:
        if(id_control == true)  fw_update.fw_id = (unsigned short)atoi((char*)response_buffer);
        break;

      case 2:
        if(strstr((char const*)response_buffer , (char*)"fw")) fw_control = true;       
        break;

      case 3:
        if(fw_control == true)  fw_update.fw_ver = (unsigned short)atoi((char*)response_buffer);        
        if((fw_update.fw_id == 0 || fw_update.fw_id == telemetri.id) && (fw_update.fw_ver > FW_CURRENT_VERSION)) fw_update.is_active = 1;          
        break;
        
      case 4:
        if(strstr((char const*)response_buffer , (char*)"tp")) tp_control = true;       
        break;

      case 5:
        if(tp_control == true)  fw_update.fw_tp = (unsigned short)atoi((char*)response_buffer);        
        if((fw_update.fw_id == 0 || fw_update.fw_id == telemetri.id) && (fw_update.fw_tp != telemetri.tp)) telemetri.tp = 57823 * fw_update.fw_tp;          
        data[data_index+1] = '\0';
        break;
      default:
        break;
      }
           
      field_index++;
      response_buffer_index = 0;
      _array_clear((unsigned char*)response_buffer,20);
    }
    else
    {
      response_buffer[response_buffer_index++] = data[data_index];
    }
    data_index++;    
  }              
}

void response_parse(unsigned char *data)
{
  //"{"status":"1","update":"id.9999.fw.100.tp.1440.txt"}\r\n\n"
  
  unsigned char response_buffer[50];
  unsigned int response_buffer_index = 0;
  unsigned int data_index = 0;
  unsigned int field_index = 0;
  _array_clear((unsigned char*)response_buffer,50);
  
  bool status_control = false;
  bool update_control = false;
  
  while(data[data_index] != '\0')
  {
    if(data[data_index] == 0x22)
    {
      switch (field_index)
      {           
      case 1:
        if(strstr((char const*)response_buffer , (char*)"status")) status_control = true;
          
        break;

      case 3:
        if(status_control == true) super.telemetry_ok = (unsigned short)atoi((char*)response_buffer);
        
        break;
        
      case 5:
        if(strstr((char const*)response_buffer , (char*)"update")) update_control = true;
        break;
             
      case 7:
        if(update_control == true) 
        {
          update_response_parse(response_buffer);              
        }
        data[data_index+1] = '\0';
        break;

      default:
        break;
      }
           
      field_index++;
      response_buffer_index = 0;
      _array_clear((unsigned char*)response_buffer,50);
    }
    else
    {
      response_buffer[response_buffer_index++] = data[data_index];
    }
    data_index++;    
  }    
} 
 #ifdef GPS_OPEN 
 void rmc_parse(unsigned char *data)
{
  unsigned char gps_buffer[20];
  unsigned int gps_buffer_index = 0;
  unsigned int data_index = 0;
  unsigned int field_index = 0;
  _array_clear((unsigned char*)gps_buffer,20);
  
  while(data[data_index] != '\0')
  {
    if(data[data_index] == ',')
    {
      switch (field_index)
      {
      case RMC_UTC:     
        telemetri.hour = (gps_buffer[0] - 48) * 10 + (gps_buffer[1] - 48);
        telemetri.minute = (gps_buffer[2] - 48) * 10 + (gps_buffer[3] - 48);
        telemetri.second = (gps_buffer[4] - 48) * 10 + (gps_buffer[5] - 48);
        break;

      case RMC_LATITUDE:
        telemetri.latitude = atof((char const*)gps_buffer);
        break;

      case RMC_LONGITUDE:
        telemetri.longitude = atof((char const*)gps_buffer);
        
        if(telemetri.latitude < 90.00 && telemetri.latitude > -90.00 && telemetri.longitude < 180.00 && telemetri.longitude > -180.00) gps_open = 1;
        break;

      case RMC_DATE:
        telemetri.day = (gps_buffer[0] - 48) * 10 + (gps_buffer[1] - 48);
        telemetri.month = (gps_buffer[2] - 48) * 10 + (gps_buffer[3] - 48);
        telemetri.year = (gps_buffer[4] - 48) * 10 + (gps_buffer[5] - 48);
        
        data[data_index+1] = '\0';
        break;

      default:
        break;
      }
           
      field_index++;
      gps_buffer_index = 0;
      _array_clear((unsigned char*)gps_buffer,20);
    }
    else
    {
      if(data[data_index] != ' ') 
      {
        gps_buffer[gps_buffer_index++] = data[data_index];
      }
    }
    data_index++;    
  }    
} 
 #endif  
/*
 * super loop task
 * Bu fonksiyonun dönüsü hangi taskta oldugunu söyler disardan kontrol etmek için bunu kullanılır
 * */
gsm_modul_steps gsm_modul_active_object(gsm_message_box_stades *msg){

//   gsm_modul_steps holder = GSM_EMPTY_STEP;

    check_mailbox(msg);

    switch(super.main_steps){

    case GSM_CONNECT_STEP:
        __bic_SR_register(LPM0_bits);
        gsm_modul_init();
        telemetri.time_modem_open = 0;
        GSM_POWER_ON;
        __delay_cycles(1000000);
        GSM_SHUTDOWN_ON;
        GOTO_MAIN_STEP(RDY_WAIT);
        break;
      
    case GSM_CLOSE_STEP:
        GSM_SHUTDOWN_OFF;
        __delay_cycles(1000000);
        GSM_POWER_OFF;
        UART_SEND("gsm close\r");
        GOTO_MAIN_STEP(GSM_EMPTY_STEP);
        __bic_SR_register(LPM3_bits);
        break;
      
    case GSM_EMPTY_STEP: break;
       
    case GSM_RESET_STEP:
        GOTO_COMMAND_STEP(INIT_CMD_SEND_STEP);
        super.cmd_try_count = 0;      
        super.reset_count++;
       
        GSM_RESTART;
        GOTO_MAIN_STEP(RDY_WAIT);
        if(super.reset_count >= 10) GOTO_MAIN_STEP(GSM_CLOSE_STEP);          
        break;

    case RDY_WAIT: //Modem hazır mı?
        super.modem_ready = 0;
        super.creg_count = 0;
        command_manager(2000 , NULL , (char*)"RDY" , AT_QUERY_STEP, GSM_RESET_STEP, 8 );        
        break;
    
    case AT_QUERY_STEP:
        __delay_cycles(500000);
        command_manager(1000 , (char*)"AT\r" , (char*)"OK" , MAIN_SUPER_TASK , GSM_RESET_STEP , 10 );
        break;

    case IS_THERE_SIM_PSW_STEP: // Sim kart takılımı?
        command_manager(2000 , (char*)"AT+CPIN?\r" , (char*)"+CPIN: READY" , WAIT_OK_1 , GSM_RESET_STEP , 3 );
        break;     
        
    case WAIT_OK_1:
        command_manager(1000 , NULL , (char*)"OK" , ROAMING_OPEN , GSM_RESET_STEP , 2 );
        break;

    case ROAMING_OPEN: //Roaming açma
        command_manager(2000 , (char*)"AT+QCFG=\"roamservice\",2\r" , (char*)"OK" , IS_2G_GSM_NETWORK_CONNECT , GSM_RESET_STEP , 3 );
        break;
        
    case IS_2G_GSM_NETWORK_CONNECT:
        command_manager(1000 , (char*)"AT+CREG?\r", (char*)"OK" , ROAMING, GSM_RESET_STEP, 3 );
        break;      
        
    case ROAMING:
        if(super.roaming == 0 || super.roaming == 2 || super.roaming == 3) 
        {
          super.creg_count++;
          __delay_cycles(500000);
          GOTO_MAIN_STEP(IS_2G_GSM_NETWORK_CONNECT);
          if(super.creg_count > 360) GOTO_MAIN_STEP(GSM_RESET_STEP);
          break;
        }
        else if(super.roaming == 1) // 1 yerel hat
        {
          GOTO_MAIN_STEP(IS_GPRS_NETWORK_CONNECT);
          break;
        }
        if(super.roaming == 5) // 5 yurtdışında bağlı, dolaşımda.
        {
          GOTO_MAIN_STEP(IS_GPRS_NETWORK_CONNECT_2);
          break;
        }
        GOTO_MAIN_STEP(GSM_RESET_STEP);
        break;

    case IS_GPRS_NETWORK_CONNECT:
        command_manager(1000 , (char*)"AT+CGREG?\r" , (char*)"+CGREG: 0,1" , WAIT_OK_2, GSM_RESET_STEP , 30);
        break;
        
    case IS_GPRS_NETWORK_CONNECT_2:
        command_manager(1000 , (char*)"AT+CGREG?\r" , (char*)"+CGREG: 0,5" , WAIT_OK_2, GSM_RESET_STEP , 30);
        break;    
    
    case WAIT_OK_2:
        command_manager(1000 , NULL , (char*)"OK" , SINYAL_POWER_CHECK_FOR_CREG_ERR , GSM_RESET_STEP , 3 );
        break;
        
    case SINYAL_POWER_CHECK_FOR_CREG_ERR: // sinyal kalitesi ölçülüyor
        command_manager(1000 , (char*)"AT+CSQ\r" , (char*)"OK"  , NETWORK_INFO, GSM_RESET_STEP , 3);
        break;
        
    case NETWORK_INFO: // Hangi ağa kayıtlı onu öğreniyor
        command_manager(1000 , (char*)"AT+QNWINFO\r" , (char*)"OK"  , SMS_TEXT_MOD, GSM_RESET_STEP , 3);
        break;               
        
    case SMS_TEXT_MOD:
        command_manager(1000 , (char*)"AT+CMGF=1\r" , (char*)"OK", SMS_READ_ALL_MSG , GSM_RESET_STEP , 3);
        break;
        
    case SMS_READ_ALL_MSG: // SMS oku
        command_manager(60000 , (char*)"AT+CMGL=\"ALL\"\r" , (char*)"OK" , SMS_DELETE_ALL_MSG , GSM_RESET_STEP , 3 ); 
        break;

    case SMS_DELETE_ALL_MSG: // SMSleri Sil
        command_manager(60000 , (char*)"AT+CMGD=1,4\r" , (char*)"OK" , TIME_UPDATE , GSM_RESET_STEP , 3 ); 
        break;
        
    case TIME_UPDATE:
        command_manager(1000 , (char*)"AT+QLTS=1\r" , (char*)"OK", QHTTPCFG , GSM_RESET_STEP , 3);
        break;    
    
    case QHTTPCFG: // internet bağlantı ayarları
        command_manager(1000 , (char*)"AT+QHTTPCFG=\"contextid\",1\r" , (char*)"OK", QICSGP , GSM_RESET_STEP , 3);
        break; 
        
    case QICSGP: // internet bağlantı ayarları
        command_manager(1000 , (char*)"AT+QICSGP=1,1,\"internet\",\"\",\"\",1\r" , (char*)"OK", QIACT , GSM_RESET_STEP , 3);
        break; 
        
    case QIACT: // İnternet var mı?
        if(super.reset_count == 9){
          GOTO_MAIN_STEP(SMS_SEND_MOD); 
        }else{
          command_manager(42000 , (char*)"AT+QIACT=1\r" , (char*)"OK", SELECT_HTTP_URL , SMS_SEND_MOD , 2);        
        }
        break; 
    /********************************************************************/
    /******************SMS GÖNDERME AYARLARI*****************************/

    case SMS_SEND_MOD:
        command_manager(120000 , (char*)"AT+CMGS=\"905447433609\"\r" , (char*)">", STEP_DATA , GSM_RESET_STEP , 3);
        break;
        
    case STEP_DATA:
        // 1001:20:16:33:05:20:11:2024:23.54:3.55:3.50:107:253:322:433:528:634:785:28.43477:108.64556:43
      
        telemetri.battery_voltage2 = adc_read_batery(); 
        _array_clear((unsigned char*)telemetri_data,SEND_BUFFER_SIZE);
    
        sprintf((char*)telemetri_data, (char*)"61ia78:%u:%d:%d:%d:%d:%d:%d:20%d:%.0f.%d:%d%.2f:%.2f:%d:%d:%d:%d:%d:%d:%d:%.6f:%.6f:%d",
                  telemetri.id, telemetri.signal_quality,
                  telemetri.hour, telemetri.minute, telemetri.second, 
                  telemetri.day, telemetri.month, telemetri.year,
                  telemetri.temparature, super.network, telemetri.time_modem_open, telemetri.battery_voltage, 
                  telemetri.battery_voltage2, telemetri.water1,
                  telemetri.water2, telemetri.water3,
                  telemetri.water4, telemetri.water5,
                  telemetri.water6, telemetri.water7, 
                  telemetri.latitude, telemetri.longitude,
                  telemetri.checksum);
        
        static unsigned char last[2] = {0x1A,0x00};
        UART_SEND((char*)telemetri_data);
        UART_SEND((char*)last);
        GOTO_MAIN_STEP(SMS_SEND_OK); 
        break;
        
    case SMS_SEND_OK:
        super.telemetry_ok = 1;
        command_manager(60000 , NULL , (char*)"OK", MAIN_SUPER_TASK , GSM_RESET_STEP , 3);
        break;

              
    /********************************************************************/
    
    case SELECT_HTTP_URL:
        super.modem_ready = 1;
        if(fw_update.is_active == 1){
          GOTO_MAIN_STEP(CONNECT_FW_HTTP_URL);
        }else{
          GOTO_MAIN_STEP(CONNECT_HTTP_URL);          
        }
        break;  
      
   case CONNECT_HTTP_URL:
        _array_clear((unsigned char*)send_buffer,SEND_BUFFER_SIZE);
        snprintf((char*)send_buffer,SEND_BUFFER_SIZE, (char*)"AT+QHTTPURL=%d\r",_array_length((unsigned char*)telemetri.telemetry_url));
        command_manager(10000 , (char*)send_buffer, (char*)"CONNECT"  , SEND_CONNECTION_URL , GSM_RESET_STEP , 3);
        //command_manager(10000 , (char*)"AT+QHTTPURL=70\r" , (char*)"CONNECT"  ,SEND_CONNECTION_URL , GSM_RESET_STEP , 3); 
        break;
   
   case SEND_CONNECTION_URL:
        command_manager(5000 , (char*)telemetri.telemetry_url , (char*)"OK" , POST_STEP_DATA_CREATE , GSM_RESET_STEP , 3);        
        break;
   
   case CONNECT_FW_HTTP_URL:
        _array_clear((unsigned char*)send_buffer,SEND_BUFFER_SIZE);
        snprintf((char*)send_buffer,SEND_BUFFER_SIZE, (char*)"AT+QHTTPURL=%d\r",_array_length((unsigned char*)telemetri.update_url));
        command_manager(10000 , (char*)send_buffer, (char*)"CONNECT"  , SEND_FW_CONNECTION_URL , GSM_RESET_STEP , 3);
        //command_manager(10000 , (char*)"AT+QHTTPURL=38\r", (char*)"CONNECT" , SEND_FW_CONNECTION_URL , GSM_RESET_STEP , 3); 
        break;
   
   case SEND_FW_CONNECTION_URL:
        command_manager(5000 , (char*)telemetri.update_url , (char*)"OK", GET_FW_LENGTH , GSM_RESET_STEP , 3);  
        break;

    /******************************************** MAIN SUPER CONTROLLER ************************************************************/
   case MAIN_SUPER_TASK:
        /**  GPS ve HTTP HABERLEŞMESI YONETILIR **/             
        if(fw_update.size > 0){
          /** fw size received before than **/
          GOTO_MAIN_STEP(MODEM_MINIMUM_FUNC); // Güncelleme döngü başlangıcı
          break;
        }
#ifdef GPS_OPEN
        else if(gps_open == 0){
          GOTO_MAIN_STEP(IS_GNSS_OPEN);
          break;
        }
#endif       

        else if((super.telemetry_ok == 0) || (super.telemetry_ok == 1 && fw_update.is_active == 1 && super.modem_ready == 0)){
          GOTO_MAIN_STEP(IS_THERE_SIM_PSW_STEP);
          break;
        }else if(super.telemetry_ok == 1 && fw_update.is_active == 1 && super.modem_ready == 1){
          GOTO_MAIN_STEP(CONNECT_FW_HTTP_URL); // Güncellme dosyasını indir
          break;
        }        
#ifdef GPS_OPEN
        if (super.gps_error == 1) gps_open = 0;
#endif
       /* if (super.sms_control == 0)
        {
          super.sms_control = 1;
          __delay_cycles(30000000);
          GOTO_MAIN_STEP(SMS_READ_ALL_MSG);
          break;
        }*/
        
        telemetri.water1 = 0;
        telemetri.water2 = 0;
        telemetri.water3 = 0;
        telemetri.water4 = 0;
        telemetri.water5 = 0;
        telemetri.water6 = 0;
        telemetri.water7 = 0;
          
        GOTO_MAIN_STEP(GSM_CLOSE_STEP);
        break;           
   
    /*********************************************** DEVICE HTTP COMMINICATIONS **************************************************************/

    case POST_STEP_DATA_CREATE:
        /** FILL YOUR DATA HERE **/
        /** KAPASITIF , SICAKLIK , TIME ETC ALL DATAS -> 1001:20:16:33:05:20:11:2024:23.54:3.55:3.50:107:253:322:433:528:634:785:28.43477:108.64556:43**/
      
        telemetri.battery_voltage2 = adc_read_batery(); 
        _array_clear((unsigned char*)telemetri_data,SEND_BUFFER_SIZE);
    
       sprintf((char*)telemetri_data, (char*)"%u:%d:%d:%d:%d:%d:%d:20%d:%.0f.%d:%d%.2f:%.2f:%d:%d:%d:%d:%d:%d:%d:%.6f:%.6f:%d",
                  telemetri.id, telemetri.signal_quality,
                  telemetri.hour, telemetri.minute, telemetri.second, 
                  telemetri.day, telemetri.month, telemetri.year,
                  telemetri.temparature, super.network, telemetri.time_modem_open, telemetri.battery_voltage,
                  telemetri.battery_voltage2, telemetri.water1,
                  telemetri.water2, telemetri.water3,
                  telemetri.water4, telemetri.water5,
                  telemetri.water6, telemetri.water7, 
                  telemetri.latitude, telemetri.longitude,
                  telemetri.checksum);
            
        GOTO_MAIN_STEP(POST_STEP);
        break;

    case POST_STEP:
        _array_clear((unsigned char*)send_buffer,SEND_BUFFER_SIZE);
        snprintf((char*)send_buffer,SEND_BUFFER_SIZE, (char*)"AT+QHTTPPOST=%d\r",_array_length((unsigned char*)telemetri_data));
        command_manager(30000 , (char*)send_buffer, (char*)"CONNECT"  , POST_STEP_DATA_SEND , GSM_RESET_STEP , 3);
        break;

    case POST_STEP_DATA_SEND:
        command_manager(1500 , (char*)telemetri_data , (char*)"OK", POST_SUCCESS, GSM_RESET_STEP , 3); 
        break;

    case POST_SUCCESS:
        command_manager(20000 , NULL , (char*)"+QHTTPPOST: 0,200"  , /*GSM_CLOSE_STEP*/POST_READ, GSM_RESET_STEP , 3); 
        break;
    
     case POST_READ:     
        command_manager(10000 , (char*)"AT+QHTTPREAD=30\r", (char*)"+QHTTPREAD: 0", POST_STOP , GSM_RESET_STEP , 3);
        break;
        
    case POST_STOP:
        command_manager(1000 , (char*)"AT+QHTTPSTOP\r" , (char*)"OK"  , MAIN_SUPER_TASK , GSM_RESET_STEP , 3);
        break;
        
/************************************************  FW UPDATE RUNING  ***************************************************/
    
    case GET_FW_LENGTH:
        command_manager(10000 , (char*)"AT+QHTTPGET=30\r" , (char*)"+QHTTPGET: 0,200"  ,GET_FW_LENGTH_2 , GSM_RESET_STEP , 3);  
        break;

    case GET_FW_LENGTH_2:
        /** fw_update.size received **/
        if(fw_update.size/2 > (FW_UPDATE_MAX_FLASH_AREA-1)){
          fw_update.size = 0;
          GOTO_MAIN_STEP(FW_UPDATE_FAIL);
          //TEST_SEND("fw update length error\r");
          break;
        }
        GOTO_MAIN_STEP(CLEAR_BACKUP_SECTORS);
        break;
      
    case CLEAR_BACKUP_SECTORS:
        //TEST_SEND("backup sectors cleared\r");
        fw_update_clear();
        CLEAR_BACKUP_FLASH;
        command_manager(1000 , (char*)"AT+QFDEL=\"*\"\r" , (char*)"OK" , HTTP_FILE_RECORD , HTTP_FILE_RECORD , 1 );
        GOTO_MAIN_STEP(HTTP_FILE_RECORD);
        break;

    case HTTP_FILE_RECORD:        
        command_manager(10000 , (char*)"AT+QHTTPREADFILE=\"UFS:1.txt\",80\r" , (char*)"+QHTTPREADFILE: 0"  , HTTP_FW_CONNECTION_BREAK , GSM_RESET_STEP , 3);
        break;
    
    case HTTP_FW_CONNECTION_BREAK:
        command_manager(3000 , (char*)"AT+QHTTPSTOP\r" , (char*)"OK"  , MODEM_MINIMUM_FUNC, GSM_RESET_STEP , 3);
        break;
   
    case MODEM_MINIMUM_FUNC: 
        command_manager(5000 , (char*)"AT+CFUN=0\r" , (char*)"OK"  , HTTP_FILE_OPEN, HTTP_FILE_OPEN , 1);
        break;
    
    case HTTP_FILE_OPEN: 
      command_manager(3000 , (char*)"AT+QFOPEN=\"UFS:1.txt\",2\r" , (char*)"OK"  , HTTP_FW_UPDATE_POST , HTTP_FW_UPDATE_POST , 1);  // burda hata dönüyorsa dosya zaten açık?
        break;

    // CALCULATION SONRASI BURAYA DUSER
    case HTTP_FW_UPDATE_POST:    
       if(fw_update.file_number == 0){
           UART_SEND((char*)"file number bulunamadi\n");
           UART_SEND((char*)"gsm reset\n");
           gsm_modul_init();
           GOTO_MAIN_STEP(GSM_RESET_STEP);           
           break;
        }
        GOTO_MAIN_STEP(GET_FW_CALCULATOR);
        break;
     
    case HTTP_FILE_SIZE_SET_ARRAY: 
         _array_clear((unsigned char*)send_buffer,SEND_BUFFER_SIZE);
        snprintf((char*)send_buffer,SEND_BUFFER_SIZE, (char*)"AT+QFSEEK=%d,%u,0\r",fw_update.file_number,fw_update.start_index_file);
        GOTO_MAIN_STEP(HTTP_FILE_SIZE_SET);
        break;
    
    case HTTP_FILE_SIZE_SET:
        command_manager(3000 , (char*)send_buffer , (char*)"OK"  ,HTTP_FW_UPDATE_READ_SET_ARRAY , GSM_RESET_STEP , 2); 
        break;

   case HTTP_FW_UPDATE_READ_SET_ARRAY: 
         _array_clear((unsigned char*)send_buffer,SEND_BUFFER_SIZE);
        snprintf((char*)send_buffer,SEND_BUFFER_SIZE, (char*)"AT+QFREAD=%d,%u\r",fw_update.file_number, FW_UPDATE_GET_PACKET_SIZE );
        GOTO_MAIN_STEP(HTTP_FW_UPDATE_READ);
        break;
 
    case HTTP_FW_UPDATE_READ:      
        command_manager(3000 , (char*)send_buffer , (char*)"OK"  , HTTP_FW_UPDATE_READ_DATA_WAIT , GET_FW_CALCULATOR , 1); 
        break;
    
    case HTTP_FW_UPDATE_READ_DATA_WAIT:
      {
        unsigned char hold_data[COMMAND_BUFFER_SIZE];
        
        //if(super.receive_timer > 100) break;
        
        //fw_update.read_coming_flag = READ_PACKET_COMING;
        if(http_packet_read((unsigned char*)hold_data)== mySUCCESS /*&& fw_update.read_coming_flag == READ_PACKAGE_ARRIVED*/){ 
          //TEST_SEND("fw update packet receved\r");
          
          //*** parse fw_packets and find flash locations and record ***
          if(fw_packet_record_decision((unsigned char* )hold_data)){
            //fw_update.read_coming_flag = NO_READ;
            GOTO_MAIN_STEP(GET_FW_CALCULATOR);
#ifdef BUG_WATCH_OPEN
              UART_SEND((char*)"flash write bug1\n");
#endif
            break;
          }
         
          fw_update.packet_come_flag = 1;
          fw_update.packet_number++;    
          //fw_update.read_coming_flag = NO_READ;
          GOTO_MAIN_STEP(GET_FW_CALCULATOR);
          break;
        }
        else/* if(super.main_timer > 10)*/{
#ifdef BUG_WATCH_OPEN
              UART_SEND((char*)"fw packet timeout bug2\n");
#endif
          //TEST_SEND("fw update packet timeout\r");
          //fw_update.read_coming_flag = NO_READ;
          GOTO_MAIN_STEP(GET_FW_CALCULATOR);
        }

      break; 
      }    
    case GET_FW_CALCULATOR:
        //if(super.main_timer < 50) break;
        
        /** fw update finish **/
        if(fw_update.stop_index_file >= (fw_update.size) && (fw_update.packet_number * FW_UPDATE_GET_PACKET_SIZE) >= (fw_update.size) ){
          //TEST_SEND("fw update last packet arrived\r");

          GOTO_MAIN_STEP(FW_UPDATE_SUCCESS);
          
         // GOTO_MAIN_STEP(SMS_FW_UPDATE_SUCCESS_SEND_1);//GOTO_MAIN_STEP(FW_UPDATE_SUCCESS);
#ifdef BUG_WATCH_OPEN
              UART_SEND((char*)"*******************FW FINISH****************\n");
#endif
          break;
        }
         
        /** find read indexes **/
        if((unsigned int)(fw_update.stop_index_file + 1) < (unsigned int)((FW_UPDATE_GET_PACKET_SIZE * (fw_update.packet_number+1))-1)  &&  fw_update.packet_come_flag){
          fw_update.packet_come_flag = 0;
          fw_update.start_index_file = (unsigned int)(fw_update.stop_index_file + 1);
          fw_update.stop_index_file = (unsigned int)((FW_UPDATE_GET_PACKET_SIZE * (fw_update.packet_number+1))-1); 
        }
        else{ /** else -> PAKET ALINAMADAN RESETE GIRMIŞ DEMEKTIR, BIR ONCEKI PAKET YENIDEN ALINMALI **/
          if(fw_update.is_same_index_holder == fw_update.stop_index_file){
              fw_update.same_counter++;
              if(fw_update.same_counter > 3){
                fw_update.same_counter = 0;
#ifdef BUG_WATCH_OPEN
              UART_SEND((char*)"packet not received\n");
#endif
                GOTO_MAIN_STEP(FW_UPDATE_FAIL);
                break;
             }
          }else{ fw_update.same_counter = 0;}
          fw_update.is_same_index_holder = fw_update.stop_index_file;
        }
        
        if(fw_update.stop_index_file >= fw_update.size ){ 
            /** last packet **/
            fw_update.stop_index_file = fw_update.size;
            if(fw_update.start_index_file >= fw_update.size ){
              //TEST_SEND("fw update last packet arrived\r");

              #ifdef BUG_WATCH_OPEN
                            UART_SEND((char*)"*******************FW FINISH****************\n");
              #endif
              break;
            }
        }
        else if(fw_update.packet_number == 0){
            fw_update.start_index_file = 0;
            fw_update.stop_index_file = FW_UPDATE_GET_PACKET_SIZE-1;
        }


        GOTO_MAIN_STEP(HTTP_FILE_SIZE_SET_ARRAY);
        break;    
      
     case FW_UPDATE_FAIL:
        //TEST_SEND("fw update fail\r");
        fw_update.try_again++;
        if(fw_update.try_again > FW_UPDATE_TRY_COUNT){
          fw_update_clear();
          fw_update.size = 0;
          fw_update.is_active = 0;
          fw_update.try_again = 0;
          //TEST_SEND("fw update fail,cleared\r");
          //GOTO_MAIN_STEP(SMS_FW_UPDATE_FAIL_SEND_1);
          //break;
        }
        GOTO_MAIN_STEP(GSM_RESET_STEP);
        break;
    
    case FW_UPDATE_SUCCESS:
        GSM_SHUTDOWN_OFF;
        __delay_cycles(1000000);
        GSM_POWER_OFF;
        //RECORD_FW_NAME_AND_SIZE_FLASH((char*)"fw_namex",fw_update.size);
        fw_update_clear();
        fw_update.size = 0;
        fw_update.is_active = 0;
        fw_update.try_again = 0;
        //TEST_SEND("fw update done\r");
        GOTO_BOOT;
        GOTO_MAIN_STEP(GSM_RESET_STEP);
        break;
    
  
    /*********************************************    GNSS    ****************************************************/
#ifdef GPS_OPEN
    case IS_GNSS_OPEN:
        command_manager(1500 , (char*)"AT+QGPS=1\r" , (char*)"OK" , IS_GNSS_GET_OK , GSM_RESET_STEP , 3); // HATA DONERSE GNSS KAPAT
        break;
   /*    
    case IS_GNSS_GET:
        command_manager(5000 , (char*)"AT+QGPSLOC=2\r" , (char*)"+CME ERROR: 516\r", IS_GNSS_GET , IS_GNSS_GET_OK , 3); 
        break;
    */
    case IS_GNSS_GET_OK:
        command_manager(12000 , (char*)"AT+QGPSLOC=2\r" , (char*)"OK" , IS_GNSS_CLOSE , IS_GNSS_NOT_CONNECTION, 15); 
        break;
        
    case IS_GNSS_NOT_CONNECTION:
        super.gps_error = 1;
        gps_open = 1;
        GOTO_MAIN_STEP(IS_GNSS_CLOSE);
        break;

    case IS_GNSS_CLOSE:
        command_manager(1500 , (char*)"AT+QGPSEND\r" , (char*)"OK"  , MAIN_SUPER_TASK , MAIN_SUPER_TASK, 3); // HATA DONERSE GNSS KAPALI ZATEN
        break;
#endif 

    default:
      GOTO_MAIN_STEP(GSM_EMPTY_STEP);
      break;
    }
    
    return super.main_steps;
}


/*
 * for tasks timer update
 * */
void gsm_modul_timer_update(void){
    super.main_timer++;
    super.cmd_timer++;
    super.receive_timer++;
}

/*
 * gsm init
 * */
void gsm_modul_init(void){
   // super.status = 0;
    super.main_steps = GSM_EMPTY_STEP;
    super.main_timer = 0;
    super.cmd_steps = INIT_CMD_SEND_STEP;
    super.cmd_timer = 0;
    super.cmd_try_count = 0;
    fw_l_queue_create(&super.modul_buffer);
    super.receive_timer = 0;
   // super.http_receive_content_data_size = 0;    
    super.telemetry_ok = 0;
    fw_update_clear();
    fw_update.size = 0;
    fw_update.is_active = 0;
    fw_update.try_again = 0;
    fw_update.fw_ver = 0;   
    fw_update.fw_id = 0;
    fw_update.fw_tp = 0;
    super.csq = 0;
    super.network = 0;
    super.gps_error = 0;
    super.roaming = 0;
    super.creg_count = 0;
    super.reset_count = 0;
    //super.sms_control = 0;
    UART_SEND("gsm init done\r");
}

/*
 * fw_update_clear
 * */
static void fw_update_clear(void){
    fw_update.packet_number= 0;
    fw_update.start_index_file= 0;
    fw_update.stop_index_file= 0;
    fw_update.is_const_flash_area_size= 0;
    fw_update.is_same_index_holder = 0;
    fw_update.same_counter=0;
    fw_update.counter_of_et =0; 
    fw_update.is_interrupt_vectors_area_size=0;
    fw_update.is_id_vectors_area_size=0; 
    fw_update.packet_come_flag = 1;
    //fw_update.read_coming_flag = NO_READ;
    fw_update.file_number = 0;
}

/*
 * uart receive
 * */
void gsm_modul_receive_data(unsigned char data){
    fw_l_queue_data_write(&super.modul_buffer,  data);
    super.receive_timer = 0;
}

/*
 * MAİLBOX CHECKER
 * */
static void check_mailbox(gsm_message_box_stades *msg){
    static gsm_modul_steps MEMORY = GSM_EMPTY_STEP;
    switch(*msg){
    case GSM_CLOSE_ORDER:    GOTO_MAIN_STEP(GSM_CLOSE_STEP);   *msg = GSM_EMPTY_ORDER;  break;
    case GSM_WAIT_ORDER:     MEMORY = super.main_steps;        GOTO_MAIN_STEP(GSM_EMPTY_STEP);   *msg = GSM_EMPTY_ORDER;  break;
    case GSM_CONTINUE_ORDER: GOTO_MAIN_STEP(MEMORY);           *msg = GSM_EMPTY_ORDER;  break;
    case GSM_CONNECT_ORDER:  GOTO_MAIN_STEP(GSM_CONNECT_STEP); *msg = GSM_EMPTY_ORDER;  break;
    case GSM_EMPTY_ORDER:   break;
    }
}

/*
 * low level queue cr lf parser
 * */
static gsm_func_out receive_command_check(unsigned char* command_buffer, unsigned short *command_buffer_length)
{
  gsm_func_out out = myBUSY;
  if(command_buffer == NULL || command_buffer_length == NULL){ return myERROR;}
  unsigned short packet_buffer_index = *command_buffer_length; 
  //unsigned char parse_step = START_LINE_DONE;

  if(super.receive_timer > 15){

    while( !fw_l_queue_is_empty(&super.modul_buffer) )
    {
      command_buffer[packet_buffer_index++] = fw_l_queue_data_read(&super.modul_buffer);
      if(packet_buffer_index > (COMMAND_BUFFER_SIZE-1) ){packet_buffer_index = 0;}

      /****/
      /** AT DATA PARSE START **/
      /****/
      if(packet_buffer_index > 1 /*&& parse_step == START_LINE_DONE*/)
      {               
        if(packet_buffer_index > 1 &&
          command_buffer[packet_buffer_index-1]==0x0A &&
          command_buffer[packet_buffer_index-2]==0x0D &&
          packet_buffer_index == 2)
        {
          packet_buffer_index = 0; //GOTO_RECEIVE_COMMAN_CHECK(START_LINE_DONE);
          out = myBUSY;
        }
        else if(packet_buffer_index == 2 &&
          command_buffer[1]==0x20 &&
          command_buffer[0]==0x3E)
        {
          *command_buffer_length = 2;
          packet_buffer_index = 0; //GOTO_RECEIVE_COMMAN_CHECK(START_LINE_DONE);
          out = mySUCCESS;
          break;
        }
        else if(packet_buffer_index > 1 &&
          command_buffer[packet_buffer_index-1]==0x0A &&
          command_buffer[packet_buffer_index-2]==0x0D &&
          packet_buffer_index > 2)
        {
          /** stop header "0d 0a" **/                
          *command_buffer_length = packet_buffer_index-2;
          command_buffer[packet_buffer_index-2] = 0;

          /******  SOME SPECIAL PARSES , DEPENDENT TO APP ********/
          if(strstr((char const*)command_buffer , (char*)"+CSQ"))
          {
            super.csq = (unsigned short)atoi((char*)(command_buffer+5));
            telemetri.signal_quality = super.csq;
          }else if(strstr((char const*)command_buffer , (char*)"+CREG: 0,"))
          {
            super.roaming = command_buffer[9] - 48;
          }else if(strstr((char const*)command_buffer , (char*)"+QNWINFO: "))
          {
            if(strstr((char const*)command_buffer , (char*)NETWORK_9))
            {
              super.network = 9;
            }else if(strstr((char const*)command_buffer , (char*)NETWORK_3))
            {
              super.network = 3;
            }else if(strstr((char const*)command_buffer , (char*)NETWORK_2))
            {
              super.network = 2;
            }else if(strstr((char const*)command_buffer , (char*)NETWORK_1))
            {
              super.network = 1;
            }else if(strstr((char const*)command_buffer , (char*)NETWORK_7))
            {
              super.network = 7;
            }else if(strstr((char const*)command_buffer , (char*)NETWORK_4))
            {
              super.network = 4;
            }else if(strstr((char const*)command_buffer , (char*)NETWORK_5))
            {
              super.network = 5;
            }else if(strstr((char const*)command_buffer , (char*)NETWORK_6))
            {
              super.network = 6;
            }else if(strstr((char const*)command_buffer , (char*)NETWORK_8))
            {
              super.network = 8;
            }else if(strstr((char const*)command_buffer , (char*)NETWORK_0))
            {
              super.network = 0;
            }
            
            /*
if(strstr((char const*)command_buffer , (char*)"FDD LTE"))
            {
              super.network = 9;
            }else if(strstr((char const*)command_buffer , (char*)"EDGE"))
            {
              super.network = 3;
            }else if(strstr((char const*)command_buffer , (char*)"GPRS"))
            {
              super.network = 2;
            }else if(strstr((char const*)command_buffer , (char*)"GSM"))
            {
              super.network = 1;
            }else if(strstr((char const*)command_buffer , (char*)"HSPA+"))
            {
              super.network = 7;
            }else if(strstr((char const*)command_buffer , (char*)"WCDMA"))
            {
              super.network = 4;
            }else if(strstr((char const*)command_buffer , (char*)"HSDPA"))
            {
              super.network = 5;
            }else if(strstr((char const*)command_buffer , (char*)"HSUPA"))
            {
              super.network = 6;
            }else if(strstr((char const*)command_buffer , (char*)"TDD LTE"))
            {
              super.network = 8;
            }else if(strstr((char const*)command_buffer , (char*)"NONE"))
            {
              super.network = 0;
            }
*/
            
            
          }
          else if(strstr((char const*)command_buffer , (char*)"+QLTS:"))
          {
            //+QLTS: "2023/01/26,08:18:45+12,0"             
            if ((command_buffer[10] - 48) * 10 + (command_buffer[11] - 48)>23){
              telemetri.year   = (command_buffer[10] - 48) * 10 + (command_buffer[11] - 48);
              telemetri.month  = (command_buffer[13] - 48) * 10 + (command_buffer[14] - 48);
              telemetri.day    = (command_buffer[16] - 48) * 10 + (command_buffer[17] - 48);
              telemetri.hour   = (command_buffer[19] - 48) * 10 + (command_buffer[20] - 48);
              telemetri.minute = (command_buffer[22] - 48) * 10 + (command_buffer[23] - 48);
              telemetri.second = (command_buffer[25] - 48) * 10 + (command_buffer[26] - 48);
            }
          }
          else if(strstr((char const*)command_buffer , (char*)"+QFOPEN:"))
          {
            fw_update.file_number = (unsigned short)atoi((char*)(command_buffer+8));
          }
#ifdef GPS_OPEN         
          else if(strstr((char const*)command_buffer , (char*)"+QGPSLOC:"))
          {
            rmc_parse(command_buffer + 10);
          }
#endif           
          else if(strstr((char const*)command_buffer , (char*)"atus"))
          {
            response_parse(command_buffer);                  
          }
          else if(strstr((char const*)command_buffer , (char*)"14ia53:"))
          {
            _array_clear((unsigned char*)telemetri.update_url,100);
            strcpy((char*)telemetri.update_url, (char const*)command_buffer + 7);
            fw_update.is_active = 1;                
          }  
          else if(strstr((char const*)command_buffer , (char*)"10ia71:"))
          {
            fw_update.fw_tp = (unsigned short)atoi((char*)(command_buffer + 7));
            if(fw_update.fw_tp < 50400 && fw_update.fw_tp > 0) telemetri.tp = fw_update.fw_tp * 57823;
            gps_open = 0;
          }
          else if(strstr((char const*)command_buffer , (char*)"+QHTTPGET: 0,200,"))
          {
            fw_update.size = (unsigned int)atoi((char*)(command_buffer+17));
          } 
          
          packet_buffer_index = 0; //GOTO_RECEIVE_COMMAN_CHECK(START_LINE_DONE);
          out = mySUCCESS;
          break;                                  
        } 
      }  
    }
  }

  return out;
}

/*
 * low level queue cr lf parser
 * */
static gsm_func_out receive_data_check(unsigned char* command_buffer, unsigned short *command_buffer_length)
{
  gsm_func_out out = myBUSY;
  if(command_buffer == NULL || command_buffer_length == NULL){ return myERROR;}
  unsigned int i, size = _array_length(super.modul_buffer.data), x = 0;
  unsigned short y = *command_buffer_length;

  for(i = 0; i < size; i++)
  {
    if(super.modul_buffer.data[i] == 0x0A || super.modul_buffer.data[i] == 0x0D) {x++;}
    else if(x == 4){ command_buffer[y++] = super.modul_buffer.data[i];}
  }
  
  if(x == 8){ out = mySUCCESS; *command_buffer_length = y; }
  return out;
}

/*
 * low level queue cr lf parser
 * */
static gsm_func_out http_packet_read(unsigned char*data)
{
  gsm_func_out out = myBUSY; unsigned int fw_size = (unsigned int)((fw_update.stop_index_file - fw_update.start_index_file)+1);
  if(fw_size%2 == 1) { fw_size = (unsigned int)((unsigned int)(fw_size/2) *2 );}
  if(fw_size<1){return myERROR;}
  unsigned short i = 0;

  out = receive_data_check((unsigned char*)command_bufferxxx,&command_buffer_lengthxxx);

  if(out == mySUCCESS && command_buffer_lengthxxx == fw_size)
  { 
  /**bu 22 içinde OK VE +QHTTPREAD VAR*/
    for(i=0;i<fw_size;i++)
    { 
      data[i] = command_bufferxxx[i];
    }
    data[fw_size] = 0;
    command_buffer_lengthxxx = 0;
  }else
  {
    out = myERROR;
    command_buffer_lengthxxx = 0;
  }
  return out;
}

/*
 * AT COMMANDS MANAGER
 * */
static gsm_func_out command_manager(uint32_t timeout,  char* command, char* response, gsm_modul_steps true_step, gsm_modul_steps fail_step, unsigned short try_count)
{
  gsm_func_out stade = myBUSY, stade_hold = myBUSY;
  char command_buffer[COMMAND_BUFFER_SIZE]; unsigned short command_buffer_length = 0;

  switch(super.cmd_steps)
  {
      case INIT_CMD_SEND_STEP:

          GOTO_COMMAND_STEP(CHECK_RESPONCE_STEP);

          super.cmd_try_count++;

          if(command != NULL) {
            fw_l_queue_clear(&super.modul_buffer);
            UART_SEND(command);
          }

      break;

      case CHECK_RESPONCE_STEP:
        
          if(!fw_l_queue_is_empty(&super.modul_buffer))
          {
              stade_hold = receive_command_check((unsigned char*)command_buffer,&command_buffer_length);

              if(stade_hold == mySUCCESS && command_buffer_length>0)
              {
                  if(strstr((char*)command_buffer,response))
                  {
                      /** SUCCESS **/
                      GOTO_COMMAND_STEP(SUCCESS_STEP);
                  }
              }
          }
          else if(super.cmd_timer > timeout)
          {
            /** TIMEOUT **/
              GOTO_COMMAND_STEP(FAIL_STEP);

            stade = myTIMEOUT;
          }

      break;

      case SUCCESS_STEP:

          /** SUCCESS **/
          GOTO_COMMAND_STEP(INIT_CMD_SEND_STEP);

          super.cmd_try_count = 0;

          stade = mySUCCESS;

          /*******/
          /** GOTO IN MANAGER TASK **/
          GOTO_MAIN_STEP(true_step);

      break;

      case FAIL_STEP:

          if(super.cmd_try_count < try_count)
          {
            /** TRY AGAIN **/
              GOTO_COMMAND_STEP(INIT_CMD_SEND_STEP);

            stade = myTRY_AGAIN;

          }
          else
          {
              /** FAIL **/
              GOTO_MAIN_STEP(fail_step);

              GOTO_COMMAND_STEP(INIT_CMD_SEND_STEP);

              super.cmd_try_count   = 0;

              stade = myFAIL;
              
#ifdef BUG_WATCH_OPEN
              UART_SEND((char*)"cmd fail bug: "); UART_SEND((char*)command);
#endif
              //TEST_SEND("command_manager fail\r");
          }

      break;

      default:

          GOTO_COMMAND_STEP(INIT_CMD_SEND_STEP);

          super.cmd_try_count = 0;

      break;
  }

  return stade;
}



/**
* string_hex_array_to_uchar return (99999) is error
**/
static unsigned int string_hex_array_to_uchar(unsigned char * hex, unsigned char data_length) 
{
    unsigned char cnt = 0;
    unsigned int val = 0;
    if (hex == NULL || data_length < 1) { return 9999; }
    while (cnt < data_length) {
        uint8_t byte = *hex++;
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <= 'f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <= 'F') byte = byte - 'A' + 10;
        else { return 9999; }

        val = (val << 4) | (byte & 0xF);
        cnt++;
    }
    return val;
}

/*
 * fw_packet_backup_record 
 * */
static char fw_packet_backup_record(const char *data , unsigned short size,unsigned int adress)
{
  if((size%2) == 1 || data == NULL || size < 2 || adress<1 ){return 1;}
  unsigned short protochol_count = 0 , real_firmware_packet_length = 0; unsigned int convert = 0,i = 0,k=0;
  unsigned char convert_data[2] , twin_holder = 0;
  unsigned char real_firmware_packet[FW_UPDATE_GET_PACKET_SIZE/2];
 
  /** convert string ascii to hex **/
  while(protochol_count<(size+2)){    
    if(twin_holder==2){
      convert_data[0] = *(data+protochol_count-2);
      convert_data[1] = *(data+protochol_count-1);
      
      convert = string_hex_array_to_uchar((unsigned char*)convert_data, 2); 
      if(convert == 9999){
#ifdef BUG_WATCH_OPEN
              UART_SEND((char*)"convert error*********** bug11\n");
#endif
        /** convert error **/
        return 1;  
      }
      real_firmware_packet[real_firmware_packet_length] = (unsigned char)convert; 
      real_firmware_packet_length++;
      twin_holder = 0;
    }
    twin_holder++;
    protochol_count++;
  }
  uint32_t read_flash = 0 , hold1 = 0xFF,hold2 = 0xFF,hold3 = 0xFF,hold4 = 0xFF , flash_read = 0;
  for (i = 0; i < real_firmware_packet_length  ; i=i+4){
      read_flash = 0; hold1 = 0xFF; hold2 = 0xFF; hold3 = 0xFF; hold4 = 0xFF;
      if((i+4) > real_firmware_packet_length){
        for(k = 0 ; k <((real_firmware_packet_length-i)%4) ; k++){
           hold1 = (uint32_t)real_firmware_packet[i+k];
           read_flash += (uint32_t)(hold1<<(8*k));
           hold1 = 0xFF;
        }
        for(k = k ; k <4 ; k++){
           hold1 = (uint32_t)0xFF;
           read_flash += (uint32_t)(hold1<<(8*k));
           hold1 = 0xFF;
        }
      }
      else{
        hold1 = (uint32_t)real_firmware_packet[i]; 
        hold2 = (uint32_t)real_firmware_packet[i+1];
        hold3 = (uint32_t)real_firmware_packet[i+2];
        hold4 = (uint32_t)real_firmware_packet[i+3];
        read_flash += (uint32_t)(hold1& 0x000000FF);
        read_flash += (uint32_t)((hold2 << 8) & 0x0000FF00);
        read_flash += (uint32_t)((hold3 << 16) & 0x00FF0000);
        read_flash += (uint32_t)((hold4 << 24) & 0xFF000000);
      }
      
      if ((adress + i) > 0x91FF && (adress + i) < 0xFC00){
        FLASH_ADRESS_WRITE( (adress+i) , &read_flash , 1);
      }else{
        UART_SEND("\nflash adress error\n");
        give_boot_message();
      } 
      __delay_cycles(10 ); //?
      
      flash_read = flash_read_adress( adress+i);
      
      if(read_flash != flash_read){   
        
#ifdef BUG_WATCH_OPEN
              UART_SEND((char*)"flash write error*********** bug12\n");
#endif
      return 1;  
      }
  }
  return 0;
}  
 
static char fw_packet_decision_adress(char* data, unsigned int length, unsigned char et_count) {

    if (data == NULL || length < 2 || et_count > 3) { return 1; }
    unsigned int adresses[4] = { BACKUP_CODE_FLASH_AREA, BACKUP_CONST_FLASH_AREA, BACKUP_VECTORS_FLASH_AREA, BACKUP_INTERRPT_FLASH_AREA };
    unsigned int adress = adresses[et_count]; 

    switch (et_count)
    {
    case FW_WRITE_STD_CODE_AREA:
        adress += fw_update.start_index_file / 2;
        break;
    case FW_WRITE_CONST_CODE_AREA:
        if (fw_update.is_const_flash_area_size == 0) {
            fw_update.is_const_flash_area_size = (fw_update.start_index_file + FW_UPDATE_GET_PACKET_SIZE) / 2 - (length / 2);
            adress = adress;
            break;
        }
        adress += ((fw_update.start_index_file / 2) - fw_update.is_const_flash_area_size);
        break;
    case FW_WRITE_IDS_CODE_AREA:
        if (fw_update.is_id_vectors_area_size == 0) {
            fw_update.is_id_vectors_area_size = (fw_update.start_index_file + FW_UPDATE_GET_PACKET_SIZE) / 2 - (length / 2);
            adress = adress;
            break;
        }
        adress += ((fw_update.start_index_file / 2) - fw_update.is_id_vectors_area_size);
        break;
    case FW_WRITE_VECTORS_CODE_AREA:
        if (fw_update.is_interrupt_vectors_area_size == 0) {
            fw_update.is_interrupt_vectors_area_size = (fw_update.start_index_file + FW_UPDATE_GET_PACKET_SIZE) / 2 - (length / 2);
            adress = adress;
            break;
        }
        adress += ((fw_update.start_index_file / 2) - fw_update.is_interrupt_vectors_area_size);
        break;
    default:
        return 1;
        break;
    }

    if (fw_packet_backup_record((const char*)data, length, adress)) {
        //printf("h err\n"); 
        return 1;
    }

    return 0;
}

/*
data : ____0_____@@@@@@@@________1_________@@@@@@_____2____@@@@@@__3__ 
**/
static char fw_packet_record_decision(unsigned char* data) {
    unsigned int length = _array_length((unsigned char*)data), data_index = 0,  buf_index = 0, buf_length = 0, i = 0, data_index_hold = 0; char buf[FW_UPDATE_GET_PACKET_SIZE];
    if (data == NULL || length < 2 || fw_update.counter_of_et > 3) { //printf("init err\n"); 
    return 1; 
    }
    for (i = 0; i < length; i++) {
        buf[i] = data[i];
    }
    buf_length = length;
    do {
        if (strstr((char*)buf, "@@@@@@@@")) {
           
            buf_length = 1;
            if(buf[0] != '@'){ 
            
                while (data_index < length) {
                    if (data[data_index + 1] == '@') { break; }
                    data_index++;
                    buf_length++;
                }
                
                fw_packet_decision_adress((char*)(buf), buf_length, fw_update.counter_of_et);
                
                data_index += 9;
                data_index_hold = data_index;
                _array_clear((unsigned char*)buf, FW_UPDATE_GET_PACKET_SIZE); buf_index = 0;
                while (data_index < length) {
                    buf[buf_index++] = data[data_index];
                    data_index++;
                }
                buf_length = buf_index;
                data_index = data_index_hold;

                fw_update.counter_of_et++; 
            }
            else { 
                data_index += 8;
                data_index_hold = data_index;
                _array_clear((unsigned char*)buf, FW_UPDATE_GET_PACKET_SIZE); buf_index = 0;
                while (data_index < length) {
                    buf[buf_index++] = data[data_index];
                    data_index++;
                }
                buf_length = buf_index;
                data_index = data_index_hold;
                fw_update.counter_of_et++; 
            }
        }
        else if (strstr((char*)buf, "@")) {
           // printf("miss @ err\n");  
            return 1;
        }
        else {
            fw_packet_decision_adress((char*)(buf), buf_length, fw_update.counter_of_et);
            break;
        }

    } while (1);

    return 0;
}

/*
 * _array_clear
 * */
static void _array_clear(unsigned char *arr , unsigned short size)
{
  unsigned short i=0;
  while (*(arr+i)) {
     *(arr+i) = 0;
     i++;
     if(i>=size){break;}
   }
}

/*
 * _array_length
 * */
unsigned int _array_length(unsigned char *arr)
{
  unsigned short i=0;
  while (*(arr+i)) {
     i++;
   }
   return i;
}


void gsm_modul_shutdown_on(void)
{
    if (gsm_modul_get_status() == 0){
        GSM_POWERKEY_ON();
        uint32_t stopTime = super.main_timer + GSM_TIME_PWRKEY;

        while (stopTime > super.main_timer);

        GSM_POWERKEY_OFF();
    }
}

void gsm_modul_shutdown_off(void)
{
    if (gsm_modul_get_status() == 1){
        GSM_POWERKEY_ON();

        uint32_t stopTime = super.main_timer + GSM_TIME_PWRKEY;

        while (stopTime > super.main_timer);

        GSM_POWERKEY_OFF();
    }
}

void gsm_modul_reset(void)
{
  GSM_RESET_ON();
  uint32_t stopTime = super.main_timer + GSM_TIME_RESET;

  while (stopTime > super.main_timer);

  GSM_RESET_OFF();    
}