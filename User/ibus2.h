#ifndef  __IBUS2_H__
#define  __IBUS2_H__

#ifdef      EXTERN
    #undef  EXTERN
#endif

#ifdef  __DEV_IBUS2_C__
    #define EXTERN
#else
    #define EXTERN  extern
#endif     
    
#define USART1_BUFFER_SIZE 			    64
    
// Packet types
#define IB2_PT_CHANNELS         0
#define IB2_PT_COMMAND          1
#define IB2_PT_RESPONSE         2

// Packet subtypes for channels packet
#define IB2_PST_CHANNELS        0
#define IB2_PST_CHANNELS_TYPE   1
#define IB2_PST_FAILSAFE        2

#define IB2_CMD_RESET           0
#define IB2_CMD_GET_TYPE        1
#define IB2_CMD_GET_VALUE       2
#define IB2_CMD_GET_PARAM       3
#define IB2_CMD_SET_PARAM       4

#define SES_NB_MAX_CHANNELS                     32 
#define SES_NB_BITS_PER_CHANNEL_TYPE            5
#define SES_MAX_CHANNELS_LENGTH                 32
#define EB2_MAX_PARAM_LENGTH                    16 
#define EB2_MAX_PARAM_LENGTH1                   14 
#define SES_CHANNELS_TYPE_LENGTH                ((SES_NB_MAX_CHANNELS*SES_NB_BITS_PER_CHANNEL_TYPE+7)/8)
#define IB2PC_RECEIVER_INTERNAL_SENSORS         0xC000
 


#define SES_NB_MAX_CHANNELS                     32
#define SES_NB_BITS_PER_CHANNEL_TYPE            5
#define SES_CHANNEL_RANGE_100                   16384 // Channel range for 100% endpoint
#define SES_CHANNEL_RANGE_150                   ((SES_CHANNEL_RANGE_100*150)/100) // Channel range for 150% endpoint
#define SES_CT_NB_BITS_MASK                     0x0F 
#define SES_KEEP_FAILSAFE_CHANNEL               (-32768) // Keep outputing the last channel value 
#define SES_STOP_FAILSAFE_CHANNEL               (-32767) // For 6-bit channels and more only, outputs low or high level depending on the interface 


#define APP_CHANNEL_US_VALUE_MIN  750
#define APP_CHANNEL_US_VALUE_MAX 2250

typedef __packed struct
{
	unsigned char ChannelsTypes:1;
	unsigned char Failsafe:1;
	unsigned char ReceiverInternalSensors:1;
	unsigned char Reserved:5;
} sIB2_RequiredResourcesTypes;

typedef __packed union
{
	unsigned char All;
	sIB2_RequiredResourcesTypes Types;
} uIB2_RequiredResources;

typedef __packed struct
{
	unsigned char PacketType:2;
	unsigned char PacketSubtype:4;
	unsigned char SynchronizationLost:1;
	unsigned char FailsafeTriggered:1;
	unsigned char Length;
	unsigned char AddressLevel1:3;
	unsigned char AddressLevel2:3;
	unsigned char Reserved2:2;
	unsigned char Channels[SES_MAX_CHANNELS_LENGTH];
	unsigned char CRC8;
} sIB2_Channels;    
    
    
typedef __packed struct
{
	unsigned short InternalVoltage; // In 10mV units
	unsigned char SignalStrength; // From 0 to 100
	unsigned short RSSI; // In -0.25dBm units
	unsigned short BackgroundNoise; // In -0.25dBm units
	signed short SNR; // From 0 to 40dBm in 0.25dBm units
} sIB2PA_ReceiverInternalSensors;
    
 typedef __packed struct
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned char Reserved[3+EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_CommandGeneric;

typedef __packed struct
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned short ParamType;
	unsigned char Reserved[1+EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_CommandGetParam;

typedef __packed struct
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned char Reserved[3+EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_CommandGetType;

typedef __packed struct
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned char Reserved[3+EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_CommandGetValue;

typedef __packed struct
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned short ParamType;
	unsigned char ParamLength;
	unsigned char ParamValue[EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_CommandSetParam;

typedef __packed struct
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned short ParamType;
	unsigned char ParamLength;
	sIB2PA_ReceiverInternalSensors InternalSensors; 
	//unsigned char Reserved[EB2_MAX_PARAM_LENGTH-sizeof(sIB2PA_ReceiverInternalSensors)];
	unsigned char CRC8;
} sIB2_CommandSetParamReceiverInternalSensors;

typedef __packed union
{
	sIB2_CommandGeneric Generic;
	sIB2_CommandGetType GetType;
	sIB2_CommandGetValue GetValue;
	sIB2_CommandGetParam GetParam;
	sIB2_CommandSetParam SetParam;
	sIB2_CommandSetParamReceiverInternalSensors SetParamReceiverInternalSensors;
} uIB2_Command;

typedef __packed struct
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned char Reserved[3+EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_ResponseGeneric;

typedef __packed struct
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned short ParamType;
	unsigned char ParamLength; // 0 if parameter not supported
	unsigned char ParamValue[EB2_MAX_PARAM_LENGTH1];
//    unsigned char Vid;
//    unsigned char Pid;
	unsigned char CRC8;
} sIB2_ResponseGetParam;

typedef __packed struct
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	eIB_DeviceType Type;
	unsigned char ValueLength; // From 1 to 16
	uIB2_RequiredResources RequiredResources;
	unsigned char Reserved[EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_ResponseGetType;

typedef __packed struct
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned char Value[EB2_MAX_PARAM_LENGTH1];
    unsigned char Vid;
    unsigned char Pid;
	unsigned char Reserved[3];
	unsigned char CRC8;
} sIB2_ResponseGetValue;

typedef __packed struct
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned short ParamType;
	unsigned char ParamLength; // 0 if parameter not supported
	unsigned char Reserved[EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_ResponseSetParam;

typedef __packed union
{
	sIB2_ResponseGeneric Generic;
	sIB2_ResponseGetType GetType;
	sIB2_ResponseGetValue GetValue;
	sIB2_ResponseGetParam GetParam;
	sIB2_ResponseSetParam SetParam;
    uint8_t Data[7];
} uIB2_Response;

typedef struct
{
	eIB_DeviceType SensorType;
	unsigned char ValueLength;
    unsigned char Vid;
    unsigned char Pid;
	unsigned char *pChannelsPacked;
	unsigned char *pChannelsType; // 5 bits per channel from the values defined in SES_CT_*
	unsigned char *pFailsafePacked;
    unsigned char ChannelNb;
} sIB2D_InitStruct;


extern uint32_t Ibus2_timeout;


void Dev_Ibus_Init( void );   
void Ibus2_InterruptHandler( void );    
void ProcessCommand( void );    
void ProcessChannels( void );    
void SES_UnpackChannels(const void *pPackedChannels,signed short *pChannels,unsigned long NbChannels,const unsigned char *pChannelsType);    
    
    
    
    
    
#endif   
