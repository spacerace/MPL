'***** FCount.Bas - contains BASIC function returning number of matching 
'      file names


'syntax: Count = FileCount%(FileSpec$)
' where: FileSpec$ is in the form "*.BAS" or "A:\subdir\*.*", 
'        and so forth
'  note: You may also specify a complete file name to see if 
'        it exists

DEFINT A-Z

TYPE FileFindBuf		'this receives the date, time, size, etc.
    MiscInfo AS STRING * 36
END TYPE

DECLARE FUNCTION _
	DosFindFirst%(BYVAL SpecSeg, BYVAL SpecAdr, _
		      SEG Handle, BYVAL Attrib, SEG Buffer _
		      AS FileFindBuf, BYVAL BufLen, SEG MaxCount, _
		      BYVAL Reserved&)

DECLARE FUNCTION DosFindNext%(BYVAL Handle, SEG Buffer AS FileFindBuf, _
			      BYVAL BufLen, SEG MaxCount)

DECLARE SUB DosFindClose(BYVAL Handle)

FUNCTION FileCount%(FileSpec$)

 
   FileCount% = 0               'default to no matching file names

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

   IF ErrorCode THEN EXIT FUNCTION  'no matching files or another 
                                    'error, exit

   Count = 0                        'we got one, increment at next step 

   DO
      Count = Count + 1             'we got another one
   LOOP WHILE (DosFindNext%(Handle, Buffer, BufLen, MaxCount) = 0)

   DosFindClose%(Handle)            'close the file handle
   FileCount% = Count               'assign the function output

END FUNCTION
