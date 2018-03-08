@ECHO OFF
REM Build for debugging

echo ==== COMPILING WITH DEBUG SYMBOLS ====
set "CCOPTS=-d2"
set "LOPTS=DEBUG DWARF"

call build.bat
