; Display image from file

%DEFINE NBYTES 64000					; Number of bytes in 320x200 8bpp img
%DEFINE VGASEG 0xA000					; VRAM segment in RAM

ORG 100h
SECTION .text
; Entry point

; Open image
MOV		AX, 3D00h
MOV		DX, z_imgfile
INT		21h
JNC		openOK
	; Error message if failed
	MOV		AH, 09h
	MOV		DX, s_eopen
	INT		21h
	JMP		exit
openOK:
MOV		BX, AX							; File handle in BX

; Switch to mode 13h
MOV		AX, 13h
INT		10h

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

; Exit
exit:
INT		20h								; Try to exit with DOS 20h
CLI										; Otherwise, halt system
HLT

; Constants
z_imgfile	DB "IMAGE.VGA", 0
s_eopen		DB "Error opening file", '$'
s_eread		DB "Error reading image", '$'
