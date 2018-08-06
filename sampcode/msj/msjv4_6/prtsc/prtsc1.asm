;
; WINDOWS SCREEN CAPTURE - DYNAMIC LIBRARY
;
; LANGUAGE : Microsoft Assembler 5.1
; SDK      : Windows 2.03 SDK
; MODEL    : small
; STATUS   : operational
;
; 06/13/88 1.00 - Kevin P. Welch - initial creation.
;

         Extrn    PrtScInit:Near
         
_TEXT    SEGMENT  BYTE PUBLIC 'CODE'
         ASSUME   CS:_TEXT
         PUBLIC   LibInit
         
LibInit  PROC     FAR

         Push     DI                ; hInstance
         Push     DS                ; Data Segment
         Push     CX                ; Heap Size
         Push     ES                
         Push     SI                ; Command Line
         
         Call     PrtScInit
         
         Ret
         
LibInit  ENDP
_TEXT    ENDS

End      LibInit 

