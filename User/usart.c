#include "include.h"

/*==================================================================================================
name:
funciton:
input;
output:
==================================================================================================*/
extern void Ibus2_InterruptHandler( void );
void USART1_IRQHandler( void )
{
    if( USART1->ISR&USART_ISR_RTOF )// Receive timeout
    {
        // Clear the receiver timeout flag
        USART1->ICR = USART_ICR_RTOCF;
        start_timer( 160 );//135~185us
        Ibus2_InterruptHandler();
    }
    else if( USART1->ISR & USART_ISR_TC )  // Transmission complete
    {
        // Clear the transmission complete flag and set to receive mode
        USART1->ICR = USART_ICR_TCCF;
        receive_start();
    }
}
/*==================================================================================================
name:
funciton:
input;
output:
==================================================================================================*/
void uart_init( void )
{
    // USART1 on PB6 and PB7
    // Disable the USART1 IRQ
    NVIC_DisableIRQ( USART1_IRQn ); 
    // USART1SW = 1 System clock (SYSCLK) selected as USART1 clock
    //RCC->CFGR3 = (RCC->CFGR3&~RCC_CFGR3_USART1SW) | (RCC_CFGR3_USART1SW_0*1);
    // Remap the USART1 DMA to DMA 4 and 5
    //SYSCFG->CFGR1 |=  (SYSCFG_CFGR1_USART1TX_DMA_RMP | SYSCFG_CFGR1_USART1RX_DMA_RMP); 
    RCC->IOPENR|=RCC_IOPENR_GPIOBEN;    
    // Enable the USART module clock
    RCC->APBENR2 |= RCC_APBENR2_USART1EN;
    // Reset USART1
    RCC->APBRSTR2 |= RCC_APBRSTR2_USART1RST;
    RCC->APBRSTR2 &= ~RCC_APBRSTR2_USART1RST;
    // Clear the receiver timeout and transmission complete flag 
    USART1->ICR = USART_ICR_RTOCF | USART_ICR_TCCF;
    // UE = 0 USART disabled
    // RE = 0 Receiver disabled
    // TE = 0 Transmitter disabled
    // IDLEIE = 0 RX not empty interrupt disabled
    // RXNEIE = 0 RXNE interrupt disabled
    // TCIE = 0 Transmission complete interrupt disabled
    // TXEIE = 0 TX empty interrupt disabled
    // PEIE = 0 Parity error interrupt disabled
    // PS = 0 Not used
    // PCE = 0 Parity control disabled
    // WAKE = 0 Not used
    // M0 = 0 8 bit word length
    // MME = 0 Mute mode disabled
    // CMIE = 0  Character match interrupt disabled
    // OVER8 = 0 Oversampling by 16
    // DEDT = 0 Driver Enable deassertion time is null
    // DEAT = 0 Driver Enable assertion time is null
    // RTOIE = 0 Receiver timeout interrupt disabled
    // M1 = 0 8 bit word length
    USART1->CR1 = 0;
    // ADDM7 = 0 Not used
    // LBDL = 0 LIN break detection not used
    // LBDIE = 0 LIN break detection interrupt disabled
    // LBCL = 0 Not used
    // CPHA = 0 Not used
    // CPOL = 0 Not used
    // CLKEN = 0 SCLK pin disabled
    // STOP = 0 1 stop bit
    // LINEN = 0 LIN mode disabled
    // SWAP = 0 RX and TX lines not swapped
    // RXINV = 0 RX signal not inverted
    // TXINV = 0 TX signal not inverted
    // DATAINV = 0 Data not inverted
    // MSBFIRST = 0 LSB first
    // ABREN = 0 Auto baud rate disabled
    // ABRMOD = 0 Not used
    // RTOEN = 1 Receiver timeout feature enabled
    // ADD = 0 Not used
    USART1->CR2 = USART_CR2_RTOEN;
    // Receive timeout after 10 data bits
    USART1->RTOR = 10;
    // EIE = 0 Error interrupt disabled
    // HDSEL = 0 Half duplex mode is not selected
    // DMAR = 0 DMA mode is disabled for reception
    // DMAT = 0 DMA mode is disabled for transmission
    // RTSE = 0 RTS hardware flow control disabled
    // CTSE = 0 CTS hardware flow control disabled
    // CTSIE = 0 CTS interrupt disabled
    // ONEBIT = 1 One sample bit method
    // OVRDIS = 1 Overrun flag disabled
    // DDRE = 0 DMA is not disabled in case of reception error
    // DEM = 0 DE function is disabled
    // DEP = 0 Not used
    USART1->CR3 = USART_CR3_ONEBIT | USART_CR3_OVRDIS;

    // Set PB6 and PB7(USART1_RX) to their respective USART1_TX and USART1_RX functions
    GPIOB->AFR[0] &= 0x00FFFFFF;
    // Enable PB7(USART1_RX) pull-up
    GPIOB->PUPDR = (GPIOB->PUPDR&~(3<<(7*2))) | (1<<(7*2));
    // Set PB6 (USART1_TX) as alternate function and PB7 (USART1_RX) as alternate function
    GPIOB->MODER = (GPIOB->MODER&~((3<<(6*2)) | (3<<(7*2)))) | (2<<(6*2)) | (2<<(7*2));    
    // Set the baud rate
    USART1->BRR = ( SystemCoreClock + IB2_USART_SPEED / 2 ) / IB2_USART_SPEED;
    //	// Enable the USART
    //	USART1->CR1 = USART_CR1_UE;
    // Clear the transmission complete and the receiver timeout flags
    USART1->ICR = USART_ICR_TCCF | USART_ICR_RTOCF;   
    // Clear the USART IRQ pending flag
    NVIC_ClearPendingIRQ( USART1_IRQn );      
    //Register interrupt function
    // SetInterruptHandler( INT_USART1, Usart1_InterruptHandler );     
    // Set the USART1 IRQ to realtime priority
    NVIC_SetPriority( USART1_IRQn, IRQ_PRI_HIGH );
    // Enable the USART1 IRQ
    NVIC_EnableIRQ( USART1_IRQn );
}
/*==================================================================================================
name:
funciton:
input;
output:
==================================================================================================*/
void set_to_receive_mode(const void *pdata, uint16_t len )
{
     // Set PB6 (USART1_TX) as input
    GPIOB->MODER = (GPIOB->MODER&~(3<<(6*2))) | (0<<(6*2));
    // TE=0 Transmitter disabled
    // TCIE=0 Transmission complete interrupt disable
    USART1->CR1 &= ~( USART_CR1_TE | USART_CR1_TCIE);

    IBUS2_RX_DMAMUX->CCR = 50;
    //DMA Channel disabled
    IBUS2_RX_DMA->CCR = 0;
    // Set up the RX DMA
    IBUS2_RX_DMA->CPAR = (uint32_t)&USART1->RDR;
    IBUS2_RX_DMA->CMAR = (uint32_t)pdata;
    IBUS2_RX_DMA->CNDTR = len;
    // EN=1 DMA channel enabled
    // TCIE=0 Transfer complete interrupt disabled
    // HTIE=0 Half transfer interrupt disabled
    // TEIE=0 Transfer error interrupt disabled
    // DIR=0 Read from peripheral
    // CIRC=1 Circular mode enabled
    // PINC=0 Peripheral increment mode disabled
    // MINC=1 Memory increment mode enabled
    // PSIZE=0 8-bit peripheral size
    // MSIZE=0 8-bit memory size
    // PL=0 Low priority
    // MEM2MEM=0 Memory to memory mode disabled
    IBUS2_RX_DMA->CCR = DMA_CCR_CIRC | DMA_CCR_EN | DMA_CCR_MINC;
    // Enable RX DMA
    USART1->CR3 |= USART_CR3_DMAR;
    // Enable the receiver timeout interrupt 
    USART1->CR1 = USART_CR1_UE | USART_CR1_RTOIE;

}
/*==================================================================================================
name:
funciton:
input;
output:
==================================================================================================*/
void receive_start(void)
{
     // Set PB6 (USART1_TX) as input
    GPIOB->MODER = (GPIOB->MODER&~(3<<(6*2))) | (0<<(6*2));
    // TE=0 Transmitter disabled
    // TCIE=0 Transmission complete interrupt disable
    USART1->CR1 &= ~( USART_CR1_TE | USART_CR1_TCIE);
    //TX DMA Channel disabled
    IBUS2_TX_DMA->CCR = 0;

    // Clear the receiver timeout flag
    USART1->ICR = USART_ICR_RTOCF;
    // UE=1 USART enable
    // RE=1 Receiver enabled
    // RTOIE=1 Receiver timeout interrupt enabled
    USART1->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_RTOIE;
}

/*==================================================================================================
name:
funciton:
input;
output:
==================================================================================================*/
void set_to_send_mode(const void *pdata, uint16_t len)
{
    {   //turn off receive
        // RE=0 Receiver disabled
        // RTOIE=0 Receiver timeout interrupt disabled
        USART1->CR1 &= ~( USART_CR1_RE | USART_CR1_RTOIE );
    }
    // Set PB6 (USART1_TX) as alternate function
    GPIOB->MODER = ( GPIOB->MODER & ~(3<<(6*2))) | (2<<(6*2));

    IBUS2_TX_DMAMUX->CCR = 51;
    //DMA Channel disabled
    IBUS2_TX_DMA->CCR = 0;
    // Set up the TX DMA
    IBUS2_TX_DMA->CPAR = (uint32_t)&USART1->TDR;
    IBUS2_TX_DMA->CMAR = (uint32_t)pdata;
    IBUS2_TX_DMA->CNDTR = len;
    // EN=1 DMA channel enabled
    // TCIE=0 Transfer complete interrupt disabled
    // HTIE=0 Half transfer interrupt disabled
    // TEIE=0 Transfer error interrupt disabled
    // DIR=1 Read from memory
    // CIRC=0 Circular mode disabled
    // PINC=0 Peripheral increment mode disabled
    // MINC=1 Memory increment mode enabled
    // PSIZE=0 8-bit peripheral size
    // MSIZE=0 8-bit memory size
    // PL=0 Low priority
    // MEM2MEM=0 Memory to memory mode disabled
    IBUS2_TX_DMA->CCR = DMA_CCR_EN | DMA_CCR_DIR | DMA_CCR_MINC;
    // Clear the transmission complete flag
    USART1->ICR = USART_ICR_TCCF;
    // Enable TX DMA
    USART1->CR3 |= USART_CR3_DMAT;
    // Enable the transmit complete interrupt and start transmit
    USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_TCIE;
}

