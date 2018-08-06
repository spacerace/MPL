;       RAM Sort Function

_TEXT   SEGMENT  BYTE PUBLIC 'CODE'
_TEXT   ENDS
CONST   SEGMENT  WORD PUBLIC 'CONST'
CONST   ENDS
_BSS    SEGMENT  WORD PUBLIC 'BSS'
_BSS    ENDS
_DATA   SEGMENT  WORD PUBLIC 'DATA'
_DATA   ENDS

DGROUP  GROUP   CONST,_BSS,_DATA

        ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP

PUBLIC  _sortLines

_DATA   SEGMENT
EXTRN   __chkstk:NEAR
EXTRN   _buffer:BYTE
EXTRN   _lPtr:BYTE
EXTRN   _nextLPtr:WORD
EXTRN   _strcmp:NEAR
_DATA   ENDS

_TEXT      SEGMENT

; sortLines()

        PUBLIC  _sortLines

_sortLines PROC NEAR
        push  bp
        mov   bp,sp
        mov   ax,10
        call  __chkstk
        push  si
        push  di

;       struct linePtr *sortLPtr;
;       struct linePtr *srchLPtr;
;       struct linePtr *lowLPtr;
;       struct linePtr newLPtr;

        newLPtr = 4
        srchLPtr = 6
        sortLPtr = 8
        lowLPtr = 10

;       for (sortLPtr = lPtr; sortLPtr < nextLPtr; sortLPtr++) {
        mov     WORD PTR [bp-sortLPtr],OFFSET DGROUP:_lPtr
        jmp     SHORT $L20002
$F46:

;               for (lowLPtr = srchLPtr = sortLPtr;
;                    srchLPtr < nextLPtr; srchLPtr++)
        mov     si,[bp-sortLPtr]
        mov     di,si
        jmp     SHORT $L20001
$F50:

; *** New code ***
;                       if (strcmp(lowLPtr->sortAt,srchLPtr->sortAt) > 0)
        push    di                      ; Save si/di
        push    si
        mov     di,WORD PTR [di+2]      ; di = lowLPtr->sortAt
        mov     si,WORD PTR [si+2]      ; si = srchLPtr->sortAt

loop:   lods    BYTE PTR [si]           ; Get the next byte to compare
        scas    BYTE PTR [di]           ; Compare it
        jne     noteq                   ; If they don't match
        or      al,al                   ; End-of-string?
        jnz     loop                    ; If not
        pop     si                      ; Pop si/di
        pop     di
        jmp     $FC51                   ; The strings are equal
noteq:  pop     si                      ; Pop si/di
        pop     di
        ja      $FC51                   ; Jump if string 1 is more than 2
; ***

;                               lowLPtr = srchLPtr;
        mov     di,si

;               newLPtr = *sortLPtr;
$FC51:
        add     si,4
$L20001:
        cmp     _nextLPtr,si
        ja      $F50
        mov     bx,[bp-sortLPtr]
        mov     ax,[bx]
        mov     dx,[di]
        mov     [bx],dx
        mov     [di],ax
        mov     ax,[bx+2]
        mov     dx,[di+2]
        mov     [bx+2],dx
        mov     [di+2],ax

;       };
        add     WORD PTR [bp-sortLPtr],4
$L20002:
        mov     ax,_nextLPtr
        cmp     [bp-sortLPtr],ax
        jb      $F46

; }
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret

_sortLines ENDP

_TEXT   ENDS

        END

