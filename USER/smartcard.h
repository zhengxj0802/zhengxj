#ifndef _smartcard_h_
#define _smartcard_h_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported constants --------------------------------------------------------*/
#define T0_PROTOCOL        0x00  /* T0 protocol */
#define DIRECT             0x3B  /* Direct bit convention */
#define INDIRECT           0x3F  /* Indirect bit convention */
#define SETUP_LENGTH       20
#define HIST_LENGTH        20
#define LCmax              20
#define SC_Receive_Timeout 0x4000  /* Direction to reader */

/* SC Tree Structure -----------------------------------------------------------
                              MasterFile
                           ________|___________
                          |        |           |
                        System   UserData     Note
------------------------------------------------------------------------------*/

/* SC ADPU Command: Operation Code -------------------------------------------*/
#define SC_CLA_GSM11       0xA0

/*------------------------ Data Area Management Commands ---------------------*/
#define SC_SELECT_FILE     0xA4
#define SC_GET_RESPONCE    0xC0
#define SC_STATUS          0xF2
#define SC_UPDATE_BINARY   0xD6
#define SC_READ_BINARY     0xB0
#define SC_WRITE_BINARY    0xD0
#define SC_UPDATE_RECORD   0xDC
#define SC_READ_RECORD     0xB2

/*-------------------------- Administrative Commands -------------------------*/ 
#define SC_CREATE_FILE     0xE0

/*-------------------------- Safety Management Commands ----------------------*/
#define SC_VERIFY          0x20
#define SC_CHANGE          0x24
#define SC_DISABLE         0x26
#define SC_ENABLE          0x28
#define SC_UNBLOCK         0x2C
#define SC_EXTERNAL_AUTH   0x82
#define SC_GET_CHALLENGE   0x84

/*-------------------------- Answer to reset Commands ------------------------*/ 
#define SC_GET_A2R         0x00


/* SC STATUS: Status Code ----------------------------------------------------*/
#define SC_EF_SELECTED     0x9F
#define SC_DF_SELECTED     0x9F
#define SC_OP_TERMINATED   0x9000

/* Smartcard Inteface GPIO pins */
#define SC_3_5V            GPIO_Pin_9 /* GPIOD Pin 9 */
#define SC_RESET           GPIO_Pin_9 /* GPIOB Pin 9 */
#define SC_OFF             GPIO_Pin_8 /* GPIOE Pin 8 */ 
#define SC_IO              GPIO_Pin_8 /* GPIOB Pin 8 */
#define SC_CLK             GPIO_Pin_10 /* GPIOB Pin 10 */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  SC_POWER_ON = 0x00,
  SC_RESET_LOW = 0x01,
  SC_RESET_HIGH = 0x02,
  SC_ACTIVE = 0x03,
  SC_ACTIVE_ON_T0 = 0x04,
  SC_POWER_OFF = 0x05
} SC_State;

/* ATR structure - Answer To Reset -------------------------------------------*/
typedef struct
{
  u8 TS;               /* Bit Convention */
  u8 T0;               /* High nibble = Number of setup byte; low nibble = Number of historical byte */
  u8 T[SETUP_LENGTH];  /* Setup array */
  u8 H[HIST_LENGTH];   /* Historical array */
  u8 Tlength;          /* Setup array dimension */
  u8 Hlength;          /* Historical array dimension */
} SC_ATR;

/* ADPU-Header command structure ---------------------------------------------*/
typedef struct
{
  u8 CLA;  /* Command class */
  u8 INS;  /* Operation code */
  u8 P1;   /* Selection Mode */
  u8 P2;   /* Selection Option */
} SC_Header;

/* ADPU-Body command structure -----------------------------------------------*/
typedef struct 
{
  u8 LC;           /* Data field length */
  u8 Data[LCmax];  /* Command parameters */
  u8 LE;           /* Expected length of data to be returned */
} SC_Body;

/* ADPU Command structure ----------------------------------------------------*/
typedef struct
{
  SC_Header Header;
  SC_Body Body;
} SC_ADPU_Commands;

/* SC response structure -----------------------------------------------------*/
typedef struct
{
  u8 Data[LCmax];  /* Data returned from the card */
  u8 SW1;          /* Command Processing status */
  u8 SW2;          /* Command Processing qualification */
} SC_ADPU_Responce;

void SC_Handler(SC_State *SCState, SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_Response);
/*--------------APDU-----------*/
static void SC_SendData(SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_ResponceStatus);
/*------------ ATR ------------*/
static void SC_AnswerReq(SC_State *SCState, u8 *card, u8 length);/* Ask ATR */
static u8 SC_decode_Answer2reset(u8 *card);/* Decode ATR */
/* Physical Port Layer -------------------------------------------------------*/
static void SC_Init(void);
static void SC_DeInit(void);
void SC_Reset(BitAction ResetState);
static u8 SC_Detect(void);
void SC_PowerCmd(FunctionalState NewState);
static ErrorStatus USART_ByteReceive(u8 *Data, u32 TimeOut);

#endif /* __SMARTCARD_H */
