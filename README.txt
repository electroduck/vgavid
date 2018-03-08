VGAVid: Set of programs for displaying images and video on MS-DOS using VGA and the PC speaker.
VGA graphics are in the 320x200 8bpp mode.

32-bit Windows code is in the root directory of this repository.
These are the 32-bit files:
bmp.c                           24bpp bitmap loading/saving code
bmp2vga.c    -> bmp2vga[d].exe  24bpp-to-VGA converter - the 'd' version does dithering
pconv.c      -> pconv.exe       Text-to-binary converter for color palettes
pconv2.c     -> pconv2.exe      Converts VGA palette data to binary color palettes
vga_13h.c                       Code used for conversion to and from BMP / VGA
vga_p13h.c                      VGA palette as C code
vga2bmp.c    -> vga2bmp.exe     VGA-to-24bpp converter
vvmerge.c    -> vvmerge.exe     Images-to-video merge program

16-bit MS-DOS code is in the 'dos' directory.
These are the 16-bit files:
getplt.asm   -> getplt.com      Saves the VGA palette to VGAPLT.BIN
sndtest.asm  -> sndtest.com     Tests the sound card (not working)
sndtest2.asm -> sndtest2.com    Tests the PC speaker
vgaimg.asm   -> vgaimg.com      Displays IMAGE.VGA, an image produced by bmp2vga
vgavid.asm   -> vgavid.com      Plays VIDEO.VGV, a video produced by vvmerge

How to build:
1. Install Open Watcom (C compiler).
2. Run build.bat in the root directory of the repository.
   -> Use build_debug.bat to build for debugging, or build_opt.bat to build optimized
3. Install NASM (assembler).
4. Run build.bat in the 'dos' directory.

How to convert a video to the format:
1. Scale the video to 320x200.
2. Dump the video frames as 24 bit BMP images to some folder.
3. Run bmp2vga on all frames - here is a batch script to do it:
	for %%f in (*.bmp) do (
		bmp2vga %%f ..\converted\%%f.bin
	)
   Make sure to replace 'bmp2vga' and '..\converted' with the proper paths.
   Run the script from inside the folder of frames.
4. Use vvmerge to merge the frames together into a video.
   -> Run vvmerge without parameters to see its instructions (esp. relating to audio)
5. You should now have a VGV file, this is your video.

How to play a video:
1. Make sure that you have converted it into the VGV format.
2. Copy the video into the 'dos' directory and name it VIDEO.VGV.
3. Run DOSBox and mount the 'dos' directory, or put VGAVID.COM and VIDEO.VGV in an ISO file.
   -> You can insert the ISO into a VirtualBox VM running DOS, or burn it to a CD
4. Once you are in the folder in DOS, run VGAVID.COM to play the video.

NOTE: The .COM extension does NOT refer to web domains, nor to the Component Object Model.
      COM files are simple, small executables that run on DOS.  They are similar to EXE files.
