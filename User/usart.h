#ifndef  __DRV_IBUS2_H__
#define  __DRV_IBUS2_H__
    
#ifndef IB2_USART_SPEED    
#define IB2_USART_SPEED     1500000     
#endif    
    
#define IBUS2_TX_PORT                          		( GPIOB )
#define IBUS2_TX_PIN	                            ( 6 )
#define IBUS2_RX_PORT                          		( GPIOB )
#define IBUS2_RX_PIN	                            ( 7 )

#define IBUS2_RX_DMAMUX                                  DMAMUX1_Channel1
#define IBUS2_RX_DMA									 DMA1_Channel2     
#define IBUS2_TX_DMAMUX                                  DMAMUX1_Channel2
#define IBUS2_TX_DMA									 DMA1_Channel3         
    
void uart_init( void ); 
void set_to_receive_mode(const void *pdata, uint16_t len );    
void receive_start( void );    
void set_to_send_mode( const void *pdata, uint16_t len);      

#endif  
