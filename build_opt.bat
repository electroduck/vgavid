@ECHO OFF
REM Build optimized for Pentium Pro and newer

echo ==== COMPILING WITH OPTIMIZATION FOR MODERN PROCESSORS ====
set "CCOPTS=-6r -fp6 -obmiler"

call build.bat
