title IBM PC Communications I/O Routines -- Curt Klinsing
pgroup group prog
prog segment byte public 'prog'
;
;A set of Lattice C callable functions to support
;interrupt driven character I/O on the IBM PC. Input
;is buffered, output is polled.
;
public init_comm ;initialize the comm port,
public uninit_comm ;remove initialization,
public set_xoff  ;enable/disable XON/XOFF,
public get_xoff  ;read XON/XOFF state,
public rcvd_xoff ;returns true if XOFF rcvd,
public sent_xoff ;true if XOFF sent,
public inp_cnt  ;returns count of rcv chars,
public inp_char  ;get one char from buffer,
public inp_flush ;flush input buffer,
public outp_char ;output a character,
;
;A better description can be found in the comment
;block in each function.
;
 assume cs:pgroup
 
;
FALSE EQU 0
TRUE EQU NOT FALSE
;
BASE EQU 03F8H ;BASE FOR SERIAL BOARD
;
LCR equ BASE+3 ; Line control register
IER equ BASE+1 ; Interrup Enable Register
MCR EQU BASE+4 ;modem control register
EnblDRdy equ 01H ; enable 'data-ready' interrupt bit
IntCtlr  EQU 21H ;OCW 1 FOR 8259 CONTROLLER
EnblIRQ4 EQU 0EFH ;Enable COMMUNICATIONS (IRQ4)
DATAPORT EQU BASE ;transmit/receive data port
MaskIRQ4 EQU 10H ;BIT TO DISABLE COMM INTERRUPT (IRQ4)
 
MDMSTA EQU BASE+5 ;line status register
MDMMSR EQU BASE+6 ;modem status register
MDMBAD EQU BASE ;lsb baud resgister
MDMBD1 EQU BASE+1 ;msb baud rate register
MDMCD EQU 80H ;mask for carrier dectect
SETBAU EQU 80H ;code for Divisor Latch Access Bit
MDMTBE EQU 20H ;8250 tbe flag
MDMBRK EQU 40H ;command code for 8250 break
LINMOD EQU 03H ;line mode=8 bit, no parity
MDMMOD EQU 0BH ;modem mode = DTR and RTS HIGH
STOP2 EQU 04H ;BIT FOR TWO STOP BITS IF BAUD<300
RS8259 EQU 20H ;OCW 3 FOR 8259
RSTINT EQU 64H ;SPECIFIC EOI FOR COMM INTERRUPT
XOFF EQU 13H ;XOFF character
XON EQU 11H ;XON character
;
; MISCELLANEOUS EQUATES
;
CR EQU 13
LF EQU 10
DosCall EQU 33 ;INTERRUPT NUMBER FOR DOS CALL
CNSTAT EQU 11 ;FUNCTION NUMBER FOR CONSOLE STATUS
CNIN EQU 1 ;FUNCTION NUMBER FOR CONSOLE INPUT
BUFSIZ EQU 512 ;Max NUMBER OF CHARS
SetIntVect  EQU 25H ;SET INTERRUPT VECTOR FUNCTION NUMBER
 
;
; DUMP BUFFER, COUNT AND POINTER.
;
CIRC_BUF DB BUFSIZ DUP(?) ;ALLOW 512 MaxIMUM BUFFERED CHARACTERS
BUF_TOP EQU $ - 1  ;KEEP TRACK OF THE TOP OF THE BUFFER
CIRC_TOP DW BUF_TOP  ;
;
CIRC_IN DW OFFSET CIRC_BUF ;POINTER TO LAST CHAR. PLACED IN BUFFER
CIRC_CUR DW OFFSET CIRC_BUF ;POINTER TO NEXT CHAR. TO BE RETRIEVED FROM
    ; BUFFER
CIRC_CT DW 0  ;COUNT OF CHARACTERS USED IN BUFFER
SNT_XOFF DB FALSE  ;FLAG TO CHECK IF AN XOFF HAS BEEN SEND
GOT_XOFF  DB FALSE  ;FLAG TO CHECK IF AN XOFF HAS BEEN RECEIVED
SEE_XOFF  DB FALSE  ;FLAT TO SEE IF WE ARE INTERESTED IN XON/XOFF
;
;
;
;set_xoff(flag)  Enable (flag != 0) or disable
;int flag;  (flag == 0) XON/ XOFF protocol
;   for the character input stream.
;If enabled, an XOFF will be sent when the buffer
;reaches 3/4 full. NOTE: an XON will not be sent auto-
;matically. Your program must do it when it sees
;the rcvd_xoff() flag, and ready for more chars.
;
set_xoff proc near
 push bp
 PUSH DS  ;SAVE DATA SEGMENT
 mov bx,[bp+6]
 push cs
 pop ds  ; move code seg addr to data seg reg.
 cmp bx,0
 jnz to_on
 mov see_xoff,FALSE
 jmp done1
to_on: mov see_xoff,TRUE
done1: pop ds
 pop bp
 ret
set_xoff endp
;
;flag= get_xoff() Returns the current setting
;   of the XON/ XOFF flag set
;by set_xoff(), above.
;
get_xoff proc near
 push bp
 push ds  ; save data reg
 push cs
 pop ds  ; move code seg addr to data seg reg.
 xor ax,ax
 mov al,see_xoff
 pop ds
 pop bp
 ret
get_xoff endp
;
;flag= sent_xoff(); Returns true if an XOFF
;   character was sent, indicating
;the receive buffer is 3/4 full.
;
sent_xoff proc near
 push bp
 push ds  ; save data reg
 push cs
 pop ds  ; move code seg addr to data seg reg.
 xor ax,ax
 mov al,snt_xoff
 pop ds
 pop bp
 ret
sent_xoff endp
;
;rcvd_xoff()  Returns true if an XOFF was
;   received; will return false as
;soon as an XON is received. Does not effect data output,
;only indicates the above. (Obviously useless for binary
;data.)
;
rcvd_xoff proc near
 push bp
 push ds  ; save data reg
 push cs
 pop ds  ; move code seg addr to data seg reg.
 xor ax,ax
 mov al,got_xoff
 pop ds  ; restore data reg
 pop bp
 ret
rcvd_xoff endp
;
;count= inp_cnt() Returns the number of characters
;   available in the input buffer.
;
 
inp_cnt proc near
 push bp
 push ds  ; save data segment
 push cs
 pop ds  ; move code seg addr to data seg reg
 mov ax,circ_ct
 pop ds
 pop bp
 ret
inp_cnt endp
;
;inp_flush() Flush the input buffer.
;
inp_flush proc near
 push bp
 push ds  ; save data reg
 push cs
 pop ds  ; move code seg addr to data seg reg.
 mov bx,offset circ_buf
 mov circ_in,bx
 mov circ_cur,bx
 xor ax,ax
 mov circ_ct,ax
 pop ds
 pop bp
 ret
inp_flush endp
 
; --------- Init -----------------------------------
; Program initialization:
;   -- Set up vector for RS232 interrupt (0CH)
;   -- Enbl IRQ4
;   -- Enbl RS232 interrupt on data ready
;
; ---------------------------------------------------
 
init_comm proc near
 push bp
 cli
 
;  ---- Set up INT x'0C' for IRQ4
 
 push ds
 push cs
 pop ds  ;cs to ds
 mov dx,offset IntHdlr ;relative adddres of interrupt handler
 mov al,0cH  ;interrupt number for comm.
 mov ah,SetIntVect ;function number for setting int vector
 int DosCall  ;set interrupt in 8086 table
 pop ds  ;restore DS
 
;  ---- Enbl IRQ4 on 8259 interrupt controller
 
 cli
 
 in al,IntCtlr ; get current masks
 and al,EnblIRQ4 ; Reset IRQ4 mask
 out IntCtlr,al ; And restore to IMR
 
;  ---  Enbl 8250 data ready interrupt
 
 mov dx,LCR  ; DX ==> LCR
 in al,dx  ; Reset DLAB for IER access
 and al,7FH
 out dx,al
 mov dx,IER  ; Interrupt Enbl Register
 mov al,EnblDRdy ; Enable 'data-ready' interrupt
 out dx,al
 
;  ---  Enbl OUT2 on 8250
 
 mov dx,MCR  ; modem control register
 mov al,08H  ; Enable OUT2
 out dx,al
 
 sti
 
 pop bp
 ret
init_comm endp
;
;uninit_comm()  Removes the interrupt structure
;   installed by init_comm(). Must be
;done before passing control to the DOS, else chars received
;will be stored into the next program loaded!
;
uninit_comm proc near
 push bp
; --- Disable IRQ4 on 8259
 
 cli
 in al,IntCtlr ;GET OCW1 FROM 8259
 or al,MaskIRQ4 ;DISABLE COMMUNICATIONS INTERRUPT
 out IntCtlr,al
 
; --- Disable 8250 data ready interrupt
 
 mov dx,LCR  ; DX ==> LCR
 in al,dx  ; Reset DLAB for IER access
 and al,7FH
 out dx,al
 mov dx,IER  ; Interrupt Enbl Register
 mov al,0  ; Disable all 8250 interrupts
 out dx,al
 
;  ---  Disable OUT2 on 8250
 
 mov dx,MCR  ; modem control register
 mov al,0  ; Disable OUT2
 out dx,al
 
 sti
  pop bp
 ret
uninit_comm endp
;
;char inp_char() Return a character from the input
;   buffer. Assumes you have called
;inp_cnt() to see if theres any characters to get.
;
inp_char proc near
 push bp
 push ds  ; save data reg
 push cs
 pop ds  ; move code seg addr to data seg reg.
 mov bx,circ_cur
 xor ax,ax
 mov al,[bx]  ;get next char from circ_buf
 DEC circ_ct  ;decrement circ_buf COUNT
 CMP bx,circ_top ;ARE WE AT THE TOP OF THE circ_buf?
 JZ reset_cur ;JUMP IF SO
 INC bx  ;ELSE, BUMP PTR
 JMP SHORT upd_cur
reset_cur:
 mov bx,OFFSET circ_buf ;RESET circ_in TO BOTTOM OF BUF.
upd_cur:
 mov circ_cur,bx  ;SAVE NEW PTR
 xor cx,cx
 mov cl,see_xoff ;check if interested in xon/xoff
 cmp cl,TRUE
 jnz clnup2  ;not interested, so goto return
 cmp snt_xoff,TRUE ;have we sent an xoff?
 jnz clnup2  ;no, so return
 cmp circ_ct,80h ;yes, so see in buf is now emptying
 jg clnup2  ;not empty enuf to send xon, jump to ret
 mov snt_xoff,FALSE
 mov cl,XON
 push ax  ; save char
 call comout
 pop ax
clnup2: pop DS  ;GET BACK ENTERING DS
 pop bp
 ret
inp_char endp
;
;outp_char(c)  Output the character to the
;char c;  serial port. This is not buffered
;   or interrupt driven.
;
outp_char proc near
 push bp
 mov bp,sp
 mov cl,[bp+4]
 sti
 call comout
 pop bp
 ret
outp_char endp
;
;Local subroutine: output CL to the port.
;
comout: mov dx,MDMSTA
 in al,dx  ; get 8250 status
 and al,MDMTBE ; check for transmitter ready
 jz comout  ; jump if not to wait
 mov al,cl  ; get char to al
 mov dx,DATAPORT
 out dx,al  ; output char to 8251
 ret
;
; RECEIVE INTERRUPT HANDLER (CHANGED TO PLACE CHARACTERS IN A
;  CIRCULAR circ_buf AND TO SEND AN XOFF IF THE circ_buf IS MORE THAN
;  3/4 FULL - S.G.)
;
IntHdlr:
 CLI
 push cx
 push dx
 push bx
 push ax
 push ds
 mov ax,cs  ;get cur code segment
 mov ds,ax  ; and set it as data segment
 mov bx,circ_in ;GET circ_buf IN PTR
 mov DX,DATAPORT ;GET DATA PORT NUMBER
 IN AL,DX  ;GET RECEIVED CHARACTER
; push ax
; push dx
; xor ax,ax
; xor dx,dx
; mov dl,al
; mov ah,2
; int DosCall
; pop dx
; pop ax
 xor cx,cx
 mov cl,see_xoff ;check if interested in xon/xoff
 cmp cl,TRUE
 jnz ck_full  ;not interested goto ck if buf full
 mov cl,al  ;put char in cl for testing
 and cl,7fh  ;turn off any parity bits
 cmp cl,XOFF  ;see if we got an xoff
 jnz ck_xon
 mov got_Xoff,TRUE ; code for handling xon/xoff from remote
 jmp clnup
ck_xon: cmp cl,XON
 jnz reg_ch
 mov got_Xoff,FALSE
 jmp clnup
;
;Normal character; not XON/XOFF, or XON/XOFF disabled.
;
reg_ch: test snt_Xoff,TRUE ;SEE IF sentXoff IS SET
 jnz ck_full  ;IF SO, DON'T SEND ANOTHER XOFF
 CMP circ_ct,(BUFSIZ * 3)/4 ;ALLOW BUF TO BECOME 3/4 FULL BEFORE
     ; SENDING XOFF
 jb savch  ;IF IT'S OK, CONTINUE
 push ax  ;SAVE CHARACTER
 mov CL,XOFF  ;GET XOFF CHARACTER
 mov snt_Xoff,TRUE  ;RESET sentXoff
 call comout  ; AND SEND IT
 pop ax  ;RETRIEVE CHARACTER
 JMP SHORT savch  ;IF WE'RE HERE, THE circ_buf HAS BUFSIZ-80H
    ;  CHARACTERS
ck_full:
 CMP circ_ct,BUFSIZ ;SEE IF circ_buf ALREADY FULL
 JZ clnup  ; JUMP IF SO, DO NOT PLACE CHARACTER IN BFR
savch:
 mov [bx],AL  ;SAVE NEW CHARACTER IN circ_buf
 inc circ_ct  ;BUMP circ_buf COUNT
 CMP bx,circ_top ;ARE WE AT THE TOP OF THE circ_buf?
 JZ reset_in ;JUMP IF SO
 inc bx  ;ELSE, BUMP PTR
 JMP SHORT into_buf
reset_in:
 mov bx,OFFSET circ_buf ;RESET circ_in TO BOTTOM OF BUF.
into_buf:
 mov circ_in,bx  ;SAVE NEW PTR
clnup:
 mov AL,RSTINT
 OUT RS8259,AL ;ISSUE SPECIFIC EOI FOR 8259
 pop ds  ;GET BACK ENTERING DS
 pop ax
 pop bx
 pop dx
 pop cx
 sti
 iret
;
prog ends
 
end
 
*** COPIED 01/25/84 04:31:17 BY KSC ***
