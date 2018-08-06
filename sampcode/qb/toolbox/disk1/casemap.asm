 ; **********************************************
 ; **  CASEMAP.ASM                   MASM 5.0  **
 ; **                                          **
 ; **  Assembly subprogram for translating     **
 ; **  some characters according to the        **
 ; **  currently loaded MS-DOS country-        **
 ; **  dependent information.                  **
 ; **                                          **
 ; **  Use:  CALL CASEMAP (CHAR%, SEG%, OFS%)  **
 ; **  Note: CHAR% is passed by reference      **
 ; **        SEG% and OFS% are passed by value **
 ; **********************************************
 ;
 ; EXAMPLE OF USE:  CALL CaseMap (char%, seg%, ofs%)
 ; PARAMETERS:      char%      Character byte to be translated
 ;                  seg%       Segment of address of MS-DOS translate routine
 ;                  ofs%       Offset of address of MS-DOS translate routine
 ; VARIABLES:       (none)
 ; MODULE LEVEL
 ;   DECLARATIONS:  DECLARE SUB GetCountry (country AS CountryType)
 ;                  DECLARE SUB CaseMap (character%, BYVAL Segment%,
 ;                                       BYVAL Offset%)
 ;              DECLARE FUNCTION TranslateCountry$ (a$, country AS CountryType)


.MODEL  MEDIUM
.CODE
        public casemap

casemap proc

; Standard entry
        push bp
        mov bp, sp
        
; Get CHAR% into AX register
        mov bx, (bp+10)
        mov ax, (bx)

; Call the translate function in MS-DOS
        call    dword ptr [bp+6]
        
; Return translated character to CHAR%
        mov bx, (bp+10)
        mov (bx), ax
        
; Standard exit, assumes three variables passed
        pop bp
        ret 6

; End of the procedure
casemap endp
        END

