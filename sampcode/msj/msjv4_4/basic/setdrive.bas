'***** SetDrive.Bas - BASIC subprogram that sets the current drive

'syntax: CALL SetDrive(Drive$)
' where: Drive$ is specified as "A", "B", and so forth

DEFINT A-Z
DECLARE FUNCTION DosSelectDisk%(BYVAL Drive)

SUB SetDrive(Drive$)

   Drive = ASC(UCASE$(Drive$)) - ASC("A") + 1    'now "A" = 1, "B" = 2, etc.
   ErrorCode = DosSelectDisk%(Drive)

END SUB
