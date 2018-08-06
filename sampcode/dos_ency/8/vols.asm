                TITLE   'VOLS.ASM'

;-----------------------------------------------------------------------------
;
; C-callable routines for manipulating MS-DOS volume labels
; Note: These routines modify the current DTA address.
;
;-----------------------------------------------------------------------------

ARG1            EQU     [bp + 4]        ; stack frame addressing

DGROUP          GROUP   _DATA

_TEXT           SEGMENT byte public 'CODE'
                ASSUME  cs:_TEXT,ds:DGROUP

;-----------------------------------------------------------------------------
;
; char *GetVolLabel();          /* returns pointer to volume label name */
;
;-----------------------------------------------------------------------------

                PUBLIC  _GetVolLabel
_GetVolLabel    PROC    near

                push    bp
                mov     bp,sp
                push    si
                push    di

                call    SetDTA          ; pass DTA address to MS-DOS
                mov     dx,offset DGROUP:ExtendedFCB
                mov     ah,11h          ; AH = INT 21H function number
                int     21h             ; Search for First Entry
                test    al,al
                jnz     L01
                                        ; label found so make a copy
                mov     si,offset DGROUP:DTA + 8
                mov     di,offset DGROUP:VolLabel
                call    CopyName
                mov     ax,offset DGROUP:VolLabel ; return the copy's address
                jmp     short L02

L01:            xor     ax,ax           ; no label, return 0 (null pointer)

L02:            pop     di
                pop     si
                pop     bp
                ret

_GetVolLabel    ENDP

;-----------------------------------------------------------------------------
;
; int RenameVolLabel( label );          /* returns error code */
;         char *label;                  /* pointer to new volume label name */
;
;-----------------------------------------------------------------------------

                PUBLIC  _RenameVolLabel
_RenameVolLabel PROC    near

                push    bp
                mov     bp,sp
                push    si
                push    di

                mov     si,offset DGROUP:VolLabel  ; DS:SI -> old volume name
                mov     di,offset DGROUP:Name1
                call    CopyName        ; copy old name to FCB

                mov     si,ARG1
                mov     di,offset DGROUP:Name2
                call    CopyName        ; copy new name into FCB

                mov     dx,offset DGROUP:ExtendedFCB    ; DS:DX -> FCB
                mov     ah,17h          ; AH = INT 21H function number
                int     21h             ; rename
                xor     ah,ah           ; AX = 00H (success) or 0FFH (failure)

                pop     di              ; restore registers and return
                pop     si
                pop     bp
                ret

_RenameVolLabel ENDP

;-----------------------------------------------------------------------------
;
; int NewVolLabel( label );             /* returns error code */
;         char *label;                  /* pointer to new volume label name */
;
;-----------------------------------------------------------------------------

                PUBLIC  _NewVolLabel
_NewVolLabel    PROC    near

                push    bp
                mov     bp,sp
                push    si
                push    di

                mov     si,ARG1
                mov     di,offset DGROUP:Name1
                call    CopyName        ; copy new name to FCB

                mov     dx,offset DGROUP:ExtendedFCB
                mov     ah,16h          ; AH = INT 21H function number
                int     21h             ; create directory entry
                xor     ah,ah           ; AX = 00H (success) or 0FFH (failure)

                pop     di              ; restore registers and return
                pop     si
                pop     bp
                ret

_NewVolLabel    ENDP

;-----------------------------------------------------------------------------
;
; int DeleteVolLabel();                 /* returns error code */
;
;-----------------------------------------------------------------------------

                PUBLIC  _DeleteVolLabel
_DeleteVolLabel PROC    near

                push    bp
                mov     bp,sp
                push    si
                push    di

                mov     si,offset DGROUP:VolLabel
                mov     di,offset DGROUP:Name1
                call    CopyName        ; copy current volume name to FCB

                mov     dx,offset DGROUP:ExtendedFCB
                mov     ah,13h          ; AH = INT 21H function number
                int     21h             ; delete directory entry
                xor     ah,ah           ; AX = 0 (success) or 0FFH (failure)

                pop     di              ; restore registers and return
                pop     si
                pop     bp
                ret

_DeleteVolLabel ENDP

;-----------------------------------------------------------------------------
;
; miscellaneous subroutines
;
;-----------------------------------------------------------------------------

SetDTA          PROC    near

                push    ax              ; preserve registers used
                push    dx

                mov     dx,offset DGROUP:DTA    ; DS:DX -> DTA
                mov     ah,1Ah          ; AH = INT 21H function number
                int     21h             ; set DTA

                pop     dx              ; restore registers and return
                pop     ax
                ret

SetDTA          ENDP


CopyName        PROC    near            ; Caller:  SI -> ASCIIZ source
                                        ;          DI -> destination
                push    ds
                pop     es              ; ES = DGROUP
                mov     cx,11           ; length of name field

L11:            lodsb                   ; copy new name into FCB ..
                test    al,al
                jz      L12             ; .. until null character is reached
                stosb
                loop    L11

L12:            mov     al,' '          ; pad new name with blanks
                rep     stosb
                ret

CopyName        ENDP

_TEXT           ENDS


_DATA           SEGMENT word public 'DATA'

VolLabel        DB      11 dup(0),0

ExtendedFCB     DB      0FFh            ; must be 0FFH for extended FCB
                DB      5 dup(0)        ; (reserved)
                DB      1000b           ; attribute byte (bit 3 = 1)
                DB      0               ; default drive ID
Name1           DB      11 dup('?')     ; global wildcard name
                DB      5 dup(0)        ; (unused)
Name2           DB      11 dup(0)       ; second name (for renaming entry)
                DB      9 dup(0)        ; (unused)

DTA             DB      64 dup(0)

_DATA           ENDS

                END
