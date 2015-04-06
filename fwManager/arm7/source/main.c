#include <stdlib.h>
#include <nds.h>
#include <dswifi7.h>
#include <maxmod7.h>

#include "interrupts.h"
#include "nvram.h"

static void wait(void) {
	// without any delay at all, it will not flash correctly
	// but 5 vblanks may not be necessary
	// reduce this number at your own risk
	
	int i;
	for(i = 0; i < 5; i++) swiWaitForVBlank();
}

void flash(unsigned char *firmware) {
	unsigned int i = 0;
	while(i < 0x3f800) {
		if(writeFirmwarePage(i, firmware + i)) {
			wait();
		}
		else {
			i += 256;
			fifoSendValue32(FIFO_USER_02, i);
			wait();
		}
	}
}

int main(void) {
	readUserSettings();
	
	irqInit();
	initClockIRQ();
	fifoInit();
	
	mmInstall(FIFO_MAXMOD);
	
	SetYtrigger(80);
	
	installWifiFIFO();
	installSoundFIFO();
	
	installSystemFIFO();
	
	setInterrupts();
	
	while(!quit) {
		if(fifoCheckValue32(FIFO_USER_01)) {
			unsigned char *firmware = (unsigned char *)fifoGetValue32(FIFO_USER_01);
			flash(firmware);
		}
		
		if(fifoCheckValue32(FIFO_USER_03)) {
			unsigned int address = fifoGetValue32(FIFO_USER_03);
			unsigned char *destination = (unsigned char *)fifoGetValue32(FIFO_USER_03);
			size_t length = fifoGetValue32(FIFO_USER_03);
			
			readFirmware(address, destination, length);
		}
		
		swiIntrWait(1, IRQ_FIFO_NOT_EMPTY | IRQ_VBLANK);
	}
	return 0;
}
