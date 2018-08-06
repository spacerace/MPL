        TITLE   CH1.ASM

; CH1.ASM -- support file for CTERM.C terminal emulator
;       set up to work with COM2
;       for use with Microsoft C and SMALL model only...

_TEXT   segment byte public 'CODE'
_TEXT   ends
_DATA   segment byte public 'DATA'
_DATA   ends
CONST   segment byte public 'CONST'
CONST   ends
_BSS    segment byte public 'BSS'
_BSS    ends

DGROUP  GROUP   CONST, _BSS, _DATA
        assume  cs:_TEXT, DS:DGROUP, ES:DGROUP, SS:DGROUP

_TEXT   segment

        public  _i_m,_rdmdm,_Send_Byte,_wrtmdm,_set_mdm,_u_m

bport   EQU     02F8h           ; COM2 base address, use 03F8H for COM1
getiv   EQU     350Bh           ; COM2 vectors, use 0CH for COM1
putiv   EQU     250Bh
imrmsk  EQU     00001000b       ; COM2 mask, use 00000100b for COM1
oiv_o   DW      0               ; old int vector save space
oiv_s   DW      0

bf_pp   DW      in_bf           ; put pointer (last used)
bf_gp   DW      in_bf           ; get pointer (next to use)
bf_bg   DW      in_bf           ; start of buffer
bf_fi   DW      b_last          ; end of buffer

in_bf   DB      512 DUP (?)     ; input buffer

b_last  EQU     $               ; address just past buffer end

bd_dv   DW      0417h           ; baud rate divisors (0=110 bps)
        DW      0300h           ; code 1 =  150 bps
        DW      0180h           ; code 2 =  300 bps
        DW      00C0h           ; code 3 =  600 bps
        DW      0060h           ; code 4 = 1200 bps
        DW      0030h           ; code 5 = 2400 bps
        DW      0018h           ; code 6 = 4800 bps
        DW      000Ch           ; code 7 = 9600 bps

_set_mdm proc   near            ; replaces BIOS 'init' function
        PUSH    BP
        MOV     BP,SP           ; establish stackframe pointer
        PUSH    ES              ; save registers
        PUSH    DS
        MOV     AX,CS           ; point them to CODE segment
        MOV     DS,AX
        MOV     ES,AX
        MOV     AH,[BP+4]       ; get parameter passed by C
        MOV     DX,BPORT+3      ; point to Line Control Reg
        MOV     AL,80h          ; set DLAB bit (see text)
        OUT     DX,AL
        MOV     DL,AH           ; shift param to BAUD field
        MOV     CL,4
        ROL     DL,CL
        AND     DX,00001110b    ; mask out all other bits
        MOV     DI,OFFSET bd_dv
        ADD     DI,DX           ; make pointer to true divisor
        MOV     DX,BPORT+1      ; set to high byte first
        MOV     AL,[DI+1]
        OUT     DX,AL           ; put high byte into UART
        MOV     DX,BPORT        ; then to low byte
        MOV     AL,[DI]
        OUT     DX,AL
        MOV     AL,AH           ; now use rest of parameter
        AND     AL,00011111b    ; to set Line Control Reg
        MOV     DX,BPORT+3
        OUT     DX,AL
        MOV     DX,BPORT+2      ; Interrupt Enable Register
        MOV     AL,1            ; Receive type only
        OUT     DX,AL
        POP     DS              ; restore saved registers
        POP     ES
        MOV     SP,BP
        POP     BP
        RET
_set_mdm endp

_wrtmdm proc    near            ; write char to modem
_Send_Byte:                     ; name used by main program
        PUSH    BP
        MOV     BP,SP           ; set up pointer and save regs
        PUSH    ES
        PUSH    DS
        MOV     AX,CS
        MOV     DS,AX
        MOV     ES,AX
        MOV     DX,BPORT+4      ; establish DTR, RTS, and OUT2
        MOV     AL,0Bh
        OUT     DX,AL
        MOV     DX,BPORT+6      ; check for on line, CTS
        MOV     BH,30h
        CALL    w_tmr
        JNZ     w_out           ; timed out
        MOV     DX,BPORT+5      ; check for UART ready
        MOV     BH,20h
        CALL    w_tmr
        JNZ     w_out           ; timed out
        MOV     DX,BPORT        ; send out to UART port
        MOV     AL,[BP+4]       ; get char passed from C
        OUT     DX,AL
w_out:  POP     DS              ; restore saved regs
        POP     ES
        MOV     SP,BP
        POP     BP
        RET
_wrtmdm endp

_rdmdm  proc    near            ; reads byte from buffer
        PUSH    BP
        MOV     BP,SP           ; set up ptr, save regs
        PUSH    ES
        PUSH    DS
        MOV     AX,CS
        MOV     DS,AX
        MOV     ES,AX
        MOV     AX,0FFFFh       ; set for EOF flag
        MOV     BX,bf_gp        ; use "get" ptr
        CMP     BX,bf_pp        ; compare to "put"
        JZ      nochr           ; same, empty
        INC     BX              ; else char available
        CMP     BX,bf_fi        ; at end of bfr?
        JNZ     noend           ; no
        MOV     BX,bf_bg        ; yes, set to beg
noend:  MOV     AL,[BX]         ; get the char
        MOV     bf_gp,BX        ; update "get" ptr
        INC     AH              ; zero AH as flag
nochr:  POP     DS              ; restore regs
        POP     ES
        MOV     SP,BP
        POP     BP
        RET
_rdmdm  endp

w_tmr   proc    near
        MOV     BL,1            ; wait timer, double loop
w_tm1:  SUB     CX,CX           ; set up inner loop
w_tm2:  IN      AL,DX           ; check for requested response
        MOV     AH,AL           ; save what came in
        AND     AL,BH           ; mask with desired bits
        CMP     AL,BH           ; then compare
        JZ      w_tm3           ; got it, return with ZF set
        LOOP    w_tm2           ; else keep trying
        DEC     BL              ; until double loop expires
        JNZ     w_tm1
        OR      BH,BH           ; timed out, return NZ
w_tm3:  RET
w_tmr   endp

; hardware interrupt service routine
rts_m:  CLI
        PUSH    DS              ; save all regs
        PUSH    AX
        PUSH    BX
        PUSH    CX
        PUSH    DX
        PUSH    CS              ; set DS same as CS
        POP     DS
        MOV     DX,BPORT        ; grab the char from UART
        IN      AL,DX
        MOV     BX,bf_pp        ; use "put" ptr
        INC     BX              ; step to next slot
        CMP     BX,bf_fi        ; past end yet?
        JNZ     nofix           ; no
        MOV     BX,bf_bg        ; yes, set to begin
nofix:  MOV     [BX],AL         ; put char in buffer
        MOV     bf_pp,BX        ; update "put" ptr
        MOV     AL,20h          ; send EOI to 8259 chip
        OUT     20h,AL
        POP     DX              ; restore regs
        POP     CX
        POP     BX
        POP     AX
        POP     DS
        IRET

_i_m    proc    near            ; install modem service
        PUSH    BP
        MOV     BP,SP           ; save all regs used
        PUSH    ES
        PUSH    DS
        MOV     AX,CS           ; set DS,ES=CS
        MOV     DS,AX
        MOV     ES,AX
        MOV     DX,BPORT+1      ; Interrupt Enable Reg
        MOV     AL,0Fh          ; enable all ints now
        OUT     DX,AL

im1:    MOV     DX,BPORT+2      ; clear junk from UART
        IN      AL,DX           ; read IID reg of UART
        MOV     AH,AL           ; save what came in
        TEST    AL,1            ; anything pending?
        JNZ     im5             ; no, all clear now
        CMP     AH,0            ; yes, Modem Status?
        JNZ     im2             ; no
        MOV     DX,BPORT+6      ; yes, read MSR to clear
        IN      AL,DX
im2:    CMP     AH,2            ; Transmit HR empty?
        JNZ     im3             ; no (no action needed)
im3:    CMP     AH,4            ; Received Data Ready?
        JNZ     im4             ; no
        MOV     DX,BPORT        ; yes, read it to clear
        IN      AL,DX
im4:    CMP     AH,6            ; Line Status?
        JNZ     im1             ; no, check for more
        MOV     DX,BPORT+5      ; yes, read LSR to clear
        IN      AL,DX
        JMP     im1             ; then check for more

im5:    MOV     DX,BPORT+4      ; set up working conditions
        MOV     AL,0Bh          ; DTR, RTS, OUT2 bits
        OUT     DX,AL
        MOV     AL,1            ; enable RCV interrupt only
        MOV     DX,BPORT+1
        OUT     DX,AL
        MOV     AX,GETIV        ; get old int vector
        INT     21h
        MOV     oiv_o,BX        ; save for restoring later
        MOV     oiv_s,ES
        MOV     DX,OFFSET rts_m ; set in new one
        MOV     AX,PUTIV
        INT     21h
        IN      AL,21h          ; now enable 8259 PIC
        AND     AL,NOT IMRMSK
        OUT     21h,AL
        MOV     AL,20h          ; then send out an EOI
        OUT     20h,AL
        POP     DS              ; restore regs
        POP     ES
        MOV     SP,BP
        POP     BP
        RET
_i_m    endp

_u_m    proc    near            ; uninstall modem service
        PUSH    BP
        MOV     BP,SP           ; save registers
        IN      AL,21h          ; disable COM int in 8259
        OR      AL,IMRMSK
        OUT     21h,AL
        PUSH    ES
        PUSH    DS
        MOV     AX,CS           ; set same as CS
        MOV     DS,AX
        MOV     ES,AX
        MOV     AL,0            ; disable UART ints
        MOV     DX,BPORT+1
        OUT     DX,AL
        MOV     DX,oiv_o        ; restore original vector
        MOV     DS,oiv_s
        MOV     AX,PUTIV
        INT     21h
        POP     DS              ; restore registers
        POP     ES
        MOV     SP,BP
        POP     BP
        RET
_u_m    endp

_TEXT   ends

        END
