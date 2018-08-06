;
; WINDOWS CLIPBOARD VIEWER - OWNER DISPLAY FORMAT SOURCE CODE
;
; LANGUAGE      : Microsoft Macro Assembler 5.1
; MODEL         : small
; ENVIRONMENT   : Microsoft Windows 2.1 SDK
; STATUS        : operational
;
;
; Kevin P. Welch
;
         Extrn    OwnerInit:Near

_TEXT    SEGMENT  BYTE PUBLIC 'CODE'
         ASSUME   CS:_TEXT
         PUBLIC   LibInit

LibInit  PROC     FAR

         Push     DI                ; hInstance
         Push     DS                ; Data Segment
         Push     CX                ; Heap Size
         Push     ES
         Push     SI                ; Command Line

         Call     OwnerInit

         Ret

LibInit  ENDP
_TEXT    ENDS

End      LibInit

