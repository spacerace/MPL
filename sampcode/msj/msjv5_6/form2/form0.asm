;
; FORM LIBRARY - DLL ENTRY POINT MODULE
;
; LANGUAGE      : Microsoft Macro Assembler
; MODEL         : medium
; ENVIRONMENT   : Microsoft Windows 3.0 SDK
; STATUS        : operational
;
;    Eikon Systems, Inc.
;    989 East Hillsdale Blvd, Suite 260
;    Foster City, California 94404
;    415-349-4664
;
; 07/12/90 1.00 - Kevin P. Welch - initial creation.
;

Extrn    FormInit:Far

_TEXT    SEGMENT     BYTE PUBLIC 'CODE'
         ASSUME      CS:_TEXT
         PUBLIC      LibEntry

LibEntry PROC  FAR

         push  di
         call  FormInit
         Ret

LibEntry ENDP

_TEXT    ENDS

End      LibEntry
