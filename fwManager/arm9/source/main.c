#include <stdio.h>
#include <stdlib.h>
#include <nds.h>
#include <fat.h>

#include "firmware.h"
#include "fileSelector.h"

#include "main.h"

PrintConsole *console;

static unsigned char flashing = 0;

void firmwareRead(unsigned int address, unsigned char *destination, size_t length) {
	fifoSendValue32(FIFO_USER_03, address);
	fifoSendValue32(FIFO_USER_03, (u32)destination);
	fifoSendValue32(FIFO_USER_03, length);
}

void startFlash(unsigned char *firmware) {
	consoleClear();
	printf("\n Flashing firmware!\n\n Keep SL1 terminal shorted to\n progress.\n");
	flashing = 1;
	fifoSendValue32(FIFO_USER_01, (u32)firmware);
}

int main(void) {
	console = consoleDemoInit();
	
	printf("\n fwManager - CTurt\n");
	printf(" =================\n\n");
	
	printf(" Warning!\n This tool may damage your\n system! Use at your own risk!\n\n");
	
	if(!fatInitDefault()) {
		printf(" Could not init FAT!\n");
	}
	
	char *firmwareFilename = selectFirmware();
	
	if(!firmwareFilename) {
		while(1) swiWaitForVBlank();
	}
	
	consoleClear();
	
	printf("\n Press Start to flash:\n\n %s\n\n", firmwareFilename);
	
	do {
		scanKeys();
		swiWaitForVBlank();
	} while(keysHeld() & KEY_START);
	
	do {
		scanKeys();
		swiWaitForVBlank();
	} while(!(keysHeld() & KEY_START));
	
	do {
		scanKeys();
		swiWaitForVBlank();
	} while(keysHeld() & KEY_START);
	
	FILE *f = fopen(firmwareFilename, "rb");
	if(!f) {
		printf(" Could not open file!\n");
		while(1) swiWaitForVBlank();
	}
	
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	if(size < 0x3fe00) {
		printf(" Firmware is too small!\n");
		while(1) swiWaitForVBlank();
	}
	
	unsigned char *firmware = malloc(size);
	if(!firmware) {
		printf(" Malloc failed!\n");
		while(1) swiWaitForVBlank();
	}
	
	rewind(f);
	fread(firmware, size, 1, f);
	fclose(f);
	
	// To do: check console is compatible
	/*
	unsigned char system = readPM(4);
	if(system != ((struct header *)firmware)->console) {
		printf(" This firmware is not for this console!\n");
		printf(" This console is type %d (%s)\n firmware is for type %d (%s)\n", system, system & 0x40 == 0x40 ? "lite" : "phat", ((struct header *)firmware)->console, ((struct header *)firmware)->console & 0x40 == 0x40 ? "lite" : "phat");
		while(1) swiWaitForVBlank();
	}
	*/
	
	// To do: check boot CRC is correct
	/*
	unsigned short crc;
	// decrypt and decompress arm 7 and 9 binaries
	crc = swiCRC16(0xffff, part1, part1size);
	crc = swiCRC16(crc, part2, part2size);
	if(((struct header *)firmware)->part12crc != crc) {
		printf(" Incorrect boot CRC!\n");
		while(1) swiWaitForVBlank();
	}
	*/
	
	unsigned char *originalFirmware = malloc(0x40000);
	firmwareRead(0, originalFirmware, 0x40000);
	
	unsigned short crc = *(u16 *)(originalFirmware + 0x17e);
	if(crc == 0xffff) crc = *(u16 *)(originalFirmware + 6);
	
	int i;
	for(i = 0; i < (0x170 - 0x28); i++) {
		firmware[i + 0x28] = firmware[i + 0x3f680 + 0x28] = originalFirmware[i + 0x28];
	}
	
	*(u16 *)(firmware + 0x17e) = *(u16 *)(firmware + 0x3f680 + 0x17e) = crc;
	
	free(originalFirmware);
	
	startFlash(firmware);
	
	while(1) {
		if(flashing) {
			if(fifoCheckValue32(FIFO_USER_02)) {
				unsigned int progress = fifoGetValue32(FIFO_USER_02);
				
				console->cursorX = 1;
				console->cursorY = 6;
				printf("Progress: %d%%", (int)((double)progress / 0x3f800 * 100));
				//printf("Progress: %d / %d", progress, 0x3f800);
				
				if(progress == 0x3f800) {
					printf("\n\n Done!\n");
					flashing = 0;
				}
			}
		}
		
		swiWaitForVBlank();
		scanKeys();
	}
	
	free(firmware);
	
	return 0;
}
