; PC speaker test

BITS 16
ORG 100h
SECTION .text

; Set up PIT
; okay... we don't have 32 bit ints so we use a float (lol)
FILD	DWORD [freq_pit]
FILD	DWORD [freq_snd]
FDIVP
FISTP	WORD [divisor]
MOV		AL, 0B6h
OUT		43h, AL
MOV		AL, BYTE [divisor]
OUT		42h, AL
MOV		AL, BYTE [divisor + 1]
OUT		42h, AL

; Play
IN		AL, 61h
MOV		AH, AL
OR		AL, 3
CMP		AH, AL
JE		skipExtraOutb
	OUT		61h, AL
skipExtraOutb:

; Wait
XOR		AH, AH
INT		1Ah
MOV		BX, DX
waitOneSec:
	XOR		AH, AH
	INT		1Ah
	SUB		DX, BX
	CMP		DX, 18
	JL		waitOneSec

; Stop sound
IN		AL, 61h
AND		AL, 0FCh
OUT		61h, AL

; Exit
exit:
INT		20h
JMP		exit

; Constants
freq_pit	DD 1193180
freq_snd	DD 500

; Variables
divisor		DW 0
