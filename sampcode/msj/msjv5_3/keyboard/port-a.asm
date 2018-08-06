               TITLE   PORT-A.ASM
               PAGE    60,132

;---------------------------------------------------;
;  A visual display of the keyboard scan code port. ;
;  Michael J. Mefford                               ;
;---------------------------------------------------;

_TEXT          SEGMENT PUBLIC 'CODE'
               ASSUME  CS:_TEXT
               ASSUME  DS:_TEXT

               ORG     100H

START:         JMP     MAIN

;              DATA AREA
;              ---------
               DB      CR,SPACE,SPACE,SPACE,CR,LF

COPYRIGHT      DB      "PORT-A 1.0 (c) 1990 "
PROGRAMMER     DB      "Michael J. Mefford",CR,LF,LF,"$"
               DB      CTRL_Z

CR             EQU     13
LF             EQU     10
CTRL_Z         EQU     26
SPACE          EQU     32
BOX            EQU     254

ESC_SCAN       EQU     1
PORT_A         EQU     60H

BIOS_INT_9     DW      ?,?

MENU           LABEL   BYTE

DB   "Press and release any key to see "
DB   "make and break scan code",CR,LF
DB   "Press Esc to Exit",CR,LF,LF,"$"


;              CODE AREA
;              ---------
MAIN           PROC    NEAR

  CALL    CLS                     ;Clear the screen.

  MOV     DX,OFFSET COPYRIGHT     ;Display Copyright and menu.
  MOV     AH,9
  INT     21H
  MOV     DX,OFFSET MENU
  INT     21H

  MOV     AX,3509H                ;Get keyboard interrupt.
  INT     21H
  MOV     BIOS_INT_9[0],BX        ;Save old interrupt.
  MOV     BIOS_INT_9[2],ES

  MOV     DX,OFFSET PORT_A_INT_9  ;Install new interrupt.
  MOV     AX,2509H
  INT     21H

;-----------------------------------;
;  Loop here until Esc is detected. ;
;-----------------------------------;

GET_KEY:     
  XOR     AH,AH                   ;Go and wait for a keystroke.
  INT     16H
  CMP     AH,ESC_SCAN             ;If it's Esc, exit.
  JNZ     GET_KEY                 ;Else, continue.

EXIT:       
  MOV     DX,BIOS_INT_9[0]        ;Restore old INT 9.
  MOV     DS,BIOS_INT_9[2]
  MOV     AX,2509H
  INT     21H

  CALL    CLS                     ;Clear the screen.

  MOV     AX,4C00H                ;Exit with error level zero.
  INT     21H

MAIN           ENDP

               ;***************;
               ;* SUBROUTINES *;
               ;***************;

;--------------------------------------------------------;
; This "hooked" INT 9 procedure will be called whenever  ;
; a key is pressed which gives us the opportunity to get ;
; a look at the scan code and display it.                ;
;--------------------------------------------------------;

PORT_A_INT_9   PROC    NEAR

  ASSUME  DS:NOTHING
  PUSH    AX                      ;Preserve registers.
  PUSH    BX
  PUSH    CX
  IN      AL,PORT_A               ;Get the scan code.
  CALL    HEX_OUTPUT              ;And display it.

OLD_INT_9:   
  POP     CX                      ;Restore registers.
  POP     BX
  POP     AX
  JMP     DWORD PTR BIOS_INT_9    ;Jump to the BIOS INT 9 routine.

PORT_A_INT_9   ENDP

;--------------------------;

HEX_OUTPUT     PROC    NEAR

  MOV     BX,AX                   ;Store number in BX.
  MOV     CX,204H                 ;4 positions/word; 4bits/char.
ROTATE_HEX:  
  ROL     BL,CL                   ;Move highest bits to lowest.
  MOV     AL,BL                   ;Store number in AL.
  AND     AL,1111B                ;Mask off all but four lowest.
  ADD     AL,"0"                  ;Convert to ASCII.
  CMP     AL,"9"                  ;Is it alpha?
  JLE     PRINT_HEX               ;If no, print it.
  ADD     AL,7                    ;Else, adjust.
PRINT_HEX:   
  MOV     AH,0EH                  ;Print via BIOS.
  INT     10H
  DEC     CH                      ;Done all four positions?
  JNZ     ROTATE_HEX              ;If no, get next.

  MOV     CX,2                    ;Display two spaces
DELIMIT:     
  MOV     AL,SPACE                ; between scan codes
  MOV     AH,0EH                  ; as delimiters.
  INT     10H
  LOOP    DELIMIT
  RET

HEX_OUTPUT     ENDP

;--------------------------;

CLS            PROC    NEAR
  MOV     AH,0FH                  ;Get current video mode.
  INT     10H
  CMP     AL,7                    ;Is is mono?
  JZ      CLEAR_SCREEN            ;If yes, clear screen.
  MOV     AL,3                    ;Else, make sure in a text mode.

CLEAR_SCREEN:
  XOR     AH,AH                   ;Clear screen by setting mode.
  INT     10H
  MOV     AX,500H                 ;Make sure page zero.
  INT     10H
  RET

CLS            ENDP


_TEXT          ENDS
               END     START
