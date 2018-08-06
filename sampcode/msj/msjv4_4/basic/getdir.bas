'***** GetDir.Bas - BASIC function that returns the current directory

'syntax: Directory$ = GetDir$(Drive$)
' where: Drive$ is "A", "C", etc, or a null string meaning 
'        the current drive

DEFINT A-Z
DECLARE FUNCTION DosQCurDir%(BYVAL Drive, BYVAL StrSeg, _
			     BYVAL StrAdr, SEG Length)


FUNCTION GetDir$(Drive$)

   IF LEN(Drive$) THEN          'see if a drive letter was specified
      DriveNum = ASC(UCASE$(Drive$)) - ASC("A") + 1 'convert to a drive number
   ELSE
      DriveNum = 0         'else, tell OS/2 to use the current drive
   END IF

   BufLen = 64             'define the buffer's length
   Path$ = SPACE$(BufLen)  'create a buffer to receive the directory
                           'invoke the function
   ErrorCode = DosQCurDir%(DriveNum, VARSEG(Path$), SADD(Path$), _
                          BufLen)

   IF ErrorCode THEN      'see if there was an error
      GetDir$ = ""       'return null string to indicate the error
   ELSE                  'otherwise return what precedes the CHR$(0)
      GetDir$ = "\" + LEFT$(Path$, INSTR(Path$, CHR$(0)) - 1)
   END IF

END FUNCTION
