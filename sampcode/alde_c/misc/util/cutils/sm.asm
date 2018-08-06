                PAGE            65,132
                TITLE           The stripped down Lattice Header.
                NAME            Main
; ---------------------------------------------------------------------
; Author:  Jonathan Wesener
; Date:    3 / 14 / 85
; Purpose: This header will allow us to assemble, compile and link
;       our programs into .COM format and also got rid of the 10K of
;       garbage that LATTICE used to add.
 
LF              EQU     0ah
 
PGROUP          GROUP   BASE,PROG,TAIL
 
; BASE is included because its alphabetically less than DATA so the linker
; will put the data after the code and allow us to generate a .COM
 
BASE            SEGMENT BYTE PUBLIC 'PROG'
                ASSUME  CS:PGROUP
                EXTRN   MAIN:NEAR
                EXTRN   PRINTE:NEAR
                ORG     100H
 
; We compute the segment ourselves because if we let the linker figure it
; out, like     MOV     AX,DGROUP       , the loader would have to update
; our code, hence it would not be .COM compatable.
 
C               PROC    NEAR
                mov     ax,offset PGROUP:TAIL
                mov     cl,4            ; compute the number of
                shr     ax,cl           ; paragraphs away the data segment
                mov     bx,cs           ; compute the data segment
                add     ax,bx           ; by adding offset to base
                mov     ds, ax
                mov     es, ax
                mov     ss, ax
                mov     sp, offset stack + stacksize
                cld                     ; set correct direction of the stack
                call	main
                mov	ah, 4ch
                int	21h
                int	20h
C               ENDP
BASE            ENDS
 
PROG            SEGMENT         BYTE PUBLIC 'PROG'
PROG            ENDS
 
; I set the alignment to paragraph because TAIL is empty.  The TAIL segment
; will then be the same thing as the DATA segment and we don't need to
; compute the segment address .
 
TAIL            SEGMENT         PARA 'PROG'
TAIL            ENDS
 
DGROUP          GROUP   DATA, LAST
DATA            SEGMENT         PARA PUBLIC 'DATA'
STACKSIZE       EQU     200                  ; a 800 byte stack
 
                db	"(C) Copyright 1985, BEYOND CONTROL Software." 
                db      " Jonathan Wesener."
stack           db      STACKSIZE dup ("$")     ; buffer for output
DATA            ENDS
 
LAST            SEGMENT         PARA PUBLIC 'DATA'
LAST            ENDS
                END     C               ; Tells where the start of the code is
