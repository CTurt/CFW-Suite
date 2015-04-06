#include <stdio.h>
#include <nds.h>
#include <fat.h>
#include <dirent.h>

#include "main.h"

#include "fileSelector.h"

static char filenames[5][256];
static char fullPath[512];

char *selectFirmware(void) {
	DIR *pdir;
	struct dirent *pent;
	
	int fileCount = 0;
	int selection = 0;
	
	pdir = opendir("/firmwares");
	
	printf(" Select firmware image to flash:\n");
	
	if(pdir) {
		while((pent = readdir(pdir)) != NULL && fileCount < 5) {
			if(strcmp(".", pent->d_name) == 0 || strcmp("..", pent->d_name) == 0) continue;
			if(pent->d_type == DT_DIR) continue;
			
			strncpy(filenames[fileCount], pent->d_name, 255);
			fileCount++;
		}
		
		closedir(pdir);
		
		while(1) {
			scanKeys();
			
			console->cursorY = 10;
			
			int i;
			for(i = 0; i < fileCount; i++) {
				if(i == selection) printf("*");
				else printf(" ");
				
				printf("%s\n", filenames[i]);
			}
			
			if(keysDown() & KEY_DOWN && selection < fileCount - 1) selection++;
			if(keysDown() & KEY_UP && selection > 0) selection--;
			
			if(keysDown() & KEY_A) {
				sprintf(fullPath, "/firmwares/%s", filenames[selection]);
				return fullPath;
			}
			
			swiWaitForVBlank();
		}
	}
	else {
		printf(" \"/firmwares\" not found!\n");
		return NULL;
	}
}