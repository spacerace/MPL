                TITLE   'DIRS.ASM'

;
; Subroutines for DIRDUMP.C
;


ARG1            EQU     [bp + 4]        ; stack frame addressing for C arguments
ARG2            EQU     [bp + 6]


_TEXT           SEGMENT byte public 'CODE'
                ASSUME  cs:_TEXT

;------------------------------------------------------------------------------
;
; void SetDTA( DTA );
;         char *DTA;
;
;------------------------------------------------------------------------------

                PUBLIC  _SetDTA
_SetDTA         PROC    near

                push    bp
                mov     bp,sp

                mov     dx,ARG1         ; DS:DX -> DTA
                mov     ah,1Ah          ; AH = INT 21H function number
                int     21h             ; pass DTA to MS-DOS

                pop     bp
                ret

_SetDTA         ENDP

;------------------------------------------------------------------------------
;
; int GetCurrentDir( *path );           /* returns error code */
;         char *path;                   /* pointer to buffer to contain path */
;
;------------------------------------------------------------------------------

                PUBLIC  _GetCurrentDir
_GetCurrentDir  PROC    near

                push    bp
                mov     bp,sp
                push    si

                mov     si,ARG1         ; DS:SI -> buffer
                xor     dl,dl           ; DL = 0 (default drive number)
                mov     ah,47h          ; AH = INT 21H function number
                int     21h             ; call MS-DOS; AX = error code
                jc      L01             ; jump if error

                xor     ax,ax           ; no error, return AX = 0

L01:            pop     si
                pop     bp
                ret

_GetCurrentDir  ENDP

;------------------------------------------------------------------------------
;
; int FindFirstFile( path, attribute ); /* returns error code */
;         char *path;
;         int  attribute;
;
;------------------------------------------------------------------------------

                PUBLIC  _FindFirstFile
_FindFirstFile  PROC    near

                push    bp
                mov     bp,sp

                mov     dx,ARG1         ; DS:DX -> path
                mov     cx,ARG2         ; CX = attribute
                mov     ah,4Eh          ; AH = INT 21H function number
                int     21h             ; call MS-DOS; AX = error code
                jc      L02             ; jump if error

                xor     ax,ax           ; no error, return AX = 0

L02:            pop     bp
                ret

_FindFirstFile  ENDP

;------------------------------------------------------------------------------
;
; int FindNextFile();                   /* returns error code */
;
;------------------------------------------------------------------------------

                PUBLIC  _FindNextFile
_FindNextFile   PROC    near

                push    bp
                mov     bp,sp

                mov     ah,4Fh          ; AH = INT 21H function number
                int     21h             ; call MS-DOS; AX = error code
                jc      L03             ; jump if error

                xor     ax,ax           ; if no error, set AX = 0

L03:            pop     bp
                ret

_FindNextFile   ENDP

_TEXT           ENDS


_DATA           SEGMENT word public 'DATA'

CurrentDir      DB      64 dup(?)
DTA             DB      64 dup(?)

_DATA           ENDS

                END
