
;                         Listing 3
;                  C to Assembly Interface
;   Computer Language, Vol. 2, No. 2 (February, 1985), pp. 49-59

;Interfacing C to access architecture dependent capabilities 

TITLE DISPIO - SUBROUTINE TO PERFORM DIRECT SCREEN I/O
;
;  #define MAX 8
;  char retvals [MAX] /* used by 3 functions */
;           /* (1) - read cursor position; retvals[0-1] = current cursor mode */
;           /*         retvals[3] = col, retvals[2] = row */
;           /* (2) - read light pen position  */
;           /*         retvals[0] = active(1)/inactive(0) */
;           /*         retvals[2-3] = pixel column, retvals[4]=raster line */
;           /*         retvals[6] = row, retvals[7] = col */
;           /* (3) - get state; retvals[0] = cols, retvals[1] = mode */
;           /*         retvals[2] = page */ 
;
;   setmode - dispio(mode) ;
;   set cursor type - dispio(set_curs_type,type) ;
;   set cursor position - dispio(set_cursor,page<<8,row<<8+col) ;
;   read cursor position - dispio(read_cursor_position,page<<8,retvals) ;
;   read light pen position - dispio(read_pen,retvals) ;
;   select active page - dispio(select_page+new_page_value) ;
;   scroll active page up - dispio(scroll_up+number_of_lines,page<<8,
;                                  lrow<<8+lcol, rrow<<8+rcol) ;
;   scroll active page down - dispio(scroll_down+number_of_lines,page<<8,
;                                    lrow<<8+lcol,rrow<<8+rcol) ;
;   get attribute character at current cursor pos - dispio(readac,page<<8) ;
;     returns attribute character as function value - high byte attribute,
;     low byte character
;   write attribute character at current cursor position - dispio(writeac+char,
;            page<<8+attribute, repeat count)
;   write character - dispio(write_char+character,page<<8,repeat count) ;
;   set color palette - dispio(set_pallette,pallette<<8+color_value) ;
;   write dot - dispio(write_dot+color,col,row) ;
;   read dot - dispio(read_dot,col,row)
;              dot read returned as low byte of function value
;   write teletype - dispio(write_teletype+char,(page<<8)+color) ;
;   get state - dispio(get_state,retval) 
;
DGROUP  GROUP  DATA
DATA    SEGMENT WORD PUBLIC 'DATA'
        ASSUME  DS:DGROUP
CASE_TABLE DW SETMODE,CURSTYPE,SETCURS,READCURS,READPEN,SETMODE
           DW SCROLL,SCROLL,READAC,WRITEAC,WRITEAC,READAC,WRITEDOT
           DW WRITEDOT,READAC,STATE
; case table uses 16 words
RLAYOUT  STRUC        ; Return address space layout
RAX     DW    ?       
RBX     DW    ?
RCX     DW    ?
RDX     DW    ?
RLAYOUT ENDS
DATA    ENDS
;
PGROUP  GROUP  PROG
PROG    SEGMENT  BYTE  PUBLIC 'PROG'
        ASSUME   CS:PGROUP
        PUBLIC   DISPIO
;
DISPIO  PROC    NEAR
        POP     SI
        POP     AX
        MOV     BL,AH
        XOR     BH,BH
        CMP     BL,15             ; INVALID FUNCTION CODE ?
        JA      ERROR
        SHL     BX,1
        JMP     CASE_TABLE[BX]
;
ERROR:  MOV     AX,-1
        JMP     SI
;
SETCURS:   POP    BX
           POP    DX
SETMODE:   INT    10H
           JMP    SI
;
READCURS:  POP    BX
           INT    10H
           POP    DI
           MOV    [DI].RAX,CX
           MOV    [DI].RBX,DX
           JMP    SI
;
SCROLL:    POP    BX
WRITEDOT:  POP    CX
           POP    DX
           INT    10H
           JMP    SI
;
READAC:    POP    BX
           INT    10H
           JMP    SI
;
WRITEAC:   POP    BX
CURSTYPE:  POP    CX
           INT    10H
           JMP    SI
;
STATE:     INT    10H
           POP    DI
           MOV    [DI].RAX,AX
           MOV    [DI].RBX,BX
           JMP    SI
;
READPEN:   INT    10H
           POP    DI
           MOV    [DI].RAX,AX
           MOV    [DI].RBX,BX
           MOV    [DI].RCX,CX
           MOV    [DI].RDX,DX
           JMP    SI
DISPIO  ENDP
PROG    ENDS
        END
