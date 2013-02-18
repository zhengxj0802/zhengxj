#ifndef _CPUCard_h_
#define _CPUCard_h_


/*********************宏定义*********************/
#define RST_L			{PORTG= PORTG&0xFB;	DDRG  = DDRG|0x04;}//PG2输出低电平
#define RST_H			{PORTG= PORTG|0x04;	DDRG  = DDRG|0x04;}//PG2输出高电平
#define CLK_L			{PORTF = PORTF&0x7f;DDRF  = DDRF|0x80;}//PF7输出低电平
#define CLK_H			{PORTF = PORTF|0x80;DDRF  = DDRF|0x80;}//PF7输出高电平

#define SETUP_LENGTH		20
#define HIST_LENGTH			20
#define LCmax				20

/* SC ADPU Command: Operation Code*/
#define SC_CLA_GSM11       	0xA0

/*Data Area Management Commands*/
#define SC_SELECT_FILE     	0xA4
#define SC_GET_RESPONCE    	0xC0
#define SC_STATUS          	0xF2
#define SC_UPDATE_BINARY   	0xD6
#define SC_READ_BINARY     	0xB0
#define SC_WRITE_BINARY    	0xD0
#define SC_UPDATE_RECORD   	0xDC
#define SC_READ_RECORD     	0xB2

/*Administrative Commands*/ 
#define SC_CREATE_FILE     	0xE0

/*Safety Management Commands*/
#define SC_VERIFY          	0x20
#define SC_CHANGE          	0x24
#define SC_DISABLE         	0x26
#define SC_ENABLE          	0x28
#define SC_UNBLOCK         	0x2C
#define SC_EXTERNAL_AUTH   	0x82
#define SC_GET_CHALLENGE   	0x84

/*Answer to reset Commands*/ 
#define SC_GET_A2R         	0x00

/* SC STATUS: Status Code*/
#define SC_EF_SELECTED     	0x9F
#define SC_DF_SELECTED     	0x9F
#define SC_OP_TERMINATED   	0x9000

/*********************定义结构体*********************/
/* ATR structure - Answer To Reset*/
typedef struct
{
	u8 TS;               			/* Bit Convention */
	u8 T0;               			/* High nibble = Number of setup byte; low nibble = Number of historical byte */
	u8 T[SETUP_LENGTH];  	/* Setup array */
	u8 H[HIST_LENGTH];   	/* Historical array */
	u8 Tlength;          			/* Setup array dimension */
	u8 Hlength;          			/* Historical array dimension */
} SC_ATR;

/* ADPU-Header command structure*/
typedef struct
{
	u8 CLA;  /* Command class */
	u8 INS;  /* Operation code */
	u8 P1;   /* Selection Mode */
	u8 P2;   /* Selection Option */
} SC_Header;

/* ADPU-Body command structure*/
typedef struct 
{
	u8 LC;           		/* Data field length */
	u8 Data[LCmax];  	/* Command parameters */
	u8 LE;           		/* Expected length of data to be returned */
} SC_Body;

/* ADPU Command structure*/
typedef struct
{
	SC_Header Header;
	SC_Body Body;
} SC_ADPU_Commands;

/* SC response structure*/
typedef struct
{
	u8 Data[LCmax];  	/* Data returned from the card */
	u8 SW1;          			/* Command Processing status */
	u8 SW2;          			/* Command Processing qualification */
} SC_ADPU_Responce;

/*********************函数声明***************************/
/*CPU卡管脚配置*/
void CPUCARD_Configuration(void);

/*IC卡槽上电*/
void POWERONICCARD(void);

/*IC卡槽下电*/
void POWEROFFICCARD(void);

/*接收数据，1个字节*/
u8 ReceiveData(u8 *data);

/*发送数据，一个字节*/
u8 SendData(u8 data);

/*启动CPU卡*/
u8 StartCPUCard(u8 *Reset_ACK);

/*停止CPU卡*/
void StopCPUCard(void);

/*发送ADPU指令*/
//u8 SendADPUData(SC_ADPU_Commands *SC_ADPU,SC_ADPU_Responce *SC_ResponceStatus);
u8 SendADPUData(void);
u8 Get_Challenge(u8 *data,u8 length);
u8 External_Authenticate(u8 *data,u8 length,u8 *key,u8 keyleng,u8 type);
#endif

