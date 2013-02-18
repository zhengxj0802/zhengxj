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
/*********************************ģ��I2C1**************************************/
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
	if(!SDA1_read)return !C_OK;	/* SDA��Ϊ�͵�ƽ������æ,�˳� */
	SDA1_L;
	I2C_delay();
	if(SDA1_read) return !C_OK;	/* SDA��Ϊ�ߵ�ƽ�����߳���,�˳� */
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
* Return         : ����Ϊ:=1��ACK,=0��ACK
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
* Description    : дһ���ֽڣ����ݴӸ�λ����λ
* Input          : - SendByte: ���͵�����
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
* Description    : ��һ���ֽڣ����ݴӸ�λ����λ
* Input          : None
* Output         : None
* Return         : I2C���߷��ص�����
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
/*дһҳ������*/
u8 I2C1_WritePage(uint8_t* pBuffer, uint16_t length,uint16_t WriteAddress, uint8_t DeviceAddress)
{
	uint16_t i=0;
	if(I2C1_Start())return C_EEPROMWRITEERR;//����I2C����
	I2C1_SendByte(((0xA0|DeviceAddress)& 0xFFFE));//����������ַ
	if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMWRITEERR;}
	I2C1_SendByte((WriteAddress & 0x0700) >>8) ;//���͸���ʼ��ַ
	if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMWRITEERR;}
	I2C1_SendByte((uint8_t)(WriteAddress & 0x00FF));//���͵���ʼ��ַ
	if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMWRITEERR;}
	for(i=0;i<length;i++)
	{
		I2C1_SendByte(pBuffer[i]); //д����
		if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMWRITEERR;}
	}
	I2C1_Stop(); 
	delay_ms(10);//��ʱ�ȴ�д�꣬10ms
    return C_OK;
}
/*д����ֽ�*/									 
u8 I2C1_WriteNByte(uint8_t* pBuffer, uint16_t length,uint16_t WriteAddress, uint8_t DeviceAddress)
{
	uint16_t n=0;
	uint16_t psize=32;

	//ȷ��Ҫд�����ݵ�λ������һҳ�ϵ���һ���ϣ�WriteAddress%psize��
	//��ȷ����ҳ��ʣ��ĵ�ַ�ռ��Ƕ���n
	n=psize-(uint16_t)(WriteAddress%psize);
	if(n>=length) //�жϸ�ҳ�Ƿ�д�����Ļ���ֱ��д��
	{
		return(I2C1_WritePage(pBuffer,length,WriteAddress,DeviceAddress));
	}
	else
	{
		/*д��ʣ�ಿ��*/
		if(I2C1_WritePage(pBuffer,n,WriteAddress,DeviceAddress)!=C_OK)	return C_EEPROMWRITEERR;
		length-=n;
		WriteAddress+=n;
		pBuffer+=n;
		delay_ms(25);
		/*��һҳһҳ��д*/
		while(length>=psize)
		{
			if(I2C1_WritePage(pBuffer,psize,WriteAddress,DeviceAddress)!=C_OK) return C_EEPROMWRITEERR;	
			length-=n;
			WriteAddress+=n;
			pBuffer+=n;
			delay_ms(25);
		}
		/*д�����ʣ�ಿ��*/
		if(length)
		{
			if(I2C1_WritePage(pBuffer,length,WriteAddress,DeviceAddress)!=C_OK)	return C_EEPROMWRITEERR;
		}
		delay_ms(25);
		return 	C_OK;		
	}
	//return C_EEPROMWRITEERR;
}

/*��ȡһ������*/          
u8 I2C1_ReadNByte(uint8_t* pBuffer,   uint16_t length,   uint16_t ReadAddress,  uint8_t DeviceAddress)
{		
    if(I2C1_Start())return C_EEPROMREADEER;
	I2C1_SendByte((0xA0|DeviceAddress) & 0xFFFE);//����������ַ
	if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMREADEER;}
	I2C1_SendByte((ReadAddress & 0x0700) >>8) ;//���͸���ʼ��ַ
	if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMREADEER;}
	I2C1_SendByte((uint8_t)(ReadAddress & 0x00FF));//���͵���ʼ��ַ
	if(I2C1_WaitAck()){I2C1_Stop(); return C_EEPROMREADEER;}

   	if(I2C1_Start())return C_EEPROMREADEER;
	I2C1_SendByte(0xA1|DeviceAddress);//����������ַ
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
/*********************************ģ��I2C2**************************************/
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
	if(!SDA2_read)return !C_OK;	/* SDA��Ϊ�͵�ƽ������æ,�˳� */
	SDA2_L;
	I2C_delay();
	if(SDA2_read) return !C_OK;	/* SDA��Ϊ�ߵ�ƽ�����߳���,�˳� */
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
* Return         : ����Ϊ:=1��ACK,=0��ACK
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
* Description    : дһ���ֽڣ����ݴӸ�λ����λ
* Input          : - SendByte: ���͵�����
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
* Description    : ��һ���ֽڣ����ݴӸ�λ����λ
* Input          : None
* Output         : None
* Return         : I2C���߷��ص�����
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
/*дһҳ������*/
u8 I2C2_WritePage(uint8_t* pBuffer, uint16_t length,uint16_t WriteAddress, uint8_t DeviceAddress)
{
	uint16_t i=0;
	if(I2C2_Start())return C_EEPROMWRITEERR;//����I2C����
	I2C2_SendByte(((0xA0|DeviceAddress)& 0xFFFE));//����������ַ
	if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMWRITEERR;}
	I2C2_SendByte((WriteAddress & 0x0700) >>8) ;//���͸���ʼ��ַ
	if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMWRITEERR;}
	I2C2_SendByte((uint8_t)(WriteAddress & 0x00FF));//���͵���ʼ��ַ
	if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMWRITEERR;}
	for(i=0;i<length;i++)
	{
		I2C2_SendByte(pBuffer[i]); //д����
		if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMWRITEERR;}
	}
	I2C2_Stop(); 
	delay_ms(10);//��ʱ�ȴ�д�꣬10ms
    return C_OK;
}
/*д����ֽ�*/									 
u8 I2C2_WriteNByte(uint8_t* pBuffer, uint16_t length,uint16_t WriteAddress, uint8_t DeviceAddress)
{
	uint16_t n=0;
	uint16_t psize=32;

	//ȷ��Ҫд�����ݵ�λ������һҳ�ϵ���һ���ϣ�WriteAddress%psize��
	//��ȷ����ҳ��ʣ��ĵ�ַ�ռ��Ƕ���n
	n=psize-(uint16_t)(WriteAddress%psize);
	if(n>=length) //�жϸ�ҳ�Ƿ�д�����Ļ���ֱ��д��
	{
		return(I2C2_WritePage(pBuffer,length,WriteAddress,DeviceAddress));
	}
	else
	{
		/*д��ʣ�ಿ��*/
		if(I2C2_WritePage(pBuffer,n,WriteAddress,DeviceAddress)!=C_OK)	return C_EEPROMWRITEERR;
		length-=n;
		WriteAddress+=n;
		pBuffer+=n;
		delay_ms(25);
		/*��һҳһҳ��д*/
		while(length>=psize)
		{
			if(I2C2_WritePage(pBuffer,psize,WriteAddress,DeviceAddress)!=C_OK) return C_EEPROMWRITEERR;	
			length-=n;
			WriteAddress+=n;
			pBuffer+=n;
			delay_ms(25);
		}
		/*д�����ʣ�ಿ��*/
		if(length)
		{
			if(I2C2_WritePage(pBuffer,length,WriteAddress,DeviceAddress)!=C_OK)	return C_EEPROMWRITEERR;
		}
		delay_ms(25);
		return 	C_OK;		
	}
	//return C_EEPROMWRITEERR;
}

/*��ȡһ������*/          
u8 I2C2_ReadNByte(uint8_t* pBuffer,   uint16_t length,   uint16_t ReadAddress,  uint8_t DeviceAddress)
{		
    if(I2C2_Start())return C_EEPROMREADEER;
	I2C2_SendByte((0xA0|DeviceAddress) & 0xFFFE);//����������ַ
	if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMREADEER;}
	I2C2_SendByte((ReadAddress & 0x0700) >>8) ;//���͸���ʼ��ַ
	if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMREADEER;}
	I2C2_SendByte((uint8_t)(ReadAddress & 0x00FF));//���͵���ʼ��ַ
	if(I2C2_WaitAck()){I2C2_Stop(); return C_EEPROMREADEER;}

   	if(I2C2_Start())return C_EEPROMREADEER;
	I2C2_SendByte(0xA1|DeviceAddress);//����������ַ
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
/*********************************Ӳ��I2C**************************************/
/******************************************************************************/
/******************************************************************************/

/*******************************************************************************
* Function Name  : I2C_AcknowledgePolling
* Description    : �ȴ���ȡI2C���߿���Ȩ �ж�æ״̬
* Input          : - I2Cx:I2C�Ĵ�����ַ
*                  - I2C_Addr:��������ַ
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_AcknowledgePolling(I2C_TypeDef *I2Cx,uint8_t I2C_Addr)
{
	vu16 SR1_Tmp;
	do
	{   
		I2C_GenerateSTART(I2Cx, ENABLE); /*��ʼλ*/
		/*��SR1*/
		SR1_Tmp = I2C_ReadRegister(I2Cx, I2C_Register_SR1);
		/*������ַ(д)*/
		I2C_Send7bitAddress(I2Cx, I2C_Addr&0xfe, I2C_Direction_Transmitter);
	}while(!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002));
	
	I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
	I2C_GenerateSTOP(I2Cx, ENABLE);  /*ֹͣλ*/  
}
/*******************************************************************************
* Function Name  : I2C_Read
* Description    : ͨ��ָ��I2C�ӿڶ�ȡ����ֽ�����
* Input          : - I2Cx:I2C�Ĵ�����ַ
*                  - I2C_Addr:��������ַ
*                  - addr:Ԥ��ȡ�ֽڴ洢λ��
*                  - *buf:��ȡ���ݵĴ洢λ��
*                  - num:��ȡ�ֽ���
* Output         : None
* Return         : �ɹ�����0
* Attention		 : None
*******************************************************************************/
uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t DeviceAddress,uint8_t ReadAddress,uint8_t *buf,uint16_t length)
{
	if(length==0)
		return 1;
	
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	/*����1�ֽ�1Ӧ��ģʽ*/
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	/* ������ʼλ */
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
    /*����������ַ(д)*/
    I2C_Send7bitAddress(I2Cx,  (DeviceAddress&0xfe), I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	/*���͵�ַ*/
	I2C_SendData(I2Cx, (ReadAddress & 0x0700) >>8);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	/*���͵�ַ*/
	I2C_SendData(I2Cx,ReadAddress & 0x00FF);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	/*��ʼλ*/
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	/*������*/
	I2C_Send7bitAddress(I2Cx,  DeviceAddress, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	while(length)
	{ 
	  	if(length==1)
	    {
			I2C_AcknowledgeConfig(I2Cx, DISABLE);	/* ���һλ��Ҫ�ر�Ӧ��� */
	    	I2C_GenerateSTOP(I2Cx, ENABLE);	
		}
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED ));
      	*buf = I2C_ReceiveData(I2Cx);
	    buf++;
	    length--;
    }
	/* �ٴ�����Ӧ��ģʽ */
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	return 0;
}	
/*******************************************************************************
* Function Name  : I2C_WriteOneByte
* Description    : ͨ��ָ��I2C�ӿ�д��һ���ֽ�����
* Input          : - I2Cx:I2C�Ĵ�����ַ
*                  - I2C_Addr:��������ַ
*                  - addr:Ԥд���ֽڵ�ַ
*                  - value:д������
* Output         : None
* Return         : �ɹ�����0
* Attention		 : None
*******************************************************************************/
uint8_t I2C_WritePageByte(I2C_TypeDef *I2Cx,uint8_t DeviceAddress,uint8_t WriteAddress,uint8_t *buf,uint16_t length)
{
    /* ��ʼλ */
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
  	/* ����������ַ(д)*/
  	I2C_Send7bitAddress(I2Cx, DeviceAddress, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  	/*���͵�ַ*/
	I2C_SendData(I2Cx, (WriteAddress & 0x0700) >>8);//���͸���ʼ��ַ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2Cx, (WriteAddress & 0x00FF));	//���͵���ʼ��ַ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))	;
	/* ÿ��дһ���ֽ�*/
	while(length--)
	{
  		I2C_SendData(I2Cx, *buf); 
		buf++;
	    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	} 
  	/* ֹͣλ*/
  	I2C_GenerateSTOP(I2Cx, ENABLE);
	I2C_AcknowledgePolling(I2Cx,DeviceAddress);
	delay_ms(10);
	return 0;
}
/*******************************************************************************
* Function Name  : I2C_Write
* Description    : ͨ��ָ��I2C�ӿ�д�����ֽ�����
* Input          : - I2Cx:I2C�Ĵ�����ַ
*                  - I2C_Addr:��������ַ
*                  - addr:Ԥд���ֽڵ�ַ
*                  - *buf:Ԥд�����ݴ洢λ��
*                  - num:д���ֽ���
* Output         : None
* Return         : �ɹ�����0
* Attention		 : None
*******************************************************************************/
uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t DeviceAddress,uint8_t WriteAddress,uint8_t *buf,uint16_t length)
{

    uint16_t n=0;
	uint16_t psize=0;
	if((DeviceAddress==ICCARD_WRITE)||(DeviceAddress==ICCARD_READ))
	  psize = 32;	//IC��
	else
	  psize = 64;	//EEPROM
	 
	//ȷ��Ҫд�����ݵ�λ������һҳ�ϵ���һ���ϣ�WriteAddress%psize��
	//��ȷ����ҳ��ʣ��ĵ�ַ�ռ��Ƕ���n
	n=psize-(uint16_t)(WriteAddress%psize);
//	if(n>=length) //�жϸ�ҳ�Ƿ�д�����Ļ���ֱ��д��
//	{
//	    return I2C_WritePageByte(I2Cx, DeviceAddress,WriteAddress,buf,length);
//	}
//	else 
//	{
//		/*д��ʣ�ಿ��*/
//		if(I2C_WritePageByte(I2Cx, DeviceAddress,WriteAddress,buf,n)!=C_OK)	
//			return C_EEPROMWRITEERR;
//
//		length-=n;
//		WriteAddress+=n;
//		buf+=n;
//		delay_ms(25);
//		/*��һҳһҳ��д*/
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
//		/*д�����ʣ�ಿ��*/
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
	 	if(length<=n)   //���ʣ��Ŀռ��㹻����ô��ֱ��д��
		{
			if(I2C_WritePageByte(I2Cx, DeviceAddress,WriteAddress,buf,length)!=C_OK)
			{
				return C_EEPROMWRITEERR;
			}
			return 	C_OK;
		}

	 	else            //���ʣ��Ŀռ䲻������ô��дʣ��Ŀռ䣻
	    {
			if(I2C_WritePageByte(I2Cx, DeviceAddress,WriteAddress,buf,n)!=C_OK)
			{
				 return C_EEPROMWRITEERR;
			}
		}
	    WriteAddress=WriteAddress+n;  //�ڶ�ҳҪ��ʼ�ĵ�ַ�����ַ�Ѿ����롣
	 	buf = buf+n;        //д��n���ֽڣ�����ҲҪ��Ӧ�ļ� n
		length =length-n;  //ÿдһҳ�ͼ�д�˶��ٸ��ֽڡ�
		n=psize; //���ʱ�����д64���ֽڣ�Ҳ����һҳ��	
	 }
}
