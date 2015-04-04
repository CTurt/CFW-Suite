#include <stdlib.h>
#include <string.h>

#include "bitstream.h"

void bitstream_clear(BITSTREAM *bs) {
	memset(bs, 0, sizeof(BITSTREAM));
}

void bitstream_write(BITSTREAM *bs, u32 size, u32 data) {
	u32 i;
	
	for(i = 0; i < size; i++) {
		bs->byte = (u8)((bs->byte << 1) | ((data >> (size - 1 - i)) & 1));
		bs->bit++;
		if(bs->bit == 8) {
			*(bs->ptr + bs->pos) = bs->byte;
			bs->pos++;
			bs->bit = 0;
		}
	}
}

u32 bitstream_read(BITSTREAM *bs, u32 size) {
	u32 i, result = 0;
	
	for(i = 0; i < size; i++) {
		if(bs->bit == 0) {
			bs->byte = *(bs->ptr + bs->pos);
			bs->pos++;
			bs->bit = 8;
		}
		
		bs->bit--;
		result = (result << 1) | ((bs->byte >> bs->bit) & 1);
	}
	
	return result;
}

u32 bitstream_peek(BITSTREAM *bs, u32 size) {
	u32 value;
	BITSTREAM save;
	
	save = *bs;
	value = bitstream_read( bs, size);
	*bs = save;
	return value;
}
