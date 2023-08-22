#include "include.h"
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void timer_init( void )
{
    
    RCC->APBRSTR2 |= RCC_APBRSTR2_TIM16RST;
    RCC->APBRSTR2 &= ~RCC_APBRSTR2_TIM16RST;
    RCC->APBENR2  |= RCC_APBENR2_TIM16EN;
    
    TIM16->CR1 = 0;
    TIM16->CR2 = 0;
    TIM16->CNT = 0;
    TIM16->PSC = 63;
    TIM16->ARR = 100;
    TIM16->DIER = 0x01;
    NVIC_SetPriority(TIM16_IRQn,IRQ_PRI_HIGH);
    NVIC_EnableIRQ( TIM16_IRQn );
    //TIM16->CR1 |= 0x01;
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void start_timer( uint16_t us )
{
    TIM16->CR1 &= ~0x01;
    TIM16->CNT = 0;
    TIM16->ARR = us;
    TIM16->CR1 |= 0x01;
}


