#include "stm32f10x.h"
#include "smartcard.h"
#include "USART.h"
SC_ATR SC_A2R;

/*******************************************************************************
* Function Name  : SC_Init
* Description    : Initializes all peripheral used for Smartcard interface.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void SC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;  
	
	/* Enable GPIOD*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);
	                     
	/* Enable USART3 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	                       
	/* Configure USART3 CK(PD.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = SC_CLK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* Configure USART3 Tx (PD.8) as alternate function open-drain */
	GPIO_InitStructure.GPIO_Pin = SC_IO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* Configure Smartcard Reset (PD.9)  */
	GPIO_InitStructure.GPIO_Pin = SC_RESET;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* Configure Smartcard OFF (PA.8)  */
	GPIO_InitStructure.GPIO_Pin = SC_OFF;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*Configure Smartcard SC_3_5V (PC.9)*/
	GPIO_InitStructure.GPIO_Pin =  SC_3_5V;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	 
	/* USART Clock set to 3.6 MHz (PCLK1 (36 MHZ) / 10) */
	USART_SetPrescaler(USART3, 0x05);
	
	/* USART Guard Time set to 16 Bit */
	USART_SetGuardTime(USART3, 16);
	
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 9677;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_Init(USART3, &USART_InitStructure);
	
	USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
	USART_ClockInitStructure.USART_CPOL=USART_CPOL_High;
	USART_ClockInitStructure.USART_CPHA=USART_CPHA_1Edge;
	USART_ClockInit(USART3, &USART_ClockInitStructure);
	
	/* Enable the USART3 Parity Error Interrupt */
	USART_ITConfig(USART3, USART_IT_PE, ENABLE);
	
	/* Enable the USART3 Framing Error Interrupt */
	USART_ITConfig(USART3, USART_IT_ERR, ENABLE);
	
	/* Enable USART3 */
	USART_Cmd(USART3, ENABLE);
	
	/* Enable the NACK Transmission */
	USART_SmartCardNACKCmd(USART3, ENABLE);
	
	/* Enable the Smartcard Interface */
	USART_SmartCardCmd(USART3, ENABLE);
	
	SC_Reset(Bit_SET);

	SC_PowerCmd(DISABLE);  //power off
}
/*******************************************************************************
* Function Name  : SC_DeInit
* Description    : Deinitializes all ressources used by the Smartcard interface.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void SC_DeInit(void)
{
	/*REset low*/
	SC_Reset(Bit_RESET);

	/* Deinitializes the USART3 */
	USART_DeInit(USART3);
                         
	/* Disable USART3 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, DISABLE);
}
/*******************************************************************************
* Function Name  : SC_Reset
* Description    : Sets or clears the Smartcard reset pin.
* Input          : - ResetState: this parameter specifies the state of the Smartcard 
*                    reset pin.
*                    BitVal must be one of the BitAction enum values:
*                       - Bit_RESET: to clear the port pin.
*                       - Bit_SET: to set the port pin.
* Output         : None
* Return         : None
*******************************************************************************/
void SC_Reset(BitAction ResetState)
{
	GPIO_WriteBit(GPIOD, SC_RESET, ResetState);
}
/*******************************************************************************
* Function Name  : SC_Detect
* Description    : Detects whether the Smartcard is present or not.
* Input          : None. 
* Output         : None.
* Return         : 0 - Smartcard inserted
*                  1 - Smartcard not inserted
*******************************************************************************/
static u8 SC_Detect(void)
{
	SC_PowerCmd(ENABLE);//power on
	if(GPIO_ReadInputDataBit(GPIOD, SC_OFF))// Smartcard not inserted
	{
		//SC_PowerCmd(DISABLE);//power off
		return 0xff;
	}
	else//Smartcard	inserted
	{
		//SC_PowerCmd(DISABLE);//power off
		return C_OK;	
	}
}
/*******************************************************************************
* Function Name  : SC_PowerCmd
* Description    : Enables or disables the power to the Smartcard.
* Input          : NewState: new state of the Smartcard power supply. 
*                  This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void SC_PowerCmd(FunctionalState NewState)
{
	if(NewState == ENABLE)//POWER ON
	{
		GPIO_SetBits(GPIOC, SC_3_5V);
	}
	else //POWER OFF
	{
		GPIO_ResetBits(GPIOC, SC_3_5V);
	} 
}
/*******************************************************************************
* Function Name  : SC_Handler
* Description    : Handles all Smartcard states and serves to send and receive all
*                  communication data between Smartcard and reader.
* Input          : - SCState: pointer to an SC_State enumeration that will contain
*                    the Smartcard state.
*                  - SC_ADPU: pointer to an SC_ADPU_Commands structure that will be
*                    initialized.  
*                  - SC_Response: pointer to a SC_ADPU_Responce structure which will
*                    be initialized.
* Output         : None
* Return         : None
*******************************************************************************/
void SC_Handler(SC_State *SCState, SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_Response)
{
	u32 i = 0;
	u8 SC_ATR_Table[20];

	switch(*SCState)
	{
		case SC_POWER_ON:
			/*智能卡外围设备被使能和初始化，没有电源和时钟供给智能卡*/
			if (SC_ADPU->Header.INS == SC_GET_A2R)
      		{
				/* Smartcard intialization ------------------------------------------*/
				SC_Init();
				
				/* Reset Data from SC buffer -----------------------------------------*/
				for (i = 0; i <20; i++)
				{
					SC_ATR_Table[i] = 0;
				}
				
				/* Reset SC_A2R Structure --------------------------------------------*/
				SC_A2R.TS = 0;
				SC_A2R.T0 = 0;
				for (i = 0; i < SETUP_LENGTH; i++)
				{
					SC_A2R.T[i] = 0;
				}
				for (i = 0; i < HIST_LENGTH; i++)
				{
				  	SC_A2R.H[i] = 0;
				}
				SC_A2R.Tlength = 0;
				SC_A2R.Hlength = 0;
        
        		/* Next State --------------------------------------------------------*/
        		*SCState = SC_RESET_LOW;
      		}
			USARTSendByte(USART1,*SCState);
    		break;

		case SC_RESET_LOW:
			/*电源和时钟供给智能卡*/
     		if(SC_ADPU->Header.INS == SC_GET_A2R)
      		{
        		/* If card is detected then Power ON, Card Reset and wait for an answer) */
        		if (!SC_Detect())
        		{
          			while(((*SCState) != SC_POWER_OFF) && ((*SCState) != SC_ACTIVE))
          			{
            			SC_AnswerReq(SCState, &SC_ATR_Table[0], 20); /* Check for answer to reset */
          			}
        		}
        		else
        		{
          			(*SCState) = SC_POWER_OFF;
        		} 
      		}
			USARTSendByte(USART1,*SCState);
    		break;

		case SC_ACTIVE:
			/*收到复位响应，解码ATR帧,判断是否为T0协议*/
			if (SC_ADPU->Header.INS == SC_GET_A2R)
			{
        		if(SC_decode_Answer2reset(&SC_ATR_Table[0]) == T0_PROTOCOL)
        		{
          			(*SCState) = SC_ACTIVE_ON_T0;
        		}
        		else
        		{
          			(*SCState) = SC_POWER_OFF; 
        		}
      		}
			USARTSendByte(USART1,*SCState);
    		break;

    	case SC_ACTIVE_ON_T0:
			/*使用T0协议，可以发送ADPU报文*/
      		SC_SendData(SC_ADPU, SC_Response);
    		break;

    	case SC_POWER_OFF:
      		SC_DeInit(); /* Disable Smartcard interface */
    		break;

    	default: 
			(*SCState) = SC_POWER_OFF;
			break;
	}
}
/*******************************************************************************
* Function Name  : SC_AnswerReq
* Description    : Requests the reset answer from card.
* Input          : - SCState: pointer to an SC_State enumeration that will contain
*                    the Smartcard state.
*                  - card: pointer to a buffer which will contain the card ATR.
*                  - length: maximum ATR length
* Output         : None
* Return         : None
*******************************************************************************/
static void SC_AnswerReq(SC_State *SCstate, u8 *card, u8 length)
{
	u8 Data = 0;
	u32 i = 0;

	switch(*SCstate)
	{
    	case SC_RESET_LOW:
	      	/* Check responce with reset low ---------------------------------------*/
			SC_Reset(Bit_RESET); //Reset low
	      	for (i = 0; i < length; i++)
	      	{
	        	if((USART_ByteReceive(&Data, SC_Receive_Timeout)) == SUCCESS)
	        	{
	          		card[i] = Data;
					USARTSendByte(USART1,card[i]);
	        	}
	      	}
	      	if(card[0])
	      	{
	        	(*SCstate) = SC_ACTIVE;
	        	SC_Reset(Bit_SET);
	      	}
	      	else
	      	{
	        	(*SCstate) = SC_RESET_HIGH;
	      	}
			USARTSendByte(USART1,*SCstate);
	    	break;

    	case SC_RESET_HIGH:
	      	/* Check responce with reset high --------------------------------------*/
	      	SC_Reset(Bit_SET); //Reset High
	  
	      	while(length--)
	      	{
	        	if((USART_ByteReceive(&Data, SC_Receive_Timeout)) == SUCCESS)
	        	{
	          		*card++ = Data; /* Receive data for timeout = SC_Receive_Timeout */
	        	}       
	      	}
	      	if(card[0])
	      	{
	        	(*SCstate) = SC_ACTIVE;
	      	}
	      	else
			{
	        	(*SCstate) = SC_POWER_OFF;
			}
			USARTSendByte(USART1,*SCstate);
	    	break;

    	case SC_ACTIVE:
    		break;
    
		case SC_POWER_OFF:
      		/* Close Connection if no answer received ------------------------------*/
			SC_Reset(Bit_SET); /* Reset high - a bit is used as level shifter from 3.3 to 5 V */
    		break;

    	default:
			(*SCstate) = SC_RESET_LOW;
			break;
	}
}

/*******************************************************************************
* Function Name  : SC_decode_Answer2reset
* Description    : Decodes the Answer to reset received from card.
* Input          : - Card: pointer to the buffer containing the card ATR.
* Output         : None
* Return         : None
*******************************************************************************/
static u8 SC_decode_Answer2reset(u8 *card)
{
	u32 i = 0, flag = 0, buf = 0, protocol = 0;
	
	SC_A2R.TS = card[0];  /* Initial character */
	USARTSendByte(USART1,SC_A2R.TS);
	SC_A2R.T0 = card[1];  /* Format character */
	USARTSendByte(USART1,SC_A2R.T0);
	SC_A2R.Hlength = SC_A2R.T0 & (u8)0x0F;/*屏蔽高4位，Number of setup byte*/
	
	if ((SC_A2R.T0 & (u8)0x80) == 0x80)
	{
		flag = 1;
	}

	for (i = 0; i < 4; i++)	/*屏蔽低四位，Number of historical byte*/
	{
		SC_A2R.Tlength = SC_A2R.Tlength + (((SC_A2R.T0 & (u8)0xF0) >> (4 + i)) & (u8)0x1);
	}
	USARTSendByte(USART1,SC_A2R.Tlength);
	for (i = 0; i < SC_A2R.Tlength; i++)
	{
		SC_A2R.T[i] = card[i + 2];
		USARTSendByte(USART1,SC_A2R.T[i]);
	}
	
	protocol = SC_A2R.T[SC_A2R.Tlength - 1] & (u8)0x0F;
	USARTSendByte(USART1,protocol);
	while (flag)
	{
		if ((SC_A2R.T[SC_A2R.Tlength - 1] & (u8)0x80) == 0x80)
		{
		  flag = 1;
		}
		else
		{
		  flag = 0;
		}
		
		buf = SC_A2R.Tlength;
		SC_A2R.Tlength = 0;
	
		for (i = 0; i < 4; i++)
		{
		  SC_A2R.Tlength = SC_A2R.Tlength + (((SC_A2R.T[buf - 1] & (u8)0xF0) >> (4 + i)) & (u8)0x1);
		}
		
		for (i = 0;i < SC_A2R.Tlength; i++)
		{
		  SC_A2R.T[buf + i] = card[i + 2 + buf];
		}
		SC_A2R.Tlength += (u8)buf;
	}
	
	for (i = 0; i < SC_A2R.Hlength; i++)
	{
		SC_A2R.H[i] = card[i + 2 + SC_A2R.Tlength];
		USARTSendByte(USART1,SC_A2R.H[i]);
	}
	
	return (u8)protocol;
}

/*******************************************************************************
* Function Name  : SC_SendData
* Description    : Manages the Smartcard transport layer: send APDU commands and
*                  receives the APDU responce.
* Input          : - SC_ADPU: pointer to a SC_ADPU_Commands structure which 
*                    will be initialized.  
*                  - SC_Response: pointer to a SC_ADPU_Responce structure which 
*                    will be initialized.
* Output         : None
* Return         : None
*******************************************************************************/
static void SC_SendData(SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_ResponceStatus)
{
	u32 i = 0;
	u8 locData = 0;
	static vu8 SCData = 0;

	/* Reset responce buffer ---------------------------------------------------*/
	for(i = 0; i < LCmax; i++)
	{
		SC_ResponceStatus->Data[i] = 0;
	}
	
	SC_ResponceStatus->SW1 = 0;
	SC_ResponceStatus->SW2 = 0;
	
	/* Enable the DMA Receive (Set DMAR bit only) to enable interrupt generation
	 in case of a framing error FE */  
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	
	/* Send header -------------------------------------------------------------*/
	SCData = SC_ADPU->Header.CLA;
	USART_SendData(USART3, SCData);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
	{
	}  
	
	SCData = SC_ADPU->Header.INS;
	USART_SendData(USART3, SCData);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
	{
	}
   
	SCData = SC_ADPU->Header.P1;
	USART_SendData(USART3, SCData);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
	{
	} 
	
	SCData = SC_ADPU->Header.P2;
	USART_SendData(USART3, SCData);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
	{
	}   

	/* Send body length to/from SC ---------------------------------------------*/
	if(SC_ADPU->Body.LC)
	{
		SCData = SC_ADPU->Body.LC;
		USART_SendData(USART3, SCData);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
		{
		}     
	}
	else if(SC_ADPU->Body.LE)
	{ 
		SCData = SC_ADPU->Body.LE;
		USART_SendData(USART3, SCData);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
		{
		}     
	}
	/* Flush the USART3 DR */
	(void)USART_ReceiveData(USART3);

	/* --------------------------------------------------------
	Wait Procedure byte from card:
	1 - ACK
	2 - NULL
	3 - SW1; SW2
	-------------------------------------------------------- */

	if((USART_ByteReceive(&locData, SC_Receive_Timeout)) == SUCCESS)
	{
		if(((locData & (u8)0xF0) == 0x60) || ((locData & (u8)0xF0) == 0x90))
		{
		  /* SW1 received */
		  SC_ResponceStatus->SW1 = locData;
		
		  if((USART_ByteReceive(&locData, SC_Receive_Timeout)) == SUCCESS)
		  {
		    /* SW2 received */
		    SC_ResponceStatus->SW2 = locData;
		  }
		}
		else if (((locData & (u8)0xFE) == (((u8)~(SC_ADPU->Header.INS)) & (u8)0xFE))||((locData & (u8)0xFE) == (SC_ADPU->Header.INS & (u8)0xFE)))
		{
		  SC_ResponceStatus->Data[0] = locData;/* ACK received */
		}
	}

	/* If no status bytes received ---------------------------------------------*/
	if(SC_ResponceStatus->SW1 == 0x00)
	{
		/* Send body data to SC--------------------------------------------------*/
		if (SC_ADPU->Body.LC)
		{
		  for(i = 0; i < SC_ADPU->Body.LC; i++)
		  {
			SCData = SC_ADPU->Body.Data[i];
			
			USART_SendData(USART3, SCData);
			while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
			{
			} 
		  }
		  /* Flush the USART3 DR */
		  (void)USART_ReceiveData(USART3);
		  /* Disable the DMA Receive (Reset DMAR bit only) */  
		  USART_DMACmd(USART3, USART_DMAReq_Rx, DISABLE);
		}
		/* Or receive body data from SC ------------------------------------------*/
		else if (SC_ADPU->Body.LE)
		{
			for(i = 0; i < SC_ADPU->Body.LE; i++)
			{
				if(USART_ByteReceive(&locData, SC_Receive_Timeout) == SUCCESS)
				{
				  SC_ResponceStatus->Data[i] = locData;
				}
			}
		}
    	/* Wait SW1 --------------------------------------------------------------*/
    	i = 0;
    	while(i < 10)
    	{
	      	if(USART_ByteReceive(&locData, SC_Receive_Timeout) == SUCCESS)
	      	{
	        	SC_ResponceStatus->SW1 = locData;
	        	i = 11;
	      	}
	      	else
	      	{
	        	i++;
	      	}
    	}
    	/* Wait SW2 ------------------------------------------------------------*/   
    	i = 0;
    	while(i < 10)
    	{
	      	if(USART_ByteReceive(&locData, SC_Receive_Timeout) == SUCCESS)
	      	{
	        	SC_ResponceStatus->SW2 = locData;
	        	i = 11;
	      	}
	      	else
	      	{
	        	i++;
	      	}
    	}
	}
}
/*******************************************************************************
* Function Name  : USART_ByteReceive
* Description    : Receives a new data while the time out not elapsed.
* Input          : None
* Output         : None
* Return         : An ErrorStatus enumuration value:
*                         - SUCCESS: New data has been received
*                         - ERROR: time out was elapsed and no further data is 
*                                  received
*******************************************************************************/
static ErrorStatus USART_ByteReceive(u8 *Data, u32 TimeOut)
{
	u32 Counter = 0;
	
	while((USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET) && (Counter != TimeOut))
	{
		Counter++;
	}
	if(Counter != TimeOut)
	{
		*Data = (u8)USART_ReceiveData(USART3);
		return SUCCESS;    
	}
	else 
	{
		return ERROR;
	}
}
