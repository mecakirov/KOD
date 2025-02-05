/*************************************************************
 *
 * COMP: SCIENCE ENERGY
 *
 * Date          : 05.11.2023
 * By            : Selim Öztürk
 * e-mail        : selim@iddef.org
 *
 * */
#ifndef __GSM_MODUL_TASKS_H
#define __GSM_MODUL_TASKS_H

#include "hardware_configs.h"
#include "rtc_tasks.h"
  
  
typedef enum{
  GSM_EMPTY_STEP,
  GSM_CONNECT_STEP,
  GSM_CLOSE_STEP,
  GSM_RESET_STEP,
  RDY_WAIT,
  AT_QUERY_STEP,
  IS_THERE_SIM_PSW_STEP,
  IS_2G_GSM_NETWORK_CONNECT,
  ROAMING_OPEN,
  ROAMING,
  SINYAL_POWER_CHECK_FOR_CREG_ERR,
  NETWORK_INFO,
  TIME_UPDATE,
  QHTTPCFG,
  QICSGP,
  QIACT,
  IS_GPRS_NETWORK_CONNECT,
  IS_GPRS_NETWORK_CONNECT_2,
  IS_GNSS_OPEN,
  IS_GNSS_GET,
  IS_GNSS_GET_OK,
  IS_GNSS_NOT_CONNECTION,
  IS_GNSS_CLOSE,
  CONNECT_HTTP_URL,
  WAIT_OK_1,
  WAIT_OK_2,
  SELECT_HTTP_URL,
  MAIN_SUPER_TASK,
  SEND_CONNECTION_URL,
  POST_STEP,
  POST_STEP_DATA_CREATE,
  POST_STEP_DATA_SEND,
  POST_SUCCESS,
  POST_READ,
  POST_STOP,
  FW_UPDATE_READY_STEP,
  FW_UPDATE_START_STEP,
  GET_FW_LENGTH,
  GET_FW_LENGTH_2,
  FW_UPDATE_FAIL,
  FW_UPDATE_SUCCESS,
  GET_FW_CALCULATOR,
  CLEAR_BACKUP_SECTORS,
  SEND_FW_CONNECTION_URL,
  CONNECT_FW_HTTP_URL,
  HTTP_FW_UPDATE_POST,
  HTTP_FW_UPDATE_READ,
  HTTP_FW_UPDATE_READ_DATA_WAIT,
  MODEM_MINIMUM_FUNC,
  HTTP_FW_CONNECTION_BREAK,
  HTTP_FILE_RECORD,
  HTTP_FILE_SIZE_SET_ARRAY,
  HTTP_FILE_SIZE_SET,
  HTTP_FILE_OPEN,
  HTTP_FW_UPDATE_READ_SET_ARRAY,
  SMS_TEXT_MOD,
  SMS_SEND_MOD,
  STEP_DATA,
  SMS_SEND_OK,
  SMS_READ_ALL_MSG,
  SMS_DELETE_ALL_MSG
}gsm_modul_steps;

/** Func Out **/
typedef enum
{
  mySUCCESS    = 0x00U,
  myTRY_AGAIN  = 0x01U,
  myBUSY       = 0x02U,
  myTIMEOUT    = 0x03U,
  myCLEAR      = 0x04U,
  myERROR      = 0x05U,
  myFAIL       = 0x06U,
  myRESET      = 0x07U,
  myNO_CARRIER = 0x08U,
  myTRUE_OUT   = 0x09U,
  myFALCE_OUT  = 0x10U
} gsm_func_out;


typedef enum{
    GSM_CLOSE_ORDER,
    GSM_WAIT_ORDER,
    GSM_CONTINUE_ORDER,
    GSM_CONNECT_ORDER,
    GSM_EMPTY_ORDER /* USERS CAN NOT USE THIS , just in init*/
}gsm_message_box_stades;

typedef enum
{
    RMC_UTC,
    RMC_LATITUDE,
    RMC_LONGITUDE,
    RMC_HDOP,
    RMC_ALTITUDE,
    RMC_FIX,
    RMC_COG,
    RMC_SPKM,
    RMC_SPKN,
    RMC_DATE,
    RMC_NSAT
} gps_field_t;

/*** GLOBALS ***/
gsm_modul_steps gsm_modul_active_object(gsm_message_box_stades *msg); // IN SUPER LOOP
void gsm_modul_timer_update(void);  // IN 1MS TIMER UPDATER
void gsm_modul_init(void);  // INIT
void gsm_modul_receive_data(unsigned char data); // RECEIVE DATA 1 BYTE
void gps_parse(unsigned char *data);
void response_parse(unsigned char *data);
void update_response_parse(unsigned char *data);
void gsm_modul_reset(void);
void gsm_modul_shutdown_on(void);
void gsm_modul_shutdown_off(void);


/**** hardware configs ****/
#define UART_SEND                        gsm_modul_uart_send
#define GET_STATUS                       gsm_modul_get_status
#define GSM_POWER_ON                     GSM_ON() // 4V POWER OFF
#define GSM_POWER_OFF                    GSM_OFF()// 4V POWER ON
#define GSM_SHUTDOWN_ON                  gsm_modul_shutdown_on()   // Modemi acar
#define GSM_SHUTDOWN_OFF                 gsm_modul_shutdown_off()  // Modemi kapar
#define GSM_RESTART                      gsm_modul_reset()


#define FLASH_ADRESS_WRITE(x,y,z)        flash_write_adress((uint32_t)x, (uint32_t*)y , (uint16_t)z)
#define GOTO_BOOT                        give_boot_message()   // burada hardware reset atilmali !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define CLEAR_BACKUP_FLASH               clear_all_backup() 
#define FW_UPDATE_MAX_FLASH_AREA         0x6400         // dogru BELIRLE
#define BACKUP_CODE_FLASH_AREA           0x9200         // dogru BELIRLE
#define BACKUP_CONST_FLASH_AREA          0xEE00         // dogru BELIRLE BU 800ü flash headerina kaydet

#define FW_WRITE_STD_CODE_AREA       0
#define FW_WRITE_CONST_CODE_AREA     1
#define FW_WRITE_IDS_CODE_AREA       2
#define FW_WRITE_VECTORS_CODE_AREA   3


/***** set params ****/
#define GOTO_MAIN_STEP(step)             super.main_steps = step; super.main_timer = 0; /** GOTO_RECEIVE_COMMAN_CHECK(INIT_CMD_SEND_STEP) ->tasklar arasi senkranizasyon bozulursa bunu aç **/
#define GOTO_COMMAND_STEP(step)          super.cmd_steps = step; super.cmd_timer = 0;
#define GOTO_RECEIVE_COMMAN_CHECK(step)  parse_step = step;  packet_buffer_index = 0;
#define POST_HTTP_DATA_TIMEOUT           60000
#define GET_GPS_DATA_TIMEOUT             2000
#define HEADER_STEP                      0
#define START_LINE_DONE                  1
#define END_LINE_IS_CAME_STEP            2
#define DATA_COMING_STEP                 3
#define SEND_BUFFER_SIZE                 150
#define FW_UPDATE_TRY_COUNT              60//10
#define FW_UPDATE_GET_PACKET_SIZE        200//(0x200)
#define COMMAND_BUFFER_SIZE              (FW_UPDATE_GET_PACKET_SIZE*2)
#define NO_READ                          0
#define READ_PACKET_COMING               1
#define READ_PACKAGE_ARRIVED             2

#define NETWORK_0               "\"NONE\""
#define NETWORK_1               "\"GSM\""
#define NETWORK_2               "\"GPRS\""
#define NETWORK_3               "\"EDGE\""
#define NETWORK_4               "\"WCDMA\""
#define NETWORK_5               "\"HSDPA\""
#define NETWORK_6               "\"HSUPA\""
#define NETWORK_7               "\"HSPA+\""
#define NETWORK_8               "\"TDD LTE\""
#define NETWORK_9               "\"FDD LTE\""

/******* testxxxxxxxxx ********/
static char command_bufferxxx[COMMAND_BUFFER_SIZE]; static unsigned short command_buffer_lengthxxx = 0 ;
 
typedef enum{
  INIT_CMD_SEND_STEP         ,
  CHECK_RESPONCE_STEP        ,
  SUCCESS_STEP               ,
  FAIL_STEP
}gsm_cmd_steps;

typedef struct{
 /** read_coming_flag: -> 0 NO READ (NO_READ), -> 1 READ PACKET COMING (READ_PACKET_COMING), -> 2 READ CAME (READ_PACKAGE_ARRIVED)**/
// char          read_coming_flag;  
 char          is_active;
 char          try_again; 
 char          same_counter; 
 char          counter_of_et;
 char          packet_come_flag;
 unsigned short file_number;
/** is_const_flash_area_size: if fw packet has "@@" than trig that and hold the start_index_file for const flash area record ***/
 unsigned int  is_const_flash_area_size;
 unsigned int  size;
 unsigned int  packet_number;
 unsigned int  start_index_file;
 unsigned int  stop_index_file;
 unsigned int  is_same_index_holder;  
 unsigned int  is_interrupt_vectors_area_size;
 unsigned int  is_id_vectors_area_size;
 unsigned int  fw_id;
 unsigned int  fw_ver;
 unsigned int  fw_tp;

}fw_update_modul_str;

typedef struct{
  gsm_modul_steps main_steps;
  unsigned int main_timer;
  unsigned int cmd_timer;
  unsigned int start_timer;
  gsm_cmd_steps cmd_steps;
  unsigned int cmd_try_count;
  unsigned int receive_timer;
  fw_l_queue_str  modul_buffer;
 // unsigned short http_receive_content_data_size;
  unsigned short telemetry_ok;
  unsigned short modem_ready;
 // unsigned int status;
  /** some valueble params**/
  unsigned short csq;
  unsigned short network;
  unsigned short gps_error;
  unsigned int  roaming;
  unsigned int  creg_count;
  unsigned int  reset_count;  
  //unsigned int  sms_control;
}gsm_modul_str;

static gsm_modul_str super;
static fw_update_modul_str fw_update;
static unsigned char send_buffer[SEND_BUFFER_SIZE];       
static unsigned char telemetri_data[SEND_BUFFER_SIZE];

static void check_mailbox(gsm_message_box_stades *msg);
static gsm_func_out command_manager(uint32_t timeout,  char* command, char* response, gsm_modul_steps true_step, gsm_modul_steps fail_step, unsigned short try_count);
static void reset_manager(unsigned int timeout_off,unsigned int timeout_on, gsm_modul_steps true_step );
//static gsm_modul_steps cuncurently_receive_datas_parse(void);
static void _array_clear(unsigned char *arr , unsigned short size);
unsigned int _array_length(unsigned char *arr);
static void fw_update_clear(void);
static char fw_packet_record_decision(unsigned char* data);
static gsm_func_out http_packet_read(unsigned char*data);
static gsm_func_out receive_data_check(unsigned char* command_buffer, unsigned short *command_buffer_length);

extern unsigned short gps_open;


/*
typedef struct
{
    char raw_data[80];
    unsigned char hour;         //!< 0-23
    unsigned char minute;       //!< 0-59
    unsigned char second;       //!< 0-59
    float latitude; //!< Latitude in decimal degrees multiplied by 1 000 000
    float longitude; //!< Longitude in decimal degrees multiplied by 1 000 000
    unsigned char day;   //!< 1-31
    unsigned char month; //!< 1-12
    unsigned char year; //!< 2000-2999
} gps_data_t;

static gps_data_t gps_data; 
*/


#endif 
