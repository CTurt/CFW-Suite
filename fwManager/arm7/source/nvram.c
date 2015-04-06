// This code is from libnds, but for some reason it isn't exposed to us

#include <nds/arm7/serial.h>
#include <nds/interrupts.h>
#include <nds/system.h>

#include "nvram.h"

static u8 readwriteSPI(u8 data) {
	REG_SPIDATA = data;
	SerialWaitBusy();
	return REG_SPIDATA;
}

int writeFirmwarePage(u32 address,u8 *buffer) {
	int i;
	u8 pagebuffer[256];
	readFirmware(address, pagebuffer, 256);
	
	if(memcmp(pagebuffer, buffer, 256) == 0) return 0;
	
	int oldIME = enterCriticalSection();
	
	//write enable
	REG_SPICNT = SPI_ENABLE | SPI_CONTINUOUS | SPI_DEVICE_NVRAM;
	readwriteSPI(FIRMWARE_WREN);
	REG_SPICNT = 0;
	
	//Wait for Write Enable Latch to be set
	REG_SPICNT = SPI_ENABLE | SPI_CONTINUOUS | SPI_DEVICE_NVRAM;
	readwriteSPI(FIRMWARE_RDSR);
	while((readwriteSPI(0) & 0x02) == 0); //Write Enable Latch
	REG_SPICNT = 0;
	
	//page write
	REG_SPICNT = SPI_ENABLE | SPI_CONTINUOUS | SPI_DEVICE_NVRAM;
	readwriteSPI(FIRMWARE_PW);
	// Set the address
	readwriteSPI((address>>16) & 0xff);
	readwriteSPI((address>> 8) & 0xff);
	readwriteSPI((address) & 0xff);
	
	for(i = 0; i < 256; i++) {
		readwriteSPI(buffer[i]);
	}
	
	REG_SPICNT = 0;
	
	// wait for programming to finish
	REG_SPICNT = SPI_ENABLE|SPI_CONTINUOUS|SPI_DEVICE_NVRAM;
	readwriteSPI(FIRMWARE_RDSR);
	while(readwriteSPI(0) & 0x01); //Write In Progress
	REG_SPICNT = 0;
	
	leaveCriticalSection(oldIME);
	
	// read it back & verify
	readFirmware(address, pagebuffer, 256);
	if(memcmp(pagebuffer, buffer, 256) == 0) return 0;
	return -1;
}
