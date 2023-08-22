#include "include.h"

uint8_t ReceiveBuffer[USART1_BUFFER_SIZE];
uint8_t Ibus2Buffer[USART1_BUFFER_SIZE];    
static uIB2_Command  SetUpdata;
uIB2_Response Responsedata;


uint32_t Ibus2_timeout = 0;
                                  
static sIB2_Channels Channeldata;
static uIB2_Command  SetUpdata;
uIB2_Response Responsedata;
uint8_t APP_ChannelsPacked[SES_MAX_CHANNELS_LENGTH];
uint8_t APP_ChannelsType[SES_CHANNELS_TYPE_LENGTH]; // 5 bits per channel from the values defined in SES_CT_*
uint8_t APP_FailsafePacked[SES_MAX_CHANNELS_LENGTH];

uIB2_RequiredResources APP_RequiredResources = {
                                                    .Types={
                                                            .ChannelsTypes = 1,
                                                            .Failsafe = 1,
                                                            .ReceiverInternalSensors = 1,
                                                          }        
                                              };

sIB2D_InitStruct IB2D_InitStruct = {
                                    .SensorType = IBDT_DIGITAL_SERVO,
                                    .ValueLength = EB2_MAX_PARAM_LENGTH1+2,
                                    .Vid = 0x01,
                                    .Pid = 0x01,
                                    .pChannelsPacked = APP_ChannelsPacked,
                                    .pChannelsType = APP_ChannelsType,
                                    .pFailsafePacked = APP_FailsafePacked
                                  };
uint8_t  ChannelUpdateFlg;
                                  /*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
__STATIC_INLINE void my_memcpy( void *dst, const void *src, size_t num )
{
    uint8_t *psrc = (uint8_t *)src;
    uint8_t *pdst = (uint8_t *)dst;
    while( num-- > 0 )
        *pdst++ = *psrc++;

}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void my_memset( void *dst, uint8_t setdata, size_t num )
{
    uint8_t *pdst = (uint8_t *)dst;
    while( num-- > 0 )
        *pdst++ = setdata;

}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
int16_t FailsafeChanneldata[SES_MAX_CHANNELS_LENGTH];
void ProcessChannels( void )
{
    uint8_t Temp = CRC8((uint8_t*)&Channeldata, Channeldata.Length - 1 );
    if( Temp != Channeldata.Channels[Channeldata.Length-4] ) 
    {
        return;
    }
    switch (Channeldata.PacketSubtype)
    {
        case IB2_PST_CHANNELS:
             {
                 my_memcpy( IB2D_InitStruct.pChannelsPacked, Channeldata.Channels, SES_MAX_CHANNELS_LENGTH );
                 ChannelUpdateFlg = 1;
             }
            break;
        case IB2_PST_CHANNELS_TYPE:
             {
                 my_memcpy( IB2D_InitStruct.pChannelsType, Channeldata.Channels, SES_CHANNELS_TYPE_LENGTH );
                 APP_RequiredResources.Types.ChannelsTypes = 0;
             }
            break;
        case IB2_PST_FAILSAFE:
            {
                my_memcpy( IB2D_InitStruct.pFailsafePacked, Channeldata.Channels, SES_MAX_CHANNELS_LENGTH );
                APP_RequiredResources.Types.Failsafe = 0;
                SES_UnpackChannels( IB2D_InitStruct.pFailsafePacked, FailsafeChanneldata,  SES_MAX_CHANNELS_LENGTH, IB2D_InitStruct.pChannelsType );
            }
            break;
        default:
            break;
    }
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void Dev_Ibus_Init( void )
{
    uart_init();
    timer_init();
    set_to_receive_mode( ReceiveBuffer, sizeof( ReceiveBuffer ));
    receive_start();
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
uint16_t USART1_Receive( void *pBuffer, uint16_t BufferSize )
{
    static uint16_t FirstAddr = 0;
    static uint16_t NextAddr = 0;  
    
    NextAddr = USART1_BUFFER_SIZE - IBUS2_RX_DMA->CNDTR;

    if(FirstAddr == NextAddr)
    {
        return 0;
    }
    else if( FirstAddr < NextAddr )
    {
        uint16_t RXSize;
        RXSize = NextAddr-FirstAddr;
        if( RXSize > BufferSize )
        {
            RXSize = BufferSize;
        }
        my_memcpy( pBuffer, ReceiveBuffer + FirstAddr, RXSize );
        FirstAddr = NextAddr;
        return RXSize;
    }
    else
    {
        uint16_t RXSize;
        uint16_t RXSize1;
        uint16_t RXSize2;
        RXSize = USART1_BUFFER_SIZE - ( FirstAddr - NextAddr );	
        RXSize1 = USART1_BUFFER_SIZE - FirstAddr;
        my_memcpy( pBuffer, ReceiveBuffer+FirstAddr, RXSize1 );
        RXSize2 = RXSize - RXSize1;
        pBuffer = (uint8_t *)pBuffer + RXSize1;
        if( 0 == RXSize2 )
        {
            FirstAddr += RXSize1;
            if( FirstAddr >= USART1_BUFFER_SIZE )
            {
                FirstAddr = 0;
            }	
        }
        else
        {
            my_memcpy( pBuffer, ReceiveBuffer, RXSize2 );
            FirstAddr = RXSize2;	
        }
        return RXSize;
    }
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void Ibus2_InterruptHandler( void )
{
    uint8_t Length;
    uint8_t crc;
    Length = USART1_Receive( Ibus2Buffer, USART1_BUFFER_SIZE );
    if(Length<5) return;
    crc =  CRC8( Ibus2Buffer, Length-1 );
    if(crc == Ibus2Buffer[Length-1] )
    {
        if( 0 == ( Ibus2Buffer[0] & 0x03 ) )
        {
            my_memcpy( (void*)&Channeldata, Ibus2Buffer, Length );
            ProcessChannels();
            return;
        }
        else if( 1 == ( Ibus2Buffer[0] & 0x03 ) )
        {
            Ibus2_timeout++;
            my_memcpy( (void*)&SetUpdata, Ibus2Buffer, 7);//Length);
            ProcessCommand();
        }
    }
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
uint8_t SensorTestData[EB2_MAX_PARAM_LENGTH1] = {0x00, 0x12, 0x23,0x34, 0x45, 0x56,0x67,0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde};
void Ibus2_GetSensorValueCallback(void *pValue)
{
    my_memcpy( pValue, SensorTestData, EB2_MAX_PARAM_LENGTH1 ); 
    return;
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
uint8_t SetupdataRx[16];
uint8_t SetupdataRx1[16];
uint8_t Ibus2_SetParamCallback( uint16_t ParamType, uint8_t ParamLength, const uint8_t *pParamValue)
{
    switch (ParamType)
    {
        case 0x1234:
        {
            my_memcpy(SetupdataRx, pParamValue, ParamLength);
            return 1;
        }
    case 0x1235:
        {
            my_memcpy(SetupdataRx, pParamValue, ParamLength);
            return 1;
        }
        case 0xC000:
        {
            my_memcpy(SetupdataRx1, pParamValue, ParamLength);//如果要了解 参考附录一
            APP_RequiredResources.Types.ReceiverInternalSensors = 0;
            return 1;
        }
        default:
            return 0;
    }
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
uint8_t SetupdataTx[EB2_MAX_PARAM_LENGTH] = {0x01,0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde};
uint8_t Ibus2_GetParamCallback( uint16_t ParamType, uint8_t *pParamValue)
{
    switch (ParamType)
    {
        case 0x1234:
        {
            my_memcpy( pParamValue, SetupdataTx, EB2_MAX_PARAM_LENGTH );
            return EB2_MAX_PARAM_LENGTH;
        }
            
        default:
            return 0;
    }
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
uint8_t SendFlg = 0;
void ProcessCommand( void )
{
    my_memset( Responsedata.Data, 0, sizeof( Responsedata ) );
    SendFlg = 0;
    switch( SetUpdata.Generic.CommandCode )
    {
        case IB2_CMD_RESET:
             {
             }break;
        case IB2_CMD_GET_TYPE:
             {
                 Responsedata.GetType.Type = IB2D_InitStruct.SensorType ; 
                 Responsedata.GetType.ValueLength = IB2D_InitStruct.ValueLength;
                 Responsedata.GetType.RequiredResources.All = APP_RequiredResources.All;
                 SendFlg = 1;
             }
             break;
        case IB2_CMD_GET_VALUE:
             {
                 Ibus2_GetSensorValueCallback( Responsedata.GetValue.Value );
                 Responsedata.GetValue.Vid = IB2D_InitStruct.Vid;
                 Responsedata.GetValue.Pid = IB2D_InitStruct.Pid;
                 SendFlg = 1;
             }break;
        case IB2_CMD_GET_PARAM:
             {
                 Responsedata.GetParam.ParamType = SetUpdata.GetParam.ParamType;
                 Responsedata.GetParam.ParamLength = Ibus2_GetParamCallback( Responsedata.GetParam.ParamType, Responsedata.GetParam.ParamValue );
                 SendFlg = 1;
             }break;
        case IB2_CMD_SET_PARAM:
          {
                 Responsedata.SetParam.ParamLength = Ibus2_SetParamCallback( SetUpdata.SetParam.ParamType, SetUpdata.SetParam.ParamLength, SetUpdata.SetParam.ParamValue);
                 SendFlg = 1;
             }break;
        default: break;
    }
    if( SendFlg )
    {
        SendFlg = 2;
        Responsedata.Generic.PacketType = 2;
        Responsedata.Generic.CommandCode = SetUpdata.Generic.CommandCode;
        Responsedata.GetType.CRC8 = CRC8((uint8_t*)&Responsedata.Data, sizeof( Responsedata ) - 1 );
    }
}  
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void TIM16_IRQHandler( void )
{
    if( TIM16->SR & 0x01 )
    {
        TIM16->CR1 &= ~0x01;
        TIM16->SR = 0;
        if( 2 == SendFlg )
        {
            SendFlg = 0;
            set_to_send_mode( &Responsedata.Data, sizeof( Responsedata ));
        }
    }
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
static const unsigned long SES_UnpackChannelsFactors[]={
    0, 0,0x40000000,          0,          0,          0, 0x028F5C29, 0x0147AE15, 0x0083126F, 0x00418938, 0x0020C49C, 0x0010624E, 0x00083127, 0x00041894, 0, 0,
    0, 0,         0, 0x20000000, 0x10000000, 0x06666667, 0x03333334,          0, 0x0147AE15, 0x00A3D70B, 0x00418938, 0x0020C49C, 0x0010624E, 0x00083127, 0, 0
};

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void SES_UnpackChannels(const void *pPackedChannels,signed short *pChannels,unsigned long NbChannels,const unsigned char *pChannelsType)
{
    unsigned char *pChannelIn;
    unsigned long ChannelInBitNb;
    unsigned long ChannelNb;
    unsigned long ChannelsTypeBitNb;

    pChannelIn=(unsigned char *)pPackedChannels;
    ChannelInBitNb=0;
    ChannelsTypeBitNb=0;
    for (ChannelNb=0;ChannelNb<NbChannels;ChannelNb++)
    {
        signed long ChannelInValue;
        signed long ChannelOutValue;
        unsigned long ChannelType;
        unsigned long NbBits;
        ChannelType=((*((__packed unsigned short *)pChannelsType))>>ChannelsTypeBitNb)&((1<<SES_NB_BITS_PER_CHANNEL_TYPE)-1);
        ChannelsTypeBitNb+=SES_NB_BITS_PER_CHANNEL_TYPE;
        if (ChannelsTypeBitNb>=8)
        {
            pChannelsType++;
            ChannelsTypeBitNb-=8;
        }
        NbBits=ChannelType&SES_CT_NB_BITS_MASK;
        // Calculate ChannelOutValue
        if (NbBits>=2)
        {
            ChannelInValue=((*(__packed unsigned long *)pChannelIn)>>ChannelInBitNb) & ((1<<NbBits)-1);
            // Keep failsafe value
            if (ChannelInValue==1<<(NbBits-1))
                ChannelOutValue=SES_KEEP_FAILSAFE_CHANNEL;
            // Stop failsafe value
            else if (ChannelInValue==(1<<(NbBits-1))+1 && NbBits>=6)
                ChannelOutValue=SES_STOP_FAILSAFE_CHANNEL;
            // Normal channel value
            else
            {
                uint8_t Negative;

                Negative=0;
                if (ChannelInValue & (1<<(NbBits-1)))
                {
                    Negative=1;
                    ChannelInValue=(-ChannelInValue) & ((1<<(NbBits-1))-1);
                }
                ChannelOutValue=(ChannelInValue*SES_UnpackChannelsFactors[ChannelType]+(1<<15))>>16;
                if (ChannelOutValue>SES_CHANNEL_RANGE_150)
                    ChannelOutValue=SES_CHANNEL_RANGE_150;
                if (Negative)
                    ChannelOutValue=-ChannelOutValue;
            }
        }
        else
            ChannelOutValue=0;
        pChannels[ChannelNb]=ChannelOutValue;
        ChannelInBitNb+=NbBits;
        if (ChannelInBitNb>=8)
        {
            pChannelIn++;
            ChannelInBitNb-=8;
        }
        if (ChannelInBitNb>=8)
        {
            pChannelIn++;
            ChannelInBitNb-=8;
        }
    }
    return;
}
