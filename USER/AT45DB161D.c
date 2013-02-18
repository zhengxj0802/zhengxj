#include "stm32f10x.h"
#include "AT45DB161D.h"
#include "systick.h"
/******************************************************************************/
/*at45db161d初始化*/
void AT45DB161D_Configuration(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	//SPI1在APB2桥上
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE) ;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
								  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	SPI_FLASH_CS_HIGH();
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE); 
}
/*从SPI口输出一字节数据*/
unsigned char spi_write(unsigned char data)
{
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, data);
	
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);
}
/*at45db161d写操作*/
void Write_AT45DB161D(unsigned int PageAddress,unsigned int RowAddress,unsigned int size,unsigned char *buffer,unsigned char mode)
{
	if(FLASH_PAGE_SIZE-RowAddress>=size)//所写的数据从开始列地址起不超过一页
	{
		WritePage_AT45DB161D(PageAddress,RowAddress,size,buffer,mode);
	}
	else//所写的数据从开始列地址起超过一页
	{
		WritePage_AT45DB161D(PageAddress,RowAddress,FLASH_PAGE_SIZE-RowAddress,buffer,mode); //先写满页
		buffer+=(FLASH_PAGE_SIZE-RowAddress); //数据位后移
		PageAddress++;	 //页地址自加1
		size-=(FLASH_PAGE_SIZE-RowAddress);	//计算剩下数据的字节数
		
		while(size>=FLASH_PAGE_SIZE) //所剩数据的字节数大于每页最多可写的字节数
		{
			WritePage_AT45DB161D(PageAddress,0,FLASH_PAGE_SIZE,buffer,mode); //先写满页
			buffer+=FLASH_PAGE_SIZE;	//数据位后移
			PageAddress++;				//页地址自加1
			size-=FLASH_PAGE_SIZE;		//计算剩下数据的字节数
		}
		
		if(size>0)
			WritePage_AT45DB161D(PageAddress,0,size,buffer,mode); //写剩余的字节数
	}			
}
/*at45db161d写主存储页操作*/
void WritePage_AT45DB161D(unsigned int PageAddress,unsigned int RowAddress,unsigned int size,unsigned char *buffer,unsigned char mode) 
{
	unsigned int i=0;

	SPI_FLASH_CS_LOW();
	
	if(mode==1)// 缓冲1
	{
		spi_write(0x82);
	}
	else if(mode==2)// 缓冲2
	{
		spi_write(0x85);
	} 

	spi_write((unsigned char)(PageAddress>>6));
	spi_write((unsigned char)(PageAddress<<2)|(RowAddress>>8));
	spi_write((unsigned char)RowAddress); 
	 
	for(i=0;i<size;i++) 
		spi_write(buffer[i]);

	SPI_FLASH_CS_HIGH();
	delay_ms(11);//延时，等待写完	

//	Writebuf_AT45DB161D(0,size,buffer,mode);//将数据写入到缓存区第0开始地址上
//	BufWritePage(PageAddress,RowAddress,mode);//将缓冲上中的数据写入到主存储页中
}
/*at45db161d写缓冲区*/
void Writebuf_AT45DB161D(unsigned int address,unsigned int size,unsigned char *buffer,unsigned char mode)
{
	unsigned int i=0;

	SPI_FLASH_CS_LOW();
	
	if(mode==1)// 缓冲1
	{
		spi_write(0x84);
	}
	else if(mode==2)// 缓冲2
	{
		spi_write(0x87);
	} 
	spi_write(0x00);
	spi_write(0x00);
	spi_write((unsigned char)address); 

	for(i=0;i<size;i++) 
		spi_write(buffer[i]);

	SPI_FLASH_CS_HIGH();
	delay_ms(11);//延时，等待写完
}
/*将AT45DB161D缓冲区中的数据 写入到主存储页*/
void BufWritePage(unsigned int PageAddress,unsigned int RowAddress,unsigned char mode)
{
	SPI_FLASH_CS_LOW();
	
	if(mode==1)// 缓冲1
	{
		spi_write(0x83);
	}
	else if(mode==2)// 缓冲2
	{
		spi_write(0x86);
	} 

	spi_write((unsigned char)(PageAddress>>6));
	spi_write((unsigned char)(PageAddress<<2)|(RowAddress>>8));
	spi_write((unsigned char)RowAddress); 

	SPI_FLASH_CS_HIGH();
	delay_ms(11);//延时，等待写完	
}
/*at45db161d读操作*/
void Read_AT45DB161D(unsigned int PageAddress,unsigned int RowAddress,unsigned int size,unsigned char *buffer)
{
	if(FLASH_PAGE_SIZE-RowAddress>=size)//所读的数据从开始列地址起不超过一页
	{
		ReadPage_AT45DB161D(PageAddress,RowAddress,size,buffer);
	}
	else//所读的数据从开始列地址起超过一页
	{
		ReadPage_AT45DB161D(PageAddress,RowAddress,FLASH_PAGE_SIZE-RowAddress,buffer); //先读满页
		buffer+=(FLASH_PAGE_SIZE-RowAddress); //数据位后移
		PageAddress++;	 //页地址自加1
		size-=(FLASH_PAGE_SIZE-RowAddress);	//计算剩下数据的字节数
		
		while(size>=FLASH_PAGE_SIZE) //所剩数据的字节数大于每页最多的字节数
		{
			ReadPage_AT45DB161D(PageAddress,0,FLASH_PAGE_SIZE,buffer); //先读满页
			buffer+=FLASH_PAGE_SIZE;	//数据位后移
			PageAddress++;				//页地址自加1
			size-=FLASH_PAGE_SIZE;		//计算剩下数据的字节数
		}
		
		if(size>0)
			ReadPage_AT45DB161D(PageAddress,0,size,buffer); //读剩余的字节数
	}	
}
/*at45db161d读主存储页操作*/
void ReadPage_AT45DB161D(unsigned int PageAddress,unsigned int RowAddress,unsigned int size,unsigned char *buffer)
{
	unsigned int i=0;

	SPI_FLASH_CS_LOW();
	
	spi_write(0xd2);

	spi_write((unsigned char)(PageAddress>>6));
	spi_write((unsigned char)(PageAddress<<2)|(RowAddress>>8));
	spi_write((unsigned char)RowAddress); 

	spi_write(0x00); //don't care bytes
	spi_write(0x00);
	spi_write(0x00);
	spi_write(0x00);

	for(i=0;i<size;i++) 
		buffer[i]=spi_write(0x00);
	
	SPI_FLASH_CS_HIGH();
}
/*at45db161d读缓冲区*/
void Readbuf_AT45DB161D(unsigned int address,unsigned int size,unsigned char *buffer,unsigned char mode)
{
	unsigned int i=0;

	SPI_FLASH_CS_LOW();
	
	if(mode==1)// 缓冲1
	{
		spi_write(0xd4);
	}
	else if(mode==2)// 缓冲2
	{
		spi_write(0xd6);
	} 
	spi_write(0x00);
	spi_write((unsigned char)(address>>8));
	spi_write((unsigned char)address); 
	spi_write(0x00);

	for(i=0;i<size;i++) 
		buffer[i]=spi_write(0x00);

	SPI_FLASH_CS_HIGH();
}
void EraseChip(void)
{
   SPI_FLASH_CS_LOW();
   spi_write(0xc7);
   spi_write(0x94);
   spi_write(0x80);
   spi_write(0x9a);
   SPI_FLASH_CS_HIGH();
   delay_ms(1000);
}
