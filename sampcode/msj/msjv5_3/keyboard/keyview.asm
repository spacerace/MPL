               TITLE   KEYVIEW.ASM
               PAGE    60,132

;-----------------------------------------------;
;  KeyView - Visual display of keyboard buffer. ;
;  Michael J. Mefford                           ;
;-----------------------------------------------;

BIOS_DATA      SEGMENT AT 40H

               ORG     1AH

BUFFER_HEAD    DW      ?
BUFFER_TAIL    DW      ?

               ORG     71H

BIOS_BREAK     DB      ?

               ORG     80H

KBD_BUFF_START DB      ?

BIOS_DATA      ENDS


_TEXT          SEGMENT PUBLIC 'CODE'
               ASSUME  CS:_TEXT
               ASSUME  DS:_TEXT

               ORG     100H

START:         JMP     MAIN

;              DATA AREA
;              ---------
               DB      CR,SPACE,SPACE,SPACE,CR,LF

COPYRIGHT      DB      "KEYVIEW 1.0 (c) 1990 "
               DB      "Michael J. Mefford",CR,LF,LF,"$"
               DB      CTRL_Z

CR             EQU     13
LF             EQU     10
CTRL_Z         EQU     26
SPACE          EQU     32
BOX            EQU     254

BUFFER_START   EQU     1EH
BUFFER_END     EQU     3EH

UP_ARROW       EQU     24
DN_ARROW       EQU     25

TRUE           EQU     1
FALSE          EQU     0

BOX_ROW        EQU     8
BOX_COL        EQU     7
BOX_TWO        EQU     6
CHAR_START     EQU     1400H

ESC_SCAN_CODE  EQU     1
F1_SCAN_CODE   EQU     3BH
F2_SCAN_CODE   EQU     3CH

PORT_A         EQU     60H
PORT_B         EQU     61H
COMMAND_PORT   EQU     20H
EOI            EQU     20H

;-------------------------;

BIOS_INT_9     DW      ?,?

NORMAL         EQU     07H
INVERSE_BLUE   EQU     17H
INVERSE        DB      70H

LAST_POS       DW      CHAR_START

LAST_HEAD      DW      BUFFER_START
LAST_TAIL      DW      BUFFER_START

F1_FLAG        DB      FALSE
ESC_FLAG       DB      FALSE

EXTENDED_CALL  EQU     10H
KEY_SUPPORT    DB      0
SUPPORT_FLAG   DB      0

HEAD           DB      "Head",DN_ARROW
TAIL           DB      "Tail",UP_ARROW
ERASE          DB      5 DUP (SPACE)

INVALID_MSG    DB      "Keyboard Buffer not supported",CR,LF,"$"

MENU           LABEL   BYTE

DB   "Press any key to add to keyboard buffer",CR,LF
DB   "Press F1 to retrieve a character from buffer",CR,LF,"$"

INT_16_MSG     DB      "Press F2 to toggle extended keyboard "
               DB      "support; Support is now: ",CR,LF,"$"
INT_16_LEN     EQU     $ - INT_16_MSG - 3

ESC_MSG        DB      "Press Esc to Exit$"

INACTIVE_MSG   DB      "INACTIVE  "
INACTIVE_LEN   EQU     $ - INACTIVE_MSG - 2

BUFFER_WINDOW  LABEL   BYTE

DB             201,7 DUP(7 DUP(205),203),7 DUP(205),187
DB             2 DUP(186,7 DUP(7 DUP(32),186),7 DUP(32),186)
DB             200,7 DUP(7 DUP(205),202),7 DUP(205),188


;              CODE AREA
;              ---------
MAIN           PROC    NEAR

;--------------------------------------;
; Exit with non-support message if     ;
; original keyboard buffer not active. ;
;--------------------------------------;

  MOV     AX,SEG BIOS_DATA        ;Point to BIOS data.
  MOV     ES,AX
  CMP     ES:KBD_BUFF_START,BUFFER_START   ;Original buffer active?
  JZ      GOOD_BUFFER                      ;If yes, continue.
  MOV     DX,OFFSET INVALID_MSG   ;Else, display invalid message.
  CALL    PRINT_STRING
  MOV     AX,4C01H                ;Exit with error level 1.
  JMP     TERMINATE

GOOD_BUFFER:   
  CLD                             ;All string moves forward.
  CALL    VIDEO                   ;Check the video equip. and CLS.

  MOV     DX,OFFSET COPYRIGHT     ;Display copyright.
  CALL    PRINT_STRING
  MOV     DX,OFFSET MENU
  CALL    PRINT_STRING

;----------------------------------------------------------;
; BIOS Tech Ref recommended 101-keyboard detection method. ;
;----------------------------------------------------------;

  MOV     DL,2                    ;Make two attempts to write
  MOV     CX,0FFFFH               ; char/scan code of FFFFh to
KBD_WRITE:   
  MOV     AH,05H                  ; buffer via extended keyboard
  INT     16H                     ; write funtion.
  OR      AL,AL                   ;Was it successful?
  JZ      RETRIEVE                ;If yes, extended supported.
  MOV     AH,10H                  ;Else, maybe buffer full. Make
  INT     16H                     ; room by retrieving char.
  DEC     DL                      ;Is this the second time through?
  JNZ     KBD_WRITE               ;If no, try to write again.
  JMP     SHORT MENU_END          ;Else, done here; no support.

RETRIEVE:   
  MOV     CX,15                   ;Try 15 times to retrieve
SEARCH_KBD:  
  MOV     AH,11H                  ; the FFFFh scan code.
  INT     16H                     ;Buffer empty?
  JZ      MENU_END                ;If yes, no support.
  MOV     AH,10H                  ;Else, extended Kbd read.
  INT     16H
  CMP     AX,0FFFFH               ;Did we find the FFFFh?
  JZ      EXTENDED_KBD            ;If yes, extended Kbd support.
  LOOP    SEARCH_KBD              ;Else, search all 15 possible.
  JMP     SHORT MENU_END          ;If feel through, no support.

;-------------------------------------------------------------;
; If 101-keyboard exists, display additional support message. ;
;-------------------------------------------------------------;

EXTENDED_KBD:
  MOV     SUPPORT_FLAG,1          ;Flag that supported.
  MOV     DX,OFFSET INT_16_MSG    ;Display support message.
  CALL    PRINT_STRING
  CALL    EXTENDED                ;Display "ACTIVE" message.
  MOV     DX,600H                 ;Move cursor to next line.
  CALL    SET_CURSOR

;-----------------------------------------------------;
; Display Esc message regardless of keyboard support. ;
;-----------------------------------------------------;

MENU_END:   
  MOV     DX,OFFSET ESC_MSG       ;Display Esc message.
  CALL    PRINT_STRING

;----------------------------------------------------------;
; Display my visual interpretation of the keyboard buffer. ;
;----------------------------------------------------------;

  MOV     DX,BOX_ROW SHL 8 + BOX_COL  ;Start pos. of first box.
  MOV     BL,INVERSE                  ;Display in inverse color.
  MOV     CX,2                        ;Two boxes to display.

NEXT_BOX:   
  PUSH    CX                          ;Save counter.
  MOV     CX,4                        ;Four rows per box.
  MOV     SI,OFFSET BUFFER_WINDOW     ;Point to buffer box.

NEXT_ROW:    
  PUSH    CX                      ;Save counter.
  MOV     CX,65                   ;65 columns per box.
NEXT_BYTE:   
  LODSB                           ;Get a byte
  CALL    WRITE_CHAR              ; and display it.
  LOOP    NEXT_BYTE               ;Repeat for all 65 columns.
  MOV     DL,BOX_COL              ;Box column start
  INC     DH                      ;Next row.
  POP     CX                      ;Retrieve counter.
  LOOP    NEXT_ROW                ;Repeat for all four rows.

  MOV     DX,(BOX_ROW + BOX_TWO) SHL 8 + BOX_COL ;2nd box start.
  POP     CX                                     ;Retrieve counter.
  LOOP    NEXT_BOX                ;Do both boxes.

  CALL    INITIALIZE              ;Fill the visual buffer.

;-------------------------;
; Hook our INT 9 handler. ;
;-------------------------;

  MOV     AX,3509H                ;Get keyboard interrupt.
  INT     21H
  MOV     BIOS_INT_9[0],BX        ;Save old interrupt.
  MOV     BIOS_INT_9[2],ES

  MOV     DX,OFFSET KEYVIEW_INT_9 ;Install new interrupt.
  MOV     AX,2509H
  INT     21H

  MOV     AX,SEG BIOS_DATA        ;Point to BIOS data area.
  MOV     ES,AX

;--------------------------------------------------------------;
; Ready for action.  Hide the cursor off screen so it's not    ;
; distracting.  Retrieve characters if F1 pressed.  Toggle     ;
; extended support on/off if F2 pressed.  Clear visual buffer  ;
; if Ctrl Break detected.  Exit if Esc pressed.                ;
;--------------------------------------------------------------;

GET_KEY:    
  MOV     DX,1900H                ;Hide the cursor off screen
  CALL    SET_CURSOR              ; on row 25.
CK_F1:       
  CMP     F1_FLAG,TRUE            ;Was F1 pressed?
  JZ      CK_AVAILABLE            ;If yes, retrieve a character.

  MOV     AH,1                    ;This will extract extended
  OR      AH,KEY_SUPPORT          ; codes if extended support
  INT     16H                     ; is not active.

  CMP     SUPPORT_FLAG,2          ;F2 pressed and extended
  JZ      DO_F2                   ; support?  If yes, toggle.

  CMP     ESC_FLAG,TRUE           ;Was Esc pressed?
  JZ      EXIT                    ;If yes, exit.

  TEST    ES:BIOS_BREAK,10000000B ;Was Ctrl Break pressed?
  JNZ     CTRL_BREAK              ;If yes, clear visual buffer.

  CLI                             ;No interrupts.
  MOV     DI,ES:BUFFER_TAIL       ;Retrieve buffer tail and head.
  MOV     SI,ES:BUFFER_HEAD
  STI                             ;Interrupts OK now.

  MOV     BP,LAST_TAIL            ;Retrieve last tail.
  CMP     DI,BP                   ;Has the tail moved?
  JZ      CK_HEAD                 ;If no, check head.
  MOV     LAST_TAIL,DI            ;Else, store new tail.
  MOV     SI,OFFSET TAIL          ;Indicate tail moved
  JMP     SHORT DO_BUFFER         ; and update visual buffer.

CK_HEAD:     
  MOV     BP,LAST_HEAD            ;Retrieve last head.
  MOV     DI,SI                   ;Assume head moved.
  CMP     DI,BP                   ;Has head moved?
  JZ      CK_F1                   ;If no, nothing to do; Check F1.

  MOV     SI,BP                   ;Else, next head position.
  INC     SI
  INC     SI
  CMP     SI,BUFFER_END           ;If moved past end of buffer
  JNZ     STORE_HEAD              ; circle to beginning of buffer.
  MOV     SI,BUFFER_START

STORE_HEAD:  
  MOV     LAST_HEAD,SI            ;Store new head.
  MOV     SI,OFFSET HEAD          ;Indicate head has moved

DO_BUFFER:   
  CALL    UPDATE_BUFFER           ; and update visual buffer.
  JMP     SHORT GET_KEY           ;Next key.

CK_AVAILABLE:
  MOV     F1_FLAG,FALSE           ;Reset F1 flag.
  MOV     AH,1                    ;Get keystroke status
  OR      AH,KEY_SUPPORT          ; add in type keyboard support.
  INT     16H
  JNZ     DO_F1                   ;If key available, get it.

  MOV     AX,0E07H                ;Else, beep speaker.
  INT     10H
  JMP     SHORT CK_F1             ;Done here.

DO_F1:       
  CALL    DISPLAY_CHAR            ;Retrieve character from buffer
  JMP     GET_KEY                 ; and display; next key.

DO_F2:       
  CALL    EXTENDED                ;Toggle extended support.
  MOV     SUPPORT_FLAG,1          ;Reset support flag.
  JMP     GET_KEY                 ;Next key.

CTRL_BREAK:  
  AND     ES:BIOS_BREAK,NOT 80H   ;Reset Ctrl Break bit.
  CALL    INITIALIZE              ;Clear the visual buffer.
  JMP     GET_KEY                 ;Next key.

;---------------------------------------------------------;
; Clear the buffer by setting head = tail.  Clear screen. ;
; Restore INT 9.                                          ;
;---------------------------------------------------------;

EXIT:       
  CLI                                   ;No interrupts.
  MOV     ES:BUFFER_HEAD,BUFFER_START   ;Set head = buffer start.
  MOV     ES:BUFFER_TAIL,BUFFER_START   ;Set tail = buffer start.
  STI                                   ;Interrupts OK now.

  CALL    VIDEO                   ;Clear screen.

  MOV     DX,BIOS_INT_9[0]        ;Restore old INT 9.
  MOV     DS,BIOS_INT_9[2]
  MOV     AX,2509H
  INT     21H

  MOV     AX,4C00H                ;Error level zero.
TERMINATE:
  INT     21H

MAIN           ENDP

               ;***************;
               ;* SUBROUTINES *;
               ;***************;

;-----------------------------------------------------;
; This subroutine will toggle the extended keyboard   ;
; support on/off and display ACTIVE/INACTIVE message. ;
;-----------------------------------------------------;

EXTENDED       PROC    NEAR

  MOV     DX,4 SHL 8 + INT_16_LEN    ;Row 4; column at end of msg.
  MOV     CX,INACTIVE_LEN            ;Length of message.
  MOV     BL,NORMAL                  ;Normal attribute.
  MOV     SI,OFFSET INACTIVE_MSG     ;Assume INACTIVE msg.
  XOR     KEY_SUPPORT,EXTENDED_CALL  ;Toggle support.
  JZ      DISPLAY_EXT                ;If zero, guessed right.
  INC     SI                         ;Else, bump pointer past "IN".
  INC     SI

DISPLAY_EXT:
  LODSB                              ;Display the message.
  CALL    WRITE_CHAR
  LOOP    DISPLAY_EXT
  RET

EXTENDED       ENDP

;------------------------------------------------------;
; All sixteen positions of visual buffer are updated   ;
; when program starts and when Ctrl Break is detected. ;
;------------------------------------------------------;

INITIALIZE     PROC    NEAR

  MOV     BP,LAST_TAIL            ;Retrieve last tail.
  MOV     DI,ES:BUFFER_HEAD       ;Retrieve current head.
  PUSH    DI                      ;Preserve last tail.
  MOV     LAST_TAIL,DI            ;Make last tail = head.
  MOV     SI,OFFSET TAIL          ;Point to tail msg.
  CALL    UPDATE_BUFFER           ;Update the visual buffer.

  MOV     BP,LAST_HEAD            ;Retrieve last head.
  MOV     AX,BP                   ;Save in AX

NEXT_INIT:   
  PUSH    AX                      ;Preserve AX.
  MOV     DI,BP                   ;Move head up one.
  INC     DI
  INC     DI
  CMP     DI,BUFFER_END           ;If head = buffer end
  JNZ     DO_INIT                 ; then head = buffer start.
  MOV     DI,BUFFER_START

DO_INIT:     
  MOV     SI,OFFSET HEAD          ;Point to head msg.
  CALL    UPDATE_BUFFER           ;Update visual buffer.
  MOV     BP,DI
  POP     AX                      ;Retrieve last head.
  CMP     BP,AX                   ;Did we do all 16 positions?
  JNZ     NEXT_INIT               ;If not, continue until done.

  POP     DI                      ;Retrieve last tail.
  MOV     LAST_HEAD,DI            ;Last head = last tail.
  MOV     SI,OFFSET HEAD          ;Point to head msg.
  CALL    UPDATE_BUFFER           ;Update visual buffer.

  RET

INITIALIZE     ENDP

;----------------------------;
; This is the INT 9 handler. ;
;----------------------------;

KEYVIEW_INT_9  PROC    NEAR

  ASSUME  DS:NOTHING
  STI                             ;Interrupts OK.
  PUSH    AX                      ;AX will be destroyed; preserve.
  IN      AL,PORT_A               ;Get the scan code.
  CMP     AL,F1_SCAN_CODE         ;Is it F1 make?
  JZ      RESET_MAKE              ;If yes, flag and reset KBD.
  CMP     AL,F1_SCAN_CODE OR 80H  ;Else, is it F1 break?
  JZ      RESET_BREAK             ;If yes, ignore.
  CMP     AL,ESC_SCAN_CODE        ;Else, is it Esc?
  JZ      ESC_EXIT                ;If yes, flag Esc.
  CMP     SUPPORT_FLAG,1          ;Extended keyboard support?
  JNZ     OLD_INT_9               ;If no, done here.
  CMP     AL,F2_SCAN_CODE OR 80H  ;Else, is it F2 break?
  JZ      RESET_BREAK             ;If yes, ignore.
  CMP     AL,F2_SCAN_CODE         ;Else, is it F2 make?
  JNZ     OLD_INT_9               ;If no, done here.
 
  MOV     SUPPORT_FLAG,2          ;Else, flag pressed.
  JMP     SHORT RESET_BREAK       ;Reset the KBD.

ESC_EXIT:    
  MOV     ESC_FLAG,TRUE           ;If Esc pressed, flag and exit.

OLD_INT_9:   
  POP     AX                      ;Restore AX.
  JMP     DWORD PTR BIOS_INT_9    ;Go to BIOS INT 9 routine.

;------------------------------------------------------;
; If F1 or F2 pressed, don't let BIOS store character. ;
; Instead reset KBD and throw away the keystroke.      ;
;------------------------------------------------------;

RESET_MAKE:  
  MOV     F1_FLAG,TRUE            ;Flag F1 pressed.
RESET_BREAK: 
  IN      AL,PORT_B               ;Retrieve Port B.
  OR      AL,80H                  ;Turn bit 7 on to reset
  JMP     $ + 2                   ;I/O delay.
  OUT     PORT_B,AL               ;Reset KBD.
  AND     AL,NOT 80H              ;Turn bit 7 back off.
  JMP     $ + 2                   ;I/O delay.
  OUT     PORT_B,AL               ;Restore port.

  CLI                             ;Interrupts off.
  MOV     AL,EOI                  ;Send End Of Interrupt
  OUT     COMMAND_PORT,AL         ; to 8259A PIC.

  POP     AX                      ;Restore AX.
  IRET                            ;Interrupt return.
  ASSUME  DS:_TEXT

KEYVIEW_INT_9  ENDP

;-----------------------------------------------;
; When F1 is pressed, a character is retrieved  ;
; from the buffer and displayed.                ;
;-----------------------------------------------;

DISPLAY_CHAR   PROC    NEAR

  MOV     AX,LAST_POS             ;Get last char display column.
  CMP     AL,78                   ;Was it column 78?
  JBE     RESTORE_POS             ;If below or equal, OK.
  MOV     CX,CHAR_START           ;Else, clear the line.
  MOV     DX,CHAR_START + 78
  MOV     BH,NORMAL
  MOV     AX,600H
  INT     10H
  MOV     AX,CHAR_START           ;Start at beginning of line.

RESTORE_POS: 
  MOV     DX,AX                   ;Cursor position in DX.
  INC     AX                      ;Bump to new cursor position
  MOV     LAST_POS,AX             ; and save for next time.
  MOV     AH,0                    ;Retrieve character; include
  OR      AH,KEY_SUPPORT          ; appropriate keyboard support.
  INT     16H
  MOV     BL,INVERSE              ;Display in inverse video.
  CALL    WRITE_CHAR
  RET

DISPLAY_CHAR   ENDP

;------------------------------------;
; INPUT                              ;
;   BP = Last position.              ;
;   DI = New position.               ;
;   SI = OFFSET HEAD or OFFSET TAIL. ;
;------------------------------------;

UPDATE_BUFFER  PROC    NEAR

  MOV     DX,BP                   ;Last position in DX.
  CALL    CURSOR_POS              ;Calculate cursor position.
  PUSH    DX                      ;Preserve cursor position.

  ADD     DH,2                    ;Move two columns right.
  MOV     AX,ES:[BP]              ;Retrieve last scan/char code.
  MOV     BL,INVERSE              ;Display in inverse
  CMP     SI,OFFSET TAIL          ; if tail moved.
  JZ      DO_CHAR
  MOV     BL,NORMAL               ;Else, head moved;display normal.

DO_CHAR:    
  PUSH    DX                      ;Save cursor position.
  CALL    SPACES                  ;Center char in top of box with
  CALL    WRITE_CHAR              ; three spaces on either side.
  CALL    SPACES

  POP     DX                      ;Retrieve cursor position.
  INC     DH                      ;Next row.
  XCHG    AL,AH                   ;Swap scan code and ASCII char.
  PUSH    AX                      ;Preserve.
  MOV     AL,SPACE                ;Display a space.
  CALL    WRITE_CHAR
  POP     BP                      ;Retrieve char/scan code.
  CALL    HEX_OUTPUT              ;Display in hex.

  POP     DX                      ;Retrieve cursor position.
  INC     DL                      ;Next column.
  MOV     BL,NORMAL               ;Normal attribute.
  CMP     SI,OFFSET HEAD          ;Erase either head or tail msg.
  JZ      DO_ERASE
  ADD     DH,5                    ;Tail at bottom of box.

DO_ERASE:    
  PUSH    SI                      ;Save msg pointer.
  MOV     SI,OFFSET ERASE         ;Erase old msg.
  CALL    DISPLAY_POINT
  MOV     DX,DI                   ;Move to new position.
  CALL    CURSOR_POS
  INC     DL                      ;Move right one column.
  POP     SI
  CMP     SI,OFFSET HEAD          ;Display either head or tail msg.
  JZ      DO_POINTER
  ADD     DH,5
DO_POINTER:  
  CALL    DISPLAY_POINT

  RET

UPDATE_BUFFER  ENDP

;--------------------------;

SPACES         PROC    NEAR

  PUSH    AX                      ;Preserve AX.
  MOV     CX,3                    ;Display three spaces.
  MOV     AL,SPACE
NEXT_SPACE:  
  CALL    WRITE_CHAR
  LOOP    NEXT_SPACE
  POP     AX
  RET

SPACES         ENDP

;-----------------------------;
; INPUT                       ;
;   BP = Character/scan code. ;
;-----------------------------;

HEX_OUTPUT     PROC    NEAR

  MOV     CX,2                    ;Two codes to display.
NEXT_NIBBLE:
  PUSH    CX                      ;Preserve counter.
  MOV     CX,204H                 ;4 positions/word; 4bits/char.
ROTATE_HEX:  
  ROL     BP,CL                   ;Move highest bits to lowest.
  MOV     AX,BP                   ;Char/scan code in AX.
  AND     AL,1111B                ;Mask off all but four lowest.
  ADD     AL,"0"                  ;Convert to ASCII.
  CMP     AL,"9"                  ;Is it alpha?
  JLE     PRINT_HEX               ;If no, print it.
  ADD     AL,7                    ;Else, adjust.
PRINT_HEX:   
  CALL    WRITE_CHAR              ;And write them.
  DEC     CH                      ;Done all four positions?
  JNZ     ROTATE_HEX              ;If no, get next.

  MOV     AL,SPACE                ;Delimit with a space.
  CALL    WRITE_CHAR
  POP     CX                      ;Do both char and scan code.
  LOOP    NEXT_NIBBLE
  RET

HEX_OUTPUT     ENDP

;------------------------------;
; INPUT                        ;
;   SI points to pointer text. ;
;------------------------------;

DISPLAY_POINT  PROC    NEAR

  MOV     CX,5                    ;Five characters to pointer.
NEXT_POINT:  
  LODSB
  CALL    WRITE_CHAR              ;Write them.
  LOOP    NEXT_POINT
  RET

DISPLAY_POINT  ENDP

;--------------------------;

CURSOR_POS     PROC    NEAR

  SUB     DX,BUFFER_START         ;Difference = offset.
  MOV     DH,BOX_ROW - 1          ;Point to starting row of box.
  CMP     DL,16                   ;If offset less than 16 OK.
  JB      GET_COLUMN
  SUB     DL,16                   ;Else adjust offset.
  ADD     DH,BOX_TWO              ;Point to second box.
GET_COLUMN:  
  SHL     DL,1                    ;Multiply by 4 to get cursor
  SHL     DL,1                    ; column.
  ADD     DL,BOX_COL + 1          ;Add offset of box start.
  RET

CURSOR_POS     ENDP

;-------------------------;
; INPUT                   ;
;   DX = cursor position. ;
;-------------------------;

SET_CURSOR:  
  PUSH    AX                      ;Preserve AX.
  XOR     BH,BH                   ;Page zero.
  MOV     AH,2
  INT     10H                     ;Set cursor position.
  POP     AX                      ;Retrieve AX.
  RET

;-----------------------------;
; INPUT                       ;
;   DX = new cursor position. ;
;   AL = Character to write.  ;
;-----------------------------;

WRITE_CHAR     PROC    NEAR

  PUSH    AX                      ;Preserve AX and CX.
  PUSH    CX
  CALL    SET_CURSOR              ;Set cursor position.
  MOV     CX,1                    ;One character to write.
  MOV     AH,9
  INT     10H
  INC     DL
  POP     CX                      ;Restore registers.
  POP     AX
  RET

WRITE_CHAR     ENDP


;--------------------------;

VIDEO          PROC    NEAR

  MOV     AH,0FH                  ;Retrieve current video mode.
  INT     10H
  CMP     AL,7                    ;Is it mono?
  JZ      CLEAR_SCREEN            ;If yes, clear screen.
  CMP     AL,2                    ;Is it black and white CGA?
  JZ      CLEAR_SCREEN            ;If yes, clear screen.

  MOV     INVERSE,INVERSE_BLUE    ;Else, use color attributes.
  MOV     AL,3                    ; and video mode CO80.

CLEAR_SCREEN:
  XOR     AH,AH                   ;Set video mode.
  INT     10H
  MOV     AX,500H                 ;Set page zero.
  INT     10H
  RET

VIDEO          ENDP

;--------------------------;

PRINT_STRING:
  MOV     AH,9                    ;Print string via DOS.
  INT     21H
  RET


_TEXT          ENDS
               END     START
