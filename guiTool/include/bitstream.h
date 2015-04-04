#pragma once

#include "types.h"

typedef struct _BITSTREAM BITSTREAM;

struct _BITSTREAM {
	u8 *ptr;
	u8 bit;
	u8 byte;
	u32 pos;
};

void bitstream_clear(BITSTREAM *bs);
void bitstream_write(BITSTREAM *bs, u32 size, u32 data);
u32  bitstream_read(BITSTREAM *bs, u32 size);
u32  bitstream_peek(BITSTREAM *bs, u32 size);
