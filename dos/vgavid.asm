; Display video from file

%DEFINE NBYTES 64000					; Number of bytes in 320x200 8bpp img
%DEFINE VGASEG 0xA000					; VRAM segment in RAM
%DEFINE METALEN 6						; Video metadata bytes
%DEFINE SOUND							; Sound enabled if defined

%DEFINE VIRTUALBOX_FAILS_TO_EMULATE_INT15_86

ORG 100h
SECTION .text
; Entry point

%IFDEF VIRTUALBOX_FAILS_TO_EMULATE_INT15_86
; Print warning
MOV		AH, 09h
MOV		DX, s_wtiming
INT		21h

; Wait one second (approx.)
XOR		AH, AH
INT		1Ah
MOV		BX, DX
waitOneSec:
	XOR		AH, AH
	INT		1Ah
	SUB		DX, BX
	CMP		DX, 18
	JL		waitOneSec
%ENDIF

; Open image
MOV		AX, 3D00h
MOV		DX, z_vidfile
INT		21h
JNC		openOK
	; Error message if failed
	MOV		AH, 09h
	MOV		DX, s_eopen
	INT		21h
	JMP		exit
openOK:
MOV		BX, AX							; File handle in BX

; Load metadata
MOV		AH, 3Fh
MOV		CX, METALEN
MOV		DX, metadata
INT		21h
JNC		loadMetaOK
	; Error message if failed
	MOV		AH, 09h
	MOV		DX, s_ereadmd
	INT		21h
	JMP		exit
loadMetaOK:

; Check magic constant
MOV		CX, [md_magic]
CMP		CX, [s_magic]
JZ		magicOK
	; Error message if magic not valid
	MOV		AH, 09h
	MOV		DX, s_eformat
	INT		21h
	JMP		exit
magicOK:

%IFNDEF VIRTUALBOX_FAILS_TO_EMULATE_INT15_86
; Convert milliseconds to microseconds
MOV		AX, [md_msper]
MOV		CX, 1000
MUL		CX								; Most PITA instruction in all of x86
MOV		WORD [microsec], DX
MOV		WORD [microsec + 2], AX
%ENDIF

; Switch to mode 13h
MOV		AX, 13h
INT		10h

; Video loop
MOV		SI, [md_framect]				; All gen purp regs used by DOS call
displayFrame:
	%IFDEF SOUND
	; Load sound frequency
	MOV		AH, 3Fh
	MOV		CX, 2
	MOV		DX, freq_snd
	INT		21h
	JNC		loadSndOK
		; Error message if failed
		MOV		AH, 09h
		MOV		DX, s_eread
		INT		21h
		JMP		exit
	loadSndOK:
	%ENDIF
	
	; Load image
	MOV		AH, 3Fh
	MOV		CX, NBYTES
	MOV		DX, VGASEG
	PUSH	DS
	MOV		DS, DX
	XOR		DX, DX
	INT		21h
	POP		DS
	JNC		loadOK
		; Error message if failed
		MOV		AH, 09h
		MOV		DX, s_eread
		INT		21h
		JMP		exit
	loadOK:
	
	%IFDEF SOUND
	; See: https://wiki.osdev.org/PC_Speaker
	MOV		AX, [freq_snd]
	CMP		AX, 0
	JZ		noSoundThisFrame
		; Set up sound
		FILD	DWORD [freq_pit]
		FILD	WORD [freq_snd]
		FDIVP
		FISTP	WORD [snd_divisor]
		MOV		AL, 0B6h
		OUT		43h, AL
		MOV		AL, BYTE [snd_divisor]
		OUT		42h, AL
		MOV		AL, BYTE [snd_divisor + 1]
		OUT		42h, AL
		
		; Play
		IN		AL, 61h
		MOV		AH, AL
		OR		AL, 3
		CMP		AH, AL
		JE		skipExtraOutb
			OUT		61h, AL
		skipExtraOutb:
		
		JMP soundSectionEnd
		
	noSoundThisFrame:
		; Stop any active sound
		IN		AL, 61h
		AND		AL, 0FCh
		OUT		61h, AL
		
	soundSectionEnd:
	%ENDIF
	
	; Delay
	%IFDEF VIRTUALBOX_FAILS_TO_EMULATE_INT15_86
	PUSH	BX
	XOR		AH, AH
	INT		1Ah
	MOV		BX, DX
	waitForTime:
		XOR		AH, AH
		INT		1Ah
		CMP		BX, DX
		JE		waitForTime
	POP		BX
	%ELSE
	MOV		AH, 86h
	MOV		CX, [microsec]
	MOV		DX, [microsec + 2]
	INT		15h
	%ENDIF
	
	; Check for END key press
	MOV		AH, 01h
	INT		16h
	CMP		AH, 4Fh
	JE		exit
	
	DEC		SI
	CMP		SI, 0
	JNZ		displayFrame

; Exit
exit:

%IFDEF SOUND
; Stop any active sound
IN		AL, 61h
AND		AL, 0FCh
OUT		61h, AL
%ENDIF

; Switch to default mode
MOV		AX, 7
INT		10h

XOR		AX, AX
XOR		BX, BX
XOR		CX, CX
XOR		DX, DX
XOR		SI, SI
XOR		DI, DI

doExit:
INT		20h								; Try to exit with DOS 20h
JMP		doExit

; On CTRL + BREAK
break:
INT		19h								; Reboot

; Constants
z_vidfile	DB "VIDEO.VGV", 0
s_eopen		DB "Error opening file", '$'
s_eread		DB "Error reading frame", '$'
s_ereadmd	DB "Error reading metadata", '$'
s_eformat	DB "File is not a VGA video", '$'
%IFDEF VIRTUALBOX_FAILS_TO_EMULATE_INT15_86
s_wtiming	DB "VirtualBox compat. mode ON", 13, 10
            DB "FPS locked to 18.2", 13, 10, '$'
%ENDIF
s_magic		DB "VV"
freq_pit	DD 1193180

; Variables
metadata:
	md_magic	DW 0
	md_framect	DW 0
	md_msper	DW 0
microsec	DD 0
%IFDEF VIRTUALBOX_FAILS_TO_EMULATE_INT15_86
dx_orig		DW 0
%ENDIF
freq_snd	DW 0
snd_divisor	DW 0
