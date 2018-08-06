;;	Microsoft Systems Journal
;;	Volume 2; Issue 5; November, 1987

;;	Code Listings For:

;;	MDM_DRV
;;	pp. 51-65

;;	Author(s): Ross M. Greenberg
;;	Title:     A Strategy for Building And Debugging Your FIrst MS-DOS 
;;	           Device Driver



;; Code listings for MDM-DRV.ASM and DEBUG.ASM


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      MDM_DRV.ASM     An XMODEM Device Driver
;;
;;      Copyright       1987, Ross M. Greenberg
;;
;;      This program is a device driver which enables a simply DOS command
;;      such as:
;;
;;      C>COPY MDM FILENAME.EXT
;;
;;      to automatically use the popular XMODEM/Checksum protocol for
;;      transferring files through the serial port.
;;
;;      The current version of the code expects there to be a Hayes
;;       compatible modem atached to the serial port.
;;
;;      To compile and use this program, type the following commands (you
;;      must have MASM4.0):
;;
;;
;;      C> MASM MDM_DRV;
;;      C> LINK MDM_DRV;
;;      C> EXE2BIN MDM_DRV.EXE MDM_DRV.SYS
;;
;;      Remember that you must include a line of the form:
;;
;;      DEVICE=C:\devices\MDM_DRV.SYS
;;
;;      in your CONFIG.SYS file on the disk you usually boot from.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;DEBUG   equ     0                               ; remove the semi colon
                                                ; to get debug messages
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;                      GLOBAL DEFINITIONS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

MAX_CMD         equ     16      ; total number of commands in this driver

TIMER_ADD       equ     (01ch * 4h)

NULL    equ     0
TRUE    equ     1
FALSE   equ     0

FULLBLKSIZE     equ     (128 + 3 + 1)   ; Total size of an XMODEM/Checksum
                                 ; block. SOH + BLK + ~BLK + DATA + CHK
BLKSIZE         equ     (128)           ; Number of data bytes in an XMODEM Blk
NAK_MAX         equ     5               ; Total number of NAKs before an abort

SOH     equ     01h             ; the first character of an XMODEM block
EOT     equ     04h             ; End of transmission in XMODEM
ACK     equ     06h             ; Last block received alright
NAK     equ     015h            ; Last block was not received properly
ABORT   equ     018h            ; Abort (CANcel) this transfer

CR      equ     0dh
LF      equ     0ah
ESCAPE  equ     01bh

LEFT_BRACKET    equ     '['             ; characters output by the STAT
RIGHT_BRACKET   equ     ']'             ; macro.
DOT             equ     '.'
STAR            equ     '*'
QUESTION        equ     '?'
EXCLAIM         equ     '!'
DUP_BLK          equ     'D'


ONE_SECOND      equ     18
TEN_SECONDS     equ     (ONE_SECOND * 10)
ONE_MINUTE      equ     (ONE_SECOND * 60)
FOREVER         equ     -1                      ; or close enough!

ERROR           equ     08000h                  ; general error bit
BUSY            equ     0200h                   ; Device Busy Bit
DONE            equ     0100h                   ; command finished

UNK_COMMAND     equ     03h
WRITE_ERROR     equ     0ah
READ_FAULT      equ     0bh
GEN_FAILURE     equ     0ch
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;                              MACRO DEFINITIONS
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      STAT    <arg>
;;
;;      This macro simply outputs the character represtned by <arg>.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
STAT            macro   arg
        push    ax
        mov     al, arg
        call    o_char
        pop     ax

endm
        

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      PUSHALL
;;      POPALL
;;
;;       PUSHALL_AX
;;      POPALL_AX
;;
;;      Simple macros for saving all the registers, then another to pop them
;;      back.
;;
;;      The "_AX" macors do not save or restore the AX Register
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PUSHALL         macro
        pushf
        push    ax
        push    bx
        push    cx
        push    dx

        push    si
        push    di

 push    bp

        push    ds
        push    es
endm
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
POPALL          macro
        pop     es
        pop     ds

        pop     bp

        pop     di
        pop     si

        pop     dx
        pop     cx
        pop     bx
        pop     ax
 popf
endm
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PUSHALL_AX      macro
        pushf
        push    bx
        push    cx
        push    dx

        push    si
        push    di

        push    bp

        push    ds
        push    es
endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
POPALL_AX       macro
        pop     es
        pop     ds

        pop     bp

        pop     di
        pop     si

        pop     dx
        pop     cx
        pop     bx
        popf
endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

code     segment public  'CODE'

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;      The driver itself is one large procedure, called 'driver'
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
driver  proc    far

        assume  cs:code, ds:code, es:code       ; each segment should
                                                ; point to the code segment

        org     0                               ; drivers *must* start at
                                                ; ofset of zero
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                      DEVICE DRIVER HEADER
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
header1 dd      -1              ; This *must* be set to -1. Filled in by DOS
        dw      0e800h          ; character device,
                         ; IOCTL supported,
                                ; Output till busy,
                                ; open/close/rm supported

        dw      strat           ; point to the strategy routine
        dw      ints            ; point to the interrupt routine
        db      'MDM     '      ; The name of the device. Left justified,
                                ; space filled to eight characters


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                      THE REQUEST HEADER
;;      The address of the Request Header is passed to the device driver
;;      when the strategy routine is called.  This address should be saved
;;      and then made available to the interrupt routine.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
request struc
rlength db      0               ; 0  - length of pertinent data in header
unit    db      0               ; 1  - unit number (not used in MDM_DRV)
command  db      0               ; 2  - the actual command 
status  dw      0               ; 3  - return status
reserve db      8 dup (0)       ; 5  - Reserved for DOS
media   db      0               ; 13 - Media desciptor. (Not used in MDM_DRV)
address dd      0               ; 14 - Doubleword pointer for I/O
count   dw      0               ; 18 - unsigned int count for character I/O
sector  dw      0               ; 20 - starting sector. (Not used in MDM_DRV)
request ends
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      The dispatch table points to each of the routines the device
;;      driver needs.  This table is used by the interrupt routine,
;;      and the particular routine the Command Byte in the Request Header
;;      is used as index into the table.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
dispatch:
        dw      init            ; 0x00 - init the driver. Called on install
 dw      media_chk       ; 0x01 - media check (Not used in MDM_DRV)
        dw      bld_bpb         ; 0x02 - Build the BPB (Not used by MDM_DRV)
        dw      rd_ioctl        ; 0x03 - Read IOCTL
        dw      read            ; 0x04 - Read 'count' characters
        dw      nd_read         ; 0x05 - Non-destructive character read
        dw      inp_stat        ; 0x06 - Input Status
        dw      inp_flush       ; 0x07 - Input Flush
        dw      write           ; 0x08 - Write 'count' characters to device
        dw      write_vfy       ; 0x09 - Write Verify
        dw      out_stat        ; 0x0A - Output Status
        dw      out_flush       ; 0x0B - Fluch Output Buffers
        dw      wrt_ioctl       ; 0x0C - Write IOCTL
        dw      dev_open        ; 0x0D - Device Open (DOS 3.x only)
        dw      dev_close       ; 0x0E - Device Close (DOS 3.x only)
        dw      rem_media       ; 0x0F - Removable Media Routine (DOS 3.x only)
        dw      out_busy        ; 0x10 - Output Until Busy (DOS 3.x only)
                                        
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                      STRATEGY ROUTINE
;;
;;      This rouinte merely stored the address of the Request header found
;;      on entry in the ES:BX register pair.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
strat   proc    far

        mov     word ptr cs:[rh_ptr], bx
        mov     word ptr cs:[rh_ptr + 2], es
        ret

strat   endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                              GLOBAL VARIABLES
;;
;;      These variables are used for a variety of purposes.  Additionally
;;       the 'com_port' variable is the first value returned or written to
;;      for IOCTL READ and IOCTL WRITE calls, allowing application programs
;;      to change the communications port, baud rate and other information
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

com_port        dw      0               ; the comm port we are communicating
                                        ; with. COM1 = 0, COM2 = 1, etc.
init_data       db      10000011b       ; The initialization byte. See the
                                        ; BIOS Technical Reference Manual for
                                        ; details.
block_num       dw      0               ; current block being transferred
abort_xfer      dw      0               ; whether the current transfer is to
                                        ; aborted. (TRUE = YES)
nak_cnt         dw      0               ; How many Nak's have been sent or
                                        ; received

inrequest       dw      0               ; the number of bytes not yet processed
in_block        dw      0               ; has a block been received?
was_dialed      dw      0               ; if set, stat was offline at start
eof_flag        dw      0               ; set if eof seen in read

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

new_stack       dw      128 dup (0)     ; 128 words for the new stack
new_stack_end   dw      0

old_stack       dw      0               ; store old stack offset
                dw      0               ; store old stack segment

timer           dw      0               ; the timer count
old_timer       dw      0               ; store old timer offset
                dw      0               ; store old timer segment

rh_ptr          dd      0               ; this is where the Request Header
                                        ; address is stored

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      The I/O buffer, which looks like an XMODEM block
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_soh            db      0               ; This will be an SOH
_blk1           db      0               ; the block number
_blk2           db      0               ;    and its complement
_buf            db      128 dup (0)     ; the actual data
_chksum         db      0               ; the sum of all data bytes

_outptr         dw      _buf            ; points to next character to be output
_outcnt         dw      0               ; how many characters to be output

_inptr          dw      _soh            ; points to next input position
_incnt          dw      0               ; the number of chars in the buffer

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      A variety of messages
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

offline db      CR, LF, 'You are currently off line. Enter number to dial (or <RET> t
o answer): $'
dialing db      CR, LF, 'Dialing...standby....$'
no_con   db      CR, LF, 'Could not connect to remote.....', CR, LF, LF, '$'
con     db      CR, LF, 'Connected to remote...<ESC> to continue', CR, LF, LF, '$'
xfer    db      CR, LF, 'Starting Transfer Request...', CR, LF, LF, '$'
await   db      CR, LF, 'Waiting for Carrier....', CR, LF, '$'
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
kb_len          dw      0                       ; number of chars in num_buf
answerstring    db      'ATS0=1', CR, NULL      ; Output for Autoanswer
dialstring      db      'ATDT', NULL            ; Output for AutoDial
numbuf          db      20      dup (0)         ; and the number to dial
return          db      CR, NULL
pluses          db      '+++', NULL             ; used in hanging up
hangup          db      'ATH', CR, NULL         ; the hang-up-the-modem string
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
IFDEF   DEBUG                                   ; commented out above
include         debug.asm                       ; debug procedures
ELSE
        DO_PRINT        macro   arg             ; else empty macros
        endm
        DO_ERR_PRINT    macro   arg
        endm
ENDIF
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                              INTERRUPT ROUTINE
;;
;;      The main interrupt routine sets up a local stack, pushes all
;;      registers onto the new stack, gets the Request Header, determines
;;       if the Command Byte is a legitimate one and returns an error
;;      condition if it isn't.
;;
;;      Otherwise the Command Byte is used as an index into the dispatch
;;      table, and a near call is generated to the approriate routine.
;;      Since DOS doesn't casre about the Done Bit being set in the event
;;      of an error, this bit gets OR'ed in upon return of the new status
;;      in the AX register.  The status is then saved in the Request Header,
;;      all registers are popped, and the original stack is restored.
;;
;;      Finally, a far return to the caller (which should be DOS)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ints    proc    far

        cli
        mov     cs:[old_stack], sp
        mov     cs:[old_stack + 2], ss
        mov     sp, cs
        mov     ss, sp
        mov     sp, offset cs:new_stack_end
 sti

        PUSHALL

        push    cs
        pop     ds

        les     di, cs:[rh_ptr]
        mov     bl, es:[di.command]
        xor     bh, bh
        cmp     bx, MAX_CMD
        jle     ints1

        mov     ax, ERROR + UNK_COMMAND
        jmp     ints2

ints1:
        shl     bx, 1

        call    word ptr dispatch[bx]

        les     di, cs:[rh_ptr]                 ; reset just in case...

ints2:
        or      ax, DONE                        ; Set the done flag
        mov     es:[di.status], ax              ; Save status in the structure


        POPALL

        cli
        mov     ss, cs:[old_stack + 2]
        mov     sp, cs:[old_stack]
        sti

        ret

ints    endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;              READ ROUTINE
;;
;;      This routine first saves the original charactrer count request, then
;;      zeros out the original call value, and loads up the "to" buffer
;;      address.
;;
;;      If it as already processing a block, meaning that the last read
;;      request did not exhaust the buffer totally, then it skips down
;;      and starts loading characters one at a time (a reps would work just
;;      as well!), until either the read request is exhausted or the buffer
;;      is exhausted.
;;
;;      When the block is exhausted, an ACK is sent to the remote which
;;      will then start to send another block.
;;
;;      The in_block variable is turned off, and a jump to the top of
;;      thr routine is made, where data is collected a character at a time
;;      until another full block is reached, verified and the entire routine
;;       starts again.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
read            proc    near
        DO_PRINT        msg_read
        cmp     cs:[eof_flag], TRUE
        jz      r_back

        mov     ax, es:[di.count]
        mov     cs:[inrequest], ax              ; save the request count
        mov     es:[di.count], FALSE            ; and zero the rh count
        lds     bx, es:[di.address]             ; ds:bx points to data

top_read:
        cmp     cs:[in_block], TRUE             ; are we in a block?
        jnz     lp_it
        jmp     read_loop                       ; yes

lp_it:
        mov     cs:[_incnt], FALSE
 mov     si, offset cs:[_soh]            ; set pointer to block begin
        mov     cs:[timer], TEN_SECONDS         ; set up timer
        mov     cs:[nak_cnt], FALSE             ; set to no NAKS
        cmp     cs:[block_num], 1               ; first time?
        jnz     rd_blk                          ; no
        call    send_nak                        ; send the first NAK
        STAT    LEFT_BRACKET                    ; show a '['

rd_blk:
        call    get_char                        ; any characters?
        jc      rd_blk2                         ; no

        cmp     cs:[_incnt], FALSE              ; first character?
        jnz     nxt_char                        ; no

        cmp     al, SOH                         ; first character an SOH?
        jz      nxt_char                        ; yes. store it

        cmp     al, ABORT                       ; Abort xfer?
        jz      abort_it                        ; yep. Abort it!

        cmp     al, EOT                         ; are we done?
        jnz     rd_blk                          ; nope, so throw char away

        call    send_ack                        ; process EOF
        STAT    RIGHT_BRACKET                   ; show a ']'
        STAT    CR                              ; and a clean line
        STAT    LF
r_back:
        mov     es:[di.count], FALSE            ; and zero the rh count
        mov     cs:[eof_flag], TRUE
        xor     ax, ax
        ret

nxt_char:
        mov     cs:[si], al                     ; store it
        inc     cs:[_incnt]                     ; bump the character count
        inc     si                              ; and the pointer

rd_blk2:
 cmp     cs:[_incnt], FULLBLKSIZE        ; enough bytes to check?
        jz      chkblk

        cmp     cs:[timer], FALSE               ; out of time?
        jnz     rd_blk                          ; nope. try again
        STAT    DOT                             ; show a dot
        jmp     bad_blk2
bad_blk:
        STAT    QUESTION                        ; show a question mark
bad_blk2:
        call    send_nak                        ; yep. send a NAK
        cmp     word ptr cs:[abort], TRUE       ; abort?
        jnz     rd_blk3                         ; no, so reset timer, try again

abort_it:
        call    send_abort                      ; send an abort
        call    send_abort                      ; twice
        STAT    EXCLAIM                         ; finish with a bang!
        mov     ax, 800ch                       ; mark an error
        ret

rd_blk3:
        mov     cs:[timer], TEN_SECONDS         ; reset the timer
        jmp     rd_blk                          ; and try again

chkblk:
        mov     ax, cs:[block_num]
        push    ax

        dec     al                              ; temporary
        cmp     al, cs:[_blk1]                  ; duplicate block?
        jnz     real_blk                        ; no
        STAT    DUP_BLK                         ; yes
        mov     cs:[_incnt], FALSE
        jmp     ack_blk

real_blk:
        pop     ax                              ; back up to correct block

        cmp     cs:[_blk1], al                  ; is the block count alright?
 jnz     bad_blk                         ; no
        not     al
        cmp     cs:[_blk2], al                  ; block complement?
        jnz     bad_blk                         ; no

        mov     cx, BLKSIZE
        mov     si, offset _buf
        call    do_chksum
        cmp     cs:[_chksum], al
        jnz     bad_blk                         ; checksum bad

        mov     cs:[in_block], TRUE             ; looks good!
        mov     cs:[_inptr], offset cs:_buf     ; set up pointer
        sub     cs:[_incnt], FULLBLKSIZE - BLKSIZE

read_loop:
        mov     si, cs:[_inptr]                 ; reset the pointer
rd_loop:
        mov     al, cs:[si]                     ; get the character
        mov     ds:[bx], al                     ; and stuff it
 inc     bx                              ; up each pointer
        inc     si
        inc     cs:[_inptr]

        inc     word ptr es:[di.count]          ; inc the counter
        dec     cs:[_incnt]                     ; drop the characters left
        jnz     stf_nxt_char                    ; still more!

        inc     cs:[block_num]
        STAT    STAR                            ; send a '*'
ack_blk:
        call    send_ack                        ; send the ack
        mov     cs:[in_block], FALSE            ; out of characters
        dec     cs:[inrequest]
        jz      read_back
        jmp     top_read

stf_nxt_char:
        dec     cs:[inrequest]                  ; anymore?
        jnz     rd_loop

read_back:
        xor     ax,ax                           ; no more request, no errors
        ret
read            endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                      WRITE ROUTINE
;;
;;      This routine starts loading characters one at a time (again a reps
;;      could be used) into a local buffer.  When the total number of
;;      characters collected (some of which may be left over from last
;;      time through) reaches 128, then the send_block routine is
;;      called.
;;
;;      The return from the send_block routine only happens if there
;;      was a successful ACK, or too many NAKs have been sent, which
;;      causes the abort flag to be set.
;;
;;      Stay in this loop until there are no more characters to store and
;;      send, as per the original character count from the Request Header
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
write           proc    near
        PUSHALL_AX

        DO_PRINT        msg_write

        mov     cx, es:[di.count]               ; how many bytes?
        xor     dx, dx                          ; zero our character count
        mov     es:[di.count], dx               ; zero out reciept area

        lds     bx, es:[di.address]             ; ds:bx points to data

        mov     si, cs:[_outptr]                ; where to stick each char
wr_lp:
        mov     al, ds:[bx]                     ; get the character
        mov     cs:[si], al                     ; and save it in buffer
        inc     bx                              ; move pointers
 inc     cs:[_outcnt]                    ; total character count
        inc     es:[di.count]                   ; and current character count

        cmp     cs:[_outcnt], BLKSIZE           ; time to send a block?
        jnz     wr_ok                           ; not yet
        call    send_block                      ; send the block
        cmp     cs:[abort_xfer], FALSE          ; should we abort?
        jz      blk_ok                          ; no, block sent okay

        call    send_abort                      ; yes,
        call    send_abort                      ; send it twice!

        mov     es:[di.count], FALSE            ; mark no characters as sent
        mov     ax, ERROR + WRITE_ERROR         ; mark as a write error
        ret

blk_ok:
        mov     cs:[_outptr], offset _buf       ; reset the pointer,
        mov     si, offset _buf                 ; the register,
        mov     cs:[_outcnt], FALSE             ; and the number of chars
  
wr_ok:
        inc     si                              ; point to next char position
        loop    wr_lp

        mov     cs:[_outptr], si                ; now save it
        xor     ax, ax                          ; no errors
        POPALL_AX
        ret
write           endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;              WRITE WITH VERIFY ROUTINE
;;
;;      This routine simply jumps to the "regular" write routine.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
write_vfy        proc    near
        DO_PRINT        msg_write_vfy
        jmp     write
write_vfy       endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                      DEVICE OPEN ROUTINE
;;
;;      After setting counts to their initial, startup condition, this
;;      routine steals the timer tick, then initializes the comm port
;;      to the parameters stored in the init_data variable, then gets
;;      the status of the comm port.
;;
;;      It eats all characters available on the comm port, then calls
;;      the dial_rout routine which will attempt to put the attached device
;;      on-line if it isn't already.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
dev_open        proc    near
        PUSHALL_AX
        DO_PRINT        msg_dev_open

        mov     cs:[_inptr], offset cs:_soh
        mov     cs:[_incnt], FALSE

        mov     cs:[_outptr], offset cs:_buf
        mov     cs:[_outcnt], FALSE

        mov     cs:[in_block], FALSE
        mov     cs:[block_num], 1

        mov     dx, cs:[com_port]       ; init the comm port
        mov     al, cs:[init_data] 
        mov     ah, 0
        int     14h

eat_loop:
 mov     dx, ds:[com_port]
        mov     ah, 3                   ; get status
        int     14h

        test    ah, 1                   ; any data ready?
        jz      continue                ; no
        test    al, 20h                 ; DSR must be set, or next int14
        jz      continue                ; will (basically) never return

        mov     ah, 2
        int     14h                     ; eat the character and
        jmp     eat_loop                ; try again

continue:
        call    dial_rout               ; if not on line, make on line

        xor     ax, ax
        POPALL_AX
        ret
dev_open        endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;              DEVICE CLOSE ROUTINE
;;
;;      A check is made to determine if there is a partial block to be
;;      sent (127/128 chance!), and if so, the block is sent, and an EOT
;;      is sent to indicate end of transmission.
;;
;;      If this was a "dialed" call (either Autoanswer or AutoDial), then
;;      the Hayes hangup sequence is generated (1 second pause, output three
;;      plus signs, 1 second pause, then a ATH followed by a <CR>).
;;
;;      In any case, the timer tick (originally set in the dev_open routine)
;;      is reset and then control is returned to the caller
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
dev_close        proc    near
        DO_PRINT        msg_dev_close

        mov     cs:[eof_flag], FALSE

        cmp     cs:[_outcnt], FALSE             ; any characters left?
        jz      no_send                         ; no

        call    send_block                      ; yes. send the remainder
        STAT    RIGHT_BRACKET
        STAT    CR
        STAT    LF
        mov     cs:[_outptr], offset _buf       ; reset the pointer,
        mov     si, offset _buf                 ; the register,
        mov     cs:[_outcnt], FALSE             ; and the number of chars
        inc     cs:[block_num]                  ; just in case....

no_send:
        cmp     cs:[block_num], 1               ; was at least one block sent?
        jz      no_hang                         ; no

        mov     al, EOT                         ; send the EOT
        mov     ah, 1
        mov     dx, cs:[com_port]
        int     14h

        cmp     cs:[was_dialed], TRUE           ; were we the dialer?
        jnz     no_hang

        call    do_hang                         ; send the +++ATH

no_hang:
        mov     cs:[in_block], FALSE
        mov     cs:[was_dialed], FALSE

        xor     ax, ax
        ret
dev_close       endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;              NON-DESTRUCTIVE READ ROUTINE
;;
;;      This routine returns the next character in the input buffer if
;;      there is one.
;;
;;      In the event of an empty buffer, it returns a '?' and an
;;      error condition
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
nd_read         proc    near
        DO_PRINT        msg_nd_read
        PUSHALL_AX

        mov     es:[di.count], 1                ; we always return some count
        lds     bx, es:[di.address]             ; ds:bx points to data buffer
        cmp     cs:[_incnt], FALSE              ; any characters?
        jnz     get_nd_char                     ; yes!

        mov     ax, ERROR + READ_FAULT          ; mark an 
error condition
 mov     byte ptr es:[bx], '?'
        jmp     nd_return

get_nd_char:
        xor     ax, ax                          ; no error
        mov     si, cs:[_inptr]
        mov     al, cs:[si]                     ; get the next character
        mov     es:[bx], al

nd_return:
        POPALL_AX
        ret
nd_read         endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                      INPUT STATUS ROUTINE
;;
;;      This routine simply examines the in_block variable to determine
;;       if there are any outstanding characters (from a good block) and
;;      returns the BUSY bit set if there are.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
inp_stat        proc    near
        DO_PRINT        msg_inp_stat
        xor     ax,ax                   ; start off empty
        cmp     cs:[in_block], FALSE    ; are we in a block
        jz      no_chars                ; no

        or      ax, BUSY                ; yes. mark it
no_chars:
        ret
inp_stat        endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;              FLUSH INPUT BUFFER ROUTINE
;;
;;       This routine simply resets pointers and counts to indicate an
;;      empty input buffer
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
inp_flush       proc    near
        DO_PRINT        msg_inp_flush

        mov     cs:[_inptr], offset cs:_soh
        mov     cs:[_incnt], FALSE

        xor     ax, ax
        ret
inp_flush       endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;              OUTPUT STATUS ROUTINE
;;
;;      This is, basically, a do nothing routine, since the write routine
;;      never exits with a busy status.  If it did, then the status would
;;       be examined and the BUSY bit set if required.  Here, it is left
;;      unset.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
out_stat        proc    near
        DO_PRINT        msg_out_stat

        xor     ax, ax
        ret
out_stat        endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;              FLUSH OUTPUT BUFFERS ROUTINE
;;
;;      This routine would make things very difficult to sort out if
;;      it were to actually flush the output buffers.  As such, it is
;;      a "do-nothing" routine.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
out_flush       proc    near
 DO_PRINT        msg_out_flush

        xor     ax, ax
        ret
out_flush       endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;              READ I/O CONTROL ROUTINE
;;
;;      This routine simply copies 'count' bytes into the address specified
;;      as the transfer address, starting at the com_port variable.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rd_ioctl        proc    near
        DO_PRINT        msg_rd_ioctl
        ret
rd_ioctl        endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;              Write I/O CONTROL ROUTINE
;;
;;      This routine simply copies 'count' bytes from the address specified
;;      as the transfer address, starting at the com_port variable.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
wrt_ioctl       proc    near
        DO_PRINT        msg_wrt_ioctl
        ret
wrt_ioctl       endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;








;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;              These routines are not used by this driver
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
media_chk       proc    near
        DO_ERR_PRINT    msg_media_chk
        xor     ax,ax
        ret
media_chk       endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

bld_bpb         proc    near
        DO_ERR_PRINT    msg_bld_bpb
        xor     ax,ax
        ret
bld_bpb         endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

rem_media       proc    near
        DO_ERR_PRINT    msg_rem_media
        xor     ax,ax
        ret
rem_media       endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

out_busy        proc    near
        DO_ERR_PRINT    msg_out_busy
        xor     ax,ax
        ret
out_busy        endp






;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                              GENERALIZED ROUTINES
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      TIMER TICK:  this routine simply decrements a counter if the counter
;;                      is non-zero
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
my_tick proc    far
        cmp     cs:[timer], 0                   ; and ticks to decrement?
        jz      no_dec                          ; no
        dec     cs:[timer]                      ; yes, so do it
no_dec:
        jmp     dword ptr cs:[old_timer]        ; call the original tick
my_tick endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      DIAL_ROUT
;;
;;      Called from the device open routine, this routine will simply return
;;      if Carrier is detected.
;;
;;      Otherwise a message is displayed, and the user is allowed to
;;      either input a number, or to only enter a return.  The modem then
;;      is output a dial string or an answer string and the status is checked
;;      until the modem is online (or until a timeout occurs).
;;
;;      Once on-line, any data on the comm port is displayed, and any data
;;      at the keyboard is displayed and sent.  When an escape is discovered,
;;      the routine returns back to the calling program.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
dial_rout       proc    near
        test    al, 080h                ; DCD on?
        jz      off_line
        ret

off_line:
        mov     dx, offset cs:offline   ; output the message
        call    out_line

        mov     si, offset cs:numbuf
        mov     cx, 19                  ; maximum length
        call    get_num

        cmp     cs:[kb_len], 0          ; only a return?
        jnz     dial_it

        mov     dx, offset cs:await
        call    out_line

        mov     si, offset cs:answerstring
        call    out_string
        mov     cs:[timer], FOREVER
        jmp     offline_lp              ; now wait for carrier

dial_it:
 mov     dx, offset cs:dialing
        call    out_line

        mov     si, offset cs:dialstring
        call    out_string

        mov     si, offset cs:numbuf
        call    out_string

        mov     si, offset cs:return
        call    out_string
        mov     cs:[timer], ONE_MINUTE

        mov     cs:[was_dialed], TRUE   ; mark as a dialed call for ATH later

offline_lp:
        mov     ah, 3                   ; are we on-line?
        mov     dx, cs:[com_port]
        int     14h

 test    al, 080h                ; DCD on?
        jnz     made_con                ; yes

        cmp     cs:[timer], 0           ; out of time?
        jnz     offline_lp              ; nope

abort_call:
        mov     cs:[was_dialed], FALSE
        mov     dx, offset cs:no_con
        call    out_line

abort_out:
        xor     ax,ax
        mov     es:[di.count], ax       ; set count to zero

        mov     ax, ERROR + GEN_FAILURE ; mark as an error
        POPALL_AX
        ret

made_con:
 mov     dx, offset cs:con
        call    out_line

term_em_lp:
        call    get_char
        jc      get_term_char

        cmp     al, ESCAPE
        jz      term_exit               ; escape. Get out!

        call    o_char

get_term_char:
        mov     ah, 1h
        int     16h                     ; any characters?
        jz      term_em_lp              ; nope
        mov     ah, 0                   ; yep. get the character
        int     16h

        mov     ah, 1
 mov     dx, ds:[com_port]
        int     14h

        cmp     al, ESCAPE
        jz      term_exit               ; escape. Get out!

        call    o_char

        jmp     get_term_char

term_exit:
        mov     dx, offset cs:xfer      ; status message
        call    out_line

eat_c:
        call    get_char
        jnc     eat_c

        ret
dial_rout       endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;              SEND_BLOCK ROUTINE
;;
;;      This is the generalized XMODEM Send-A-Block Routine.
;;
;;      If this is the first time since dev_open that this routine is
;;      called, then the block count will be set to 1.  Wait for the
;;      initial NAK from the remote before sending, or wait for a time
;;      out.
;;
;;      Send the block, then wait for the ACK or NAK response.  If an ACK,
;;      return immediately.  If a NAK, then try again, until the error
;;      count gets too high.  If too high, set the abort flag and return.
;;      
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
send_block      proc    near
 PUSHALL
        push    cs
        pop     ds

        cmp     cs:[block_num], 1               ; is this the first block?
        jnz     not_first                       ; no
        STAT    LEFT_BRACKET

first_nak:
        call    get_ack                         ; get the first ack
        cmp     al, NAK                         ; was it a NAK?
        jnz     first_nak                       ; nope

not_first:
        mov     word ptr ds:[_soh], SOH
        mov     ax, cs:[block_num]
        mov     ds:[_blk1], al
        not     al
        mov     ds:[_blk2], al

 mov     cx, BLKSIZE
        mov     si, offset _buf
        call    do_chksum
        mov     cs:[_chksum], al

send_top:
        mov     cx, FULLBLKSIZE
        mov     si, offset _soh

send_lp:
        mov     ah, 1
        mov     al, ds:[si]
        mov     dx, cs:[com_port]
        int     14h

        inc     si
        loop    send_lp

        call    get_ack
        jnc     send_ok

        STAT    QUESTION
        inc     cs:[nak_cnt]            ; error. inc the counter
        cmp     cs:[nak_cnt], NAK_MAX   ; too many blocks?
        jle     send_top                ; no. try again
        mov     cs:[abort_xfer], TRUE   ; mark for abort
        STAT    EXCLAIM
        jmp     send_back               ; return

send_ok:
        STAT    STAR
        inc     cs:[block_num]          ; block sent okay, inc the counter
        mov     cs:[nak_cnt], FALSE     ; reset the nak counter

send_back:
        POPALL
        ret
send_block      endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;              GET_ACK ROUTINE
;;
;;      This routine will return with the carry flag set if an NAK was
;;      received or if it times out.
;;
;;      Carry clear if the character received is an ACK
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
get_ack proc    near

not_ack:
        mov     cs:[timer], TEN_SECONDS

not_yet:
        mov     ah, 3
        mov     dx, cs:[com_port]
        int     14h

        test    ah, 1                   ; data ready?
 jnz     data_ready              ; yes

        cmp     cs:[timer], 0           ; out of time?
        jnz     not_yet                 ; no
        STAT    DOT
        jmp     set_c                   ; yes. set carry and return

data_ready:
        mov     ah, 2
        mov     dx, cs:[com_port]
        int     14h

        test    ah, 087h                ; any errors?
        jnz     ack_back                ; yes

        cmp     al, ACK                 ; is it an ACK?
        jz      ack_back                ; yes
        cmp     al, NAK                 ; a NAK?
        jz      set_c                   ; set the carry flag for return
        cmp     al, ABORT               ; a cancel?
 jnz     not_ack                 ; try again
        mov     cs:[abort_xfer], TRUE   ; set for an abort
set_c:
        stc
        ret     

ack_back:
        clc
        ret

get_ack endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      Simply send an abort out the serial port
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
send_abort      proc    near

        mov     ah, 1                           ; abort by sending an abort 
        mov     al, ABORT                       ; character
 int     14h                             ; yes, abort

        ret
send_abort      endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      DO_HANG
;;
;;      Hang up the modem with a second of silence, +++, a second of silence
;;      then an ATH, followed by a return
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
do_hang proc    near
        mov     cs:[timer], ONE_SECOND          ; one second of silence

sil_lp1:
        cmp     cs:[timer], FALSE               ; expired?
        jnz     sil_lp1

        mov     si, offset cs:pluses            ; get the modems attention
 call    out_string

        mov     cs:[timer], ONE_SECOND          ; one second of silence

sil_lp2:
        cmp     cs:[timer], FALSE               ; expired?
        jnz     sil_lp2

        mov     si, offset cs:hangup            ; now hangup
        call    out_string
do_hang endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      DO_CHKSUM ROUTINE
;;
;;      Starting at CS:[SI], add the next CX characters into AX
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
do_chksum       proc    near

 mov     ax, FALSE                       ; zero out the count
chk_lp:
        add     al, cs:[si]                     ; add the individual byte
        inc     si
        loop    chk_lp                          ; for each byte

        ret

do_chksum       endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      Simply send the NAK, increment the count of NAKs, and set the
;;      abort flag if required.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
send_nak        proc    near

        push    ax
        push    dx

 mov     ah, 1                           ; send the NAK
        mov     al, NAK
        mov     dx, cs:[com_port]
        int     14h

        mov     cs:[_incnt], FALSE
        mov     si, offset cs:[_soh]            ; set pointer to block begin
        mov     cs:[timer], TEN_SECONDS         ; set up timer

        inc     cs:[nak_cnt]
        cmp     cs:[nak_cnt], NAK_MAX           ; too many NAKs?
        jnz     nak_ret                         ; no
        mov     word ptr cs:[abort], FALSE      ; yes, so abort
nak_ret:
        pop     dx
        pop     ax
        ret

send_nak        endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      Simply send the ACK
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
send_ack        proc    near

        push    ax
        push    dx

        mov     ah, 1                           ; send the ACK
        mov     al, ACK
        mov     dx, cs:[com_port]
        int     14h

        pop     dx
        pop     ax
        ret
send_ack        endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      If there is a character at the console, get the character into al,
;;      and clear the carry bit.  Otherwise, set the carry bit.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
get_char        proc    near
        mov     ah, 3                           ; is there a character?
        mov     dx, cs:[com_port]
        int     14h
        test    ah, 1                           ; data ready?
        jnz     get_ch2                         ; yes
        stc
        ret

get_ch2:
        mov     ah, 2                           ; get the character
        mov     dx, cs:[com_port]
        int     14h
        clc
        ret
get_char        endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      Set the character count to zero, get upto CX number of characters
;;      and store them into a buffer, one at a time, starting at cs:[si].
;;
;;      Return when CX characters have been entered, or a <CR> is hit.
;;      kb_len will contain how many characters were entered
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
get_num proc    near
        mov     cs:[kb_len], FALSE
get_kbc:
        mov     ah, 0                           ; get a character from kb
        int     16h

        call    o_char
        cmp     al, CR                          ; if CR, return
        jnz     store
        ret

store:
        mov     cs:[si], al                     ; store it
        inc     si                              ; bump pointer
        xor     ax, ax
        mov     cs:[si], al                     ; zero out from last time
        inc     cs:[kb_len]
        loop    get_kbc
        ret
get_num endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      Output the characters through the comm port, starting at cs:[si]
;;      until a NULL is hit.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
out_string      proc    near

out_lp:
        mov     al, cs:[si]                     ; load the character
 cmp     al, NULL                        ; at end?
        jnz     out_it                          ; no
        ret                                     ; yes

out_it:
        mov     dx, cs:[com_port]               ; output the character
        mov     ah, 1
        int     14h
        inc     si
        jmp     out_lp

out_string      endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      O_CHAR
;;
;;      Using the BIOS Teletype Calls, this routine merely dumps the character
;;      in al onto page 0 at the current cursor position
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
o_char  proc    near
 push    bx
        xor     bx,bx
        mov     ah, 0eh
        int     10h
        pop     bx
        ret
o_char  endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      OUT_LINE
;;
;;      This routine will output (via the device driver safe BIOS calls)
;;      the string pointed to by DS:DX, which is terminated with either
;;      a '$' or a NULL byte.  It used the O_CHAR routine for each character.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
out_line        proc    near
        push    ax
        push    si
        mov     si, dx
out_line_lp:
 mov     al, cs:[si]
        cmp     al, '$'
        jz      out_line_exit
        cmp     al, NULL
        jz      out_line_exit
        call    o_char
        inc     si
        jmp     out_line_lp
out_line_exit:
        pop     si
        pop     ax
        ret
out_line        endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;       INITIALIZE DRIVER ROUTINE
;;
;;      This routine determines if the correct DOS version is running, exiting
;;      with an error message if it is not.
;;
;;      The comm port is initialized, a greeting message is displayed and
;;      the data following the equal sign in the CONFIG.SYS file is then
;;      displayed.
;;
;;      Finally, the only real requirement of the initialization routine
;;      is accomplished: it sets the "break address" bnack in the request
;;      header
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
init            proc    near
        DO_PRINT        msg_init

        mov     ah, 030h                        ; get the DOS version
        int     21h

        cmp     ah, 3                           ; version 3.x?
 jge     okay_dos                        ; yes. At least.

        mov     dx, offset cs:wrong_dos         ; output the message
        call    out_line

endless_loop:
        cli
        jmp     endless_loop                    ; very ugly, but effective!

okay_dos:

        mov     dx, offset cs:greetings         ; output the message
        call    out_line

        call    set_timer

        push    ds
        mov     ds, es:[di.count + 2]   ; get the segment of CONFIG.SYS line
        mov     si, es:[di.count]       ; get the offset of CONFIG.SYS line
        call    output_chars
 pop     ds

        mov     dx, offset cs:end_greetings     ; output the message
        call    out_line

        mov     word ptr es:[di.address], offset cs:init
        mov     word ptr es:[di.address + 2], cs
        xor     ax,ax
        ret
init            endp

wrong_dos       db '????You must run this device driver under DOS 3.0 or higher',
                db CR, LF, ' System Halted! $'
greetings       db CR, LF, LF, LF, 'MDM_DRV being installed...', CR, LF
                db 'CONFIG.SYS Line is: $'
end_greetings   db CR, LF, LF, LF, '$'



output_chars    proc    near
 push    ax
output_loop:
        mov     al, ds:[si]             ; get the character
        cmp     al, LF                  ; is it NULL?
        jnz     outit                   ; no
        pop     ax
        ret

outit:
        call    o_char                  ; output character
        inc     si
        jmp     output_loop 
output_chars    endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      Save the current timer tick address at old_timer, and point the
;;      tick to my_tick in this code seg.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
set_timer       proc    near
        cli
 push    ax
        push    es
        xor     ax, ax
        mov     es, ax

        mov     ax, es:[TIMER_ADD]
        mov     cs:[old_timer], ax
        mov     ax, es:[TIMER_ADD + 2]
        mov     cs:[old_timer + 2], ax

        mov     ax, offset cs:my_tick
        mov     es:[TIMER_ADD], ax
        mov     ax, cs
        mov     es:[TIMER_ADD + 2], ax

        pop     es
        pop     ax
        sti
        ret
set_timer       endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

driver  endp
code    ends
        end









Subject: 3K source on debug.asm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        DO_PRINT        macro   arg
                push    dx
                push    ax
                mov     dx, offset      cs:arg
                call    out_line
                call    out_rh
                pop     ax
                pop     dx
        endm
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        DO_ERR_PRINT    macro   arg
                DO_PRINT        arg
                DO_PRINT        msg_char_err
 endm
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
form_num        proc    near

        push    ax
        push    bx
        push    cx
        push    dx

        mov     bx, dx
        mov     dx, cx
num_lp1:
        mov     cx, 4
        rol     ax,cl
        mov     cx, ax
        and     cx, 0fh
        add     cx, '0'
        cmp     cx, '9'
        jbe     is_decimal
        add     cx, 'A'-'9'-1
is_decimal:
        mov     [bx], cl
        inc     bx
        dec     dx
        jnz     num_lp1

        pop     dx
        pop     cx
        pop     bx
        pop     ax
        ret

form_num        endp

out_rh  proc    near

        push    ax
        push    cx
        push    dx

 mov     ax, es
        mov     dx, offset rh_0
        mov     cx, 4
        call    form_num

        mov     ax, di
        mov     dx, offset rh_00
        mov     cx, 4
        call    form_num

        mov     ah, es:[di.rlength]
        xor     al, al
        mov     dx, offset rh_1_val
        mov     cx, 2
        call    form_num

        mov     ah, es:[di.unit]
        xor     al, al
        mov     dx, offset rh_2_val
        mov     cx, 2
 call    form_num

        mov     ah, es:[di.command]
        xor     al, al
        mov     dx, offset rh_3_val
        mov     cx, 2
        call    form_num

        mov     ax, es:[di.status]
        mov     dx, offset rh_4_val
        mov     cx, 4
        call    form_num

        mov     ah, es:[di.media]
        xor     al, al
        mov     dx, offset rh_5_val
        mov     cx, 2
        call    form_num

        mov     ax, word ptr es:[di.address + 2]
 mov     dx, offset rh_6_val
        mov     cx, 4
        call    form_num

        mov     ax, word ptr es:[di.address]
        mov     dx, offset rh_7_val
        mov     cx, 4
        call    form_num

        mov     ax, es:[di.count]
        mov     dx, offset rh_8_val
        mov     cx, 4
        call    form_num

        mov     ax, es:[di.sector]
        mov     dx, offset rh_9_val
        mov     cx, 4
        call    form_num

        mov     dx, offset rh_1
 call    out_line

        pop     dx
        pop     cx
        pop     ax

        ret

out_rh  endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

rh_1            db      'RH:'
rh_0            db      'XXXX'
                db      ':'
rh_00           db      'XXXX '
                db      'RH_LEN:'
rh_1_val        db      'XX '
                db      'UNIT:'
rh_2_val        db      'XX '
                db      'Command:'
rh_3_val db      'XX '
                db      'Status:'
rh_4_val        db      'XXXX ', CR, LF
                db      'Media:'
rh_5_val        db      'XX '
                db      'Address:'
rh_6_val        db      'XXXX'
                db      ':'
rh_7_val        db      'XXXX '
                db      'Count:'
rh_8_val        db      'XXXX '
                db      'Sector:'
rh_9_val        db      'XXXX', CR, LF, '$'
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

adr_fmt db      'XXXX:XXXX', CR, LF, '$'

msg_init        db 'In init     ',CR,LF,'$'
msg_media_chk   db '??Media Check $'
msg_bld_bpb     db '??Bld_Bpb $'
msg_rd_ioctl    db 'In rd_ioctl ',CR,LF,'$'
msg_read        db 'In read     ',CR,LF,'$'
msg_nd_read     db 'In nd_read  ',CR,LF,'$'
msg_inp_stat    db 'In inp_stat ',CR,LF,'$'
msg_inp_flush   db 'In inp_flush',CR,LF,'$'
msg_write       db 'In write    ',CR,LF,'$'
msg_write_vfy   db 'In write_vfy',CR,LF,'$'
msg_out_stat    db 'In out_stat ',CR,LF,'$'
msg_out_flush   db 'In out_flush',CR,LF,'$'
msg_wrt_ioctl   db 'In wrt_ioctl',CR,LF,'$'
msg_dev_open    db 'In dev_open ',CR,LF,'$'
msg_dev_close   db 'In dev_close',CR,LF,'$'
msg_rem_media   db '??Rem_Media $'
msg_out_busy    db '??Out_Busy $'
msg_char_err    db ' called??', CR, LF, LF, '$'
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;














Subject: 1.5K of source SET_MDM.ASM


code    segment
assume  cs:code
org     100h


start:
        jmp     install                 ; install out vectors and

old_dos dw      0
        dw      0

                                        ; then go TSR
my_int21        proc    far
        pushf
 cmp     ah, 03dh                ; handle open?
        jnz     normal_dos              ; no
        
        push    si
        mov     si, dx
        cmp     [si], 'DM'              ; cmp against "MDM",0
        jnz     not_me                  ; remembering that each pair of bytes
        cmp     word ptr [si + 2], 004dh; is reversed!
        jnz     not_me
        pop     si

                                        ; with the flags already on the stack,
        call    dword ptr cs:[old_dos]  ; do a long call to the old DOS

        jc      normal_ret              ; an error, so merely return

        pushf                           ; save DOS return status
        push    ax                      ; save the registers
        push    bx
        push    dx

        mov     bx, ax                  ; the handle just returned
        mov     ax, 4400h               ; get IOCTL
        int     21h

        xor     dh, dh                  ; zero out the top of the word
        or      dl, 20h                 ; set the raw bit
        mov     ax, 4401h               ; set IOCTL
        int     21h

        pop     dx
        pop     bx
        pop     ax
        popf

normal_ret:
        ret     2                       ; return with the flag word DOS left


not_me:
 pop     si
normal_dos:
        popf
        jmp     dword ptr cs:[old_dos]  ; and call the original DOS
my_int21        endp

my_size equ     (($-code)/16 + 1)       ; waste a 'graph!

install:
        mov     ax, 3521h               ; get the original DOS int vector
        int     21h

        mov     cs:[old_dos], bx
        mov     cs:[old_dos + 2], es

        push    cs
        pop     ds
        mov     dx, offset cs:my_int21
        mov     ax, 2521h
        int     21h

        mov     dx, my_size
        mov     ax, 3100h               ; go TSR
        int     21h

        int     20h                     ; belts *and* suspenders! :-)

code    ends
        end start



======================[ End Listing for MDM-DRV.ASM ]====================



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;			DEBUG AND MACRO DEFINITIONS
;;
;;	These macros and routines are included if DEBUG is defined in the
;;	main body of the program.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	DO_PRINT <arg>
;;
;;	Outputs the '$' terminated string <arg>, then prints the contents
;;	of the Request Header, field by field
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	DO_PRINT	macro	arg
		push	dx
		push	ax
		mov	dx, offset	cs:arg
		mov	ah, 09h
		int	21h
		call	out_rh
		pop	ax
		pop	dx
	endm
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	DO_ERR_PRINT	<arg>
;;
;;	Cause arg to be output, then output the common error suffix message
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	DO_ERR_PRINT	macro	arg
		DO_PRINT	arg
		DO_PRINT	msg_char_err
	endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	OUTNUM ROUTINE
;;
;;	Output the number in AX (or AH) in hex into the buffer starting at
;;	DX, and for CX characters.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
outnum	proc	near

	push	ax
	push	bx
	push	cx
	push	dx

	mov	bx, dx
	mov	dx, cx
num_lp1:
	mov	cx, 4
	rol	ax,cl
	mov	cx, ax
	and	cx, 0fh
	add	cx, '0'
	cmp	cx, '9'
	jbe	is_decimal
	add	cx, 'A'-'9'-1
is_decimal:
	mov	[bx], cl
 	inc	bx
	dec	dx
	jnz	num_lp1

	pop	dx
	pop	cx
	pop	bx
	pop	ax
	ret

outnum	endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	OUTPUT REQUEST HEADER ROUTINE
;;
;;	Output the Request Header, a field at a time.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
out_rh	proc	near

	push	ax
	push	cx
	push	dx

	mov	ax, es
	mov	dx, offset rh_0
	mov	cx, 4
	call	outnum

	mov	ax, di
	mov	dx, offset rh_00
	mov	cx, 4
	call	outnum

	mov	ah, es:[di.rlength]
	xor	al, al
	mov	dx, offset rh_1_val
	mov	cx, 2
	call	outnum

	mov	ah, es:[di.unit]
	xor	al, al
	mov	dx, offset rh_2_val
	mov	cx, 2
	call	outnum

	mov	ah, es:[di.command]
	xor	al, al
	mov	dx, offset rh_3_val
	mov	cx, 2
	call	outnum

	mov	ax, es:[di.status]
	mov	dx, offset rh_4_val
	mov	cx, 4
	call	outnum

	mov	ah, es:[di.media]
	xor	al, al
	mov	dx, offset rh_5_val
	mov	cx, 2
	call	outnum

	mov	ax, word ptr es:[di.address + 2]
	mov	dx, offset rh_6_val
	mov	cx, 4
	call	outnum

	mov	ax, word ptr es:[di.address]
	mov	dx, offset rh_7_val
	mov	cx, 4
	call	outnum

	mov	ax, es:[di.count]
	mov	dx, offset rh_8_val
	mov	cx, 4
	call	outnum

	mov	ax, es:[di.sector]
	mov	dx, offset rh_9_val
	mov	cx, 4
	call	outnum

	mov	dx, offset rh_1
	mov	ah, 9
	int	21h

	pop	dx
	pop	cx
	pop	ax

	ret

out_rh	endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	The Layout of the Request Header output.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rh_1		db	'RH:',
rh_0		db	'XXXX'
		db	':'
rh_00		db	'XXXX '
		db	'RH_LEN:'
rh_1_val	db	'XX '
		db	'UNIT:'
rh_2_val	db	'XX '
		db	'Command:'
rh_3_val	db	'XX '
		db	'Status:'
rh_4_val	db	'XXXX ', CR, LF
		db	'Media:'
rh_5_val	db	'XX '
		db	'Address:'
rh_6_val	db	'XXXX'
		db	':'
rh_7_val	db	'XXXX '
		db	'Count:'
rh_8_val	db	'XXXX '
		db	'Sector:'
rh_9_val	db	'XXXX', CR, LF, '$'
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


msg_init	db 'In init     ',CR,LF,'$'
msg_media_chk   db '??Media Check $'
msg_bld_bpb     db '??Bld_Bpb $'
msg_rd_ioctl    db 'In rd_ioctl ',CR,LF,'$'
msg_read        db 'In read     ',CR,LF,'$'
msg_nd_read     db 'In nd_read  ',CR,LF,'$'
msg_inp_stat    db 'In inp_stat ',CR,LF,'$'
msg_inp_flush   db 'In inp_flush',CR,LF,'$'
msg_write       db 'In write    ',CR,LF,'$'
msg_write_vfy   db 'In write_vfy',CR,LF,'$'
msg_out_stat    db 'In out_stat ',CR,LF,'$'
msg_out_flush   db 'In out_flush',CR,LF,'$'
msg_wrt_ioctl   db 'In wrt_ioctl',CR,LF,'$'
msg_dev_open    db 'In dev_open ',CR,LF,'$'
msg_dev_close   db 'In dev_close',CR,LF,'$'
msg_rem_media   db '??Rem_Media $'
msg_out_busy    db '??Out_Busy $'
msg_char_err	db ' called??', CR, LF, LF, '$'
