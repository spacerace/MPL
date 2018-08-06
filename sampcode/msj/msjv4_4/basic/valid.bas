'***** Valid.Bas - BASIC function that reports if a drive 
'                  letter is valid

'syntax: Okay = Valid%(Drive$)
' where: Okay receives -1 if the drive is valid, or zero if it is not

DEFINT A-Z
DECLARE SUB DosQCurDisk(SEG Temp, SEG DriveMap&)

FUNCTION Valid%(Drive$)

   Call DosQCurDisk(Temp, DriveMap&)
   Drive = ASC(Drive$) - ASC("A")
   IF DriveMap& AND 2 ^ Drive THEN	'Check the appropriate bit in the mask
      Valid% = -1
   ELSE
      Valid% = 0
   END IF

END FUNCTION
