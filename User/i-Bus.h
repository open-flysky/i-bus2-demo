#ifndef  __I_BUS_H__
#define  __I_BUS_H__

#define IB_MAX_VALUE_BLOCKS  4 // Maximum number of data blocks

#define IB_NB_DEVICE_TYPES 256
typedef enum
{
	IBDT_INT_VOLTAGE          =0x00,
	IBDT_TEMPERATURE          =0x01,
	IBDT_ROTATION_SPEED       =0x02,
	IBDT_EXT_VOLTAGE          =0x03,
	IBDT_GPS                  =0x40,
	IBDT_PRESSURE             =0x41,
	IBDT_COMPASS              =0x42,
	IBDT_TX_VOLTAGE           =0x7F,
	IBDT_HUB_1_PORTS_PWM      =0xE0, // 1-port hub in PWM mode
	IBDT_HUB_2_PORTS_PWM      =0xE1, // 2-port hub in PWM mode
	IBDT_HUB_3_PORTS_PWM      =0xE2, // 3-port hub in PWM mode
	IBDT_HUB_4_PORTS_PWM      =0xE3, // 4-port hub in PWM mode
	IBDT_HUB_5_PORTS_PWM      =0xE4, // 5-port hub in PWM mode
	IBDT_HUB_6_PORTS_PWM      =0xE5, // 6-port hub in PWM mode
	IBDT_HUB_7_PORTS_PWM      =0xE6, // 7-port hub in PWM mode
	IBDT_HUB_8_PORTS_PWM      =0xE7, // 8-port PWM adapter
	IBDT_HUB_9_PORTS_PWM      =0xE8, // 9-port PWM adapter
	IBDT_10_PORTS_PWM_ADAPTER =0xE9, // 10-port PWM adapter
	IBDT_11_PORTS_PWM_ADAPTER =0xEA, // 11-port PWM adapter
	IBDT_12_PORTS_PWM_ADAPTER =0xEB, // 12-port PWM adapter
	IBDT_13_PORTS_PWM_ADAPTER =0xEC, // 13-port PWM adapter
	IBDT_14_PORTS_PWM_ADAPTER =0xED, // 14-port PWM adapter
	IBDT_15_PORTS_PWM_ADAPTER =0xEE, // 15-port PWM adapter
	IBDT_16_PORTS_PWM_ADAPTER =0xEF, // 16-port PWM adapter
	IBDT_17_PORTS_PWM_ADAPTER =0xF0, // 17-port PWM adapter
	IBDT_HUB_1_PORTS_HUB      =0xF1, // 1-port hub in hub mode
	IBDT_HUB_2_PORTS_HUB      =0xF2, // 2-port hub in hub mode
	IBDT_HUB_3_PORTS_HUB      =0xF3, // 3-port hub in hub mode
	IBDT_HUB_4_PORTS_HUB      =0xF4, // 4-port hub in hub mode
	IBDT_HUB_5_PORTS_HUB      =0xF5, // 5-port hub in hub mode
	IBDT_HUB_6_PORTS_HUB      =0xF6, // 6-port hub in hub mode
	IBDT_HUB_7_PORTS_HUB      =0xF7, // 7-port hub in hub mode
	IBDT_DIGITAL_SERVO        =0xF8,
	IBDT_SNR                  =0xFA,
	IBDT_BG_NOISE             =0xFB,
	IBDT_RSSI                 =0xFC,
	IBDT_SERVO_HUB            =0xFD, // i-Bus 1 only
	IBDT_SIGNAL_STRENGTH      =0xFE,
	IBDT_NONE                 =0xFF
} eIB_DeviceType;

// These receiver internal sensors are mandatory and should follow this numbering
#define IB_NB_MANDATORY_RECEIVER_INTERNAL_SENSORS 5
typedef enum
{
	IB_RISN_INT_VOLTAGE,
	IB_RISN_SIGNAL_STRENGTH,
	IB_RISN_RSSI,
	IB_RISN_BG_NOISE,
	IB_RISN_SNR
} eIB_ReceiverInternalSensorsNb;

#endif // !defined(I_BUS_H)
