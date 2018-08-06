;
; FORM LIBRARY - DLL ENTRY POINT MODULE
;
; 07/12/90 1.00 - Kevin P. Welch - initial creation.
;

Extrn         FormInit:Far

_TEXT    SEGMENT   BYTE PUBLIC 'CODE'
         ASSUME    CS:_TEXT
         PUBLIC    LibEntry

LibEntry PROC FAR

         push di
         call FormInit
         Ret

LibEntry ENDP

_TEXT    ENDS

End      LibEntry
