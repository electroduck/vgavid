; Save VGA palette to file

%DEFINE CORRECT_RGB							; Define to correct RGB values in ASM

BITS 16
ORG 100h
SECTION .text

; Open file
MOV		AH, 3Ch
XOR		CX, CX
MOV		DX, z_palfile
INT		21h
JNC		openOK
	; Error message if failed
	MOV		AH, 09h
	MOV		DX, s_eopen
	INT		21h
	JMP		exit
openOK:
PUSH	AX ; File handle on stack

; Switch mode
MOV		AX, 13h
INT		10h

; Get palette
MOV		AX, 1017h
XOR		BX, BX
MOV		CX, 768
MOV		DX, palette
INT		10h

%IFDEF CORRECT_RGB
; Correct RGB values
MOV		CX, 768
MOV		BX, palette
correctByte:
	MOV		AL, BYTE [BX]
	SHL		AL, 2
	MOV		BYTE [BX], AL
	INC		BX
	LOOP	correctByte
%ENDIF
	

; Write to file
MOV		AH, 40h
POP		BX
MOV		CX, 1024
MOV		DX, palette
INT		21h
JNC		writeOK
	; Error message if failed
	MOV		AH, 09h
	MOV		DX, s_ewrite
	INT		21h
	JMP		exit
writeOK:

; Close file
MOV		AH, 3Eh
INT		21h
JNC		closeOK
	; Error message if failed
	MOV		AH, 09h
	MOV		DX, s_eclose
	INT		21h
	JMP		exit
closeOK:

; Exit
exit:
INT		20h
CLI
HLT

; Strings
z_palfile	DB "VGAPLT.BIN", 0
s_eopen		DB "Error opening file", '$'
s_ewrite	DB "Error writing file", '$'
s_eclose	DB "Error closing file", '$'

; Variables
palette		TIMES 768 DB 0
