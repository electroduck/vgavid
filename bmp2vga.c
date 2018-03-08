#include <stdio.h>
#include <stdint.h>

#include "bmp.h"
#include "vga_p13h.h"
#include "vga_13h.h"

#ifdef DITHER
	#define DITHERING_ENABLED true
#else
	#define DITHERING_ENABLED false
#endif

static uint8_t outData[VGA_13H_NBYTES];

int main(int argc, char** argv) {
	FILE* inFile;
	FILE* outFile;
	uint8_t* inData;
	int32_t w, h;
	size_t nBytes;
	
	if(argc < 3) {
		puts("Usage: bmp2vga <input.bmp> <output.13h>");
		return 1;
	}
	
	inFile = fopen(argv[1], "rb");
	if(!inFile || ferror(inFile)) {
		printf("Unable to open %s for reading.\n", argv[1]);
		return 2;
	}
	
	outFile = fopen(argv[2], "wb");
	if(!inFile || ferror(inFile)) {
		printf("Unable to open %s for writing.\n", argv[2]);
		return 3;
	}
	
	printf("Loading bitmap... ");
	fflush(stdout);
	inData = BMPRead(inFile, &w, &h, NULL, 0);
	if(!inData) {
		puts("Unable to read input image.");
		return 4;
	}
	puts("bitmap loaded.");
	
	if((w != VGA_13H_W) || (h != VGA_13H_H)) {
		printf("VGA mode 13h is %ux%u, but your image is %ux%u.\n", VGA_13H_W,
			VGA_13H_H, w, h);
		puts("Please resize your image and try again.");
		return 5;
	}
	
	printf("Converting... ");
	VGA13hFrom24bpp(outData, inData, VGA13H_PLT_DATA, DITHERING_ENABLED);
	puts("image converted.");
	
	printf("Saving... ");
	fflush(stdout);
	nBytes = fwrite(outData, sizeof(uint8_t), VGA_13H_NBYTES, outFile);
	if(nBytes == VGA_13H_NBYTES) {
		puts("file saved.");
		return 0;
	} else {
		printf("failed - only %u of %u bytes written.\n", nBytes,
			VGA_13H_NBYTES);
		return 6;
	}
}
