#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define INPUT_BASE 16

const char* FNAME_INPUT = "plt_text.txt";
const char* FNAME_OUTPUT = "plt.bin";

int main(int argc, char** argv) {
	FILE* input;
	FILE* output;
	uint32_t color;
	char line[16];
	
	input = fopen(FNAME_INPUT, "r");
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
		fgets(line, sizeof(line), input);
		color = (uint32_t)strtol(line, NULL, INPUT_BASE);
		fwrite(&color, sizeof(uint32_t), 1, output);
	}
	
	return ferror(input) || ferror(output);
}
