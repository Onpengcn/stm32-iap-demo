#include "stm32f1xx_hal.h"
#define STM32_SECTOR_SIZE	2048	//页大小
#define STM32_SECTOR_NUM	255		//页数

//STM32 FLASH的起始地址
#define STM32_FLASH_BASE 0x08000000
#define STM32_FLASH_END  0x08010000

//读1个字节
uint8_t FLASH_ReadByte(uint32_t Addr)
{
	return *(volatile uint8_t *)Addr;
}
//读2个字节
uint16_t FLASH_ReadHalfWord(uint32_t Addr)
{
	return *(volatile uint8_t *)Addr;
}
//读N个字节
void FLASH_ReadNByte(uint32_t Addr,uint8_t *pBuff,uint32_t Len)
{
	uint32_t i;
	
	for(i = 0;i < Len;i++)
	{
		pBuff[i] = FLASH_ReadByte(Addr);
		Addr += 1;
	}
}

void FLASH_WriteNByte(uint32_t Addr,uint8_t *pBuff,uint32_t Len)
{
	uint16_t i;
	uint16_t temp = 0;
	
	if((Addr < STM32_FLASH_BASE) || (Addr > STM32_FLASH_END))
		return;
	
	HAL_FLASH_Unlock();//解锁
	
	for(i = 0;i < Len;i += 2)
	{
		temp = pBuff[i];
		temp |= (uint16_t)pBuff[i+1] << 8;
		
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Addr, temp);
		Addr += 2;
		if(Addr > STM32_FLASH_END)
		{
			HAL_FLASH_Lock();
			return;
		}
	}
	HAL_FLASH_Lock();
}
void FLASH_WritePage(uint8_t Page_Num,uint8_t *pBuff)
{
	uint16_t i;
	uint16_t Buff;
	uint32_t Addr;
	
	//是否超出范围
	if(Page_Num > STM32_SECTOR_NUM)
		return;
	//先计算页首地址
	Addr = Page_Num * STM32_SECTOR_SIZE + STM32_FLASH_BASE;
	
	for(i = 0;i < STM32_SECTOR_SIZE ;i += 2)
	{
		Buff = ((uint16_t)pBuff[i+1] << 8) | pBuff[i];
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Addr, Buff);
		Addr += 2;
	}
}

void FLASH_WriteNDataBypage(uint32_t Addr,uint8_t *pBuff,uint32_t Len)
{
	uint32_t Offset;
	uint32_t error;
	uint8_t  PageStartNum;
	uint8_t  PageNum;
	uint16_t i;
	FLASH_EraseInitTypeDef erase_config;
	if((Addr < STM32_FLASH_BASE) || (Addr > STM32_FLASH_END))
		return;
	
	Offset = Addr - STM32_FLASH_BASE;//偏移地址
	PageNum = (Len / STM32_SECTOR_SIZE) + ((Len % STM32_SECTOR_SIZE) ? 1 : 0); 
	PageStartNum = Offset / STM32_SECTOR_SIZE;//得到地址所在页
	// 定义要擦除的扇区
	erase_config.TypeErase = FLASH_TYPEERASE_PAGES;
	erase_config.Banks = FLASH_BANK_1;
	erase_config.PageAddress = PageStartNum * STM32_SECTOR_SIZE + STM32_FLASH_BASE;
	erase_config.NbPages = PageNum;
	HAL_FLASH_Unlock();//解锁
	
	// 擦除扇区
	HAL_FLASHEx_Erase(&erase_config, &error);
	for (i = 0; i < PageNum; i++)
	{
		FLASH_WritePage((PageStartNum + i), pBuff);//把缓存数据写入
		pBuff += STM32_SECTOR_SIZE;
	}
	HAL_FLASH_Lock();
}
