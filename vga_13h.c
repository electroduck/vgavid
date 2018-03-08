#include "vga_13h.h"
#include <stdlib.h>

/*
	IMPORTANT NOTE:
	The code for comparing palette entries with RGB values has been manually
	inlined for performance.  For some reason, automatic inlining does NOT
	work on it (performance is still bad) even when the function is explicitly
	declared inline and inlining is enabled in the compiler options.  Yes, it
	looks fairly disgusting.  But try encoding 3000 frames without the manual
	inlining and you will see why I did this.
*/

const uint32_t VGA_RAND_MUL = 16807;
const uint32_t VGA_RAND_SEED = 1234567;
const uint32_t VGA_RAND_DIV = 0x80000000;
const uint32_t VGA_RAND_MPLIER = 1000;
const uint32_t VGA_RAND_ONE = 1;

uint32_t __fastcall VGAGetNearestPltColors(const uint32_t* plt, uint8_t colorR,
	uint8_t colorG, uint8_t colorB, uint16_t pltSize);
	
inline int __fastcall VGAGetColorDiff(uint32_t pltColor, uint8_t r, uint8_t g,
	uint8_t b);

void __stdcall VGA13hFrom24bpp(uint8_t* outData, const uint8_t* inData,
	const uint32_t* plt, bool useDithering) {
	size_t x, y;
	uint32_t nearestColors;
	uint8_t cNearest, cNextNearest;
	uint32_t randLast = VGA_RAND_SEED;
	int randF, chance;
	uint8_t curR, curG, curB;
	int diffNextNearest, diffNearest;
	int curDiff, curComponentDiff;
	
	for(y = 0; y < VGA_13H_H; y++) {
		for(x = 0; x < VGA_13H_W; x++) {
			curR = inData[0];
			curG = inData[1];
			curB = inData[2];
			nearestColors = VGAGetNearestPltColors(plt, curR, curG, curB,
				VGA_13H_NCOLORS);
				
			if(useDithering) {
				cNearest = (uint8_t)(nearestColors & 0x0000FFFF);
				cNextNearest = (uint8_t)((nearestColors & 0xFFFF0000) >> 16);
				
				//diffNearest = VGAGetColorDiff(plt[cNearest], curR, curG,
				//	curB);
				curComponentDiff = (int)((cNearest & 0xFF0000) >> 16)
				                   - (int)curR;
				curDiff  = VGA_IABS(curComponentDiff);
				curComponentDiff = (int)((cNearest & 0x00FF00) >>  8)
				                   - (int)curG;
				curDiff += VGA_IABS(curComponentDiff);
				curComponentDiff = (int) (cNearest & 0x0000FF)
				                   - (int)curB;
				curDiff += VGA_IABS(curComponentDiff);
				diffNearest = curDiff / 3;
				
				//diffNextNearest = VGAGetColorDiff(plt[cNextNearest], curR,
				//	curG, curB);
				curComponentDiff = (int)((cNextNearest & 0xFF0000) >> 16)
				                   - (int)curR;
				curDiff  = VGA_IABS(curComponentDiff);
				curComponentDiff = (int)((cNextNearest & 0x00FF00) >>  8)
				                   - (int)curG;
				curDiff += VGA_IABS(curComponentDiff);
				curComponentDiff = (int) (cNextNearest & 0x0000FF)
				                   - (int)curB;
				curDiff += VGA_IABS(curComponentDiff);
				diffNextNearest = curDiff / 3;
				
				
				/* Converting to ints beforehand makes the comparison simpler. */
					
				//randF = abs((double)randLast / VGA_RAND_DIV);
				//chance = (double)(diffNextNearest - diffNearest)
				//         / (double)diffNextNearest;
				__asm {
					IMUL	EAX, DWORD PTR [randLast], 16807
					MOV		DWORD PTR [randLast], EAX
					FILD	DWORD PTR [randLast]
					FIDIV	DWORD PTR [VGA_RAND_DIV]
					FABS
					FIMUL	DWORD PTR [VGA_RAND_MPLIER]
					FISTP	QWORD PTR [randF]
					
					FILD	DWORD PTR [diffNextNearest]
					FISUB	DWORD PTR [diffNearest]
					FIDIV	DWORD PTR [diffNextNearest]
					FIMUL	DWORD PTR [VGA_RAND_MPLIER]
					FISTP	QWORD PTR [chance]
				}
				
				if(randF > chance)
					*outData = cNextNearest;
				else
					*outData = cNearest;
				
			} else
				*outData = (uint8_t)(nearestColors & 0x0000FFFF);
			
			outData++;
			inData += 3;
		}
	}
}

uint32_t __fastcall VGAGetNearestPltColors(const uint32_t* plt, uint8_t colorR,
	uint8_t colorG, uint8_t colorB, uint16_t pltSize) {
	int lowestDiff = 0x7FFFFFFF;
	int curDiff, curComponentDiff;
	uint16_t indexA = 0;
	uint16_t indexB = 0;
	uint16_t i;
	
	for(i = 0; i < pltSize; i++, plt++) {
		//curDiff = VGAGetColorDiff(*plt, colorR, colorG, colorB);
		curComponentDiff = (int)((*plt & 0xFF0000) >> 16) - (int)colorR;
		curDiff  = VGA_IABS(curComponentDiff);
		curComponentDiff = (int)((*plt & 0x00FF00) >>  8) - (int)colorG;
		curDiff += VGA_IABS(curComponentDiff);
		curComponentDiff = (int) (*plt & 0x0000FF)        - (int)colorB;
		curDiff += VGA_IABS(curComponentDiff);
		curDiff /= 3;
		if(curDiff < lowestDiff) {
			indexB = indexA;
			indexA = i;
			lowestDiff = curDiff;
		}
	}
	
	return (uint32_t)indexA << 16 | (uint32_t)indexB;
}

inline int __fastcall VGAGetColorDiff(uint32_t pltColor, uint8_t r, uint8_t g,
	uint8_t b) {
	int curDiff, curComponentDiff;
	curComponentDiff = (int)((pltColor & 0xFF0000) >> 16) - (int)r;
	curDiff  = VGA_IABS(curComponentDiff);
	curComponentDiff = (int)((pltColor & 0x00FF00) >>  8) - (int)g;
	curDiff += VGA_IABS(curComponentDiff);
	curComponentDiff = (int) (pltColor & 0x0000FF)        - (int)b;
	curDiff += VGA_IABS(curComponentDiff);
	return curDiff / 3;
}

void __stdcall VGA13hTo24bpp(uint8_t* outData, const uint8_t* inData,
	const uint32_t* plt) {
	size_t x, y;
	
	for(y = 0; y < VGA_13H_H; y++) {
		for(x = 0; x < VGA_13H_W; x++) {
			outData[0] = (uint8_t)((plt[*inData] & 0xFF0000) >> 16);
			outData[1] = (uint8_t)((plt[*inData] & 0x00FF00) >>  8);
			outData[2] = (uint8_t)( plt[*inData] & 0x0000FF       );
			outData += 3;
			inData++;
		}
	}	
}
