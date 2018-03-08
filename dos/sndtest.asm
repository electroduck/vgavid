; Sound card test

BITS 16
ORG 100h
SECTION .text

%MACRO WRITE_ADLIB_REG 2
PUSH	AX
PUSH	DX
; Write to address port
MOV		AL, %1
MOV		DX, 0388h
OUT		DX, AL
; Wait the proper amount of time
MOV		AH, 86h
MOV		CX, 4
XOR		DX, DX
INT		15h
; Write to data port
MOV		AL, %2
MOV		DX, 0388h
OUT		DX, AL
; Wait the proper amount of time
MOV		AH, 86h
MOV		CX, 24
XOR		DX, DX
INT		15h
POP		DX
POP		AX
%ENDMACRO

; Turn on sound
WRITE_ADLIB_REG 0x20, 0x01
WRITE_ADLIB_REG 0x40, 0x10
WRITE_ADLIB_REG 0x60, 0xF0
WRITE_ADLIB_REG 0x80, 0x77
WRITE_ADLIB_REG 0xA0, 0x98
WRITE_ADLIB_REG 0x23, 0x01
WRITE_ADLIB_REG 0x43, 0x00
WRITE_ADLIB_REG 0x63, 0xF0
WRITE_ADLIB_REG 0xB0, 0x31

; Wait a bit
XOR		AH, AH
INT		1Ah
MOV		BX, DX
waitOneSec:
	XOR		AH, AH
	INT		1Ah
	SUB		DX, BX
	CMP		DX, 18
	JL		waitOneSec

; Turn off sound
WRITE_ADLIB_REG 0xB0, 0x00

; End
exit:
INT		20h
JMP		exit
