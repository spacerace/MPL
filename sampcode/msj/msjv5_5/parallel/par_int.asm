-0
Message 205:
From c-rossgr Wed Aug  1 11:32:17 1990
To: ericm joannes
Subject: Listing3
Date: Wed Aug  1 11:32:16 1990


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  PAR_INT.ASM -     Parallel Printer Port By Interrupts
;;;
;;;  Copyright 1990 by Ross M. Greenberg
;;;        for Microsoft Systems Journal
;;;
;;;  This program demonstrates the ability to communicate between
;;;  two PCs via the parallel printer port.  In order for this
;;;  program to work, a cable must be constructed that has the
;;;  following pinouts:
;;;
;;;     1 ----------  1  - Common Ground
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
;;;  A copy of the program should be running on each side of the
;;;  cable. Once running, type whatever you like, then enter an '!'
;;;  to send the data queued up to the remote machine.  To exit,
;;;  type a '\'.  Do not control-C out of the program: vectors
;;;  are not restored nor are interrupts turned off.  You'll likely
;;;  crash badly.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  Main routine:
;;;        Initialize
;;;        - save the old interrupt 0xf vector address
;;;        - replace it with a vector to the int_handler
;;;        - turn on interrupt enable in the parallel port
;;;        - unmask the printer interrupt in the 8259 mask word
;;;        - clear any interrupts outstanding on the data port
;;;
;;;        Poll
;;;        - if there's a keyboard hit, process it.
;;;          - if the key is a '\', then exit after restoring vectors
;;;            and turning off printer interrupts
;;;          - if the key is a '!', then get and transmit the first
;;;                 character, if any, in the transmit buffer
;;;        - if there are any characters in the receive buffer
;;;               - fetch the next character and display it
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


code segment     para
assume     cs:code

TRUE       equ   1
FALSE      equ   0

BASE_PORT  equ   03bch       ; change this to your port of
                             ; choice. Look at the word at
                             ; 0040:0008 to determine your
                             ; port(s).  The 03bch value
                             ; indicates that the port is
                             ; set up for a combination display
                             ; and parallel port card.  If you
                             ; have a standard parallel port only
                             ; card, chances are your value
                             ; will be 0378h






start:     mov   ax, 350fh   ;save old 0xf interrupt vector
     int   21h
     mov   cs:[old_vect], es
     mov   cs:[old_vect + 2], bx

     push  cs
     pop   ds
     mov   dx, offset int_handler
     mov   ax, 250fh         ;set new vector to cs:int_handler
     int   21h

     mov   dx, BASE_PORT + 2 ; turn interrupts on in the printer
     in    al, dx
     or    al, 10h           ; fast instruction, so futz
     jmp   $+2
     out   dx, al

     mov   dx, 21h           ; unmask the printer interrupt bit
     in    al, dx
     and   al, 7fh           ; fast instruction, so futz
     jmp   $+2
     out   dx, al


     mov   dx, BASE_PORT     ; turn off outstanding ints for remote
     mov   al, 00
     out   dx, al

poll_lp:
     mov   ah, 0bh           ; key hit?
     int   21h
     cmp   al, 0ffh
     jz    get_keyb          ; yes

@@:  push  si                ; any outstanding characters in recbuf?
     mov   si, offset cs:inchar_head
     call  unstuff_char
     pop   si
     jnc   poll_lp           ; no
     mov   dl, al            ; yes
     mov   ah, 2             ;display it
     int   21h
     jmp   @B                ; for every character in buffer

get_keyb:
     mov   ah, 1             ; get the key
     int   21h

     cmp   al, '\'           ; time to leave?
     jz    exit              ; yes
     cmp   al, '!'           ; time to transmit?
     jnz   @F                ; no
     push  si                ; yes, get first character
     mov   si, offset cs:outchar_head
     call  unstuff_char
     pop   si
     jnc   poll_lp           ; jump if no characters

     call  send_byte         ; send the first nybble of first char
     jmp   poll_lp           ; and go back for more.

@@:  push  si                ; just stuff the character into buffer
     mov   si, offset cs:outchar_head
     call  stuff_char
     pop   si
     jmp   poll_lp           ; and go back for more

exit:
     mov   dx, BASE_PORT + 2 ; turn interrupts off in the printer
     in    al, dx
     and   al, 0efh          ; fast instruction, so futz
     jmp   $+2
     out   dx, al

     mov   dx, 21h           ; mask the printer interrupt bit
     in    al, dx
     or    al, 080h          ; fast instruction, so futz
     jmp   $+2
     out   dx, al

     mov   ds, cs:[old_vect] ; restore the original interrupt
     mov   dx, cs:[old_vect + 2]
     mov   ax, 250fh
     int   21h

     mov   ax, 4c01h         ; and exit
     int   21h


sending    dw    0           ; flag to indicate in middle of a
                             ; send

old_vect   dw    2     dup (0)  ; holds the original interrupt vector

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  Interrupt Handler
;;;
;;;  An interrupt can either be the result of an incoming character's
;;;  first nybble, or the "ACK" of the first nybble by the remote
;;;  machine in response to the first nybble.
;;;
;;;  Receiving an interrupt because of first nybble received:
;;;        - play bit games to remove the interrupt bit from the
;;;          nybble, then store the nybble and send an interrupt
;;;          as an acknowledgement.  Wait for the polarity of the
;;;          of the interrupt to show a new nybble is available,
;;;          process that nybble and create a byte.  Add the byte
;;;          to the receive circular buffer and IRET.
;;;
;;;  Receiving an interrupt as ACK of first nybble:
;;;        - send the second nybble out the port.  If there are more
;;;          characters in the transmit buffer, send the first nybble
;;;          of the next character.  Reset from the interrupt and
;;;          return from interrupt.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

int_handler:
     sti                     ; re-enable other interrupt handling
     push  ax
     push  dx

     cmp   cs:[sending], FALSE  ; is this an ACK or a first nybble?
     jnz   second_nybble        ; it's an ACK

     mov   dx, BASE_PORT + 1 ; first nybble.  Get it
     in    al, dx

     rcl   al, 1             ; save the topmost bit in carry
     pushf                   ; save the flags
     shl   al, 1             ; shift the interrupt bit away
     popf                    ; get back the flags
     rcr   al, 1             ; rotate the carry bit back. Voila!

     shr   al, 1             ; shift the low nybble low
     shr   al, 1
     shr   al, 1
     shr   al, 1

     mov   ah, al            ; save it in ah
     mov   dx, BASE_PORT     ; reset the interrupt: level or edge
     mov   al, 00h           ;   triggered, still needs to be done
     out   dx, al
     jmp   $+2
     mov   al, 08h
     out   dx, al            ; now generate an interrupt

     inc   dx                ; now poll for interrupt bit to drop
@@:  in    al, dx
     test  al, 040h          ; wait for int pin to zero out
     jnz   @B

     rcl   al, 1             ; got other nybble, process as above
     pushf
     shl   al, 1
     popf
     rcr   al, 1

     and   al, 0f0h          ; strip low nybble
     or    al, ah            ; an 'OR' creates character in al

     push  si
     mov   si, offset cs:inchar_head
     call  stuff_char        ; save it in circular buffer
     pop   si

back:
     mov   dx, 20h           ; non-specific EOI
     mov   al, 20h
     out   dx, al
     pop   dx
     pop   ax

     iret                    ; back to work

cur_byte   db    0           ; just a place to save byte



second_nybble:
     mov   dx, BASE_PORT + 1 ;swallow the interrupt
     in    al, dx
     mov   al, cs:[cur_byte] ; retrieve the byte
     rcl   al, 1
     pushf
     shr   al, 1             ; shift right with zero for bit
     popf
     rcr   al, 1             ; get back from carry
     xor   al, 080h          ; reverse the hi bit: hardware is weird

     shr   al, 1
     shr   al, 1
     shr   al, 1

     mov   dx, BASE_PORT
     out   dx, al            ; send byte
     dec   cs:[sending]      ; done sending

     push  si                ; any more characters to send?
     mov   si, offset cs:outchar_head
     call  unstuff_char
     pop   si
     jnc   back              ; no

     call  send_byte         ; yes.  Send first nybble

     jmp   back              ; and reset, then iret


send_byte:
     inc   cs:[sending]      ; set the flag
     mov   cs:[cur_byte], al ; save the byte

     shl   al, 1             ; bring low nybble high
     shl   al, 1
     shl   al, 1
     shl   al, 1

     rcl   al, 1             ;low nybble now high
     pushf
     shr   al, 1
     or    al, 080h          ; set the interrupt bit
     popf
     rcr   al, 1
     xor   al, 080h          ; reverse hi bit: hardware is weird

     shr   al, 1
     shr   al, 1
     shr   al, 1             ; shift it down to send it.

     mov   dx, BASE_PORT
     out   dx, al            ; send it, which generates first
                             ; interrupt on send

     ret                     ; back to work


BUFSIZE    equ   1024

outchar_head     dw    outchar_buf       ; ptr to head of buffer
outchar_tail     dw    outchar_buf       ; ptr to tail of buffer
outchar_end      dw    outchar_finish    ; lazy comparison
outchar_buf      db    BUFSIZE dup(0)    ; The buffer itself
outchar_finish   db    0                 ; place holder

inchar_head      dw    inchar_buf
inchar_tail      dw    inchar_buf
inchar_end dw    inchar_finish
inchar_buf db    BUFSIZE dup(0)
inchar_finish    db    0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  These routines either remove a character if there is one
;;;  available in the appropriate circular buffer (unstuff_char), or
;;;  place a character in the appropriate circular buffer.  Have SI
;;;  point to the head pointer for a given buffer.
;;;
;;;  stuff_char:   saves the character in al
;;;  unstuff_char: retrieves a character and returns with it in AL
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

unstuff_char:
     push  bx
     mov   bx, cs:[si + 2]   ; get the tail pointer
     cmp   bx, cs:[si + 4]   ; compare it with the end of buffer
     jnz   @F                ; equal?
     mov   bx, si            ; yes. Reset to beginning of buffer
     add   si, 6
@@:  cmp   bx, cs:[si]       ; running up against head pointer?
     jz    @F                ; yes. Ignore. Buffer empty
     inc   bx                ; increment the pointer
     mov   al, cs:[bx]       ; get the character
     mov   cs:[si + 2], bx   ; save the pointer
     stc                     ; carry = character retrieved
     pop   bx
     ret

@@:  clc
     pop   bx
     ret

stuff_char:
     push  bx
     mov   bx, cs:[si]       ; like above, but head instead of tail
     inc   bx
     cmp   bx, cs:[si + 4]
     jnz   @F
     mov   bx, si
     add   si, 6
@@:  cmp   bx, cs:[si + 2]   ; like above, but tail instead of head
     jz    @F
     mov   cs:[bx], al
     mov   cs:[si], bx
     stc                     ; carry = character written
     pop   bx
     ret
@@:  clc
     pop   bx
     ret

code ends

end start

