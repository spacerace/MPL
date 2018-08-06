; 
;  lmemmove.asm           far version of memmove
; 

INCLUDE         SETUP.H

CLIB            SEGMENT  WORD PUBLIC 'CODE'
CLIB            ENDS

CLIB            SEGMENT
ASSUME          CS: CLIB

PUBLIC          lmemmove

lmemmove        PROC    FAR

Destination     EQU     DWORD   PTR [bp] + 12
Source          EQU     DWORD   PTR [bp] + 8
Count           EQU     WORD    PTR [bp] + 6

                ENTRY   0

                lds     si, Source      ; DS:SI = Source
                les     di, Destination ; ES:DI = Destination
                mov     ax, di          ; Dest. in AX for return 
                mov     cx, Count       ; cx = number bytes to move
                jcxz    done            ; if cx = 0,  nothing to copy

;
; Check for overlapping buffers:
;        If segments are different, assume no overlap
;                Do normal (Upwards) Copy
;        Else If (Dest. <= Source) Or (Dest.>= Source + Count) Then
;                Do normal (Upwards) Copy
;        Else
;                Do Downwards Copy to avoid propogation
;
                mov     ax, es          ; compare the segments
                cmp     ax, WORD PTR (Source+2)
                jne     CopyUp
                cmp     di, si          ; Source <= Destination ?
                jbe     CopyUp

                mov     ax, si
                add     ax, cx
                cmp     di, ax          ; Dest. >= (Source + Count) ?
                jae     CopyUp
;
; Copy Down to avoid propogation in overlapping buffers
;
                mov     ax, di          ; AX = return value (offset)
                
                add     si, cx
                add     di, cx
                dec     si              ; DS:SI = Source + Count - 1
                dec     di              ; ES:DI = Dest. + Count - 1
                std                     ; Set Direction Flag = Down
                rep     movsb
                cld                     ; Set Direction Flag = Up
                jmp     short done

CopyUp:
                mov     ax, di          ; AX = return value (offset)
;
; There are 4 types of word alignment of "Source" and "Destination":
;        1. Source and Destination are both even        (best case)
;        2. Source is even and Destination is odd
;        3. Source is odd and Destination is even
;        4. Source and Destination are both odd        (worst case)
;
; Case #4 is much faster if a single byte is copied before the
; REP MOVSW instruction.  Cases #2 and #3 are effectively unaffected
; by such an operation.  To maximum the speed of this operation,
; only DST is checked for alignment.  For cases #2 and #4, the first
; byte will be copied before the REP MOVSW.
;
                test    al, 1      ; fast check for Dest. odd address
                jz      move

                movsb              ; move a byte to improve alignment
                dec     cx
;
; Now the bulk of the copy is done using REP MOVSW.  This is much
; faster than a REP MOVSB if the Source and Dest. addresses are both
; word aligned and the processor has a 16-bit bus.  Depending on
; the initial alignment and the size of the region moved, there
; may be an extra byte left over to be moved.  This is handled
; by the REP MOVSB, which moves either 0 or 1 bytes.
;
move:
                shr     cx, 1           ; Shift CX for Count of words
                rep     movsw           ; CF set if 1 byte left over
                adc     cx, cx          ; CX = 1 or 0, - Carry Flag
                rep     movsb           ; possible final byte
;
; Return the "Destination" address in AX/DX:AX
;
done:
                EXIT    10

lmemmove        ENDP

CLIB            ENDS

                END
