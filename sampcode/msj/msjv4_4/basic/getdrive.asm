;----- GetDrive.Asm - retrieves the default drive

;Syntax -
;
;   PRINT "The current drive is "; CHR$(GetDrive%)

.286
.Model Medium, BASIC
.Code
    Extrn DosQCurDisk:Proc   ;declare the OS/2 call as external

GetDrive Proc

    Local DriveNum:Word      ;drive number
    Local DriveMap:DWord     ;logical drive map (not used here)

    Push SS                  ;push segment for DriveNumber
    Lea  AX,DriveNum         ;get address
    Push AX                  ;and push that
    Push SS                  ;same for DriveMap segment
    Lea  AX,DriveMap         ;and address
    Push AX
    Call DosQCurDisk         ;call OS/2

    Mov  AX,DriveNum         ;load AX with the returned drive number
    Add  AX,64               ;now 1 = "A", 2 = "B", and so forth
    Ret                      ;return to BASIC with the function
                             ;output in AX

GetDrive Endp
End
