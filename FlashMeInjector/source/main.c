#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void help(char *name) {
	printf("FlashMeInjector - Inject CFW into FlashMe installer\n");
	printf("Usage:\n");
	printf("%s flashme.nds [-lite or -phat] cfw.bin\n", name);
	printf(" flashme.nds: filename of FlashMe installer\n");
	printf(" -lite: inject into DS Lite firmware space\n");
	printf(" -phat: inject into original DS firmware space\n");
	printf(" cfw.bin: filename of firmware to inject\n");
}

int main(int argc, char **argv) {
	enum {
		unspecified,
		lite,
		phat,
	} console;
	
	unsigned int firmwareOffsets[] = {
		[lite] = 0x00041234,
		[phat] = 0x00001434,
	};
	
	FILE *f;
	
	unsigned char *flashme;
	unsigned char *cfw;
	
	size_t flashmeSize;
	
	unsigned int offset;
	
	char *flashmeFilename = NULL;
	char *cfwFilename = NULL;
	
	if(argc != 4) {
		help(argv[0]);
		return 1;
	}
	
	flashmeFilename = argv[1];
	if(strcmp(argv[2], "-lite") == 0) console = lite;
	if(strcmp(argv[2], "-phat") == 0) console = phat;
	cfwFilename = argv[3];
	
	if(console == unspecified) {
		help(argv[0]);
		return 1;
	}
	
	offset = firmwareOffsets[console];
	
	f = fopen(flashmeFilename, "rb");
	fseek(f, 0, SEEK_END);
	flashmeSize = ftell(f);
	rewind(f);
	flashme = malloc(flashmeSize);
	fread(flashme, 0x3fe00, 1, f);
	fclose(f);
	
	cfw = malloc(0x3fe00);
	f = fopen(cfwFilename, "rb");
	fread(cfw, 0x3fe00, 1, f);
	fclose(f);
	
	//memcpy(flashme + offset, cfw, 0x3fe00);
	memset(flashme + offset, 0x00, 0x3fe00);
	
	f = fopen(flashmeFilename, "wb");
	fwrite(flashme, flashmeSize, 1, f);
	fclose(f);
	
	printf("Injected to 0x%08x\n", offset);
	
	free(cfw);
	free(flashme);
	
	return 0;
}
