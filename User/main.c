#include "include.h"

/*==================================================================================================
name:
funciton:4
input;
output:
==================================================================================================*/
int main(void)
{
    uint32_t temp = 0;
    uint32_t timestamp = 0;
    uint8_t ledstate = 0;
    
    SystemClock_Config();
    Led_Init();
    //ADC_Init();
    GPIO_Init();
    Dev_Ibus_Init();
    SYS_StartWatchdog();
    while (1)
    {
        SYS_ResetWatchdog();

        
        timestamp = SYS_SysTickMs;
        if( (0 == (timestamp%1000)) && ( temp != timestamp ) )
        {
            temp = timestamp;
            if( Ibus2_timeout )
            {
                Ibus2_timeout = 0;
                ledstate = NORMAL;
            }
            else
            {
                ledstate = BUS_ERROR;
            }
            
            if( Temperature == 190 || Temperature == 2410 )
            {
                ledstate = NTC_ERROR;
            }
            LED_SetState( ledstate );
        }
    }
}

