;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  PAR_POLL.ASM      -     Parallel Printer Port By Polling
;;;
;;;
;;;  Copyright 1990 by Ross M. Greenberg
;;;        for Microsoft Systems Journal
;;;
;;;  This program demonstrates the ability to communicate between
;;;  two PCs connected via the parallel printer port.  In order for
;;;  this program to work, a cable must be constructed that has the
;;;  following pinouts:
;;;
;;;     1 ----------  1- Common Ground
;;;     2 ---------- 15  \
;;;     3 ---------- 13   \
;;;     4 ---------- 12    > Data Out
;;;     5 ---------- 10   /
;;;     6 ---------- 11  /
;;;    10 ----------  5  \
;;;    11 ----------  6   \
;;;    12 ----------  4    > Data In
;;;    13 ----------  3   /
;;;    15 ----------  2  /
;;;
;;;  Run a copy of this program at each end of the cable.
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;  Public Entry Points

public     _psend, _prec, _pinit

DOSSEG
.MODEL     small

.STACK     100h


FALSE      equ   00h
TRUE       equ   01h

FPTR       equ   4
TIMEOUT    equ   -1

.DATA
;;no data


.CODE

real_port  dw    0           ; the actual port used
timer      dw    0           ; setup for timeout/dummy loop

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  PINIT(PORT) -     Initialize the port
;;;
;;;  Port is zero based (first LPT is zero).  First, find the actual
;;;  base port to use and stuff it away in "real_port".  Then, since
;;;  we need an accurate representation of the speed of the machine,
;;;  use the tick timer in INT 0x1a to see how fast the machine is
;;;  for looping constants.  Store that away in timer, and return the
;;;  timing constant.
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_pinit     proc  near
     push  bp
     mov   bp, sp

     push  bx
     push  cx
     push  dx
     push  si


     push  ds
     mov   ax, 40h             ; printer ports are stored @ 40:8
     mov   ds, ax
     mov   bx, [bp + FPTR]
     shl   bx, 1
     mov   dx, word ptr 8[bx]  ; get the base port, offset by port #
     pop   ds
     mov   cs:[real_port], dx  ; store it

     mov   ah, 0             ; get the tick
     int   01ah
     mov   si, dx

@@:  mov   ah, 0
     int   01ah
     cmp   dx, si            ; stay in loop until tick changes
     jz    @B

     mov   si, dx
     xor   bx, bx

     cli                     ; accuracy means no ints!
@@:  inc   bx                ; loop three times
     mov   cx, bx
     loop  $                 ; one
     mov   cx, bx
     loop  $                 ; two
     mov   cx, bx
     loop  $                 ; three
     mov   ah, 0
     int   01ah              ; get the time
     cmp   dx, si            ; changed?
     jz    @B                ; nope. Increase the count
     sti                     ; restart interrupts
     mov   cl, 9
     shl   bx, cl            ; just make it a number of some sort
@@:  mov   cs:[timer], bx    ; and save it
     mov   ax, bx            ; and return with it in ax
     pop   si
     pop   dx
     pop   cx
     pop   bx
     pop   bp
     ret

_pinit     endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  PSEND(STRING_PTR, LEN)
;;;
;;;  Calls the send_char routine with character to send in al for LEN
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_psend     proc  near
     push  bp
     mov   bp, sp
     push  bx
     push  cx
     push  dx

     mov   dx, cs:[real_port]      ; base port
     mov   bx, [bp + FPTR]         ; send buffer
     mov   cx, [bp + FPTR + 2]     ; count
     xor   bp, bp                  ; zero out counter

     jcxz  _psend_ret        ; if empty, go home

_psend_lp:
     mov   al, [bx]          ; get the character
     call  send_char         ; and send it
     jnc   _psend_ret        ; if it didn't send, go home

     inc   bx                ; increment the pointer
     inc   bp                ; and the counter
     loop  _psend_lp         ; do it again

_psend_ret:
     pop   dx
     pop   cx
     pop   bx
     mov   ax, bp            ; move counter into ax
     pop   bp
     ret                     ; go home
_psend     endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  PREC(STRING_PTR, LEN)
;;;
;;;  Fetch LEN characters and stuff them into STRING_PTR.
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_prec      proc  near
     push  bp
     mov   bp, sp
     push  bx
     push  cx
     push  dx

     mov   bx, [bp + FPTR]         ; receive buffer
     mov   cx, [bp + FPTR + 2]     ; count
     mov   dx, cs:[real_port]      ; the base port

     xor   bp, bp                  ; zero cnt

     jcxz  _prec_ret         ; no count, no probs

_prec_lp:

     call  get_char          ; get a character
     jnc   bad_rec           ; whoops! set for short cnt

     mov   [bx], al          ; save the character

     inc   bx                ; bump the pointer
     inc   bp                ; and the counter

     loop  _prec_lp          ; as often as you have to
     jmp   _prec_ret

bad_rec:
     or    bp, 08000h        ; set the short cnt indicator

_prec_ret:
     pop   dx
     pop   cx
     pop   bx
     mov   ax, bp            ; move the count in
     pop   bp
     ret
_prec      endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  send-char   -     sends character in al
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
send_char  proc  near
     push  cx
     mov   cx, cs:[timer]    ; get the timing constant

     mov   ah, al            ; save the character
     inc   dx                ; bump up to the control port
     in    al, dx            ; read it
     shl   al, 1             ; pop it left
     jc    go_home           ; hi bit set? Remember it's backwards

     dec   dx                ; back to the base port
     mov   al, ah            ; get the character back

     shr   al,1
     shr   al,1
     shr   al,1
     shr   al,1
     and   al,0Fh            ; hi nybble first, anded just for that

     out   dx, al            ; output the nybble

     inc   dx                ; back up to control port

@@:  in    al, dx            ; get data
     shl   al, 1             ; hi bit set to go?
     jc    @F                ; yes
     loop  @B                ; no, loop until timeout

     jmp   go_home           ; timed out. Bomb out.

@@:  dec   dx                ; back to data port
     mov   al, ah            ; get the character back
     or    al, 10h           ; set the hi bit (its backwards!)
     out   dx, al            ; output the character

     inc   dx                ; and back to control

     mov   cx, cs:[timer]    ; get the timing constant

@@:  in    al, dx            ; and wait for ACK
     shl   al, 1
     jnc   go_home           ; done
     loop  @B                ; looks like an error?

go_home:
     dec   dx                ; mark it as ready and willing
     mov   al, 10h
     out   dx, al
     xor   ah, ah
     stc
     pop   cx
     ret

send_char  endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  get-char    -     retrieves character into al
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

get_char   proc  near

     push  cx
     mov   cx, cs:[timer]    ; get looping constant

     inc   dx                ; upto control port

@@:  in    al, dx            ; we shall do no port before its time
     shl   al, 1             ; remember its reversed
     mov   ah, al            ; save it unshifted
     jc    @F                ; so carry means go ahead
     loop  @B                ; still set, try again

     clc                     ; carry clear means no good
     pop   cx
     ret                     ; go home

@@:  and   ah,0F0h           ; only the high nybble
     dec   dx                ; back to data port
     mov   al, 0             ; ACK the nybble
     out   dx, al
     inc   dx                ; back to control

     mov   cx, cs:[timer]    ; get looping constant

@@:  in    al, dx            ; wait for it
     shl   al, 1
     jnc   @F
     loop  @B

     dec   dx                ; no go. Clean up a tad.
     mov   al, 10h           ; set the flag
     out   dx, al            ; output it on the data port
     clc
     pop   cx
     ret

@@:  in    al, dx            ; get the next nybble
     shr   al, 1             ; normalize it
     shr   al, 1
     shr   al, 1
     and   al, 0Fh           ; strip off the naughty bits
     or    ah, al            ; create a character in ah
     dec   dx                ; ack the byte
     mov   al, 10h           ; set the flag
     out   dx, al            ; output it
     mov   al, ah            ; retreive the character
     stc
     pop   cx
     ret
get_char   endp

     end
