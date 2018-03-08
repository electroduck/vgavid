#ifndef VGA_13H_H
/*#define VGA_13H_H*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define VGA_13H_W 320
#define VGA_13H_H 200
#define VGA_13H_NCOLORS 256
#define VGA_13H_NBYTES (VGA_13H_W * VGA_13H_H)
#define VGA_13H_NPIX VGA_13H_NBYTES

#define VGA_IABS(n) ((n) < 0 ? 0 - (n) : (n))

void __stdcall VGA13hFrom24bpp(uint8_t* outData, const uint8_t* inData,
	const uint32_t* plt, bool useDithering);
	
void __stdcall VGA13hTo24bpp(uint8_t* outData, const uint8_t* inData,
	const uint32_t* plt);

#endif
