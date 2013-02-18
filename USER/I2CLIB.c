#include "stm32f10x.h"
#include "I2CLIB.h"
#include "comdef.h"
#include "systick.h"
#include "USART.h"

void I2C_delay(void)
{	
   delay_ms(1);
}

/******************************************************************************/
/*********************************模拟I2C1**************************************/
/******************************************************************************/
/******************************************************************************/
/*******************************************************************************
* Function Name  : I2C_Start
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
u8 I2C1_Start(void)
{
	SDA1_H;
	SCL1_H;
	I2C_delay();
	if(!SDA1_read)return !C_OK;	/* SDA线为低电平则总线忙,退出 */
	SDA1_L;
	I2C_delay();
	if(SDA1_read) return !C_OK;	/* SDA线为高电平则总线出错,退出 */
	SDA1_L;
	I2C_delay();
	return C_OK;
}

/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C1_Stop(void)
{
	SCL1_L;
	I2C_delay();
	SDA1_L;
	I2C_delay();
	SCL1_H;
	I2C_delay();
	SDA1_H;
	I2C_delay();
}

/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C1_Ack(void)
{	
	SCL1_L;
	I2C_delay();
	SDA1_L;
	I2C_delay();
	SCL1_H;
	I2C_delay();
	SCL1_L;
	I2C_delay();
}

/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C1_NoAck(void)
{	
	SCL1_L;
	I2C_delay();
	SDA1_H;
	I2C_delay();
	SCL1_H;
	I2C_delay();
	SCL1_L;
	I2C_delay();
}

/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : None
* Input          : None
* Output         : None
* Return         : 返回为:=1有ACK,=0无ACK
* Attention		 : None
*******************************************************************************/
static u8 I2C1_WaitAck(void) 	
{
	SCL1_L;
	I2C_delay();
	SDA1_H;			
	I2C_delay();
	SCL1_H;
	I2C_delay();
	if(SDA1_read)
	{
      SCL1_L;
      return !C_OK;
	}
	SCL1_L;
	return C_OK;
}

 /*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : 写一个字节，数据从高位到低位
* Input          : - SendByte: 发送的数据
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C1_SendByte(uint8_t SendByte) 
{
    uint8_t i=8;
    while(i--)
    {
        SCL1_L;
        I2C_delay();
      	if(SendByte&0x80)
        	SDA1_H;  
      	else 
        	SDA1_L;   
        SendByte<<=1;
        I2C_delay();
		SCL1_H;
        I2C_delay();
    }
    SCL1_L;
}
/*******************************************************************************
* Function Name  : I2C_ReceiveByte
* Description    : 读一个字节，数据从高位到低位
* Input          : None
* Output         : None
* Return         : I2C总线返回的数据
* Attention		 : None
*******************************************************************************/
static uint8_t I2C1_ReceiveByte(void)  
{ 
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA1_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL1_L;
      I2C_delay();
	  SCL1_H;
      I2C_delay();	
      if(SDA1_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL1_L;
    return ReceiveByte;
}
/*********************************************************************************************************/
/*写一页的数据*/
u8 I2C1_WritePage(uint8_t* pBuffer, uint16_t length,uint16_t WriteAddress, uint8_t DeviceAddress)
{
	uint16_t i=0;
	if(I2C1_Start())return C_EEPROMWRITEERR;//启动I2C总线
	I2C1_SendByte(((0xA0|DeviceAddress)& 0xFFFE));//发送器件地址
	if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMWRITEERR;}
	I2C1_SendByte((WriteAddress & 0x0700) >>8) ;//发送高起始地址
	if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMWRITEERR;}
	I2C1_SendByte((uint8_t)(WriteAddress & 0x00FF));//发送低起始地址
	if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMWRITEERR;}
	for(i=0;i<length;i++)
	{
		I2C1_SendByte(pBuffer[i]); //写数据
		if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMWRITEERR;}
	}
	I2C1_Stop(); 
	delay_ms(10);//延时等待写完，10ms
    return C_OK;
}
/*写多个字节*/									 
u8 I2C1_WriteNByte(uint8_t* pBuffer, uint16_t length,uint16_t WriteAddress, uint8_t DeviceAddress)
{
	uint16_t n=0;
	uint16_t psize=32;

	//确定要写的数据的位置在哪一页上的哪一列上（WriteAddress%psize）
	//并确定该页所剩余的地址空间是多少n
	n=psize-(uint16_t)(WriteAddress%psize);
	if(n>=length) //判断该页是否够写，够的话就直接写入
	{
		return(I2C1_WritePage(pBuffer,length,WriteAddress,DeviceAddress));
	}
	else
	{
		/*写入剩余部分*/
		if(I2C1_WritePage(pBuffer,n,WriteAddress,DeviceAddress)!=C_OK)	return C_EEPROMWRITEERR;
		length-=n;
		WriteAddress+=n;
		pBuffer+=n;
		delay_ms(25);
		/*满一页一页的写*/
		while(length>=psize)
		{
			if(I2C1_WritePage(pBuffer,psize,WriteAddress,DeviceAddress)!=C_OK) return C_EEPROMWRITEERR;	
			length-=n;
			WriteAddress+=n;
			pBuffer+=n;
			delay_ms(25);
		}
		/*写入最后剩余部分*/
		if(length)
		{
			if(I2C1_WritePage(pBuffer,length,WriteAddress,DeviceAddress)!=C_OK)	return C_EEPROMWRITEERR;
		}
		delay_ms(25);
		return 	C_OK;		
	}
	//return C_EEPROMWRITEERR;
}

/*读取一串数据*/          
u8 I2C1_ReadNByte(uint8_t* pBuffer,   uint16_t length,   uint16_t ReadAddress,  uint8_t DeviceAddress)
{		
    if(I2C1_Start())return C_EEPROMREADEER;
	I2C1_SendByte((0xA0|DeviceAddress) & 0xFFFE);//发送器件地址
	if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMREADEER;}
	I2C1_SendByte((ReadAddress & 0x0700) >>8) ;//发送高起始地址
	if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMREADEER;}
	I2C1_SendByte((uint8_t)(ReadAddress & 0x00FF));//发送低起始地址
	if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMREADEER;}

   	if(I2C1_Start())return C_EEPROMREADEER;
	I2C1_SendByte(0xA1|DeviceAddress);//发送器件地址
    if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMREADEER;}
    while(length)
    {
      *pBuffer = I2C1_ReceiveByte();
      if(length == 1)I2C1_NoAck();
      else I2C1_Ack(); 
      pBuffer++;
      length--;
    }
    I2C1_Stop();
    return C_OK;
}

/******************************************************************************/
/*********************************模拟I2C2**************************************/
/******************************************************************************/
/******************************************************************************/
/*******************************************************************************
* Function Name  : I2C_Start
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
u8 I2C2_Start(void)
{
	SDA2_H;
	SCL2_H;
	I2C_delay();
	if(!SDA2_read)return !C_OK;	/* SDA线为低电平则总线忙,退出 */
	SDA2_L;
	I2C_delay();
	if(SDA2_read) return !C_OK;	/* SDA线为高电平则总线出错,退出 */
	SDA2_L;
	I2C_delay();
	return C_OK;
}

/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C2_Stop(void)
{
	SCL2_L;
	I2C_delay();
	SDA2_L;
	I2C_delay();
	SCL2_H;
	I2C_delay();
	SDA2_H;
	I2C_delay();
}

/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C2_Ack(void)
{	
	SCL2_L;
	I2C_delay();
	SDA2_L;
	I2C_delay();
	SCL2_H;
	I2C_delay();
	SCL2_L;
	I2C_delay();
}

/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C2_NoAck(void)
{	
	SCL2_L;
	I2C_delay();
	SDA2_H;
	I2C_delay();
	SCL2_H;
	I2C_delay();
	SCL2_L;
	I2C_delay();
}

/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : None
* Input          : None
* Output         : None
* Return         : 返回为:=1有ACK,=0无ACK
* Attention		 : None
*******************************************************************************/
static u8 I2C2_WaitAck(void) 	
{
	SCL2_L;
	I2C_delay();
	SDA2_H;			
	I2C_delay();
	SCL2_H;
	I2C_delay();
	if(SDA2_read)
	{
      SCL2_L;
      return !C_OK;
	}
	SCL2_L;
	return C_OK;
}

 /*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : 写一个字节，数据从高位到低位
* Input          : - SendByte: 发送的数据
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C2_SendByte(uint8_t SendByte) 
{
    uint8_t i=8;
    while(i--)
    {
        SCL2_L;
        I2C_delay();
      	if(SendByte&0x80)
        	SDA2_H;  
      	else 
        	SDA2_L;   
        SendByte<<=1;
        I2C_delay();
		SCL2_H;
        I2C_delay();
    }
    SCL2_L;
}
/*******************************************************************************
* Function Name  : I2C_ReceiveByte
* Description    : 读一个字节，数据从高位到低位
* Input          : None
* Output         : None
* Return         : I2C总线返回的数据
* Attention		 : None
*******************************************************************************/
static uint8_t I2C2_ReceiveByte(void)  
{ 
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA2_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL2_L;
      I2C_delay();
	  SCL2_H;
      I2C_delay();	
      if(SDA2_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL2_L;
    return ReceiveByte;
}
/*********************************************************************************************************/
/*写一页的数据*/
u8 I2C2_WritePage(uint8_t* pBuffer, uint16_t length,uint16_t WriteAddress, uint8_t DeviceAddress)
{
	uint16_t i=0;
	if(I2C2_Start())return C_EEPROMWRITEERR;//启动I2C总线
	I2C2_SendByte(((0xA0|DeviceAddress)& 0xFFFE));//发送器件地址
	if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMWRITEERR;}
	I2C2_SendByte((WriteAddress & 0x0700) >>8) ;//发送高起始地址
	if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMWRITEERR;}
	I2C2_SendByte((uint8_t)(WriteAddress & 0x00FF));//发送低起始地址
	if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMWRITEERR;}
	for(i=0;i<length;i++)
	{
		I2C2_SendByte(pBuffer[i]); //写数据
		if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMWRITEERR;}
	}
	I2C2_Stop(); 
	delay_ms(10);//延时等待写完，10ms
    return C_OK;
}
/*写多个字节*/									 
u8 I2C2_WriteNByte(uint8_t* pBuffer, uint16_t length,uint16_t WriteAddress, uint8_t DeviceAddress)
{
	uint16_t n=0;
	uint16_t psize=32;

	//确定要写的数据的位置在哪一页上的哪一列上（WriteAddress%psize）
	//并确定该页所剩余的地址空间是多少n
	n=psize-(uint16_t)(WriteAddress%psize);
	if(n>=length) //判断该页是否够写，够的话就直接写入
	{
		return(I2C2_WritePage(pBuffer,length,WriteAddress,DeviceAddress));
	}
	else
	{
		/*写入剩余部分*/
		if(I2C2_WritePage(pBuffer,n,WriteAddress,DeviceAddress)!=C_OK)	return C_EEPROMWRITEERR;
		length-=n;
		WriteAddress+=n;
		pBuffer+=n;
		delay_ms(25);
		/*满一页一页的写*/
		while(length>=psize)
		{
			if(I2C2_WritePage(pBuffer,psize,WriteAddress,DeviceAddress)!=C_OK) return C_EEPROMWRITEERR;	
			length-=n;
			WriteAddress+=n;
			pBuffer+=n;
			delay_ms(25);
		}
		/*写入最后剩余部分*/
		if(length)
		{
			if(I2C2_WritePage(pBuffer,length,WriteAddress,DeviceAddress)!=C_OK)	return C_EEPROMWRITEERR;
		}
		delay_ms(25);
		return 	C_OK;		
	}
	//return C_EEPROMWRITEERR;
}

/*读取一串数据*/          
u8 I2C2_ReadNByte(uint8_t* pBuffer,   uint16_t length,   uint16_t ReadAddress,  uint8_t DeviceAddress)
{		
    if(I2C2_Start())return C_EEPROMREADEER;
	I2C2_SendByte((0xA0|DeviceAddress) & 0xFFFE);//发送器件地址
	if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMREADEER;}
	I2C2_SendByte((ReadAddress & 0x0700) >>8) ;//发送高起始地址
	if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMREADEER;}
	I2C2_SendByte((uint8_t)(ReadAddress & 0x00FF));//发送低起始地址
	if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMREADEER;}

   	if(I2C2_Start())return C_EEPROMREADEER;
	I2C2_SendByte(0xA1|DeviceAddress);//发送器件地址
    if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMREADEER;}
    while(length)
    {
      *pBuffer = I2C2_ReceiveByte();
      if(length == 1)I2C2_NoAck();
      else I2C2_Ack(); 
      pBuffer++;
      length--;
    }
    I2C2_Stop();
    return C_OK;
}
/******************************************************************************/
/*********************************硬件I2C**************************************/
/******************************************************************************/
/******************************************************************************/

/*******************************************************************************
* Function Name  : I2C_AcknowledgePolling
* Description    : 等待获取I2C总线控制权 判断忙状态
* Input          : - I2Cx:I2C寄存器基址
*                  - I2C_Addr:从器件地址
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_AcknowledgePolling(I2C_TypeDef *I2Cx,uint8_t I2C_Addr)
{
	vu16 SR1_Tmp;
	do
	{   
		I2C_GenerateSTART(I2Cx, ENABLE); /*起始位*/
		/*读SR1*/
		SR1_Tmp = I2C_ReadRegister(I2Cx, I2C_Register_SR1);
		/*器件地址(写)*/
		I2C_Send7bitAddress(I2Cx, I2C_Addr&0xfe, I2C_Direction_Transmitter);
	}while(!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002));
	
	I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
	I2C_GenerateSTOP(I2Cx, ENABLE);  /*停止位*/  
}
/*******************************************************************************
* Function Name  : I2C_Read
* Description    : 通过指定I2C接口读取多个字节数据
* Input          : - I2Cx:I2C寄存器基址
*                  - I2C_Addr:从器件地址
*                  - addr:预读取字节存储位置
*                  - *buf:读取数据的存储位置
*                  - num:读取字节数
* Output         : None
* Return         : 成功返回0
* Attention		 : None
*******************************************************************************/
uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t DeviceAddress,uint8_t ReadAddress,uint8_t *buf,uint16_t length)
{
	if(length==0)
		return 1;
	
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	/*允许1字节1应答模式*/
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	/* 发送起始位 */
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
    /*发送器件地址(写)*/
    I2C_Send7bitAddress(I2Cx,  (DeviceAddress&0xfe), I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	/*发送地址*/
	I2C_SendData(I2Cx, (ReadAddress & 0x0700) >>8);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	/*发送地址*/
	I2C_SendData(I2Cx,ReadAddress & 0x00FF);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	/*起始位*/
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	/*器件读*/
	I2C_Send7bitAddress(I2Cx,  DeviceAddress, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	while(length)
	{ 
	  	if(length==1)
	    {
			I2C_AcknowledgeConfig(I2Cx, DISABLE);	/* 最后一位后要关闭应答的 */
	    	I2C_GenerateSTOP(I2Cx, ENABLE);	
		}
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED ));
      	*buf = I2C_ReceiveData(I2Cx);
	    buf++;
	    length--;
    }
	/* 再次允许应答模式 */
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	return 0;
}	
/*******************************************************************************
* Function Name  : I2C_WriteOneByte
* Description    : 通过指定I2C接口写入一个字节数据
* Input          : - I2Cx:I2C寄存器基址
*                  - I2C_Addr:从器件地址
*                  - addr:预写入字节地址
*                  - value:写入数据
* Output         : None
* Return         : 成功返回0
* Attention		 : None
*******************************************************************************/
uint8_t I2C_WritePageByte(I2C_TypeDef *I2Cx,uint8_t DeviceAddress,uint8_t WriteAddress,uint8_t *buf,uint16_t length)
{
    /* 起始位 */
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
  	/* 发送器件地址(写)*/
  	I2C_Send7bitAddress(I2Cx, DeviceAddress, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  	/*发送地址*/
	I2C_SendData(I2Cx, (WriteAddress & 0x0700) >>8);//发送高起始地址
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2Cx, (WriteAddress & 0x00FF));	//发送低起始地址
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))	;
	/* 每次写一个字节*/
	while(length--)
	{
  		I2C_SendData(I2Cx, *buf); 
		buf++;
	    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	} 
  	/* 停止位*/
  	I2C_GenerateSTOP(I2Cx, ENABLE);
	I2C_AcknowledgePolling(I2Cx,DeviceAddress);
	delay_ms(10);
	return 0;
}
/*******************************************************************************
* Function Name  : I2C_Write
* Description    : 通过指定I2C接口写入多个字节数据
* Input          : - I2Cx:I2C寄存器基址
*                  - I2C_Addr:从器件地址
*                  - addr:预写入字节地址
*                  - *buf:预写入数据存储位置
*                  - num:写入字节数
* Output         : None
* Return         : 成功返回0
* Attention		 : None
*******************************************************************************/
uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t DeviceAddress,uint8_t WriteAddress,uint8_t *buf,uint16_t length)
{

    uint16_t n=0;
	uint16_t psize=0;
	if((DeviceAddress==ICCARD_WRITE)||(DeviceAddress==ICCARD_READ))
	  psize = 32;	//IC卡
	else
	  psize = 64;	//EEPROM
	 
	//确定要写的数据的位置在哪一页上的哪一列上（WriteAddress%psize）
	//并确定该页所剩余的地址空间是多少n
	n=psize-(uint16_t)(WriteAddress%psize);
//	if(n>=length) //判断该页是否够写，够的话就直接写入
//	{
//	    return I2C_WritePageByte(I2Cx, DeviceAddress,WriteAddress,buf,length);
//	}
//	else 
//	{
//		/*写入剩余部分*/
//		if(I2C_WritePageByte(I2Cx, DeviceAddress,WriteAddress,buf,n)!=C_OK)	
//			return C_EEPROMWRITEERR;
//
//		length-=n;
//		WriteAddress+=n;
//		buf+=n;
//		delay_ms(25);
//		/*满一页一页的写*/
//		while(length>=psize)
//		{
//		    if(I2C_WritePageByte(I2Cx, DeviceAddress,WriteAddress,buf,psize)!=C_OK)	
//				return C_EEPROMWRITEERR;
//				    
//			length-=psize;
//			WriteAddress+=psize;
//			buf+=psize;
//			delay_ms(25);
//		}
//		/*写入最后剩余部分*/
//		if(length)
//		{
//			if(I2C_WritePageByte(I2Cx, DeviceAddress,WriteAddress,buf,length)!=C_OK)	
//				return C_EEPROMWRITEERR;
//		}
//		delay_ms(25);
//		return 	C_OK;		
//	}
	while(1)
	{
	 	if(length<=n)   //如果剩余的空间足够大，那么就直接写。
		{
			if(I2C_WritePageByte(I2Cx, DeviceAddress,WriteAddress,buf,length)!=C_OK)
			{
				return C_EEPROMWRITEERR;
			}
			return 	C_OK;
		}

	 	else            //如果剩余的空间不够大，那么就写剩余的空间；
	    {
			if(I2C_WritePageByte(I2Cx, DeviceAddress,WriteAddress,buf,n)!=C_OK)
			{
				 return C_EEPROMWRITEERR;
			}
		}
	    WriteAddress=WriteAddress+n;  //第二页要开始的地址这个地址已经对齐。
	 	buf = buf+n;        //写了n个字节，所以也要相应的加 n
		length =length-n;  //每写一页就减写了多少个字节。
		n=psize; //这个时候可以写64个字节，也就是一页。	
	 }
}
