#pragma once

#include <nds.h>

void readFirmware(u32 address, void *destination, u32 size);
int writeFirmwarePage(u32 address, u8 *buffer);
