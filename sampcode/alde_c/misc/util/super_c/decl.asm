;               Assembly Equivalents for C Data

; First we declare the segments and groups:

_TEXT   SEGMENT  BYTE PUBLIC 'CODE'
_TEXT   ENDS

_BSS    SEGMENT  WORD PUBLIC 'BSS'
_BSS    ENDS

_DATA   SEGMENT  WORD PUBLIC 'DATA'
_DATA   ENDS

CONST   SEGMENT  WORD PUBLIC 'CONST'
CONST   ENDS

DGROUP  GROUP   CONST,_BSS,_DATA

; Next we declare the variables:

_DATA   SEGMENT

        EXTRN   _a:WORD         ; int a, b
        EXTRN   _b:WORD

        PUBLIC  _x              ; int x = {5}
_x      DW      5

_y      DW      9               ; static int y = {9}

_count  DW      0               ; static int count = {0}

_DATA   ENDS

_BSS    SEGMENT

_i      DW      ?               ; static int i

_lastinc DW     ?               ; static int lastinc

_BSS    ENDS

; Note that _y, _i, _lastinc, and _count are not accessible
; outside this module. Also note that the parameter inc and
; the automatic variable retValue are not declared here. They
; are allocated on the stack at run-time.

