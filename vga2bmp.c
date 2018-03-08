#include <stdio.h>
#include <stdint.h>

#include "bmp.h"
#include "vga_p13h.h"
#include "vga_13h.h"

#define VGA2BMP_BMP_BYTES (VGA_13H_W * VGA_13H_H * BMP_BYTESPERPIXEL)

static uint8_t vgaData[VGA_13H_NBYTES];
static uint8_t bmpData[VGA2BMP_BMP_BYTES];

int main(int argc, char** argv) {
	FILE* inFile;
	FILE* outFile;
	size_t nElem;
	
	if(argc < 3) {
		puts("Usage: vga2bmp <input.13h> <output.bmp>");
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
	
	printf("Reading input... ");
	fflush(stdout);
	nElem = fread(vgaData, 1, VGA_13H_NBYTES, inFile);
	if(nElem == VGA_13H_NBYTES)
		puts("file loaded.");
	else if(!nElem) {
		puts("no data could be read.");
		return 4;
	} else {
		printf("Warning: Only %u of %u bytes read.\n", nElem, 
			VGA_13H_NBYTES);
	}
	
	printf("Converting... ");
	fflush(stdout);
	VGA13hTo24bpp(bmpData, vgaData, VGA13H_PLT_DATA);
	puts("image converted.");
	
	printf("Saving... ");
	fflush(stdout);
	nElem = BMPWrite(outFile, VGA_13H_W, VGA_13H_H, bmpData);
	if(nElem == VGA_13H_NPIX) {
		puts("image saved.");
		return 0;
	} else {
		printf("failed - only %u of %u bytes written.\n", nElem,
			VGA_13H_NPIX);
		return 5;
	}
}
