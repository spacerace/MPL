;---------------------------------------------------------------;
;  KBBUFFER.CTL * Michael J. Mefford                            ;
;  Is loaded as a device driver just to get within offset       ;
;  range of the BIOS data area and the keyboard buffer so       ;
;  it can replace the default 15 key buffer with a larger one.  ;
;---------------------------------------------------------------;

BIOS_DATA      SEGMENT AT 40H
               ORG     1AH
BUFFER_HEAD    DW      ?
BUFFER_TAIL    DW      ?
               ORG     80H
BUFFER_START   DW      ?
BUFFER_END     DW      ?
BIOS_DATA      ENDS


_TEXT          SEGMENT PUBLIC 'CODE'
               ASSUME  CS:_TEXT,DS:_TEXT,ES:_TEXT,SS:_TEXT

               ORG     0H

;COPYRIGHT      DB     "KBBUFFER.CTL 1.0 (c) 1990 ",CR,LF
;PROGRAMMER     DB     "Michael J. Mefford",CR,LF,CTRL_Z

;************* DEVICE_HEADER *************;

POINTER        DD      -1
ATTRIBUTE      DW      1000000000000000B
DEVICE_STRAG   DW      STRATEGY
DEVICE_INT     DW      INTERRUPT
DEVICE_NAME    DB      "BUFFERCTL"


CR             EQU     13
LF             EQU     10
CTRL_Z         EQU     26
SPACE          EQU     32
BOX            EQU     254

;-------------------------;

REQUEST_HEADER STRUC

HEADER_LENGTH  DB      ?
UNIT_CODE      DB      ?
COMMAND_CODE   DB      ?
STATUS         DW      ?
RESERVED       DQ      ?

REQUEST_HEADER ENDS

DONE           EQU     0000000100000000B       ;Status codes.
UNKNOWN        EQU     1000000000000011B

;-------------------------;

INIT           STRUC

HEADER         DB      (TYPE REQUEST_HEADER) DUP(?)
UNITS          DB      ?
ENDING_OFFSET  DW      ?
ENDING_SEGMENT DW      ?
ARGUMENTS_OFF  DW      ?
ARGUMENTS_SEG  DW      ?

INIT           ENDS

REQUEST_OFFSET DW      ?
REQUEST_SEG    DW      ?

;              CODE AREA
;              ---------

;---------------------------------------------;
; The only task of the strategy routine is to ;
; save the pointer to the request header.     ;
;---------------------------------------------;

STRATEGY       PROC    FAR

  MOV     CS:REQUEST_OFFSET,BX    ;Request header address is
  MOV     CS:REQUEST_SEG,ES       ; passed in ES:BX.
  RET

STRATEGY       ENDP

;----------------------------------------;
; The interrupt procedure will be called ;
; immediately after the strategy.        ;
;----------------------------------------;

INTERRUPT      PROC    FAR

  PUSH    AX                      ;Responsible for all registers.
  PUSH    BX
  PUSH    CX
  PUSH    DX
  PUSH    DS
  PUSHF

  MOV     DS,CS:REQUEST_SEG       ;Retrieve request header pointer.
  MOV     BX,CS:REQUEST_OFFSET

  OR      STATUS[BX],DONE         ;Tell DOS we are done.
  CMP     COMMAND_CODE[BX],0      ;Is it INIT command?
  JZ      MAKE_STACK              ;If yes, do our stuff.
  OR      STATUS[BX],UNKNOWN      ;Else, exit with confused
  JMP     SHORT UNKNOWN_EXIT      ; message to DOS.

MAKE_STACK:  
  MOV     CX,SS                   ;Save DOS stack.
  MOV     DX,SP
  MOV     AX,CS
  CLI
  MOV     SS,AX                   ;Make new stack.
  MOV     SP,0FFFEH
  STI
  PUSH    CX                      ;Save old stack pointers on new.
  PUSH    DX

  PUSH    ES                      ;Save rest of registers.
  PUSH    SI
  PUSH    BP

  CALL    INITIALIZE              ;Go do our stuff.

  POP     BP                      ;Restore registers.
  POP     SI
  POP     ES

  POP     DX                      ;Restore old DOS stack.
  POP     CX
  CLI
  MOV     SS,CX
  MOV     SP,DX
  STI

UNKNOWN_EXIT:
  POPF                            ;Restore rest of registers.
  POP     DS
  POP     DX
  POP     CX
  POP     BX
  POP     AX
  RET                             ;Far return back to DOS.

INTERRUPT      ENDP

KBBUFFER_CTL_END LABEL   WORD

;************* END OF RESIDENT PORTION *************;

BUFFER_DEFAULT EQU     80
BUFFER_MIN     EQU     16
BUFFER_MAX     EQU     200

HEADING        LABEL   BYTE

DB "KBBUFFER.CTL 1.0 (c) 1990 "
DB "Michael J. Mefford",CR,LF,LF,"$"

INSTALLED_MSG  LABEL   BYTE

DB "Installed",CR,LF,LF

DB "Syntax:  DEVICE = KBBUFFER.CTL [buffer size]",CR,LF
DB "buffer size = 16 - 200",CR,LF
DB "default = 80",CR,LF,LF,"$"

OUT_OF_RANGE_MSG  LABEL BYTE

DB "KBBUFFER.CTL is loaded greater than "
DB "64K from BIOS data area",CR,LF
DB "KBBUFFER is inactive",CR,LF
DB "Make sure KBBUFFER.CTL is first in CONFIG.SYS",CR,LF,LF,"$"

;              ***************
;              * SUBROUTINES *
;              ***************

;------------------------------------------;
; INPUT                                    ;
;   DS:BX points to request header.        ;
;                                          ;
;   All registers destroyed.               ;
;------------------------------------------;

INITIALIZE     PROC    NEAR

  PUSH    DS                      ;Point to request segment.
  POP     ES
  MOV     ENDING_OFFSET[BX],OFFSET KBBUFFER_CTL_END
  MOV     ENDING_SEGMENT[BX],CS   ;Resident portion setup.

  MOV     CX,ARGUMENTS_SEG[BX]    ;Retrieve CONFIG.SYS buffer
  MOV     SI,ARGUMENTS_OFF[BX]    ; pointers from INIT table.

  PUSH    CS                      ;Point to our data.
  POP     DS
  MOV     DX,OFFSET HEADING       ;Display signature.
  CALL    PRINT_STRING

  MOV     DS,CX                   ;Point to argument segment.
  CLD

;------------------------------------;
; Parse CONFIG.CTL second parameter. ;
;------------------------------------;

FIND_PARA:   
  LODSB                           ;Get a byte.
  CMP     AL,SPACE                ;Leading white space?
  JA      FIND_PARA               ;If yes, parse it off.

  DEC     SI                      ;Point to start of argument.
  XOR     BP,BP                   ;Use BP to store seconds.
NEXT_NUMBER: 
  LODSB                           ;Retrieve a byte.
  CMP     AL,CR                   ;If carriage return or linefeed,
  JZ      CK_PARA                 ; found end of parameter.
  CMP     AL,LF
  JZ      CK_PARA
  SUB     AL,"0"                  ;ASCII to binary.
  JC      NEXT_NUMBER             ;If not between 0 and 9, skip.
  CMP     AL,9
  JA      NEXT_NUMBER
  CBW                             ;Convert to word.
  XCHG    AX,BP                   ;Swap old and new number.
  MOV     CX,10                   ;Shift to left by multiplying
  MUL     CX                      ; last entry by ten.
  ADD     BP,AX                   ;Add new number and store in BP.
  JMP     SHORT NEXT_NUMBER

;---------------------------------------------;
; Check minimum, maximum parameter boundaries ;
;---------------------------------------------;

CK_PARA:     
  CMP     BP,BUFFER_MIN           ;Is it below 16?
  JA      CK_MAX
  MOV     BP,BUFFER_DEFAULT       ;If yes, use default 80.
CK_MAX:      
  CMP     BP,BUFFER_MAX           ;Is it above 200?
  JBE     CK_SEGMENT
  MOV     BP,BUFFER_MAX           ;If yes, use default 80.

;----------------------------------------------------------------;
; Check to see if KBBUFFER.CTL is within 64K range.  If it is,   ;
; change keyboard buffer to point to us, else exit with message. ;
;----------------------------------------------------------------;

CK_SEGMENT:  
  INC     BP                          ;Adjust.
  SHL     BP,1                        ;Convert byte count to word.
  MOV     DX,OFFSET OUT_OF_RANGE_MSG  ;Point to out of range msg.
  MOV     AX,CS                       ;Retrieve our segment.
  SUB     AX,SEG BIOS_DATA            ;Subtract BIOS data segment.
  MOV     CX,4                        ;AX = distance in paragraphs;
PARA_TO_BYTES:
  SHL     AX,1                        ; convert to bytes.
  JC      INIT_END                    ;If > 64K, exit.
  LOOP    PARA_TO_BYTES
  ADD     AX,OFFSET KBBUFFER_CTL_END  ;Add resident portion offset.
  JC      INIT_END                    ;If > 64K, exit.
  MOV     CX,AX
  ADD     CX,BP                       ;Add requested buffer size.
  JC      INIT_END                    ;If > 64K, exit.

IN_RANGE:    
  ADD     ES:ENDING_OFFSET[BX],BP     ;Point to end of resident.
  ASSUME  DS:BIOS_DATA                ;Point to BIOS data area.
  MOV     DX,SEG BIOS_DATA
  MOV     DS,DX

  CLI                                 ;No interrupts.
  MOV     BUFFER_HEAD,AX              ;Change keyboard buffer
  MOV     BUFFER_TAIL,AX              ; pointers to point to us.
  MOV     BUFFER_START,AX
  MOV     BUFFER_END,CX
  STI                                 ;Interrupts back on.

  MOV     DX,OFFSET INSTALLED_MSG     ;Display install msg.

INIT_END:    
  PUSH    CS                          ;Point to our data.
  POP     DS
  CALL    PRINT_STRING                ;Display message.
  RET                                 ;Exit.

INITIALIZE     ENDP

;------------------------------;

PRINT_STRING:
  MOV     AH,9                    ;Print string via DOS.
  INT     21H
  RET

_TEXT          ENDS
               END
