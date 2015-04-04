#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "firmware.h"
#include "compression.h"
#include "crc.h"

static void help(char *name) {
	printf("DS firmware GUI data extractor and injector\n");
	printf("Usage:\n");
	printf("%s firmware.bin [-e or -i] gui.bin\n", name);
	printf(" firmware.bin: filename of firmware\n");
	printf(" -e: extract gui data\n");
	printf(" -i: inject gui data\n");
	printf(" gui.bin: output or input of gui data\n");
}

int main(int argc, char **argv) {
	enum {
		unspecified,
		extract,
		inject,
	} mode;
	
	unsigned char *firmware;
	
	char *firmwareFilename = NULL;
	char *guiFilename = NULL;
	
	if(argc != 4) {
		help(argv[0]);
		return 1;
	}
	
	firmwareFilename = argv[1];
	if(strcmp(argv[2], "-e") == 0) mode = extract;
	if(strcmp(argv[2], "-i") == 0) mode = inject;
	guiFilename = argv[3];
	
	if(mode == unspecified) {
		help(argv[0]);
		return 1;
	}
	
	FILE *f = fopen(firmwareFilename, "rb");
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	rewind(f);
	firmware = malloc(size);
	fread(firmware, size, 1, f);
	fclose(f);
	
	if(mode == extract) {
		unsigned int guiOffset;
		unsigned char *guiData;
		size_t guiSize;
		
		guiOffset = ((struct header *)firmware)->part5offset * 8;
		guiSize = decompress(NULL, firmware + guiOffset);
		guiData = malloc(guiSize);
		decompress(guiData, firmware + guiOffset);
		
		f = fopen(guiFilename, "wb");
		fwrite(guiData, guiSize, 1, f);
		fclose(f);
		
		printf("Decompressed gui data from %08x, size %d\n", guiOffset, guiSize);
		
		free(guiData);
	}
	
	else {
		unsigned char *gui, *compressed;
		size_t guiSize, compressedSize;
		
		unsigned char *firmware;
		unsigned int guiOffset;
		
		FILE *f = fopen(guiFilename, "rb");
		fseek(f, 0, SEEK_END);
		guiSize = ftell(f);
		rewind(f);
		gui = malloc(guiSize);
		fread(gui, guiSize, 1, f);
		fclose(f);
		
		compressed = malloc(256 * 1024);
		compressedSize = compress(compressed, gui, guiSize);
		
		printf("Compressed gui data to size %d\n", compressedSize);
		
		f = fopen(firmwareFilename, "rb");
		fseek(f, 0, SEEK_END);
		size_t size = ftell(f);
		rewind(f);
		firmware = malloc(size);
		fread(firmware, size, 1, f);
		fclose(f);
		
		guiOffset = ((struct header *)firmware)->part5offset * 8;
		
		// Gui data is not the final section, after it comes apps like pictochat
		/*if(guiOffset + compressedSize > size) {
			printf("ERROR! Not enough space for compressed gui data!\n");
			free(firmware);
			free(compressed);
			free(gui);
			return 1;
		}
		
		memset(firmware + guiOffset, 0xff, 0x0003fa00 - guiOffset);
		*/
		
		memcpy(firmware + guiOffset, compressed, compressedSize);
		
		((struct header *)firmware)->part5crc = swiCRC(0xffff, (u32 *)(firmware + guiOffset), compressedSize);
		
		f = fopen(firmwareFilename, "wb");
		fwrite(firmware, size, 1, f);
		fclose(f);
		
		free(compressed);
		free(gui);
	}
	
	free(firmware);
	
	return 0;
}
