Microsoft Systems Journal
Volume 2; Issue 3; July, 1987

Code Listings For:

	TSRCOMM.ASM
	pp. 37-50

Author(s): Ross M. Greenberg
Title:     Keeping Up With The Real World: Speedy Serial I/O Processing




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	TSRCOMM.ASM		A Replacement for Interrupt 14
;;
;;	Copyright	1987, Ross M. Greenberg
;;
;;	This program is a terminate and stay resident program which allows
;;	computers such as the IBM PC and compatibles using standard serial
;;	communications calls to take advantage of the asynchronous interrupt
;;	capabilities of the 8250 and 8259.
;;
;;	The functionality with AH=0,1,2,3 remains the same as always
;;
;;	With AH = 4, a new set of commands are now available.
;;	Sub-functions are set in AL (See below for new function descriptions)
;;
;;	To compile this program, you must have MASM 4.0 or a close relative,
;;	and need only do:
;;
;;	C> MASM TSRCOMM;
;;	C> LINK TSRCOMM;
;;	C> EXE2BIN TSRCOMM.EXE TSRCOMM.COM
;;	C> DEL TSRCOMM.EXE
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;				MACRO DEFINITIONS
;;
DOSINT	macro	set_ah_to
	mov	ah, set_ah_to
	int	21h
endm
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;				CONSTANTS
;;
FALSE		equ	00h
TRUE		equ	01h	
BELL		equ	07h
ONE_SECOND	equ	18		; roughly 18 timer ticks
TWO_SECONDS	equ	ONE_SECOND * 2	; same idea
THIRTY_SECONDS	equ	ONE_SECOND * 30 ;



TIMER_TICK_INT_NO	equ	1ch	; some might set this to 08h


;;	Send an XOFF when you wish the remote to stop sending and
;;	send an XON when the remote is allowed to continue
;;

XOFF	equ	13h	; a control-S
XON	equ	11h	; a control-Q



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	Define the size of the Communications Buffers
;;	
;;	You may want to change these to more accurately reflect your
;;	actual needs.  The high-water and low-water mark for XON/XOFF
;;	processing are a function of the size of these two variables

P1_INLEN	equ	400h
P2_INLEN	equ	400h
P1_OUTLEN	equ	400h
P2_OUTLEN	equ	400h

;; Be careful with these settings if your have different lengths for each
;; of the COM_INBUF's: these only play off of COM1_INBUF

HIGH_MARK	equ	(P1_INLEN/10 * 8)	; send XOFF when buffer is
						; 80% full
LOW_MARK	equ	(P1_INLEN/10 * 2)	; send XON when buffer is
						; only 20% full
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;	Definitions of all of the 8250 Registers and their individual
;;	bit meanings
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
DATA		equ	0h	; DATA I/O is from the base

IER		equ	1h	; Interrupt Enable Register
 IER_RDA	equ	1h	;  Received Data Available interrupt bit
 IER_THRE	equ	2h	;  Transmitter Holding Reg. Empty int bit
 IER_RLS	equ	4h	;  Receive Line Status int bit
 IER_MS		equ	8h	;  Modem Status int bit

IIR		equ	2	; Interrupt Identification Register
 IIR_RLS	equ	5h	;  *equal* to if Receiver Line Status int
 IIR_RDA	equ	4h	;  *equal* to if character ready
 IIR_THRE	equ	2h	;  *equal* to if TX Buffer empty
 IIR_PEND	equ	1h	;  set to zero if any interrupt pending
 IIR_MS		equ	0h	;  *equal* to if Modem Status int

LCR		equ	3h	; Line Control Register
 LCR_WLS0	equ	0h	;  Word Length Select Bit 0
 LCR_WLS1	equ	1h	;  Word Length Select Bit 1
 LCR_STOPBITS	equ	4h	;  number of stop bits
 LCR_PARITYEN	equ	8h	;  Enable Parity (see SPARITY & EPARITY)
 LCR_EPARITY	equ	10h	;  Even Parity Bit
 LCR_SPARITY	equ	20h	;  Stick Parity
 LCR_BREAK	equ	40h	;  set if break is desired
 LCR_DLAB	equ	80h	;  Divisor Latch Access Bit (baudrate setting)

MCR		equ	4h	; Modem Control Register
 MCR_DTR	equ	1h	;  Data Terminal Ready
 MCR_RTS	equ	2h	;  Request To Send
 MCR_OUT1	equ	4h	;  Output 1 (nobody uses this!)
 MCR_OUT2	equ	8h	;  Out 2 (Sneaky Int enable in hware gates!)
 MCR_LOOP	equ	10h	;  Loopback enable

LSR		equ	5	; Line Status Register
  LSR_DATA	equ	1h	;  Data Ready Bit
  LSR_OVERRUN	equ	2h	;  Overrun Error Bit
  LSR_PARITY	equ	4h	;  Parity Error Bit
  LSR_FRAMING	equ	8h	;  Framing Error Bit
  LSR_BREAK	equ	10h	;  Break Detect (sometimes an error!)
  LSR_THRE	equ	20h	;  Transmit Holding Register Empty
  LSR_TSRE	equ	40h	;  Transmit Shift Register Empty

MSR		equ	6	; Modem Status Register
  MSR_DEL_CTS	equ	1h	;  Delta Clear To Send
  MSR_DEL_DSR	equ	2h	;  Delta Data Set Ready
  MSR_EDGE_RI	equ	4h	;  Trailing Edge of Ring Indicator
  MSR_DEL_SIGD	equ	8h	;  Delta Receive Line Signal Detect (delta DCD)
  MSR_CTS	equ	10h	;  Clear To Send
  MSR_DSR	equ	20h	;  Data Set Ready
  MSR_RI	equ	40h	;  Ring Indicator - during the entire ring
  MSR_DCD	equ	80h	;  Data Carrier Detect - Someone On-line
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	The 8259 interrupt controller is at port I/O INT_CTRL
;;	To reset from an interrupt, you must output an INT_EOI.
;;
;;	To enable or disable interrupts for either of the com ports, read
;;	the value on port INT_MASK_PORT, turn the appropriate COM1 or COM2 bit
;;	on or off, and output the new mask back out the port.
;;
;;
CTRL_PORT	equ	20h
INT_EOI		equ	20h
;;
INT_MASK_PORT	equ	21h
COM1_MASK	equ	0efh
COM2_MASK	equ	0f7h
INTNO_COM1	equ	0ch
INTNO_COM2	equ	0bh
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	These ports may or may not respond to the ports at base 40:0.
;;	If they don't we'll refuse to run.  Not at all subtle, but effective.
;;	See the first Sidebar for more information
;;
PORT1	equ	3f8h
PORT2	equ	2f8h
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


code	segment	para		; align the code segment to the nearest
				; paragraph boundary
assume	cs:code
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
org	2ch
env_ptr	label	word		; this points to the environment address
				; as stored in the PSP. We'll free this
				; in the startup routine.

org	100h			; COM programs always start at 100h
				; just like CP/M!

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;				START OF TSR CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
start:	jmp	install



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;			COMM PORT BLOCK	(CPB)
;;
;;	Comm Port Block defines information unique for each comm port
;;	and includes information such as what the original interrupt
;;	vector pointed to, which parameters are set, etc.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CPB	struc

cpb_base	dw	?	; the base port of the comm port (2F8, 3F8)
cpb_nint_off	dw	?	; new interrupt offset address
cpb_pic_mask	db	?	; mask for enabling ints from 8259
cpb_int_no	db	?	; what interrupt we are

cpb_mode	dw	?	; whatever modes we have turned on
cpb_timeout	dw	?	; individual timeout value off timer tick
cpb_in_xoff	dw	0	; true if we output an XOFF
cpb_out_xoff	dw	0	; true if an XOFF was sent to us

cpb_inbase	dw	?	; start of input buffer
cpb_inlen	dw	?	; total length of input buffer allocated
cpb_inhead	dw	?	; pointer to next input char location
cpb_intail	dw	?	; pointer to last input char location
cpb_incnt	dw	0	; count of how many inp characters outstanding
cpb_inerrors	dw	?	; pointer to the error bits

cpb_outbase	dw	?	; start of output header
cpb_outlen	dw	?	; total length of output buffer allocated
cpb_outhead	dw	?	; pointer to next output char location
cpb_outtail	dw	?	; pointer to last output char location
cpb_outcnt	dw	0	; count of how many outp characters outstanding
cpb_outend	dw	?	; ptr to the end of the output buffer

cpb_tx_stat	dw	0	; set to no interrupts turned on
cpb_oint_add	dw	?	; original interrupt offset:segment order
		dw	?
CPB	ends
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;			HANDSHAKING OPTIONS
;;	Used for AH=4, AL =1.  Set CX as a combination of the desired options
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BREAK_IS_ERROR_OPTION	equ	01h
DSR_INPUT_OPTION	equ	02h
DCD_INPUT_OPTION	equ	04h
CTS_OUTPUT_OPTION	equ	08h
XOFF_INPUT_OPTION	equ	10h
XOFF_OUTPUT_OPTION	equ	20h
DTR_OPTION		equ	40h
XON_IS_ANY_OPTION	equ	80h
TX_INTS_ON_OPTION	equ	100h

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;			DEFAULT MODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   uncomment *this* DEF_MODE for XON/XOFF.
DEF_MODE equ XON_IS_ANY_OPTION + XOFF_OUTPUT_OPTION
;   uncomment *this* DEF_MODE for XON/XOFF *plus* some hardware handshaking
;DEF_MODE equ DSR_INPUT_OPTION + CTS_OUTPUT_OPTION + XON_IS_ANY_OPTION +XOFF_OUTPUT_OPTION

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	DEFAULT TIME_OUT INTERVAL, FLAGS, BAUD_RATES, and other various
;;	denizens.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
DEF_TIME 		equ	THIRTY_SECONDS

DEF_FLAGS	equ	DEF_MODE
DEF_T_OUT	equ	DEF_TIME

DEF_BAUD	equ	0e0h	;default baud is 9600
DEF_PARITY	equ	0h	;default parity is off
DEF_STOP	equ	0h	;defualt stop bits is 1
DEF_WORDLEN	equ	3h	;default wordlength is 8

DEF_INIT	equ	DEF_BAUD + DEF_PARITY + DEF_STOP + DEF_WORDLEN


NO_XON		equ	FALSE
NO_CHARS	equ	0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Masm has a 256 byte static initialization limit.  NC is shorter than
;; NO_CHARS....
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
NC		equ	0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;			GLOBAL DATA ALLOCATIONS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
old_int		dw	?	; used as a temporary vector for 'weird' ints
		dw	?

orig_timer	dw	?	; the original timer vector
		dw	?

special_return_value	dw	0ff0h
timed_out	dw	0	; "any time outs?" flag

my_timer	dw	?	; timer for local usage
timer_tick	dw	?	; this value used for timer countdown
old_int14	dw	?	; the original int14 vector
		dw	?

error_flag	dw	0	; used as a place holder in the RDA routine

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;	Allocate space for both com ports input and output buffers
;;	and the input error bits array
;;
;; WARNING!	Do not move the error array away from its approriate
;;		error array, or you'll probably crash at some point!
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
com1_inbuf	db	P1_INLEN dup(0)
com1_errs	db	(P1_INLEN/8) + 1 dup(0)

com2_inbuf	db	P2_INLEN dup(0)
com2_errs	db	(P2_INLEN/8) + 1 dup(0)

com1_outbuf	db	P1_OUTLEN dup(0)
com2_outbuf	db	P2_OUTLEN dup(0)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	CPB1 and CPB2
;;
;;	Allocate space and initialize the COMM PORT BLOCKS for com1 and com2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
cpb1 CPB <PORT1,offset com1_isr,COM1_MASK,INTNO_COM1,DEF_FLAGS,DEF_T_OUT,NO_XON,NO_XON,offset com1_inbuf,P1_INLEN,offset com1_inbuf,offset com1_inbuf,NC,offset com1_errs,offset com1_outbuf,P1_OUTLEN,offset com1_outbuf,offset com1_outbuf,NC>
cpb2 CPB <PORT2,offset com2_isr,COM2_MASK,INTNO_COM2,DEF_FLAGS,DEF_T_OUT,NO_XON,NO_XON,offset com2_inbuf,P2_INLEN,offset com2_inbuf,offset com2_inbuf,NC,offset com2_errs,offset com2_outbuf,P2_OUTLEN,offset com2_outbuf,offset com2_outbuf,NC>


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	EMASK_BYTE
;;
;;	Input Parameters:
;;		Pointer to input buffer in BX
;;
;;	Returns:
;;		Pointer to correct byte in error array in BX,
;;		with the correct bit mask in AX
;;
;;	All other Registers Preserved
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
emask_byte	proc	near
assume	ds:code

	push	cx
	push	dx

	sub	bx, cpb_inbase[si]	; get the actual offset
	mov	ax, bx			; get ready to divide the offset
	cwd				; (make a double word out of it)
	mov	cx, 8
	div	cx			; number of bits to a word
	add	ax, cpb_inerrors[si]	; ax now points to the proper byte
	mov	bx, ax
					; in the error array
	mov	cx, dx			; get ready to shift the remainder
					; to make the mask
	mov	ax, 1			; make the temporary mask
	shl	ax, cl			; and shift it over

	pop	dx
	pop	cx
	ret
emask_byte	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	INIT_BUFFERS
;;
;;	Input Parameters
;;		Pointer in SI to current CPB
;;
;;	All Registers Preserved
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

init_buffers	proc	near
	push	ax
	mov	ax, cpb_inbase[si]	; clear the input buffer
	mov	cpb_inhead[si], ax
	mov	cpb_intail[si], ax
	mov	cpb_incnt[si], NO_CHARS

	mov	ax, cpb_outbase[si]	; and the output buffer
	mov	cpb_outhead[si], ax
	mov	cpb_outtail[si], ax
	mov	cpb_outcnt[si], NO_CHARS
	mov	cpb_outend[si], ax
	mov	ax, cpb_outlen[si]
	add	cpb_outend[si], ax
	pop	ax
	ret
init_buffers	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	TX_ON
;;
;;	Input Parameters
;;		SI points to current CPB, DI to current Base Port.
;;		This routine enables Transmit Buffer Empty Interrupts
;;
;;	All Registers Preserved
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
tx_on	proc	near
assume ds:code

	cmp	cpb_tx_stat[si], TRUE	; are tx ints already enabled?
	jz	tx_on2			; yes

	test	cpb_mode[si], TX_INTS_ON_OPTION
	jz	tx_on2			; don't turn ints on if not needed

	mov	cpb_tx_stat[si], TRUE	; mark interrupts as enabled
	push	ax
	push	dx
	lea	dx, IER[di]		; take a look at the Interrupt
	in	al, dx			; enable register

	or	al, IER_THRE		; turn on the interrupt bit
	out	dx, al			; and turn it on in the chip
	pop	dx
	pop	ax

tx_on2:
	ret				; ints will be turned on eventually
tx_on	endp



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	TX_OFF
;;
;;	Input Parameters
;;		SI points to current CPB, DI to current Base Port.
;;		This routine disables Transmit Buffer Empty Interrupts
;;
;;	All Registers Preserved
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
tx_off	proc	near
assume ds:code

	cmp	cpb_tx_stat[si], FALSE	; are tx ints already disabled?
	jz	tx_off2			; yes

	test	cpb_mode[si], TX_INTS_ON_OPTION
	jz	tx_off2			; shouldn't be on in the first place

	mov	cpb_tx_stat[si], FALSE	; mark interrupts as disabled
	push	ax
	push	dx
	lea	dx, IER[di]		; take a look at the Interrupt
	in	al, dx			; enable register

	and	al, NOT IER_THRE	; turn off the interrupt bit
	out	dx, al			; and turn it on in the chip
	pop	dx
	pop	ax

tx_off2:
	ret				; ints will be turned on eventually
tx_off	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	GET_COM_PORT
;;
;;	Input Parameters
;;		SI points to current CPB
;;
;;	Returns proper comm port (0 or 1) in DX. All other registers preserved
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
get_com_port proc near
assume	ds:code

	cmp	si, offset cpb1
	jnz	not_com1
	mov	dx, 0
	jmp	ret_com_port

not_com1:
	cmp	si, offset cpb2
	jnz	not_com2
	mov	dx, 1
	jmp	ret_com_port

not_com2:
	mov	dx, 0ffh

ret_com_port:
	ret
get_com_port endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	GET_CPB
;;
;;	Input Parameters
;;		Comm port (0 or 1) in DX
;;
;;	Returns pointer to CPB in SI
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
get_cpb proc near
assume	ds:code

	cmp	dx, 0
	jnz	not_comm1
	mov	si, offset cpb1
	jmp	ret_cpb

not_comm1:
	cmp	dx, 1
	jnz	not_comm2
	mov	si, offset cpb2
	jmp	ret_cpb

not_comm2:
	mov	si, 0

ret_cpb:
	ret
get_cpb endp



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	SEND_CHAR
;;
;;	Input Parameters
;;		Character to be sent in al, pointer to current CPB in SI,
;;		Base port in DI
;;
;;	All Registers Preserved 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
send_char proc	near

	push	dx

	call	get_com_port
	call	out_char

	pop	dx
	ret

send_char endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	OUT_BELL
;;	SEND_XOFF
;;	SEND_XON
;;
;;
;;	Input Parameters
;;		None
;;
;;	Sends BELL out the comm port
;;	  "   XON   "   "   "    "
;;	  "   XOFF  "   "   "    "
;;
;;	All Registers Preserved 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
out_bell proc near
assume	ds:code

	push	ax
	mov	al, BELL
	call	send_char
	pop	ax
	ret

out_bell endp


send_xoff proc near
assume	ds:code
	push	ax
	mov	al, XOFF
	call	send_char
	pop	dx
	ret
send_xoff endp

send_xon proc near
assume	ds:code
	push	ax
	mov	al, XON
	call	send_char
	pop	dx
	ret
send_xon endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	TILL_CLEAR
;;
;;	Input Parameters
;;		Before affecting the transmit interrupts, this routine
;;		allows the Shift Register to clear, or for timeouts to
;;		run out before returning.  DI should point to the Base
;;		Port of the comm port. 
;;
;;	Nothing Returned, all registers preserved
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
till_clear	proc	near

	push	dx
	push	ax

	mov	my_timer, TWO_SECONDS	; for no more than one second
	lea	dx, LSR[di]		; better check till we're really done
lp1:
	in	al, dx

	test	al, LSR_TSRE		; check the shift register
	jnz	end_lp1			; done, turn off ints

	cmp	my_timer, 0		; compare the timer tick to zero
	jnz	lp1			; we'll just give up eventually
end_lp1:
	pop	ax
	pop	dx
	ret
till_clear endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	OUT_CHAR
;;
;;	Input Parameters
;;		AL should hold the character to be outputted, SI points
;;		to the CPB and DI to the base register.
;;
;;		All of the various handshaking parameters are checked and
;;		acted upon.  If a timeout occurs, the timed_out counter
;;		will be incremented
;;
;;	Returns AH=80h if timed out
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
out_char proc	near

	push	ax
	mov	bx, cpb_timeout[si]	; get the timeout
	mov	timer_tick, bx		; and stick into the autodecrementer

top_loop:
	lea	dx, MSR[di]		; check the status of the serial port
	in	al, dx

	test	cpb_mode[si], DSR_INPUT_OPTION
	jz	no_dsr_test		; not dsr handshaking
	test	al, MSR_DSR		; we're handshaking...hows it look?
	jz	loop_it			; not yet

no_dsr_test:
	test	cpb_mode[si], CTS_OUTPUT_OPTION
	jz	no_cts_test		; no cts handshaking
	test	al, MSR_CTS		; we're handshaking...hows it look?
	jz	loop_it			; not yet

no_cts_test:
	cmp	cpb_out_xoff[si], FALSE	; in an XOFF state?
	jnz	loop_it			; yes, so cycle. It may end

	lea	dx, LSR[di]		; finally, check the xmit buffer
					; on the chip
chip_check:
	in	al, dx			; get the status
	test	al, LSR_THRE		; clear to send it?
	jnz	squirt_it		; yes! send the character

loop_it:
	cmp	timer_tick, 0		; out of time yet?
	jnz	top_loop		; not yet, so cycle

	pop	ax			; keep the stack clean
	or	ah, 080h		; mark a timeout
	inc	timed_out
	ret

squirt_it:
	lea	dx, DATA[di]		; get the port to squirt from
	pop	ax			; retrieve the original character
	out	dx, al			; and squirt it.
	xor	ah, ah
	ret
out_char endp



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;			ASYNC SUB-SERVICE ROUTINES
;;
;;	These routines are only entered by the COMMON_ISR routine, which
;;	validates there is a valid interrupt on the aproriate comm port,
;;	and sets up SI to point to the correct CPB and DI to the correct
;;	Base Port.
;;
;;	COMMON_ISR also preserves all registers and the flags, and resets
;;	the EOI required at the end of each interrupt.
;;
;;	Finally, COMMON_ISR is, itself, called either by COM1_ISR or COM2_ISR
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	MS_INT
;;
;;			MODEM STATUS INTERRUPT
;;	This interrupt should never happen.  If it does, clear the status
;;	register and return.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ms_int	proc	near
assume	ds:code

	lea	dx, MSR[di]
	in	al, dx
	ret

ms_int	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	XMIT_INT
;;
;;			TRANSMITTER EMPTY INTERRUPT
;;	This interrupt is generated whenever interrupts are TX interrupts
;;	are turned on and the Transmiter Holding Register is empty.
;;
;;	If there are more characters to be transmitted, then load up
;;	another one and move the pointers. If there are no more characters
;;	to be transmitted, then wait until the the last character has been
;;	transmitted (the Shift Register must be empty), then turn TX
;;	interrupts off.  This gives a slight delay on the last character
;;	but guarantees the last character will be properly transmitted.
;;
;;	If XON/XOFF processing on outgoing characters is enabled, then if we
;;	get a TX int with the XOFF flag set (indicating we had gotten an XOFF
;;	in the RX routine earlier), we'll simply shut off TX interrupts as if
;;	we had an empty TX buffer.  Subsequent characters received in the RX
;;	routine will reset the XON/XOF flag and turn TX interrupts on as well.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xmit_int	proc	near
assume	ds:code
	cmp	cpb_outcnt[si], 0	; anywork to do?
	jnz	xmit1			; yep!

	call	till_clear		; wait for transmitter to clear

	call	tx_off			; turn xmit interrupts off

	ret				; and return

xmit1:
	mov	bx, cpb_outtail[si]	; get the next character to xmit
	inc	bx			; now point right on it
	cmp	bx, cpb_outend[si]	; cmp to the end
	jnz	xmit2			; if not past the end, jump
	mov	bx, cpb_outbase[si]	; past the end, reset to the head

xmit2:
	cli				; don't get interrupted now
	dec	cpb_outcnt[si]		; decrement the count of chars to go
	mov	cpb_outtail[si], bx	; and save a pointer to the next char
	mov	al, [bx]		; get the character in al

	cmp	cpb_outcnt[si], 0	; any work left to do?
	jnz	out_it			; yep!

	call	till_clear		; wait for transmitter to clear
	call	tx_off			; turn xmit interrupts off

out_it:
	call	out_char		; output the character
	ret
xmit_int	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	RDA_INT
;;
;;
;;			CHARACTER READY INTERRUPT
;;	This interrupt is generated whenever a character is ready on the
;;	serial port.
;;
;;	If the "DSR must be set" option is turned on, and DSR isn't, then
;;	the character is read, then discarded.  Likewise for the "DCD must
;;	be high" option.
;;
;;	If there is room in the buffer, add it and increment the pointers.
;;	If there isn't room, then clear the character and discard it, then
;;	generate a BELL character out the port (in case a human were
;;	listening).
;;
;;	After adding the character, if XON/XOFF processing of incoming
;;	characters is enabled, the HIGH_WATER level is checked. If the
;;	character count exceeds it, then if the XOFF flag isn't set, an
;;	XOFF character is sent and the XOFF flag is incremented.  The XON
;;	is sent (and the XOFF flag reset) in the "get-a-character" routine
;;	in the interrupt 0x14 replacement when the low water mark is reached.
;; 
;;	When adding a character, the error status is checked (if the
;;	check_flag is set in the cpb) If an error exists on the port
;;	the corresponding bit is set in the error buffer for the com port.
;;	This is later examined in the "get-a-character" routine and the
;;	"get-status" routine.  A generalized error is returned if the error
;;	bit is set.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rda_int		proc	near
assume	ds:code

	test	cpb_mode[si], DSR_INPUT_OPTION	+ DCD_INPUT_OPTION
					; is the DSR or DCD option turned on?
	jz	rda_2			; nope
	lea	dx, MSR[di]		; check the modem status
	in	al, dx
	test	cpb_mode[si], DSR_INPUT_OPTION
	jz	just_dcd		; just check DCD
	test	al, MSR_DSR		; is it set?
	jz	rda_1			; nope. discard.

just_dcd:
	test	cpb_mode[si], DCD_INPUT_OPTION
	jz	rda_2			; get the character
	test	al, MSR_DCD		; is there carrier?
	jnz	rda_2			; you bet! get the character

rda_1:					; read the character, discard by
					; returning
	lea	dx, DATA[di]
	in	al, dx
	ret

rda_2:
	lea	dx, LSR[di]		; reading status reg clears errors
	in	al, dx
	test	al, LSR_OVERRUN + LSR_PARITY + LSR_FRAMING
	jz	no_err1			; any of the above errors? No.
	mov	error_flag, TRUE	; set the error_flag
	jmp	get_char

no_err1:				; break an error?
	test	cpb_mode[si], BREAK_IS_ERROR_OPTION
	jz	get_char		; nope
	test	al, LSR_BREAK		; is break on?
	jz	get_char		; nope
	mov	error_flag, TRUE	; yep. Set the error

get_char:
	lea	dx, DATA[di]		; get the data
	in	al, dx

	cmp	error_flag, FALSE	; any errors?
	jnz	insert1			; yep, so skip the XON/XOFF stuff

					; no errors, so XON/XOFF would be
					; valid

	test	cpb_mode[si], XOFF_OUTPUT_OPTION
	jz	insert1			; skip if output xoff not turned on
	cmp	al, XOFF		; is this an XOFF character?
	jnz	insert0			; nope
	mov	cpb_out_xoff[si], TRUE	; turn this on for the next TX int
	call	tx_off			; turn off tx interrupts
	ret				; and return

insert0:
	cmp	cpb_out_xoff[si], FALSE	; are we in XOFF mode?
	jz	insert1			; no, so insert the character

					; do we care about exact matching only?
	test	cpb_mode[si], XON_IS_ANY_OPTION
	jz	any_char		; no. Any character will do.

	cmp	al, XON			; is this an XON character?
	jz	any_char		; yes, so turn things back on

	ret				; XOFF is on, this isn't an XON,
					; so simply ignore it
any_char:
	mov	cpb_out_xoff[si], FALSE	; turn this off and set up for
	call	tx_on			; the next TX int
	ret				; and return


insert1:
					; send an XOFF quick as you can if
					; needed
	test	cpb_mode[si], XOFF_INPUT_OPTION
	jz	insert2			; no XOFF processing required
	cmp	cpb_incnt[si], HIGH_MARK; do we need an XOFF?
	jle	insert2			; no XOFF required right now

	cmp	cpb_in_xoff[si], FALSE	; have we already sent one?
	jnz	insert2			; yes

	mov	cpb_in_xoff[si], TRUE	; set the flag and
	call	send_xoff		; send it

insert2:
	mov	bx, cpb_inhead[si]
	inc	bx			; move the head up by one
	cmp	bx, cpb_inerrors[si]	; have we gone past the end?
	jnz	insert3			; nope, so stick it in the buffer
	mov	bx, cpb_inbase[si]	; yep, so start it over

insert3:
	cmp	bx, cpb_intail[si]	; are we sneaking up on ourselves?
	jnz	insert4			; nope, just insert it
	call	out_bell		; yep! ring a bell, and 
	mov	error_flag, TRUE	; set an error on the next character
	sti
	ret				; and simply return

insert4:
	mov	[bx], al		; put the character in its place and
	cli				; turn ints off for a little while
	mov	cpb_inhead[si], bx	; preserve the new value of the head
	inc	cpb_incnt[si]		; and mark how many characters exist

	sti
	cmp	error_flag, FALSE	; any errors?
	jz	rda_return		; no

	call	emask_byte		; get the byte and the mask
	or	[bx], ax		; and make the bit turn on
	mov	error_flag, FALSE	; and turn them of for the next time

rda_return:
	ret
rda_int		endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	ERR_INT
;;
;;			ERROR INTERRUPT
;;
;;	This interrupt should never happen.  If it does, read the 
;;	register and return.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
err_int		proc	near
assume	ds:code

	lea	dx, LSR[di]		; get the status word
	in	al, dx
	iret

err_int	endp
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	This is a table containing the offsets for each of the expected
;;	interrupts from the 8250.  Interesting how the offsets off the IIR
;;	happen to be by two.  Great for offseting into this table!
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
interrupt_table	label word		;;
	dw	offset ms_int		;; modem status interrupt
	dw	offset xmit_int		;; transmitter interrupt
	dw	offset rda_int		;; character ready interrupt
	dw	offset err_int		;; receiver line error interrupt
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;			COM1_ISR AND COM2_ISR
;;
;;	These are the entry points for each of the COMM Interrupts
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
com1_isr	proc far
assume	ds:nothing
	push	ax
	mov	ax, offset cs:cpb1
jmp	short	common_isr
com1_isr	endp

com2_isr	proc far
assume	ds:nothing
	push	ax
	mov	ax, offset cs:cpb2
	jmp	short	common_isr
com2_isr	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	COMMON_ISR
;;
;;	A common entry point into the "save the registers and call the right
;;	interrupt service routine".
;;
;;	A little twist:  if it does not appear the interrupt came from a
;;	source we expect, then jump to the old interrupt vector
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
common_isr	proc	near
assume	ds:nothing

	push	bx
	push	cx
	push	dx
	push	si
	push	di
	push	ds

	push	cs			;addressing off ds as cs
	pop	ds
assume	ds:code				;makes it easier to think


	mov	si, ax			; move in the cpb
	mov	di, cpb_base[si]	; get the base port
	lea	dx, IIR[di]		; and then the interrupt ID Register
	in	al, dx			; get the interrupt type
	test	al, IIR_PEND		; is there a pending interrupt?
	jz	is_mine			; interrupt on *this* chip!

other_int:
	cli				; turn off interrupts since this
					; is non-reentrant
	mov	ax, cpb_oint_add[di]	; grab the old interrupt out of
	mov	old_int, ax		; the structure
	mov	ax, cpb_oint_add[di][2]
	mov	old_int[2], ax

	pop	ds			; pop everything back
assume ds:nothing
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	jmp	dword ptr cs:[old_int]	; jump to whatever was there

polling_loop:				; this is a requirement to be sure
					; we haven't lost any any interrupts

	lea	dx, IIR[di]		; load the interrupt ID Register
	in	al, dx			; 
	test	al, IIR_PEND		; is there a pending interrupt?
	jnz	clear			; no. time to return

is_mine:

	and	ax, 06h
	mov	bx, ax
	mov	bx, interrupt_table[bx]

	push	di			; save di for the polling loop
	call	bx
	pop	di

	jmp	polling_loop		; time to check for more work

clear:					; no further interrupt processing
	pop	ds			; pop everything back
assume ds:nothing
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx

	cli				; interrupts off, then reset
	mov	al, INT_EOI		; interrupts on the 8259
	out	CTRL_PORT, al
no_eoi:
	pop	ax


	iret				; the iret will turn interrupts back on
common_isr	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	NEW_14
;;
;;	The replacement for the interrupt 14 service routines
;;	Services 0,1,2,3 are basically the same as before the vector
;;	was stolen.
;;
;;	This routine sets up SI and DI to point to the correct CPB and
;;	Base Port. then vecots to the appropriate service routine
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
new_14	proc	far
	assume	ds:nothing

	sti
					;push everything in sight,
					; *except* ax
	push	bx
	push	cx
	push	dx
	push	di
	push	si
	push	bp
	push	ds

	push	cs			;addressing off ds as cs
	pop	ds
assume	ds:code				;makes it easier to think

	call	get_cpb			; get si to point to the proper cpb
	mov	di, cpb_base[si]	; get di to point to the base port

	cmp	ah, 4			; better check for valid function
	jle	ok_func			; function is okay
	mov	ax, 0ffffh		; set up as a bad code
	jmp	return			; and return

ok_func:
	mov	bl, ah
	xor	bh, bh
	shl	bx, 1			; make an offset into the table
	mov	bx, int14_functions[bx]	; and get the address of the function
	call	bx

return:
	pop	ds
assume	ds:nothing
	pop	bp
	pop	si
	pop	di
	pop	dx
	pop	cx
	pop	bx

	iret
new_14	endp



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
int14_functions	label	word		;;
	dw	offset	init14		;; initialize the port
	dw	offset	send14		;; send the character in al
	dw	offset	get14		;; return next charaacter in al, status
					;; in ah
	dw	offset	stat14		;; get serial status, return in ax
	dw	offset	newfuncs14	;; all of the new functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	INIT14
;;
;;	Input Parameters
;;		The topmost three bits (7,6,5) contain the baudrate:
;;			000 -  110
;;			001 -  150
;;			010 -  300
;;			011 -  600
;;			100 - 1200
;;			101 - 2400
;;			110 - 4800
;;			111 - 9600
;;
;;		The next two bits (4,3) contain the parity:
;;			00  - None
;;			01  - Odd
;;			10  - None
;;			11  - Even
;;
;;		The next bit (2), if set, indicates two stopbits, otherwise
;;		one stopbit
;;
;;		The final two bits (1,0) are the word length:
;;
;;			00 - Undefined
;;			01 - Undefined
;;			10 - 7 Bits
;;			11 - 8 Bits
;;
;;	This routine is very similar to the original BIOS routine, with
;;	the exception that it gets a divisor for determining the baud
;;	rate which exceeds the 9600 original in the BIOS. When the
;;	comm port is initialized through the old interrupt with AH=0,
;;	it is the same as the old, except for the mode and timeout reset.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
init14	proc	near
assume	ds:code

	push	ax
	lea	dx, LCR[di]			; get the Latch
	in	al, dx
	or	al, LCR_DLAB			; turn on the divisor
	out	dx, al				; in the chip
	pop	ax				; get the old ax back

	push	ax				; and save it
	mov	cl, 5				; we'll shift it over to
	shr	ax, cl				; make a table offset of it

	call	get_baud			; then get the correct divisor
						; allows higher than 9600

	lea	dx, DATA[di]			; get the base address
	out	dx, ax				; output the whole word

	pop	ax				; get back original ax
	lea	dx, LCR[di]			; get the Latch
	and	al, 01fh			; just the parity, stop bits,
						; word length
	out	dx, al				; set the params

	mov	cpb_mode[si], DEF_MODE		; default modes
	mov	cpb_timeout[si], DEF_TIME	; default timeout period

	jmp	stat14				; the old one returned the
						; status in al
init14	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	GET_BAUD
;;
;;	Input Parameters
;;		AX should be the ofset into the baudrate table.
;;		The first eight entries are exactly as the BIOS has them
;;		The others were extrapolated for 19200, and 38400 baud
;;
;;	Returns the divisor in AX
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
get_baud	proc	near
assume	ds:code

	shl	ax, 1				; make the table offset
	push	bx
	mov	bx, ax
	mov	ax, baudrate_table[bx]		; and get the divisor
	pop	bx
	ret

get_baud	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;	These numbers are in decimal.  As can be seen, to double the baudrate
;;	the divisor gets halved.  So....what comes after 384000??
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
baudrate_table	label	word
	dw	1047		; 110 baud
	dw	768		; 150 baud
	dw	384		; 300 baud
	dw	192		; 600 baud
	dw	96		; 1200 baud
	dw	48		; 2400 baud
	dw	24		; 4800 baud
	dw	12		; 9600 baud
	dw	6		; 19200 baud
	dw	3		; 38400 baud
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	SEND14
;;
;;	This routine will attempt to send the character in al for the
;;	entire timeout period if the TX_INTS_ON_OPTION is off.
;;
;;	If the option is on, it will queue the item, instead, into the
;;	queue, and let the interrupt system handle it from there.
;;
;;	This routine will return the status of the send (if any) in AX
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
send14	proc	near
assume	ds:code

	test	cpb_mode[si], TX_INTS_ON_OPTION
	jz	just_squirt		; the option isn't being
					; used, so send it the old way

	mov	bx, cpb_outhead[si]	; get the current head
	inc	bx			; move it forward by one
	cmp	bx, cpb_outend[si]	; cmp to the end
	jnz	send2			; if not past the end, jump
	mov	bx, cpb_outbase[si]	; past the end, reset to the head

send2:
	cmp	bx, cpb_outtail[si]	; about to clobber the tail?
	jz	junk_char		; yep! throw this character away!

	mov	[bx], al		; stick the character in the queue
	cli				; turn off interrupts
	mov	cpb_outhead[si], bx	; and save the new value
	inc	cpb_outcnt[si]		; and increment the output char cnt
	call	tx_on

junk_char:
	sti
	ret

just_squirt:
	call	out_char

	jmp	stat14			; then get the status
		
send14	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	GET14
;;
;;	This routine is a replacement for the AH = 2 get-a-character
;;	routine in the BIOS.
;;
;;	Input Parameters
;;		Set the COM port in DX (0 or 1), Base in DI, CPB in SI
;;
;;	Returns character (if any) in AL, with partial comm port status
;;		in AH. Check the high bit ( & 0x80) to determine timeout,
;;		or if AH is non zero it is safe to assume some error has
;;		occurred
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
get14	proc	near
assume	ds:code

	mov	ax, cpb_timeout[si]
	mov	timer_tick, ax		; in case there is no character ready,
					; set the timeout count in advance


try_again:
 	cmp	cpb_incnt[si], 0	; any characters out there?
	jnz	got_one			; yep!

	sti				; make sure interrupts are on while
	cmp	timer_tick, 0		; waiting for the timer to zero out
	jnz	try_again
	or	ah, 080h		; set the timeout error bit
	inc	timed_out		;
	ret


got_one:
	mov	bx, cpb_intail[si]	; get the tail
	inc	bx			; point to next character
	cmp	bx, cpb_inerrors[si]	; past end?
	jnz	get_char1		; no
	mov	bx, cpb_inbase[si]	; yes, so reset to beginning

get_char1:
	mov	al, [bx]		; get the actual character
	xor	ah,ah
	push	ax			; save it
	push	bx
	call	emask_byte		; to check if an error
	test	[bx], ax		; is there an error?
	pop	bx
	pop	ax

	jz	no_inp_error		; no error on input
					; there was an error or some kind,
					; so turn on all error bits, including
					; break if the flag is set.

	or	ah, LSR_OVERRUN + LSR_PARITY + LSR_FRAMING
	test	cpb_mode[si], BREAK_IS_ERROR_OPTION
	jz	no_inp_error		; nope
	or	ah, LSR_BREAK		; is break on?

no_inp_error:
	cli				; no interruptions
	dec	cpb_incnt[si]		; decrease the character count
	mov	cpb_intail[si], bx	; and save the next character ptr	

	test	cpb_mode[si], XOFF_INPUT_OPTION
	jz	skip_xon		; do an XON need to be sent?
	cmp	cpb_in_xoff[si], 0	; currently in xoff?
	jz	skip_xon		; no
	cmp	cpb_incnt[si], LOW_MARK	; down low enough for a cushion?
	ja	skip_xon		; no
	mov	cpb_in_xoff[si], FALSE	; turn off XOFF flag
	call	send_xon		; and tell remote to continue sending


skip_xon:
	sti				; and turn interrupts on again

	ret

get14	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	STAT14
;;
;;	Input Parameters
;;		Set the COM port in DX (0 or 1), Base in DI, CPB in SI
;;
;;	This routine will return the status of the comm port in AX. A
;;	bit of fudging is done, since the low level interrupt driver may be
;;	handling the XON/XOFF status.  General logic of the routine:
;;
;;	Starting with the real status of the device (from the LSR register),
;;	turn on the character ready bit if there are any characters in the
;;	low level receive buffer.  If there is a character in the buffer,
;;	determine if there is an error waiting for the next read. If there
;;	is, then set all error conditions, including BREAK (if break is
;;	considered an error condition).
;;
;;	Next, get the modem status, and then check out the XOFF status. If
;;	XOFF is set for the outgoing line, make it appear the Transmit Shift
;;	and Hold Registers are still busy.   If the option is set, OR the
;;	DTR/DSR bit to show the "terminal" is ready.
;;
;;	Finally, if there is a time out (and timed_out) is a counter, then
;;	set the TIMED_OUT flag, bit 7 of the high byte.
;;
;;	Return in AX
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
stat14	proc	near

	lea	dx, LSR[di]		; try the *real* status register
	in	al, dx
	xor	ah, ah

	cmp	cpb_incnt[si], 0	; any characters waiting in the buffer?
	jz	no_brk_error		; no.
	or	ax, LSR_DATA		; set the bit if not already set.

					; now see if the next char in the
					; buffer will bring an error with it

	push	ax
	push	bx
	mov	bx, cpb_intail[si]	; get the ptr to the next character
	call	emask_byte		; get the error status byte and mask
	test	[bx], ax		; and see if an error condition
	pop	bx
	pop	ax

	jz	no_brk_error			; no errors
	or	ax, LSR_PARITY + LSR_FRAMING + LSR_OVERRUN
	test	cpb_mode[si], BREAK_IS_ERROR_OPTION
	jz	no_brk_error		; break is not posible cause of
					; error
	or	ax, LSR_BREAK		; break considered an error, set bit

no_brk_error:
	mov	ah, al			; now get the actual line status in al
	lea	dx, MSR[di]		; modem status register
	in	al, dx

					; even if DSR is low, if set to ignore
					; its state, set status high

	cmp	cpb_out_xoff[si], FALSE	; is state currently in XOFF on output?
	jz	dsr_stat		; no XOFF worries, see about DSR

					; XOFF is set, which should look like
					; a device not ready.  Fake it out.
	and	al, not LSR_TSRE + LSR_THRE

dsr_stat:
	test	cpb_mode[si], DTR_OPTION
	jz	no_dsr_concern		; nope
	or	al, MSR_DSR		; set the status bit

no_dsr_concern:

	cmp	timed_out, FALSE	; any faked time outs?
	jz	no_time_outs		; no time_outs;
	dec	timed_out		; start zeroing it for next time
	or	ah, 080h		; and set the time out flag

no_time_outs:
	ret
stat14	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	New Functions:	All of these functions require that AH = 4, and
;;			the sub function can be found in AL as follows:
;;
;;		if	AL = 0, return 0ff0 in AX to determine load status
;;				 (actual value in 'special_return_value')
;;
;;		if	AL = 1, initialize mode as per CX, clear buffers.
;;
;;		if	AL = 2, then initialize with baudrates and other
;;				params in CL, as if AL of AH = 0 init
;;				BIT 6 & 5 ==> Baudrates
;;				    0 , 1 ==> 19,200
;;				    1 , 0 ==> 38,400
;;				BIT 4 & 3 ==> Parity
;;				BIT 2     ==> Stop Bits
;;				BIT 1 & 0 ==> Word Length
;;
;;		if	AL = 3, set the timeout value as per CX
;;
;;		if	AL = 4, Clear the Input Buffer
;;
;;		if	AL = 5, Return count in Input buffer
;;
;;		if	AL = 6, Clear the Transmit Buffer
;;
;;		if	AL = 7, Return the count in the Transmit Buffer
;;
;;		if	AL = 8, uninstall the TSR driver, then release
;;				memory
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
funcs_table	label	word		;;
	dw	offset	new00		;; Each function corresponds to the 
	dw	offset	new01		;; AL value used for the sub-function
	dw	offset	new02		;;
	dw	offset	new03		;;
	dw	offset	new04		;;
	dw	offset	new05		;;
	dw	offset	new06		;;
	dw	offset	new07		;;
	dw	offset	new08		;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	NEWFUNCS14
;;
;;	The new function dispatcher. Checks to make sure AL is valid and
;;	returns 0xffff if not.
;;
;;	If AL is valid, then call the proper routine.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
newfuncs14	proc	near
assume	ds:code

	cmp	al, 08h		; out of bounds?
	jle	dispatch	; no
	mov	ax, 0ffffh	; yes, error code
	ret

dispatch:
	call	get_cpb			; get si to point to the proper cpb
	mov	di, cpb_base[si]	; point the ports!
	xor	bx, bx
	mov	bl, al
	shl	bx, 1
	mov	bx, funcs_table[bx]
	call	bx
	ret
newfuncs14	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;		if	AL = 0, return 0ff0 in AX to determine load status
;;				 (actual value in 'special_return_value')
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
new00	proc	near

	mov	ax, special_return_value
	ret
new00	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;		if	AL = 1, initialize mode as per CX, clear buffers.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
new01	proc	near

	mov	cpb_mode[si], cx	; move the new mode in
	call	init_buffers		; and reset the pointers
	ret
new01	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;		if	AL = 2, then initialize with baudrates and other
;;				params in CL, as if AL of AH = 0 init
;;				BIT 6 & 5 ==> Baudrates
;;				    0 , 1 ==> 19,200
;;				    1 , 0 ==> 38,400
;;				BIT 4 & 3 ==> Parity
;;				BIT 2     ==> Stop Bits
;;				BIT 1 & 0 ==> Word Length
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
new02	proc	near

	lea	dx, LCR[di]			; get the Latch
	in	al, dx
	or	al, LCR_DLAB			; turn on the divisor
	out	dx, al				; in the chip

	push	cx
	mov	ax, cx

	and	ax, 00e0h			; only the highest three bits
	mov	cl, 5
	shr	ax, cl
	add	ax, 7				; makes offset start at 8
						; (19200)

	call	get_baud			; then get the correct divisor
						; allows higher than 9600
	pop	cx

	lea	dx, DATA[di]			; get the base address
	out	dx, ax				; output the whole word

	lea	dx, LCR[di]			; get the Latch
	mov	al, cl				; get the other parameters and
	and	al, 01fh			; mask only parity, stop bits,
						; word length
	out	dx, al				; set the params

	ret
new02	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;		if	AL = 3, set the timeout value as per CX
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
new03	proc	near

	mov	cpb_timeout[si], cx
	ret
new03	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;		if	AL = 4, Clear the Input Buffer
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
new04	proc	near

	cli
	mov	cpb_incnt[si], NO_CHARS
	mov	ax, cpb_inbase[si]
	mov	cpb_inhead[si], ax
	mov	cpb_intail[si], ax
	sti
	ret
new04	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;		if	AL = 5, Return count in Input buffer
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
new05	proc	near
	mov	ax, cpb_incnt[si]
	ret
new05	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;		if	AL = 6, Clear the Transmit Buffer
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
new06	proc	near

	cli
	mov	cpb_outcnt[si], NO_CHARS
	mov	ax, cpb_outbase[si]
	mov	cpb_outhead[si], ax
	mov	cpb_outtail[si], ax
	sti
	ret
new06	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;		if	AL = 7, Return the count in the Transmit Buffer
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
new07	proc	near
	mov	ax, cpb_outcnt[si]
	ret
new07	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;		if	AL = 8, uninstall the TSR driver, then release
;;				memory
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
new08	proc	near
assume	ds:code

	mov	si, offset cpb1		; set up for port1
	cmp	cpb_oint_add[si], 0	; com port installed?
	jz	new0801		; no

	call	unset_up		; and kill everything associated
					; with this comm port

new0801:
	mov	si, offset cpb2		; set up for port2
	cmp	cpb_oint_add[si], 0	; com port installed?
	jz	new0802			; no

	call	unset_up		; and kill everything associated
					; with this comm port


new0802:
	cli
	mov	dx, old_int14
	mov	al, 014h
	push	ds
	mov	ds, old_int14[2]
	DOSINT	25h			; reset the serial port interrupt
	pop	ds

	mov	dx, orig_timer
	mov	al, TIMER_TICK_INT_NO
	push	ds
	mov	ds, orig_timer[2]
	DOSINT	25h			; reset the timer_tick interrupt
	pop	ds

	push	cs
	pop	es			; free up our own memory
	DOSINT	49h			; the environment
	sti

	ret
new08	endp

unset_up proc near
assume	ds:code
	mov	di, cpb_base[si]	; get the base port

	cli
	in	al, INT_MASK_PORT	; get the old flag from the 8259
	mov	bl, cpb_pic_mask[si]	; get the interrupt enable mask
	not	bl			; and turn it into a disable mask
	or	al, bl			; turn off the masked bit
	out	INT_MASK_PORT, al

	lea	dx, IIR[di]
	xor	ax, ax			; zero out for no ints
	out	dx, al			; and turn off interrupts

	lea	dx, MCR[di]		; turn off OUT2 on comm port
	in	al, dx
	and	al, not MCR_OUT2	; hardware disable interrupts
	and	al, not MCR_DTR		; and drop DTR
	out	dx, al

	mov	dx, cpb_oint_add[si]
	mov	al, cpb_int_no[si]
	push	ds
	mov	ds, cpb_oint_add[si][2]
	DOSINT	25h			; reset the comm port interrupt
	pop	ds

	sti
	ret

unset_up endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	NEW_TICK
;;
;;	Takes over the timer tick, and simply reduces the timer count
;;	every 1/18.2 of a second
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
new_tick	proc	far
	assume	ds:nothing

	pushf
	cmp	cs:timer_tick, 0
	jz	no_dec1
	dec	cs:timer_tick

no_dec1:
	cmp	cs:my_timer, 0
	jz	no_dec2
	dec	cs:my_timer

no_dec2:
	popf
	jmp	dword ptr cs:[orig_timer]
new_tick	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;			END OF RESIDENT CODE SPACE
my_size	equ	(($-code)/16 + 1)	; waste a paragraph!
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;		Messages only needed while installing
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
say_hi		db	0dh,0ah,"Installing COM driver....", 0dh,0ah,0ah,'$'
say_whoops	db	0dh,0ah,"COM driver already installed!",0dh,0ah,0ah,'$'
say_bad_port1	db	0dh,0ah,"Port 1 exists and is not 3F8!",0dh,0ah,0ah,'$'
say_bad_port2	db	0dh,0ah,"Port 2 exists and is not 2F8!",0dh,0ah,0ah,'$'


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	INITIALIZE
;;
;;	This routine gets called exactly once from the install routine
;;	and sets up the 8250 and 8259 to generate and receive interrupts
;;	It also resets and zeroes out each of the buffers
;;
;;	SI should point to the correct CPB
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
initialize	proc near

	push	ax
	push	dx
	push	di

	mov	di, cpb_base[si]	;the port base address (2F8, 3F8)

	cli
	xor	ax,ax
	lea	dx, IER[di]		; load the interrupt register in
	out	dx, al			; and turn off *all* interrupts
	lea	dx, MCR[di]		; modem control
	out	dx, al			; turn off the sneaky bits

	call	init_buffers		; then the buffers
			
	mov	al, IER_RDA		; turn on ints for data ready
	lea	dx, IER[di]		; load up the interrupt register
	out	dx, al			; and turn 'em on

	in	al, INT_MASK_PORT	; get the old flag from the 8259
	and	al, cpb_pic_mask[si]	; turn off the masked bit
	out	INT_MASK_PORT, al

	lea	dx, MCR[di]		; put OUT2 back on
	in	al, dx
	or	al, MCR_OUT2		; hardware enable interrupts
	or	al, MCR_DTR		; bring up DTR
	out	dx, al

	lea	dx, data[di]		; clean out the receive buffer
	in	al, dx			; a few times can't hurt
	in	al, dx

	pop	di
	pop	dx
	pop	ax

	sti				;turn interrupts back on
	ret				; and return

initialize	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	INSTALL
;;
;;	First check to make sure TSRCOMM isn't already installed
;;
;;
;;	For each of the comm ports, save the old interrupt vector address,
;;	then remap the vector to point to the new routines, then
;;	initialize the port with RX and error interrrupts turned on.
;;
;;	If the comm port  vector isn't what we expect, output an error
;;	message and exit.
;;
;;	Only play with ports which exist
;;
;;	Take over the old timer_tick interrupt with the new one,
;;	and take over the old serial interrupt routine
;;
;;	Finally, release the pointer to the environment, and go TSR!
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

install	proc near
	assume	ds:code

	push	cs		; set data segment to point to code segment
	pop	ds

	mov	ax, 0400h	; see if we exist
	int	14h		; call the interrupt

	cmp	ax, special_return_value
	jnz	inst2		; already loaded....
	lea	dx, say_whoops
	DOSINT	9h

	int	20h


inst2:
	lea	dx, say_hi
	DOSINT	9h


	push	es
	mov	si, 040h		; the segment address for comm ports
	mov	es, si

	cmp	es:[0], PORT1		; is it 3f8?
	jnz	exist1			; no
	mov	ah, TRUE		; yes. set the flag
	jmp	chk_port_2

exist1:
	cmp	word ptr es:[0], FALSE	; does it exist?
	jz	chk_port_2
	lea	dx, say_bad_port1	; something is there. No good!
	jmp	bomb_out

chk_port_2:
	cmp	es:[2], PORT2		; is it 2f8?
	jnz	exist2			; no
	mov	al, TRUE		; yes. set the flag
	jmp	install_it

exist2:
	cmp	word ptr es:[2], FALSE	; does it exist?
	jz	install_it		; no
	lea	dx, say_bad_port2	; something at 40:2 which isn't 2f8!

bomb_out:
	pop	es
	DOSINT	9h

	int	20h

install_it:
	pop	es

	cmp	ah, TRUE
	jnz	install_2

	push	ax
	mov	si, offset cpb1		; set up for port1
	mov	dx, 0
	call	set_up			; set it up
	pop	ax

install_2:

	cmp	al, TRUE
	jnz	install_3

	mov	si, offset cpb2		; set up for port2
	mov	dx, 1
	call	set_up			; set it up


install_3:
	mov	al, 14h			; save the old interrupt 14 services
	DOSINT	35h
	mov	old_int14, bx
	mov	old_int14[2], es

	lea	dx, new_14		; steal the int 14 vector
	mov	al, 14h
	DOSINT	25h

	mov	al, TIMER_TICK_INT_NO	; save the "user" interrupt, not 08h
	DOSINT	35h
	mov	orig_timer, bx
	mov	orig_timer[2], es

	lea	dx, new_tick		; steal the timer tick
	mov	al, TIMER_TICK_INT_NO
	DOSINT	25h

	mov	es, env_ptr	; free up the little memory used by
	DOSINT	49h		; the environment

	mov	dx, my_size	; so, we get to junk outselves!
	DOSINT	31h

install	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	SET_UP
;;
;;	Save and replace the approriate interrupt vectors, then call
;;	initialize for the CPB pointed to by SI
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
set_up proc	near

	mov	al, cpb_int_no[si]
	DOSINT	35h
	mov	cpb_oint_add[si], bx
	mov	cpb_oint_add[si][2], es

	mov	al, cpb_int_no[si]
	mov	dx, cpb_nint_off[si]
	DOSINT	25H

	mov	al, DEF_INIT		; init through the BIOS first
	mov	ah, 0
	int	14h
	call	initialize
	ret
set_up	endp

code	ends
	end	start

                                                                                                       