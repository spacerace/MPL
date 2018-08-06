'***** ReadName.Bas - contains BASIC function returning an array of file names

'syntax: Count = FileCount%(FileSpec$)
'        DIM Array$(Count)
'        CALL ReadNames(FileSpec$, Array$())
' where: FileSpec$ is in the form "*.BAS" or "A:\subdir\*.*", and so 
'        forth, and Array$() is filled in with each file's name

DEFINT A-Z

TYPE FileFindBuf
    MiscInfo AS STRING * 22		'Receives date, time, size, etc.
    FileLen  AS STRING * 1		'Receives the length of the name
    FileName AS STRING * 13		'Receives the name
END TYPE


DECLARE FUNCTION _
	DosFindFirst%(BYVAL SpecSeg, BYVAL SpecAdr, _
		      SEG Handle, BYVAL Attrib, SEG Buffer _
		      AS FileFindBuf, BYVAL BufLen, SEG MaxCount, _
		      BYVAL Reserved&)

DECLARE FUNCTION DosFindNext%(BYVAL Handle, SEG Buffer AS FileFindBuf, _
			      BYVAL BufLen, SEG MaxCount)

DECLARE SUB DosFindClose(BYVAL Handle)

SUB ReadNames(FileSpec$, Array$())

   FSpec$ = FileSpec$ + CHR$(0) 'create an ASCIIZ string from 
                                'the file spec
   Handle = -1			'request a new search handle
   Attrib = 39                  'matches all file types except 
                                'subdirectory
   DIM Buffer AS FileFindBuf    'create a buffer to hold the 
                                'returned info
   BufLen = 36                  'tells OS/2 how big the buffer is
   MaxCount = 1                 'get just one file
   Reserved& = 0                'this is needed, but not used

                                'call DosFindFirst
   ErrorCode = DosFindFirst%(VARSEG(FSpec$), SADD(FSpec$), Handle, _
               Attrib, Buffer, BufLen, MaxCount, Reserved&)

   IF ErrorCode THEN EXIT SUB    'no matching files, exit
   Count = 0                     'initialize counter

   DO
      Count = Count + 1               'we got another one
      Length = ASC(Buffer.FileLen)    'get the name's length
      Array$(Count) = LEFT$(Buffer.FileName, Length) 'assign the name
   LOOP WHILE (DosFindNext%(Handle, Buffer, BufLen, MaxCount) = 0)

   DosFindClose(Handle)               'close the handle

END SUB
