/*************************************************************
 *
 * COMP: SCIENCE ENERGY
 *
 * Date          : 05.11.2023
 * By            : Selim Öztürk
 * e-mail        : selim@iddef.org
 *
 * */
#ifndef __RTC_TASKS_H
#define __RTC_TASKS_H

#include "hardware_configs.h"

extern unsigned long t_timer;
extern unsigned long g_timer;
extern unsigned long g_timerSn;

void rtc_timer_sampler(void);
void incrementSeconds(void);
void incrementMinutes(void);
void incrementHours(void);
void incrementDays(void);
void incrementMonths(void);

#endif 
