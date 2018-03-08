/* For merging images into a video file */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <stdlib.h>

// comment out to not place sound words in output
#define SOUND

#define SOUND_FREQ_MAX 4000
#define SOUND_THRESH 3000
#define FRAME_DATA_LEN (320 * 200)

#ifndef PI
#define PI 3.14159265
#endif

double GetRMS(int16_t* points, size_t n);
double GetAvgFreq(int16_t* points, size_t n);

const char * STR_HELP = 
	"Usage: vvmerge <filename-format> <frame-ct> <fps> <soundfile> <outfile>\n" \
	"\t<filename-format> is a printf string accepting one uint as input,\n" \
	"\t                  which matches each frame of the video.\n" \
	"\t<frame-ct>        is the number of frames in the video.\n" \
	"\t<fps>             is the video FPS (can be a decimal).\n" \
	"\t<soundfile>       is an 8kHz 16-bit signed LE audio file.\n" \
	"\t<outfile>         is the file to output the video to.";
	
#define MAGIC_LEN 2
const char * MAGIC_DATA = "VV";

uint8_t frameData[FRAME_DATA_LEN];

int main(int argc, char** argv) {
	FILE* inFile;
	FILE* outFile;
	FILE* sndFile;
	uint16_t frameCt, msPerFrame, i, sndFreq;
	double fps;
	char inFileName[256];
	int16_t* sndData;
	size_t nSndSamplesPerFrame;
	
	if(argc < 5) {
		puts(STR_HELP);
		return 1;
	}
	
	frameCt = (uint16_t)strtol(argv[2], NULL, 0);
	
	fps = strtod(argv[3], NULL);
	msPerFrame = (uint16_t)(1000.0 / fps);
	
	nSndSamplesPerFrame = (size_t)(8000.0 / fps);
	printf("%u sound samples per frame.\n", nSndSamplesPerFrame);
	
	sndData = malloc(sizeof(int16_t) * nSndSamplesPerFrame);
	if(!sndData) {
		printf("Failed to allocate memory for %u sound samples.\n",
			nSndSamplesPerFrame);
		return 1;
	}
	
	outFile = fopen(argv[5], "wb");
	if(!outFile || ferror(outFile)) {
		printf("Failed to open %s for writing.\n", argv[5]);
		return 1;
	}
	
	sndFile = fopen(argv[4], "rb");
	if(!sndFile || ferror(sndFile)) {
		printf("Failed to open %s for reading.\n", argv[4]);
		return 1;
	}
	
	if(fwrite(MAGIC_DATA, 1, MAGIC_LEN, outFile) != MAGIC_LEN) {
		puts("Error writing magic constant.");
		return 1;
	}
	
	if(!fwrite(&frameCt, sizeof(uint16_t), 1, outFile)) {
		puts("Error writing frame count.");
		return 1;
	}
	
	if(!fwrite(&msPerFrame, sizeof(uint16_t), 1, outFile)) {
		puts("Error writing frame rate.");
		return 1;
	}
	
	// Read and write each frame
	for(i = 0; i < frameCt; i++) {
		snprintf(inFileName, sizeof(inFileName), argv[1], i);
		inFile = fopen(inFileName, "rb");
		if(!inFile || ferror(inFile)) {
			printf("Failed to open frame #%u.\nFile: %s\n", i, inFileName);
			return 1;
		}
		
		if(fread(frameData, 1, FRAME_DATA_LEN, inFile) != FRAME_DATA_LEN) {
			printf("Frame #%u is not valid.\nFile: %s\n", i, inFileName);
			return 1;
		}
		
		#ifdef SOUND
		if(fread(sndData, sizeof(int16_t), nSndSamplesPerFrame, sndFile)
				!= nSndSamplesPerFrame) {
			printf("Failed to read sound data for frame #%u.\n", i);
			return 1;
		}
		
		sndFreq = GetRMS(sndData, nSndSamplesPerFrame) > SOUND_THRESH
			? GetAvgFreq(sndData, nSndSamplesPerFrame) * SOUND_FREQ_MAX : 0;
		printf(" %u", sndFreq);
		
		if(!fwrite(&sndFreq, sizeof(uint16_t), 1, outFile)) {
			printf("Failed to write sound data for frame #%u.\n",
				i, inFileName);
			return 1;
		}
		#endif
		
		if(fwrite(frameData, 1, FRAME_DATA_LEN, outFile) != FRAME_DATA_LEN) {
			printf("Failed to write frame #%u.\n", i, inFileName);
			return 1;
		}
		
		fclose(inFile);
	}
	
	fclose(sndFile);
	fclose(outFile);
	puts("Done.");
	return 0;

}

double GetRMS(int16_t* points, size_t n) {
	double sqTotal = 0.0;
	size_t i;
	
	for(i = 0; i < n; i++, points++) {
		sqTotal += *points * *points;
	}
	
	return sqrt(sqTotal / (double)n);
}

double GetAvgFreq(int16_t* points, size_t n) {
	//int16_t lastPeak, curPeak;
	uint64_t periodTotal;
	size_t i, lastPeakIndex, curPeakIndex, nPeriods;
	
	nPeriods = 0;
	periodTotal = 0;
	for(i = 1; i < (n-1); i++) {
		if((points[i-1] < points[i]) && (points[i+1] < points[i])) {
			//lastPeak = curPeak;
			lastPeakIndex = curPeakIndex;
			//curPeak = points[i];
			curPeakIndex = i;
			periodTotal += (uint64_t)(curPeakIndex - lastPeakIndex);
			nPeriods++;
		}
	}
	
	return 1.0 / ((double)periodTotal / (double)nPeriods);
}






































