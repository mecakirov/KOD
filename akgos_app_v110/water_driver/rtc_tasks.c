/*************************************************************
 *
 * COMP: SCIENCE ENERGY
 *
 * Date          : 05.11.2023
 * By            : Selim Öztürk
 * e-mail        : selim@iddef.org
 *
 * */
#include "rtc_tasks.h"

void rtc_timer_sampler(void)
{
    g_timer++;
    t_timer++; 
    g_timerSn++;
    if(g_timerSn > 963)
    {
      g_timerSn = 0;
      telemetri.time_modem_open++;
      incrementSeconds();
    }
}
void incrementSeconds(void)
{
    telemetri.second++;
    if (telemetri.second > 59)
    {
        telemetri.second = 0;
        incrementMinutes();
    }
}

void incrementMinutes(void)
{
    telemetri.minute++;
    if (telemetri.minute > 59)
    {
        telemetri.minute = 0;
        incrementHours();
    }
}

void incrementHours(void)
{
    telemetri.hour++;
    if (telemetri.hour > 23)
    {
        telemetri.hour = 0;
        incrementDays();
    }
}

void incrementDays(void)
{
    telemetri.day++;
    if (telemetri.month == 1 || telemetri.month == 3 || telemetri.month == 5 || telemetri.month == 7
            || telemetri.month == 8 || telemetri.month == 10 || telemetri.month == 12)
    {
        if (telemetri.day > 31 )
        {
            telemetri.day = 1;
            incrementMonths();
        }
    }

    if (telemetri.month == 4 || telemetri.month == 6 || telemetri.month == 9 || telemetri.month == 11)
    {
        if (telemetri.day > 30)
        {
            telemetri.day = 1;
            incrementMonths();
        }

    }

    if (telemetri.month == 2)
    {
        if ((telemetri.year % 4) == 0)
        {
            if (telemetri.day > 29)
            {
                telemetri.day = 1;
                incrementMonths();
            }
        }
        else
        {
            if (telemetri.day > 28)
            {
                telemetri.day = 1;
                incrementMonths();
            }
        }
    }
}

void incrementMonths(void)
{
    telemetri.month++;
    if (telemetri.month > 12)
    {
        telemetri.month = 1;
        telemetri.year++;
    }
}

