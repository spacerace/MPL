PAGE 60,132
TITLE DISPIO - SUBROUTINE TO PERFORM DIRECT SCREEN I/O
;                         Listing 3
;                  C to Assembly Interface
;   Computer Language, Vol. 2, No. 2 (February, 1985), pp. 49-59
;
;Interfacing C to access architecture dependent capabilities 
;
;
;   change to mono monitor - mono_dsp() ; L.P.
;   change to 80 column color monitor - c80_disp() ; L.P.
;   change to 40 column color monitor - c40_disp() ; L.P.
;   setmode - dispio(set_type) ;  AH = 0, set_type is a video mode in dispio.h
;   set cursor type - dispio(SET_TYPE,type) ;
;   set cursor position - dispio(SET_CUR,page,(row<<8)+col)
;   read cursor position - 
;      dispio(READ_POSITION,page,&start_line,&end_line,&row,&col)    L.P.
;      Function returns (row<<8)+col    L.P.
;   read cursor type - read_typ(page,&start_line,&end_line)  L.P.
;      Function returns (start_line<<8)+end_line    L.P.
;   read cursor location - read_loc(page,&row,&col)    L.P.
;      Function returns (row<<8)+col
;   read light pen position - 
;      dispio(READ_LIGHT_PEN_POSITION,&switch,&pixel,&raster,&row,&col)   L.P.
;      Function returns (row<<8)+col
;   select active page - dispio(SELECT_PAGE+new_page_value) ;
;   scroll active page up - dispio(SCROLL_UP+number_of_lines,fill_attr,
;                                  (lrow<<8)+lcol, (rrow<<8)+rcol) ;
;   scroll active page down - dispio(SCROLL_DN+number_of_lines,fill_attr,
;                                    (lrow<<8)+lcol,(rrow<<8)+rcol) ;
;   get attribute character at current cursor pos - 
;      dispio(READ_ATTRIBUTE_CHAR,page,&attribute,&character) ; L.P.
;      Function returns (attribute<<8)+character
;   write attribute character at current cursor position - 
;      dispio(WRITE_ATTRIBUTE_CHAR+char,(page<<8)+attribute, repeat count);
;      write_ac(attribute,character,page,repeat_count) ; L.P.
;   write character - dispio(WRITE_CHAR+character,page,repeat count) ;
;      write_ch(character,page,repeat_count); L.P.
;   set color palette - dispio(SET_PALETTE,(palette<<8)+color_value) ;
;   write dot - dispio(WRITE_DOT+color,col,row) ;
;   read dot - dispio(READ_DOT,col,row);
;              dot read returned as integer function value  L.P.
;   write teletype - dispio(WRITE_TELETYPE+char,(page<<8)+color) ;
;      writ_tty(character,color,page);
;   get state - dispio(GET_STATE,&modes,&cols,&page)    L.P.
;
;**************************************************************
;
;   Surrounded each INT 10H with a PUSH BP / POP BP pair.  This may not be
;   necessary in every case, but it is for SCROLL, and it makes the others
;   safe.
;
;   L. Paper
;   6/17/85
;
;   Revise documentation for all returned values.  Changed program for all 
;   returns.  Removed right shifts for input to BH with BL zero.  Added the
;   following functions which use separate integer input variables:
;      read_typ, read_loc, write_ac, write_ch, write_tty
;   I also added three functions to change the monitor:
;      mono_dis, c80_disp, c40_disp
;   Note:  I only added the functions which are not in the Greenleaf 
;   Functions.  The combination gives both the original register oriented
;   input, which is useful for repeating parameters, and separate integer
;   input variables, which requires no external manipulation.
;   L. Paper
;   7/8/85
;

mono      equ     00110000b       ;mono equip_flag             ;L.P.
c80       equ     00100000b       ;80 column color equip_flag  ;L.P.
c40       equ     00010000b       ;40 column color equip_flag  ;L.P.

          ;; set starting video mode to flag                       ;L.P.
          ;; set mode to mode                                      ;L.P.
switch    MACRO   flag,mode                                        ;L.P.
          pop     si                                               ;L.P.
          push    ds               ;save original data segment     ;L.P.
          mov     ax,40h           ;set DS to ROM BIOS data areas  ;L.P.
          mov     ds,ax            ;                               ;L.P.
          mov     bx,10h           ;offset of equip_flag           ;L.P.
      if flag - mono               ;; color monitor                ;L.P.
          mov     ax,[bx]         ;get old equip_flag              ;L.P.
          and     al,11001111b    ;clear equip_flag                ;L.P.
          or      al,flag          ;set new equip_flag             ;L.P.
          mov     [bx],ax         ;save new equip_flag             ;L.P.
      endif                                                        ;L.P.
      ife flag - mono              ;; mono monitor                 ;L.P.
          mov      ax,flag                                         ;L.P.
          or       [bx],ax        ;set new equip_flag              ;L.P.
      endif                                                        ;L.P.
          pop      ds              ;restore original data segment  ;L.P.
      if mode                                                      ;L.P.
          mov      ax,mode         ;0 ==> ah to set mode           ;L.P.
      endif                                                        ;L.P.
      ife mode                                                     ;L.P.
          xor      ax,ax ;0 ==> ah to set mode, 0 ==> al for 40 col;L.P.
      endif                                                        ;L.P.
          jmp      setmode                                        ;L.P.
          ENDM                                                     ;L.P.


popshift   macro                    ;; int input variable to bh, bl=0  L.P.
           pop     bx               ;                                  L.P.
           mov     bh,bl            ; effectively shift right 8        L.P.
           xor     bl,bl            ; 0==>bl                           L.P.
           endm
           
rcfstart   macro              ;; READ_CURSOR_POSITION functions start
           POP    SI          ;             L.P.
           MOV    AH,3        ;             L.P.
           XOR    AL,AL       ;0==>AL       L.P.
           rcstart
           endm
           
rcstart    macro              ;; READ_CURSOR_POSITION start
           popshift           ;page          L.P. 
           PUSH   BP          ;              L.P.
           INT    10H
           POP    BP          ;              L.P.
           endm
           
outfirst   macro   source,dest      ;; Return first variable to caller  L.P.
           xor      dest&h,dest&h   ; Zero for integer return   L.P.
           outlater source,dest       ;; L.P.
           endm
        
outlater   macro   source,dest     
           pop     di               ; L.P.
           mov     dest&l,source    ; L.P.
           mov     [di],dest&x      ; L.P.
           endm

DGROUP  GROUP  DATA
DATA    SEGMENT WORD PUBLIC 'DATA'
        ASSUME  DS:DGROUP
CASE_TABLE DW SETMODE,CURSTYPE,SETCURS,READCURS,READPEN,SETMODE
           DW SCROLL,SCROLL,READAC,WRITEAC,WRITECH,SETPAL,WRITEDOT
           DW WRITEDOT,SETPAL,STATE

MAX_IN     EQU ($ - CASE_TABLE - 2) SHR 1

; case table uses 16 words.  
;
DATA    ENDS
;
PGROUP  GROUP  PROG
PROG    SEGMENT  BYTE  PUBLIC 'PROG'
        ASSUME   CS:PGROUP
        PUBLIC   DISPIO, READ_TYP, READ_LOC, WRITE_AC, WRITE_CH, WRIT_TTY
        PUBLIC   MONO_DSP, C80_DISP, C40_DISP
;
DISPIO  PROC    NEAR
        POP     SI
        POP     AX
        MOV     BL,AH
        XOR     BH,BH
        CMP     BL,MAX_IN         ; INVALID FUNCTION CODE ?
        JA      ERROR
        SHL     BX,1
        JMP     CASE_TABLE[BX]
;
ERROR:  MOV     AX,-1
        JMP     SI
;
SETCURS:   popshift           ;page
           POP    DX
SETMODE:   PUSH   BP          ;L.P.
           INT    10H
           POP    BP          ;L.P.
           JMP    SI
;
READCURS:  rcstart
           outfirst ch,a           ;start_line
           outlater cl,a           ;end_line
           outlater dh,a           ;row
           outlater dl,a           ;col
           MOV    AX,DX            ;FUNCTION VALUE   L.P.
           JMP    SI
;
SCROLL:    popshift           ;fill_attr
WRITEDOT:  POP    CX
           POP    DX
           PUSH   BP          ;L.P.
           INT    10H
           POP    BP          ;L.P.
           XOR    AH,AH       ;0==>AH FOR READ DOT INTEGER VALUE   L.P.
           JMP    SI
;
READAC:    popshift           ;page
           PUSH   BP          ;L.P.
           INT    10H
           POP    BP          ;L.P.
           outfirst ah,b      ;attribute
           outlater al,b      ;character
           JMP    SI
;
WRITECH:   popshift            ; page
           JMP    CURSTYPE     ; L.P.
           
WRITEAC:   POP    BX
CURSTYPE:  POP    CX
           JMP    SETMODE       ; Finish  L.P.
;
STATE:     PUSH   BP          ;L.P.
           INT    10H
           POP    BP          ;L.P.
           outfirst al,c      ;mode
           outlater ah,c      ;cols
           outlater bh,c      ;page
           JMP    SI
;
READPEN:   PUSH   BP          ;L.P.
           INT    10H
           POP    BP          ;L.P.
           POP    DI          ; SWITCH    L.P.
           MOV    AL,AH       ;L.P.
           XOR    AH,AH       ;L.P.
           MOV    [DI],AX     ;L.P.
           POP    DI          ;PIXEL  L.P.
           MOV    [DI],BX     ;L.P.
           outlater ch,a,raster
           outlater dh,a,row
           outlater dl,a,col
           JMP    SI
;
READ_TYP:  rcfstart
           outfirst ch,a           ; start_line
           outlater cl,a           ; end_line
           MOV    AX,CX            ; FUNCTION VALUE    L.P.
           JMP    SI
;
READ_LOC:  rcfstart
           outfirst dh,a       ; row
           outlater dl,a       ; col
           MOV    AX,DX        ; FUNCTION VALUE    L.P.
           JMP    SI
;
SETPAL:    POP    BX
           JMP    SETMODE      ; FINISH      L.P.
;
WRITE_CH:  POP    SI            ;                     L.P.
           XOR    BL,BL         ;0 ==> BL             L.P.
           POP    AX            ;CHARACTER ==> AL     L.P.
           MOV    AH,10         ;                     L.P.
           JMP    WRITE_1       ;CONTINUE             L.P.
;
WRITE_AC:  POP    SI            ;L.P.
           POP    BX            ;ATTRIBUTE ==> BL     L.P.
           POP    AX            ;CHARACTER ==> AL     L.P.
           MOV    AH,9          ;                     L.P.
WRITE_1:   POP    CX            ;PAGE ==> CL          L.P.
           MOV    BH,CL         ;PAGE ==> BH          L.P.
           POP    CX            ;REPEAT_COUNT ==> CX  L.P.
           JMP    SETMODE       ;FINISH               L.P.
;
WRIT_TTY:  POP    SI            ;                            L.P.
           POP    AX            ;CHARACTER ==> AL            L.P.
           MOV    AH,0EH        ;                            L.P
           POP    BX            ;FOREGROUND COLOR ==> BL     L.P.
           POP    CX            ;PAGE ==> CX                 L.P.
           MOV    BH,CL         ;PAGE ==> BH                 L.P.
           JMP    SETMODE       ;FINISH
;
MONO_DSP: switch   mono,7          ; CHANGE TO MONOCHROME DISPLAY
;
C80_DISP: switch   c80,2           ; CHANGE TO 80 COLUMN COLOR DISPLAY
;
C40_DISP: switch   c40,0           ; CHANGE TO 40 COLUMN COLOR DISPLAY
;
DISPIO  ENDP
PROG    ENDS
        END
