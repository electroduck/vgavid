@echo off

echo.
echo *** Compiling bmp
wcc386 %CCOPTS% bmp.c

echo. 
echo *** Compiling vga_p13h
wcc386 %CCOPTS% vga_p13h.c

echo.
echo *** Compiling vga_13h
wcc386 %CCOPTS% vga_13h.c

echo. 
echo *** Compiling bmp2vga
wcc386 %CCOPTS% bmp2vga.c

echo. 
echo *** Compiling bmp2vga with dithering
wcc386 %CCOPTS% -DDITHER -fo=bmp2vgad bmp2vga.c

echo. 
echo *** Compiling vga2bmp
wcc386 %CCOPTS% vga2bmp.c

echo. 
echo *** Linking bmp2vga
wlink NAME bmp2vga SYSTEM NT %LOPTS% OPTION MAP FILE bmp2vga,vga_13h,vga_p13h,bmp

echo. 
echo *** Linking bmp2vga with dithering
wlink NAME bmp2vgad SYSTEM NT %LOPTS% OPTION MAP FILE bmp2vgad,vga_13h,vga_p13h,bmp

echo. 
echo *** Linking vga2bmp
wlink NAME vga2bmp SYSTEM NT %LOPTS% OPTION MAP FILE vga2bmp,vga_13h,vga_p13h,bmp

echo.
echo *** Compiling and linking pconv
wcl386 %CCOPTS% -"%LOPTS%" pconv.c

echo.
echo *** Compiling and linking pconv2
wcl386 %CCOPTS% -"%LOPTS%" pconv2.c

echo.
echo *** Compiling and linking vvmerge
wcl386 %CCOPTS% -"%LOPTS%" vvmerge.c

pause