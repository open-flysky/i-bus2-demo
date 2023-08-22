#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#define IRQ_PRI_LOW                                     3
#define IRQ_PRI_MEDIUM                                  2
#define IRQ_PRI_HIGH                                    1
#define IRQ_PRI_REALTIME                                0



void SystemConfig( void );
void System_Timer_Init( void );

void SYS_ResetWatchdog(void);
void SYS_StartWatchdog(void);

uint8_t CRC8(const void *pData, uint32_t DataLength);


extern volatile uint32_t SYS_SysTickMs;


#endif
