#include <nds.h>
#include <dswifi7.h>
#include <maxmod7.h>

volatile unsigned char quit = 0;

static void VblankHandler(void) {
	Wifi_Update();
}

static void VcountHandler(void) {
	inputGetAndSend();
}

static void powerButtonCB(void) {
	quit = true;
}

void setInterrupts(void) {
	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);
	
	irqEnable(IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);
	
	setPowerButtonCB(powerButtonCB);
}
