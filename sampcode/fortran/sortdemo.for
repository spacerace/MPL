$NOTRUNCATE
$STORAGE:2
        INTERFACE TO INTEGER*2 FUNCTION KbdCharIn
     +  [ALIAS: 'KBDCHARIN']
     +  (CHARDATA,
     +   IoWait [VALUE],
     +   KbdHandle [VALUE])

        INTEGER*2 CHARDATA(10)*1, IoWait, KbdHandle

        END

        INTERFACE TO INTEGER*2 FUNCTION DosBeep
     +  [ALIAS: 'DOSBEEP']
     +  (Frequency [VALUE],
     +   Duration [VALUE])

        INTEGER*2 Frequency, Duration

        END

        INTERFACE TO INTEGER*2 FUNCTION DosGetDateTime
     +  [ALIAS: 'DOSGETDATETIME']
     +  (DateTime)

        INTEGER*1 DateTime(11)

        END
        INTERFACE TO INTEGER*2 FUNCTION DosSleep
     +  [ALIAS: 'DOSSLEEP']
     +  (TimeInterval [VALUE])

        INTEGER*4 TimeInterval

        END

        INTERFACE TO INTEGER*2 FUNCTION VioScrollDn
     +  [ALIAS: 'VIOSCROLLDN']
     +  (TopRow [VALUE],
     +   LeftCol [VALUE],
     +   BotRow [VALUE],
     +   RightCol [VALUE],
     +   Lines [VALUE],
     +   Cell,
     +   VioHandle [VALUE])

        INTEGER*2 TopRow, LeftCol, BotRow, RightCol
        INTEGER*2 Lines, Cell, VioHandle

        END

        INTERFACE TO INTEGER*2 FUNCTION VioWrtCharStrAtt
     +  [ALIAS: 'VIOWRTCHARSTRATT']
     +  (CharString,
     +   Length [VALUE],
     +   Row [VALUE],
     +   Column [VALUE],
     +   Attr,
     +   VioHandle [VALUE])

        CHARACTER*80 CharString
        INTEGER*2 Length, Row, Column, Attr*1, VioHandle

        END

        INTERFACE TO INTEGER*2 FUNCTION VioReadCellStr
     +  [ALIAS: 'VIOREADCELLSTR']
     +  (CellStr,
     +   Length,
     +   Row [VALUE],
     +   Column [VALUE],
     +   VioHandle [VALUE])

        CHARACTER*8000 CellStr
        INTEGER*2 Length, Row, Column, VioHandle

        END

        INTERFACE TO INTEGER*2 FUNCTION VioWrtCellStr
     +  [ALIAS: 'VIOWRTCELLSTR']
     +  (CellStr,
     +   Length [VALUE],
     +   Row [VALUE],
     +   Column [VALUE],
     +   VioHandle [VALUE])

        CHARACTER*8000 CellStr
        INTEGER*2 Length, Row, Column, VioHandle

        END

        INTERFACE TO INTEGER*2 FUNCTION VioWrtNCell
     +  [ALIAS: 'VIOWRTNCELL']
     +  (Cell,
     +   Times [VALUE],
     +   Row [VALUE],
     +   Column [VALUE],
     +   VioHandle [VALUE])

        INTEGER*2 Cell, Times, Row, Column, VioHandle

        END

        INTERFACE TO INTEGER*2 FUNCTION VioGetCurPos
     +  [ALIAS: 'VIOGETCURPOS']
     +  (Row,
     +   Column,
     +   VioHandle [VALUE])

        INTEGER*2 Row, Column, VioHandle

        END

        INTERFACE TO INTEGER*2 FUNCTION VioSetCurPos
     +  [ALIAS: 'VIOSETCURPOS']
     +  (Row [VALUE],
     +   Column [VALUE],
     +   VioHandle [VALUE])

        INTEGER*2 Row, Column, VioHandle

        END

        INTERFACE TO INTEGER*2 FUNCTION VioGetMode
     +  [ALIAS: 'VIOGETMODE']
     +  (MODE,
     +   VioHandle [VALUE])

        INTEGER*2 MODE(6), VioHandle

        END

        INTERFACE TO INTEGER*2 FUNCTION VioSetMode
     +  [ALIAS: 'VIOSETMODE']
     +  (MODE,
     +   VioHandle [VALUE])

        INTEGER*2 MODE(6), VioHandle

        END

      PROGRAM SortDemo
C                                 SORTDEMO
C This program graphically demonstrates six common sorting algorithms.  It
C prints 25 or 43 horizontal bars, all of different lengths and all in random
C order, then sorts the bars from smallest to longest.
C
C The program also uses SOUND statements to generate different pitches,
C depending on the location of the bar being printed. Note that the SOUND
C statements delay the speed of each sorting algorithm so you can follow
C the progress of the sort. Therefore, the times shown are for comparison
C only. They are not an accurate measure of sort speed.
C
C If you use these sorting routines in your own programs, you may notice
C a difference in their relative speeds (for example, the exchange
C sort may be faster than the shell sort) depending on the number of
C elements to be sorted and how "scrambled" they are to begin with.
C
      IMPLICIT INTEGER*2(a-z)
      CHARACTER cellstr*8000
      COMMON /misc/MaxBars,MaxColors,Sound,Pause
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
      DIMENSION mode(6),wmode(6)
      DATA length,mode(1)/8000,12/
      gbg = VioGetCurPos(crow,ccol,0)
      gbg = VioReadCellStr(cellstr,length,0,0,0)
      gbg = VioGetMode(mode,0)
      DO 100 i=1,6
     	  mode(i)= mode(i)
100   CONTINUE
C
C If monochrome or color burst disabled, use one color
C
      IF((.not. btest(mode(2),0)).OR.(btest(mode(2),2))) MaxColors=1
C
C First try 43 lines on VGA, then EGA. If neither, use 25 lines.
C
      IF(wmode(4).NE.43) THEN
        wmode(4)=43
        wmode(5)=640
        wmode(6)=350
        IF(VioSetMode(wmode,0).NE.0) THEN
          wmode(5)=720
          wmode(6)=400
          IF(VioSetMode(wmode,0).NE.0) THEN
            gbg=VioGetMode(wmode,0)
            MaxBars=25
            wmode(4)=25
            gbg=VioSetMode(wmode,0)
          ENDIF
        ENDIF
      ENDIF  
      CALL Initialize
      CALL SortMenu
      IF(mode(4).NE.MaxBars) gbg = VioSetMode(mode,0)
      gbg = VioWrtCellStr(cellstr,length,0,0,0)
      gbg = VioSetCurPos(crow,ccol,0)
      END

      BLOCK DATA
      IMPLICIT INTEGER*2(a-z)
      LOGICAL Sound
      COMMON /misc/MaxBars,MaxColors,Sound,Pause
      DATA MaxBars/43/,MaxColors/15/,Sound/.TRUE./,Pause/30/
      END

      SUBROUTINE BoxInit
C
C =============================== BoxInit ====================================
C    Calls the DrawFrame procedure to draw the frame around the sort menu,
C    then prints the different options stored in the OptionTitle array.
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
      INTEGER*1 COLOR
      PARAMETER (COLOR=15,FIRSTMENU=1,LEFT=48,LINELENGTH=30,NLINES=18,
     +           WIDTH=80-LEFT)
      CHARACTER Factor*4,menu(NLINES)*30
      LOGICAL Sound
      COMMON /misc/MaxBars,MaxColors,Sound,Pause
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
      DATA menu/
     +      '     FORTRAN Sorting Demo',
     +      ' ',
     +      'Insertion',
     +      'Bubble',
     +      'Heap',
     +      'Exchange',
     +      'Shell',
     +      'Quick',
     +      ' ',
     +      'Toggle Sound: ',
     +      ' ',
     +      'Pause Factor: ',
     +      '<   (Slower)',
     +      '>   (Faster)',
     +      ' ',
     +      'Type first character of',
     +      'choice ( I B H E S Q T < > )',
     +      'or ESC key to end program: '/
C
      CALL DrawFrame (1,LEFT-3,WIDTH+3,22)
C
      DO 100 i=1,NLINES
        gbg = VioWrtCharStrAtt(menu(i),LINELENGTH,FIRSTMENU + i,
     +                         LEFT,COLOR,0)
100   CONTINUE
      WRITE(Factor,'(I2.2)')Pause/30
      gbg = VioWrtCharStrAtt(Factor,len(Factor),13,LEFT+14,COLOR,0)
C
C Erase the speed option if the length of the Pause is at a limit
C
      IF(Pause.EQ.900) THEN
        gbg = VioWrtCharStrAtt('            ',12,14,LEFT,COLOR,0)
      ELSEIF(Pause.EQ.0) THEN
        gbg = VioWrtCharStrAtt('            ',12,15,LEFT,COLOR,0)
      ENDIF
C
C Print the current value for Sound:
C
      IF(Sound) THEN
        gbg = VioWrtCharStrAtt('ON ',3,11,LEFT+14,COLOR,0)
      ELSE
        gbg = VioWrtCharStrAtt('OFF',3,11,LEFT+14,COLOR,0)
      ENDIF
C
      RETURN
      END

      SUBROUTINE BubbleSort
C
C ============================== BubbleSort ==================================
C    The BubbleSort algorithm cycles through SortArray, comparing adjacent
C    elements and swapping pairs that are out of order.  It continues to
C    do this until no pairs are swapped.
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
      COMMON /misc/MaxBars,MaxColors,Sound,Pause
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
C
      limit = MaxBars
1     CONTINUE
      switch = 0
      DO 100 row=1,limit-1
C
C Two adjacent elements are out of order, so swap their values
C and redraw those two bars:
C
        IF(SortArray(BARLENGTH,row).GT.SortArray(BARLENGTH,row+1)) THEN
          CALL SwapSortArray(row,row+1)
          CALL SwapBars(row,row+1)
          switch = row
        ENDIF
100   CONTINUE
C
C Sort on next pass only to where the last switch was made:
C
      limit = switch
      IF(switch.NE.0) GO TO 1
      RETURN
      END

      SUBROUTINE DrawFrame(Top,Left,Width,Height)
C
C ============================== DrawFrame ===================================
C   Draws a rectangular frame using the high-order ASCII characters É (201) ,
C   » (187) , È (200) , ¼ (188) , º (186) , and Í (205).
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
C
      CHARACTER tempstr*80
      INTEGER*1 Attr,COLOR
      PARAMETER (ULEFT=201,URIGHT=187,LLEFT=200,LRIGHT=188,
     +           VERTICAL=186,HORIZONTAL=205,SPACE=32,COLOR=15)
C
      Attr=COLOR
      CellAttr=ishl(COLOR,8)
      bottom=Top+Height-1
      right=Left+Width-1
      gbg = VioWrtNCell(ior(CellAttr,ULEFT),1,Top,Left,0)
      gbg = VioWrtNCell(ior(CellAttr,HORIZONTAL),
     +                  Width-2,Top,Left+1,0)
      gbg = VioWrtNCell(ior(CellAttr,URIGHT),1,Top,right,0)
      tempstr(1:1)=char(VERTICAL)
      DO 100 i=2,Width-1
        tempstr(i:i)=char(SPACE)
100   CONTINUE
      tempstr(Width:Width)=char(VERTICAL)
      DO 200 i=1,Height-2
        gbg = VioWrtCharStrAtt(tempstr,Width,i+Top,Left,COLOR,0)
200   CONTINUE
      gbg = VioWrtNCell(ior(CellAttr,LLEFT),1,bottom,Left,0)
      gbg = VioWrtNCell(ior(CellAttr,HORIZONTAL),
     +                  Width-2,bottom,Left+1,0)
      gbg = VioWrtNCell(ior(CellAttr,LRIGHT),1,bottom,right,0)
      RETURN
      END

      SUBROUTINE ElapsedTime(CurrentRow)
C
C ============================= ElapsedTime ==================================
C    Prints seconds elapsed since the given sorting routine started.
C    Note that this time includes both the time it takes to redraw the
C    bars plus the pause while the SOUND statement plays a note, and
C    thus is not an accurate indication of sorting speed.
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
      CHARACTER Timing*7
      INTEGER*1 DateTime(12),COLOR
      INTEGER*4 time0,time1
      LOGICAL Sound
      COMMON /misc/MaxBars,MaxColors,Sound,Pause
      COMMON /time/time0
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
      PARAMETER (COLOR=15,FIRSTMENU=1,LEFT=48)
      gbg = DosGetDateTime(DateTime)
      time1=DateTime(1)*360000+
     +      DateTime(2)*6000+
     +      DateTime(3)*100+
     +      DateTime(4)
      WRITE(Timing,'(F7.2)')float(time1-time0)/100.
C
C Print the number of seconds elapsed
C
      gbg = VioWrtCharStrAtt(Timing,len(Timing),Select+FIRSTMENU+3,
     +                       LEFT+15,COLOR,0)
C
      IF(Sound) gbg = DosBeep(60*CurrentRow,32)
      gbg = DosSleep(int4(Pause))
      RETURN
      END

      SUBROUTINE ExchangeSort
C
C ============================= ExchangeSort =================================
C   The ExchangeSort compares each element in SortArray - starting with
C   the first element - with every following element.  If any of the
C   following elements is smaller than the current element, it is exchanged
C   with the current element and the process is repeated for the next
C   element in SortArray.
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
      COMMON /misc/MaxBars,MaxColors,Sound,Pause
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
C
      DO 100 Row=1,MaxBars-1
        SmallestRow = Row
        DO 200 j=Row+1,MaxBars
          IF(SortArray(BARLENGTH,j) .LT. 
     +       SortArray(BARLENGTH,SmallestRow)) THEN
            SmallestRow = j
            CALL ElapsedTime(j)
          ENDIF
200     CONTINUE
        IF(SmallestRow.GT.Row) THEN
C
C       Found a row shorter than the current row, so swap those
C       two array elements:
C
          CALL SwapSortArray(Row,SmallestRow)
          CALL SwapBars(Row,SmallestRow)
        ENDIF
100   CONTINUE
      RETURN
      END

      SUBROUTINE HeapSort
C
C =============================== HeapSort ===================================
C  The HeapSort procedure works by calling two other procedures - PercolateUp
C  and PercolateDown.  PercolateUp turns SortArray into a "heap," which has
C  the properties outlined in the diagram below:
C
C                               SortArray(1)
C                               /          \
C                    SortArray(2)           SortArray(3)
C                   /          \            /          \
C         SortArray(4)   SortArray(5)   SortArray(6)  SortArray(7)
C          /      \       /       \       /      \      /      \
C        ...      ...   ...       ...   ...      ...  ...      ...
C
C
C  where each "parent node" is greater than each of its "child nodes"; for
C  example, SortArray(1) is greater than SortArray(2) or SortArray(3),
C  SortArray(3) is greater than SortArray(6) or SortArray(7), and so forth.
C
C  Therefore, once the first FOR...NEXT loop in HeapSort is finished, the
C  largest element is in SortArray(1).
C
C  The second FOR...NEXT loop in HeapSort swaps the element in SortArray(1)
C  with the element in MaxRow, rebuilds the heap (with PercolateDown) for
C  MaxRow - 1, then swaps the element in SortArray(1) with the element in
C  MaxRow - 1, rebuilds the heap for MaxRow - 2, and continues in this way
C  until the array is sorted.
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
      COMMON /misc/MaxBars,MaxColors,Sound,Pause
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
C
      DO 100 i=2,MaxBars
        CALL PercolateUp(i)
100   CONTINUE
C
      DO 200 i=MaxBars,2,-1
        CALL SwapSortArray(1,i)
        CALL SwapBars(1,i)
        CALL PercolateDown(i-1)
200   CONTINUE
      RETURN
      END

      SUBROUTINE Initialize
C
C ============================== Initialize ==================================
C    Initializes the SortBackup and OptionTitle arrays.  It also calls the
C    BoxInit procedure.
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
      INTEGER*1 DateTime(11)
      LOGICAL Sound
      REAL Seed,SRand
      COMMON /misc/MaxBars,MaxColors,Sound,Pause
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
C
      DIMENSION temparray(43)
      BARLENGTH = 1
      BARCOLOR = 2
      DO 100 i=1,MaxBars
        temparray(i) = i
100   CONTINUE
C
C Seed the random-number generator.
C
      gbg = DosGetDateTime(DateTime)
      Seed = DateTime(1)*3600+DateTime(2)*60+DateTime(3)
      Seed = SRand(Seed/86400.*259199.)
C
      MaxIndex = MaxBars
      DO 200 i=1,MaxBars
C
C Find a random element in TempArray between 1 and MaxIndex,
C then assign the value in that element to LengthBar
C
        index = RANDLIM(1,MaxIndex)
        lengthbar = temparray(index)
C
C Overwrite the value in TempArray(Index) with the value in
C TempArray(MaxIndex) so the value in TempArray(Index) is
C chosen only once:
C
        temparray(index) = temparray(MaxIndex)
C
C Decrease the value of MaxIndex so that TempArray(MaxIndex) can't
C be chosen on the next pass through the loop:
C
        MaxIndex = MaxIndex - 1
C
        SortBackup(BARLENGTH,i) = LengthBar
        IF(MaxColors.EQ.1) THEN
          SortBackup(BARCOLOR,i) = 7
        ELSE  
          SortBackup(BARCOLOR,i) = mod(LengthBar,MaxColors) + 1
        ENDIF
200   CONTINUE
      CALL cls
C Assign values in SortBackup to SortArray and draw unsorted bars on the screen.
      CALL Reinitialize
C Draw frame for the sort menu and print options.
      CALL BoxInit
      RETURN
      END

      SUBROUTINE InsertionSort
C
C ============================= InsertionSort ================================
C   The InsertionSort procedure compares the length of each successive
C   element in SortArray with the lengths of all the preceding elements.
C   When the procedure finds the appropriate place for the new element, it
C   inserts the element in its new place, and moves all the other elements
C   down one place.
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
      COMMON /misc/MaxBars,MaxColors,Sound,Pause
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
      DIMENSION TempArray(2)
      DO 100 Row=2,MaxBars
        TempArray(BARLENGTH) = SortArray(BARLENGTH,Row)
        TempArray(BARCOLOR) = SortArray(BARCOLOR,Row)
        DO 200 j=Row,2,-1
C
C As long as the length of the j-1st element is greater than the
C length of the original element in SortArray(Row), keep shifting
C the array elements down:
C
          IF(SortArray(BARLENGTH,j - 1).GT.TempArray(BARLENGTH)) THEN
            SortArray(BARLENGTH,j) = SortArray(BARLENGTH,j - 1)
            SortArray(BARCOLOR,j) = SortArray(BARCOLOR,j - 1)
            CALL PrintOneBar(j)
            CALL ElapsedTime(j)
          ELSE
            GO TO 201
          ENDIF
200     CONTINUE
201   CONTINUE
C
C Insert the original value of SortArray(Row) in SortArray(j):
C
      SortArray(BARLENGTH,j) = TempArray(BARLENGTH)
      SortArray(BARCOLOR,j) = TempArray(BARCOLOR)
      CALL PrintOneBar(j)
      CALL ElapsedTime(j)
100   CONTINUE
      RETURN
      END

C
C ============================ PercolateDown =================================
C   The PercolateDown procedure restores the elements of SortArray from 1 to
C   MaxLevel to a "heap" (see the diagram with the HeapSort procedure).
C ============================================================================
C
      SUBROUTINE PercolateDown(MaxLevel)
      IMPLICIT INTEGER*2(a-z)
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
C
      i = 1
C
C Move the value in SortArray(1) down the heap until it has reached
C its proper node (that is, until it is less than its parent node
C or until it has reached MaxLevel, the bottom of the current heap):
C
1     CONTINUE
C Get the subscript for the child node.
      Child = 2 * i
C
C Reached the bottom of the heap, so exit this procedure:
C
      IF(Child.GT.MaxLevel) RETURN
C
C If there are two child nodes, find out which one is bigger:
C
      IF(Child+1.LE.MaxLevel) THEN
        IF(SortArray(BARLENGTH,Child+1).GT.SortArray(BARLENGTH,Child))
     +    Child=Child+1
      ENDIF
C
C Move the value down if it is still not bigger than either one of
C its children:
C
      IF(SortArray(BARLENGTH,i).LT.SortArray(BARLENGTH,Child)) THEN
        CALL SwapSortArray(i,Child)
        CALL SwapBars(i,Child)
        i = Child
      ELSE
C
C Otherwise, SortArray has been restored to a heap from 1 to
C MaxLevel, so exit:
C
        RETURN
      ENDIF
      GO TO 1
      END

      SUBROUTINE PercolateUp(MaxLevel)
C
C ============================== PercolateUp =================================
C   The PercolateUp procedure converts the elements from 1 to MaxLevel in
C   SortArray into a "heap" (see the diagram with the HeapSort procedure).
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
C
      i = MaxLevel
C
C Move the value in SortArray(MaxLevel) up the heap until it has
C reached its proper node (that is, until it is greater than either
C of its child nodes, or until it has reached 1, the top of the heap):
C
1     CONTINUE
      IF(i.EQ.1) RETURN
C Get the subscript for the parent node.
      Parent = i / 2
C
C The value at the current node is still bigger than the value at
C its parent node, so swap these two array elements:
C
      IF(SortArray(BARLENGTH,i).GT.SortArray(BARLENGTH,Parent)) THEN
        CALL SwapSortArray(Parent,i)
        CALL SwapBars(Parent,i)
        i = Parent
        GO TO 1
      ENDIF
C
C Otherwise, the element has reached its proper place in the heap,
C so exit this procedure:
C
      RETURN
      END

      SUBROUTINE PrintOneBar(Row)
C
C ============================== PrintOneBar =================================
C  Prints SortArray(BARLENGTH,Row) at the row indicated by the Row
C  parameter, using the color in SortArray(BARCOLOR,Row)
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
      PARAMETER (BLOCK=223,SPACE=16#0720)
      COMMON /misc/MaxBars,MaxColors,Sound,Pause
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
C
      gbg = VioWrtNCell(ior(ishl(SortArray(BARCOLOR,ROW),8),BLOCK),
     +                  SortArray(BARLENGTH,Row),Row,1,0)
      blanks=MaxBars-SortArray(BARLENGTH,Row)
      IF(blanks.GT.0)
     +  gbg = VioWrtNCell(SPACE,blanks,Row,SortArray(BARLENGTH,Row)+1,0)
      RETURN
      END

      SUBROUTINE QuickSort(Low,High)
      IMPLICIT INTEGER*2(a-z)
      PARAMETER (LOG2MAXBARS=6)
      INTEGER*1 StackPtr,Upper(LOG2MAXBARS),Lower(LOG2MAXBARS)
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
      Lower(1)=Low
      Upper(1)=High
      StackPtr=1
100   CONTINUE
      IF(Lower(StackPtr).GE.Upper(StackPtr)) THEN
        StackPtr = StackPtr - 1
      ELSE
        i = Lower(StackPtr)
        j = Upper(StackPtr)
        Pivot = SortArray(BARLENGTH,j)
200     CONTINUE
300     IF(i.LT.j.AND.SortArray(BARLENGTH,i).LE.Pivot) THEN
          i = i + 1
          GO TO 300
        ENDIF
400     IF(j.GT.i.AND.SortArray(BARLENGTH,j).GE.Pivot) THEN
          j = j - 1
          GO TO 400
        ENDIF
        IF(i.LT.j)THEN
          CALL SwapSortArray(i,j)
          CALL SwapBars(i,j)
       ENDIF
        IF(i.LT.j) GO TO 200
        j = Upper(StackPtr)
        CALL SwapSortArray(i,j)
        CALL SwapBars(i,j)
        IF(i-Lower(StackPtr).LT.Upper(StackPtr)-i) THEN
          Lower(StackPtr+1) = Lower(StackPtr)
          Upper(StackPtr+1) = i - 1
          Lower(StackPtr) = i + 1
        ELSE
          Lower(StackPtr+1) = i + 1
          Upper(StackPtr+1) = Upper(StackPtr)
          Upper(StackPtr) = i - 1
        ENDIF
        StackPtr = StackPtr + 1
      ENDIF
      IF(StackPtr.GT.0) GO TO 100
      RETURN
      END

      INTEGER FUNCTION RandLim (Lo,Hi)
      IMPLICIT INTEGER*2(a-z)
      REAL Seed,SRand,X
      Seed = mod(int(Seed)*7141+54773,259200)
      RandLim = Lo+(Hi-Lo+1)*Seed/259200
      RETURN
C
C    REAL FUNCTION SRand (Seed)
C    initializes either generator (Seed = 0. to 259199.)
C
      ENTRY SRand (X)
      SRand = X
      Seed = X
      RETURN
      END

      SUBROUTINE Reinitialize
C
C ============================== Reinitialize ================================
C   Restores the array SortArray to its original unsorted state while
C   displaying the unsorted color bars.
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
      INTEGER*1 DateTime(11)
      INTEGER*4 time0
      COMMON /misc/MaxBars,MaxColors,Sound,Pause
      COMMON /time/time0
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
C
      DO 100 row=1,MaxBars
        SortArray(BARLENGTH,row)=SortBackup(BARLENGTH,row)
        SortArray(BARCOLOR,row)=SortBackup(BARCOLOR,row)
        CALL PrintOneBar(row)
100   CONTINUE
      gbg = DosGetDateTime(DateTime)
      time0=DateTime(1)*360000+
     +      DateTime(2)*6000+
     +      DateTime(3)*100+
     +      DateTime(4)
      RETURN
      END

      SUBROUTINE ShellSort
C
C =============================== ShellSort ==================================
C  The ShellSort procedure is similar to the BubbleSort procedure.  However,
C  ShellSort begins by comparing elements that are far apart (separated by
C  the value of the Offset variable, which is initially half the distance
C  between the first and last element), then comparing elements that are
C  closer together (when Offset is one, the last iteration of this procedure
C  is merely a bubble sort).
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
      COMMON /misc/MaxBars,MaxColors,Sound,Pause
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
C
C Set comparison offset to half the number of records in SortArray:
C
      Offset = MaxBars / 2
1     CONTINUE
      Limit = MaxBars - Offset
2     CONTINUE
C Assume no switches at this offset.
      Switch = 0
C
C Compare elements and switch ones out of order:
      DO 100 Row=1,Limit
        IF(SortArray(BARLENGTH,Row).GT.
     +     SortArray(BARLENGTH,Row+Offset)) THEN
          CALL SwapSortArray(Row,Row+Offset)
          CALL SwapBars (Row, Row + Offset)
          Switch = Row
        ENDIF
100   CONTINUE
C Sort on next pass only to where last switch was made:
      Limit = Switch - Offset
      IF(Switch.GT.0) GO TO 2
C
C No switches at last offset, try one half as big:
C
      Offset = Offset / 2
      IF(Offset.GT.0) GO TO 1
      RETURN
      END

      SUBROUTINE SortMenu
C
C =============================== SortMenu ===================================
C   The SortMenu procedure first calls the Reinitialize procedure to make
C   sure the SortArray is in its unsorted form, then prompts the user to
C   make one of the following choices:
C
C               * One of the sorting algorithms
C               * Toggle sound on or off
C               * Increase or decrease speed
C               * End the program
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
      PARAMETER (FIRSTMENU=1,LEFT=48,NLINES=18,SPACE=32)
      CHARACTER inkey*1
      INTEGER*1 chardata(10)
      LOGICAL Sound
      COMMON /misc/MaxBars,MaxColors,Sound,Pause
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
C
C     Locate the cursor
C
      gbg = VioSetCurPos(FIRSTMENU + NLINES, 75, 0)
C
1     CONTINUE
      gbg = KbdCharIn(chardata,0,0)
      inkey=char(chardata(1))
      IF(lge(inkey,'a').AND.lle(inkey,'z'))
     +  inkey=char(ichar(inkey)-SPACE)
C
C        /* Branch to the appropriate procedure depending on the key typed: */
C
      IF(inkey.EQ.'I') THEN
        Select = 0
        CALL Reinitialize
        CALL InsertionSort
        CALL ElapsedTime(0)
      ELSEIF(inkey.EQ.'B') THEN
        Select = 1
        CALL Reinitialize
        CALL BubbleSort
        CALL ElapsedTime(0)
      ELSEIF(inkey.EQ.'H') THEN
        Select = 2
        CALL Reinitialize
        CALL HeapSort
        CALL ElapsedTime(0)
      ELSEIF(inkey.EQ.'E') THEN
        Select = 3
        CALL Reinitialize
        CALL ExchangeSort
        CALL ElapsedTime(0)
      ELSEIF(inkey.EQ.'S') THEN
        Select = 4
        CALL Reinitialize
        CALL ShellSort
        CALL ElapsedTime(0)
      ELSEIF(inkey.EQ.'Q') THEN
        Select = 5
        CALL Reinitialize
        CALL QuickSort (1, MaxBars)
        CALL ElapsedTime(0)
      ELSEIF(inkey.EQ.'T') THEN
C
C       Toggle the sound, then redraw the menu to clear any timing
C       results (since they won't compare with future results):
C
        Sound=.NOT.Sound
        CALL Boxinit
      ELSEIF(inkey.EQ.'<') THEN
C
C       Increase pause length to slow down sorting time, then redraw
C       the menu to clear any timing results (since they won't compare
C       with future results):
C
        IF(Pause.NE.900) THEN
          Pause = Pause + 30
          CALL BoxInit
        ENDIF
      ELSEIF(inkey.EQ.'>') THEN
C
C       Decrease pause length to speed up sorting time, then redraw
C       the menu to clear any timing results (since they won't compare
C       with future results):
C
        IF(Pause.NE.0) THEN
          Pause = Pause - 30
          CALL BoxInit
        ENDIF
      ELSEIF(inkey.EQ.char(27)) THEN
C
C       User pressed ESC, so return to main:
C
        RETURN
      ENDIF
      GO TO 1
      END

      SUBROUTINE SwapBars(Row1,Row2)
C
C =============================== SwapBars ===================================
C   Calls PrintOneBar twice to switch the two bars in Row1 and Row2,
C   then calls the ElapsedTime procedure.
C ============================================================================
C
      IMPLICIT INTEGER*2(a-z)
C
      CALL PrintOneBar(Row1)
      CALL PrintOneBar (Row2)
      CALL ElapsedTime (Row1)
C
      RETURN
      END

      SUBROUTINE SwapSortArray(i,j)
      IMPLICIT INTEGER*2(a-z)
      COMMON SortArray(2,43),SortBackup(2,43),BARLENGTH,BARCOLOR,Select
      temp=SortArray(1,i)
      SortArray(1,i)=SortArray(1,j)
      SortArray(1,j)=temp
      temp=SortArray(2,i)
      SortArray(2,i)=SortArray(2,j)
      SortArray(2,j)=temp
      RETURN
      END

      SUBROUTINE cls
      IMPLICIT INTEGER*2(a-z)
      gbg = VioScrollDn(0, 0, -1, -1, -1, 16#720, 0)
      RETURN
      END
