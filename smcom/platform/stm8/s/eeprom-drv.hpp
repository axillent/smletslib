#ifndef STM8S_EEPROM_DRV
#define STM8S_EEPROM_DRV


#define FLASH                       ((FLASH_TypeDef *) FLASH_BASE)
#define FLASH_BASE                  (uint16_t)0x5050

#define FLASH_RASS_KEY1             ((uint8_t)0x56)        /*!< First RASS key */
#define FLASH_RASS_KEY2             ((uint8_t)0xAE)        /*!< Second RASS key */
#define FLASH_CLEAR_BYTE            ((uint8_t)0x00)
#define FLASH_SET_BYTE              ((uint8_t)0xFF)
#define EEPROM_START_ADDRESS        ((uint16_t)0x4000)     /* Address of start EEPROM */
#define EEPROM_END_ADDRESS          ((uint16_t)0x4080)     /* Address of end EEPROM */


 #define PAttr __far

void EEPROM_Unlock()
{
  /* Unlock data memory */
    FLASH_DUKR = FLASH_RASS_KEY2; /* Warning: keys are reversed on data memory !!! */
    FLASH_DUKR = FLASH_RASS_KEY1;
}

void EEPROM_Lock()
{
	FLASH_IAPSR_bit.DUL = 0;
}

int __eeprom_wait_for_last_operation(void)
{
  return 1;
}

void __eeprom_program_byte(unsigned char __near * dst, unsigned char v)
{
	EEPROM_Unlock();

	while(FLASH_IAPSR_bit.DUL == 0);

	*(__near uint8_t*)dst = v;
	EEPROM_Lock();
}

void __eeprom_program_long(unsigned char __near * dst, unsigned long v)
{
	EEPROM_Unlock();
	FLASH_CR2_bit.WPRG = 1;
	FLASH_NCR2_bit.NWPRG = 0;

	while(FLASH_IAPSR_bit.DUL == 0);

	*(__near uint8_t*)(dst) = *((uint8_t*)(&v));
	*(__near uint8_t*)(dst + 1) = *((uint8_t*)(&v)+1);
	*(__near uint8_t*)(dst + 2) = *((uint8_t*)(&v)+2);;
	*(__near uint8_t*)(dst + 3) = *((uint8_t*)(&v)+3);;

	EEPROM_Lock();
}

#endif