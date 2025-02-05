/*************************************************************
 *
 * COMP: SCIENCE ENERGY
 *
 * Date          : 05.11.2023
 * By            : Selim Öztürk
 * e-mail        : selim@iddef.org
 *
 * */
#ifndef __queue_H
#define __queue_H

#ifdef __cplusplus
extern "C" {
#endif

  
typedef unsigned char  fw_l_mychar;
    
#define FW_L_QUEUE_BUFFER_LENGTH  0xFF//*3
  
typedef struct{
  
    fw_l_mychar data[FW_L_QUEUE_BUFFER_LENGTH+1]; 
    
    unsigned short read_ptr;
    
    unsigned short write_ptr;
    
}fw_l_queue_str;

    
/***************** Global User Functions **********************************************/
    
void fw_l_queue_create(fw_l_queue_str * queue);
void fw_l_queue_clear(fw_l_queue_str * queue);
char fw_l_queue_is_empty(fw_l_queue_str * queue);
void fw_l_queue_data_buffer_write(fw_l_queue_str * queue , fw_l_mychar* data , unsigned short data_length);
void fw_l_queue_data_write(fw_l_queue_str * queue , fw_l_mychar data );  
fw_l_mychar fw_l_queue_data_read(fw_l_queue_str * queue );
fw_l_mychar fw_l_queue_get_last_data(fw_l_queue_str * queue );
char fw_l_queue_data_write_special(fw_l_queue_str * queue , fw_l_mychar data );


#ifdef __cplusplus
}
#endif

#endif 
