/*************************************************************
 *
 * COMP: SCIENCE ENERGY
 *
 * Date          : 05.11.2023
 * By            : Selim Öztürk
 * e-mail        : selim@iddef.org
 *
 * */
#include "queue.h"


/******************************************************************* 
** check_write_ptr
**/
static void check_write_ptr(fw_l_queue_str * queue){

  if(queue->write_ptr > FW_L_QUEUE_BUFFER_LENGTH){
     queue->write_ptr = 0;
  }
}


 

/******************************************************************* 
** check_read_ptr
**/
static void check_read_ptr(fw_l_queue_str * queue){

  if(queue->read_ptr > FW_L_QUEUE_BUFFER_LENGTH){
     queue->read_ptr = 0;
  }
}
  

/******************************************************************* 
** SAYKAL_queue_create:  set all zero 
**/
void fw_l_queue_create(fw_l_queue_str * queue){
  
  fw_l_queue_clear(queue);
  
}

/******************************************************************* 
**  queue_clear:  set all zero
**/
void fw_l_queue_clear(fw_l_queue_str * queue){
  
  queue->write_ptr = 0;
  
  queue->data[queue->write_ptr] = 0;
    
  queue->write_ptr++;
  
  while(queue->write_ptr){
    
    queue->data[queue->write_ptr++] = 0;
    
    check_write_ptr(queue);
  }
  
  queue->read_ptr = 0;
  
  queue->write_ptr = 0;
  
}
  


/******************************************************************* 
** l_queue_is_empty:  true -> empty , false -> not empty
**/
char fw_l_queue_is_empty(fw_l_queue_str * queue){
  if( queue->read_ptr == queue->write_ptr ){
     return 1;
  }
  return 0;
}



/******************************************************************* 
** l_queue_data_write:  add 1 by 1
**/
void fw_l_queue_data_write(fw_l_queue_str * queue , fw_l_mychar data ){
  queue->data[ queue->write_ptr++ ] = data;
  check_write_ptr(queue);
}

 

/******************************************************************* 
** l_queue_data_write:  add if before data not same to new data
**/
char fw_l_queue_data_write_special(fw_l_queue_str * queue , fw_l_mychar data ){
  if(fw_l_queue_get_last_data(queue) != data){
     fw_l_queue_data_write(queue , data);
     return 1;
  }
  return 0;
}



/******************************************************************* 
** l_queue_data_buffer_write:  add data_length 's 
**/
void fw_l_queue_data_buffer_write(fw_l_queue_str * queue , fw_l_mychar* data , unsigned short data_length){
  unsigned short  i = 1;
  while(i){
    queue->data[ queue->write_ptr++ ] = data[i-1];
    check_write_ptr(queue);
    i++;
    if(i> data_length){
      i = 0;
    }
  }
}


 
/******************************************************************* 
** l_queue_data_read:  
**/
fw_l_mychar fw_l_queue_data_read(fw_l_queue_str * queue ){
  fw_l_mychar hold_data = 0;
  if(!fw_l_queue_is_empty(queue)){
    hold_data = queue->data[ queue->read_ptr++ ];
    check_read_ptr(queue);
  }
  return hold_data;
}

/******************************************************************* 
** l_queue_data_read:  
**/
fw_l_mychar fw_l_queue_get_last_data(fw_l_queue_str * queue ){
  if(queue->write_ptr > 0){
    return queue->data[ queue->write_ptr - 1 ];
  }
  return queue->data[ FW_L_QUEUE_BUFFER_LENGTH ];
}





