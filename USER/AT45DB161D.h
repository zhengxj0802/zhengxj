#ifndef AT45DB161D_H 
#define AT45DB161D_H

#define FLASH_PAGE_SIZE 	528

#if 0

#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(GPIOC, GPIO_Pin_7)
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(GPIOC, GPIO_Pin_7)

#else

#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(GPIOA, GPIO_Pin_4)

#endif

void AT45DB161D_Configuration(void);
void Write_AT45DB161D(unsigned int PageAddress,unsigned int RowAddress,unsigned int size,unsigned char *buffer,unsigned char mode);
void WritePage_AT45DB161D(unsigned int PageAddress,unsigned int RowAddress,unsigned int size,unsigned char *buffer,unsigned char mode) ;
void Writebuf_AT45DB161D(unsigned int address,unsigned int size,unsigned char *buffer,unsigned char mode);
void BufWritePage(unsigned int PageAddress,unsigned int RowAddress,unsigned char mode);
void Read_AT45DB161D(unsigned int PageAddress,unsigned int RowAddress,unsigned int size,unsigned char *buffer);
void ReadPage_AT45DB161D(unsigned int PageAddress,unsigned int RowAddress,unsigned int size,unsigned char *buffer);
void Readbuf_AT45DB161D(unsigned int address,unsigned int size,unsigned char *buffer,unsigned char mode);
unsigned char spi_write(unsigned char data);
void EraseChip(void);
#endif
