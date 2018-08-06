
rem		Figure 2
rem		========


rem		"Test Program" for DSEARCH.


'dsearcht.bas
'simple program to check dsearch routine and make sure it's
'working.  The idea is to call it just enough to check out
'its capabilities, and perhaps vary its input by small
'amounts, so that you get a controlled test where there are
'no likely bugs in the BASIC code.


rem $dynamic
DIM A$(0)

start:
input "Path for directory? (wildcards allowed): ",path$
if instr("\:",right$(path$,1)) then path$=path$+"*.*"

'get number of filenames in path
attr%=&H10:arrofs%=0:count%=0:selective%=0
call dsearch(path$,attr%,count%,selective%,arrofs%)

'now count% has actual filename count for redim, or -1 if error
'(other than file not found, that is.)

if count%=-1 then print "Bad pathname":goto start
redim a$(count%)

print "There are "count%"filenames in path "path$"."
'set up a$ so that dsearch doesn't have to change length
for i=0 to count%-1:a$(i)=space$(12):next i

arrptr=varptr(a$(0))
arrofs%=int(arrptr)             'explicit conversion

call dsearch(path$,attr%,count%,selective%,arrofs%)

for i= 0 to count%-1:print i;":";a$(i):next i
goto start
