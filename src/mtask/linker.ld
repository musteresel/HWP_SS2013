MEMORY
{
	FLASH (rx): ORIGIN = 0x0000, LENGTH = 0x6FFF /* Assuming max bootl. size */
	ISRAM (w) : ORIGIN = 0x800200, LENGTH = 0x2000 /* Internal sram */
	ESRAM (w) : ORIGIN = 0x802200, LENGTH = 0xDE00 /* External sram */
	EEPROM (w): ORIGIN = 0x810000, LENGTH = 0x1000 /* EEPROM */
}


