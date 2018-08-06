'***** GetDrive.Bas - BASIC function that returns the current drive

'syntax: Drive$ = CHR$(GetDrive%)
' where: Drive$ is returned as "A", "B", and so forth

DEFINT A-Z
DECLARE SUB DosQCurDisk(SEG Drive, SEG DriveMap&)

FUNCTION GetDrive%

   CALL DosQCurDisk(Drive, DriveMap&)
   GetDrive% = (Drive - 1) + ASC("A")

END FUNCTION
