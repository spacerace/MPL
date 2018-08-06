;
; Page Selection - DLL ENTRY POINT MODULE
;
; LANGUAGE      : Microsoft C5.1
; MODEL         : small
; ENVIRONMENT   : Microsoft Windows 3.0 SDK
; STATUS        : operational
;
; This module defines the dynamic link library entry point
; function which calls the Page control registration function.
;
;    Eikon Systems, Inc.
;    989 East Hillsdale Blvd, Suite 260
;    Foster City, California 94404
;    415-349-4664
;
; 11/30/89 1.00 - Kevin P. Welch - initial creation.
;

         Extrn    PageInit:Far

_TEXT    SEGMENT  BYTE PUBLIC 'CODE'
         ASSUME   CS:_TEXT
         PUBLIC   LibEntry

LibEntry PROC  FAR

         push  di
         call  PageInit

         Ret

LibEntry ENDP

_TEXT    ENDS

End      LibEntry
