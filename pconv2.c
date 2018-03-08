#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define INPUT_BASE 16

const char* FNAME_INPUT = "dos\\vgaplt.bin";
const char* FNAME_OUTPUT = "plt.bin";

int main(int argc, char** argv) {
	FILE* input;
	FILE* output;
	uint32_t color;
	uint8_t colorOrig[3];
	
	input = fopen(FNAME_INPUT, "rb");
	if(!input || ferror(input)) {
		printf("Unable to open %s for reading.\n", FNAME_INPUT);
		return 1;
	}
	
	output = fopen(FNAME_OUTPUT, "wb");
	if(!output || ferror(output)) {
		printf("Unable to open %s for writing.\n", FNAME_OUTPUT);
		return 1;
	}
	
	while(!(feof(input) || ferror(input) || ferror(output))) {
		fread(colorOrig, 1, sizeof(colorOrig), input);
		/* An additional shift of 2 bits is required:
		   the VGA palette is 6 bits per channel, unpacked */
		color  = colorOrig[0] <<  2;
		color |= colorOrig[1] << 10;
		color |= colorOrig[2] << 18;
		fwrite(&color, sizeof(uint32_t), 1, output);
	}
	
	return ferror(input) || ferror(output);
}
