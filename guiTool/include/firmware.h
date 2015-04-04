#pragma once

#include "types.h"

struct header {
	u16	part3offset;
	u16	part4offset;
	u16	part34crc;
	u16	part12crc;
	u8 identifier[4];
	u16	part1offset;
	u16	part1ramAddress;
	u16	part2offset;
	u16	part2ramAddress;
	u16	shift;
	u16	part5offset;
	
	u8 timestamp[5];
	u8 console;
	u16	unused1;
	u16	userSettingsOffset;
	u16	unknown1;
	u16	unknown2;
	u16	part5crc;
	u16	unused2;
};
