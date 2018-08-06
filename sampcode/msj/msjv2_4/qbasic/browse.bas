
rem		Figure 3
rem		========



rem		BROWSE.BAS


'BROWSE.BAS - a file browser/directory maintenance routine
'To compile in the editor environment, make a user library by
'assembling DSEARCH.ASM, using BUILDLIB to add it and USERLIB.OBJ
'to a user library (call it MYLIB.EXE, for instance), and then
'use the /L switch to load QB, as in "QB BROWSE /L MYLIB.EXE".
'To compile standalone, use "LINK BROWSE+DSEARCH+USERLIB...."

defint a-z
dim winrec(8) 'holds window dimensions, colors for text and borders

'the following arrays and constants are used for int86()
dim inary(7), outary(7)
const ax = 0, bx = 1, cx = 2, dx = 3
const bp = 4, si = 5, di = 6, flag = 7

'actions returned by PageAndSelect
const disk.change = 1, delete.file = 2, type.file = 3, quit = 4
action.flag = 0  'set by PageAndSelect to one of the above

rem $dynamic
dim filename$(0)
dim shared ScreenData (0,0)

def fn.min(x,y)
   if x < y then fn.min = x else fn.min = y
end def

def fn.max(x,y)
   if x > y then fn.max = x else fn.max = y
end def

'************************ MAIN PROGRAM **************************

call GetCurrDisk(origdisk$)
call GetCurrPath(origdisk$,origpath$)

disk$ = origdisk$

'get a count of all normal and subdir files

Do
   call GetCurrPath(disk$,CurrPath$)
   searchpath$ = disk$ + ":" + CurrPath$ + "*.*"

   attr = &H10 : count  =  0 : selective = 0
   arrofs = int(varptr(filename$(0)))
   call dsearch (searchpath$,attr,count,selective,arrofs)

   ' This should never happen!...
   if count = -1 then
      locate 25,1:print "Invalid path: "searchpath$
      print "Strike a key...":a$ = input$(1):end
   end if

   redim filename$(count-1)
   for i = 0 to count-1:filename$(i) = space$(12):next i

   arrofs = int(varptr(filename$(0)))
   call dsearch (searchpath$,attr,count,selective,arrofs)

   'now filename$(0 to count-1) have all the files and subdirs
   winrec(1) = 1 : winrec(2) = 1    'upper left of window  (row,col)
   winrec(3) = 10 : winrec(4) = 80   'lower right of window
   winrec(5) = 0 : winrec(6) = 11    'fg/bg color
   winrec(7) = 10 : winrec(8) = 0    'border colors, fg/bg

   call makewindow (winrec())

   'do window/cursor/path manipulation.

   call PageAndSelect(winrec(),filename$(),action.flag,f$)

   'f$ comes back as fixed-length (12) string
   ' ...strip off trailing blanks
   i = instr(f$," ") : if i > 0 then f$ = left$(f$,i-1)

   select case action.flag
      case quit
         call SetCurrDisk(OrigDisk$)
         call SetCurrPath(OrigPath$)
         cls : end
      case disk.change
         cls : print "Enter new disk drive letter: ";
         disk$ = input$(1) : print disk$;
         if disk$ > "Z" then disk$ = chr$(asc(disk$) and &HDF)
         call SetCurrDisk(disk$)
         action.flag = 0
      case type.file
         if right$(f$,1) = "\" then
            CurrPath$ = CurrPath$ + f$
            call SetCurrPath(CurrPath$)
         else
            call ListIt (disk$+":"+CurrPath$+f$)
         end if
      case delete.file
         kill (disk$+":"+CurrPath$+f$)
   end select

   action.flag = 0
loop while (1)

'****************************************************************

sub makewindow (winrec(1)) static
y1 = winrec(1):x1 = winrec(2):y2 = winrec(3):x2 = winrec(4)
fc = winrec(5):bc = winrec(6):bfc = winrec(7):bbc = winrec(8)
wid  =  x2-x1+1 : height = y2-y1+1

const vert = 186,upright = 187,lowright = 188
const lowleft = 200,upleft = 201,horiz = 205

color bfc,bbc
locate y1,x1:print chr$(upleft);string$(wid-2,horiz);chr$(upright);
for i = 2 to height-1
   locate y1+i-1,x1:print chr$(vert);
   locate y1+i-1,x2:print chr$(vert);
next i
locate y2,x1:print chr$(lowleft);string$(wid-2,horiz);chr$(lowright);

call clearwindow(winrec())
end sub

'****************************************************************

sub clearwindow (winrec(1)) static
y1 = winrec(1):x1 = winrec(2):y2 = winrec(3):x2 = winrec(4)
fc = winrec(5):bc = winrec(6):bfc = winrec(7):bbc = winrec(8)
wid  =  x2-x1+1 : height = y2-y1+1

color fc,bc
for i = 2 to height-1
   locate y1+i-1,x1+1:print string$(wid-2," ");
next i
end sub

'****************************************************************

sub savewindow (winrec(1)) static
y1 = winrec(1):x1 = winrec(2):y2 = winrec(3):x2 = winrec(4)
fc = winrec(5):bc = winrec(6):bfc = winrec(7):bbc = winrec(8)
wid = x2-x1+1 : height = y2-y1+1

for i = x1 to x2
   for j = y1 to y2
      ScreenData(j-y1,i-x1) = screen(j,i,1) * 256 + screen (j,i,0)
   next j
next i
end sub

'****************************************************************

sub restorewindow (winrec(1)) static
y1 = winrec(1):x1 = winrec(2):y2 = winrec(3):x2 = winrec(4)
fc = winrec(5):bc = winrec(6):bfc = winrec(7):bbc = winrec(8)

for j = y1 to y2
   locate j,x1
   for i = x1 to x2
      d = ScreenData(j-y1,i-x1)
      bc = (d\256)\8 : fc = (d\256) mod 8 : color fc,bc
      print chr$(d and &hff);
   next i
next j
end sub

'****************************************************************

sub PageAndSelect_
  (winrec(1),file$(1),action.flag,FileSelected$) static

'This routine does all the work here.  It assumes MakeWindow
'has been called before entering, and does all key processing and
'subsequent window updates.  action.flag is returned as the action
'for the main program to take (see the constants in the main program
'for a list of possible actions).

shared disk$,CurrPath$,count

'second codes from extended keys used in PageAndSelect
const up = 72, down = 80, left = 75, right = 77
const f1 = 59, AltD = 32, AltX = 45

y1 = winrec(1):x1 = winrec(2):y2 = winrec(3):x2 = winrec(4)
fc = winrec(5):bc = winrec(6):bfc = winrec(7):bbc = winrec(8)
wid = x2-x1+1 : height = y2-y1+1

locate y1,x1+1:color bfc,bbc:print " ";disk$;":";CurrPath$;" "
locate y2,x1+1
print "RET-Type file  AltD - delete file  F1 - chg disk  AltX - quit"
locate y1,x2-1-9:print count;"files"

NamesPerLine = (wid-2)\15 'how many filenames on each line in window
lines = height-2          'how many usable lines inside window
FilesPerWindow = lines * NamesPerLine

StartIndex = 0 'first file$() to be displayed in window
CurrIndex = 0  'current file$() being highlighted
NewIndex = 0   'updated file$() index after moving cursor

do
   call clearwindow(winrec())
   limit = fn.min (ubound(file$) - StartIndex, FilesPerWindow - 1)
   color fc,bc : locate y1+1
   for i = 0 to limit step NamesPerLine
      for j = 0 to NamesPerLine-1
         locate ,x1 + 1 + j*15
         if StartIndex + i + j <= ubound(file$) then
            print file$(StartIndex+i+j);
         else
            print space$(15);
         end if
      next j
   print
   next i

   'initialize highlight bar position
   BarRow = y1+1 : BarCol = x1 + 1

   do
      color fc,bc : locate BarRow,BarCol : print file$(CurrIndex);
      CurrIndex = NewIndex
      BarRow = (CurrIndex-StartIndex)\NamesPerLine + y1 + 1
      BarCol = (CurrIndex mod NamesPerLine)*15 + x1 + 1
      color bc,fc : locate BarRow,BarCol : print file$(CurrIndex);

      'wait for extended character (look for cursor keys) or RETURN
      GetKey:
         a$ = "" : while a$ = "" : a$ = inkey$ : wend
         if a$ = chr$(13) then
            action.flag = type.file
            FileSelected$ = file$(CurrIndex)
            exit do
         end if
      if len(a$)<>2 then goto GetKey

      redraw = 0  'flag to indicate when window must be redrawn
                  'and in which direction the cursor was moving
      select case asc(right$(a$,1))
         case up
            NewIndex = fn.max (0,CurrIndex-NamesPerLine)
            if NewIndex < StartIndex then redraw = -1
         case down
            NewIndex = fn.min (ubound(file$), CurrIndex+NamesPerLine)
            if NewIndex > StartIndex + FilesPerWindow - 1 _
             then redraw = 1
         case left
            NewIndex = fn.max (0,CurrIndex-1)
            if NewIndex < StartIndex then redraw = -1
         case right
            NewIndex = fn.min (ubound(file$), CurrIndex + 1)
            if NewIndex > StartIndex + FilesPerWindow - 1 _
             then redraw = 1

         case f1
            action.flag = disk.change : file.selected = -1 : exit do
         case AltX
            action.flag = quit : file.selected = -1 : exit do
         case AltD
            action.flag = delete.file
            FileSelected$ = file$(CurrIndex) : exit do
         case else
            sound 440,2:sound 220,2
      end select
   loop while redraw = 0

   if action.flag then exit do
   'otherwise, fall through and redo the entire "do" loop
   select case sgn(redraw)
      case -1
        StartIndex = fn.max (StartIndex - FilesPerWindow , 0)
      case 1
        StartIndex = fn.min (StartIndex + FilesPerWindow , _
                             ubound(file$))
   end select
   CurrIndex = StartIndex
loop while not action.flag

end sub

'****************************************************************

sub     GetCurrPath(disk$,CurrPath$) static
shared inary(),outary()

'first set up 64-byte work area for Int 21h Function 47h
' (get current directory)
CurrPath$ = space$(64) : pathptr = sadd(CurrPath$)

inary(dx)=asc(disk$)-65+1
inary(si)=pathptr 'pointer to area to fill
inary(ax)=&H4700  'function number
call int86(&H21,varptr(inary(0)),varptr(outary(0)))

'strip off trailing NUL, rest of blanks
CurrPath$=left$(CurrPath$,instr(CurrPath$,chr$(0))-1)
CurrPath$="\"+CurrPath$
if CurrPath$ <> "\" then CurrPath$=CurrPath$+"\"
end sub

'****************************************************************

sub GetCurrDisk(disk$) static
shared inary(),outary()
inary(ax) = &H1900
call int86(&H21,varptr(inary(0)),varptr(outary(0)))
disk$ = chr$(65 + (outary(ax) and 255))
end sub

'****************************************************************

sub SetCurrDisk(disk$) static
shared inary(),outary()

inary(ax) = &H0E00
inary(dx) = asc(disk$) - 65
call int86(&H21,varptr(inary(0)),varptr(outary(0)))

end sub

'****************************************************************

sub SetCurrPath(path$) static
shared inary(),outary()

newpath$=path$
if newpath$<>"\" then newpath$=left$(newpath$,len(newpath$)-1)
newpath$=newpath$+chr$(0)

inary(ax) = &H3B00
inary(dx) = sadd(newpath$)
call int86(&H21,varptr(inary(0)),varptr(outary(0)))

end sub

'****************************************************************

sub TypeIt (path$) static
   cls
   shell ("type "+path$+" | more")
   locate 25,1:print "Any key to continue...";
   a$=input$(1):cls
end sub

'****************************************************************

sub ListIt (path$) static
   shell ("list "+path$)
   cls
end sub

