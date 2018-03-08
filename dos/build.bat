@ECHO OFF

echo. 
echo *** Assembling GETPLT
nasm -f bin -o getplt.com getplt.asm

echo.
echo *** Assembling VGAIMG
nasm -f bin -o vgaimg.com vgaimg.asm

echo.
echo *** Assembling VGAVID
nasm -f bin -o vgavid.com vgavid.asm

echo.
echo *** Assembling SNDTEST
nasm -f bin -o sndtest.com sndtest.asm

echo.
echo *** Assembling SNDTEST2
nasm -f bin -o sndtest2.com sndtest2.asm


echo.
pause
