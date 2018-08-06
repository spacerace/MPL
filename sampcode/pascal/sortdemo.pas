(*				   SORTDEMO
 * This	program	graphically demonstrates six common sorting algorithms.	 It
 * prints 25 or	43 horizontal bars, all	of different lengths and all in	random
 * order, then sorts the bars from smallest to longest.
 *
 * The program also uses sound statements to generate different pitches,
 * depending on the location of the bar being printed. Note that the sound
 * statements delay the	speed of each sorting algorithm	so you can follow
 * the progress	of the sort. Therefore,	the times shown	are for	comparison
 * only. They are not an accurate measure of sort speed.
 *
 * If you use these sorting routines in	your own programs, you may notice
 * a difference	in their relative speeds (for example, the exchange
 * sort	may be faster than the shell sort) depending on	the number of
 * elements to be sorted and how "scrambled" they are to begin with.
 *)

PROGRAM	SortDemo;

CONST
    INCL_SUB           = 1;	   (* Include KBD, VIO, and MOU definitions *)
    INCL_DOSDATETIME   = 1;	   (* DOS date/time definitions	*)
    INCL_DOSPROCESS    = 1;	   (* Some DOS process definitions *)
    INCL_NOCOM	       = 1;	   (* Don't include default sections of DOS *)

(*
 * Define the data type used to hold the information for each colored bar:
 *)
TYPE
    SortType = RECORD
		Length: BYTE;	(* Bar length (the element compared
				 * in the different sorts)	       *)
		ColorVal: BYTE;	(* Bar color			       *)
	       END;

    CELLINFO = RECORD
	    Char: BYTE;
	    Attr: BYTE;
	   END;

(* Declare global constants:
 *)
CONST
    BLOCK = 223;
    ESC	= CHR(27);
    FIRSTMENU =	1;
    LEFTCOLUMN = 48;
    NLINES = 18;
    NULL = 0;
    SPACE = 32;
    WIDTH = 80 - LEFTCOLUMN;
    WHITE = 15;

(* Declare global variables, and allocate storage space	for them.  SortArray
 * and SortBackup are both arrays of the data type SortType defined above:
 *)
VAR
    sTime,wTime: _DATETIME;
    KeyInfo: _KBDKEYINFO;
    wMode: _VIOMODEINFO;
    SortArray, SortBackup: ARRAY[1..43]	OF SortType;
    Menu: ARRAY[1..NLINES] OF LSTRING(30);
    Sound: BOOLEAN;
    curSelect, MaxBars,	MaxColors: INTEGER;
    oTime, nTime, Pause, RandSeed: INTEGER4;
    ret: WORD;

(* Data	statements for the different options printed in	the sort menu:
 *)

VALUE
    Menu[1] := '     PASCAL Sorting Demo';
    Menu[2] := ' ';
    Menu[3] := 'Insertion';
    Menu[4] := 'Bubble';
    Menu[5] := 'Heap';
    Menu[6] := 'Exchange';
    Menu[7] := 'Shell';
    Menu[8] := 'Quick';
    Menu[9] := ' ';
    Menu[10] := 'Toggle Sound: ';
    Menu[11] := ' ';
    Menu[12] := 'Pause Factor: ';
    Menu[13] := '<   (Slower)';
    Menu[14] := '>   (Faster)';
    Menu[15] := ' ';
    Menu[16] := 'Type first character of';
    Menu[17] := 'choice ( I B H E S Q T < > )';
    Menu[18] := 'or ESC key to end program: ';
    wMode.cb :=	SIZEOF(wMode);

FUNCTION GETMQQ	(Wants:WORD):ADSMEM; EXTERN;
FUNCTION RandInt (Lower,Upper:INTEGER):INTEGER;	FORWARD;
PROCEDURE BoxInit; FORWARD;
PROCEDURE BubbleSort; FORWARD;
PROCEDURE DrawFrame (Top,Left,Width,Height:INTEGER); FORWARD;
PROCEDURE ElapsedTime (CurrentRow:INTEGER); FORWARD;
PROCEDURE ExchangeSort;	FORWARD;
PROCEDURE HeapSort; FORWARD;
PROCEDURE Initialize; FORWARD;
PROCEDURE InsertionSort; FORWARD;
PROCEDURE PercolateDown	(MaxLevel:INTEGER); FORWARD;
PROCEDURE PercolateUp (MaxLevel:INTEGER); FORWARD;
PROCEDURE PrintOneBar (Row:INTEGER); FORWARD;
PROCEDURE QuickSort (Low,High:INTEGER);	FORWARD;
PROCEDURE Reinitialize;	FORWARD;
PROCEDURE ShellSort; FORWARD;
FUNCTION Screen	(ACTION:BYTE):BOOLEAN; FORWARD;
PROCEDURE SortMenu; FORWARD;
PROCEDURE SwapBars (Row1,Row2:INTEGER);	FORWARD;
PROCEDURE cls; FORWARD;
PROCEDURE swaps	(VAR one, two:SortType); FORWARD;

(* =============================== BoxInit ====================================
 *    Calls the	DrawFrame procedure to draw the	frame around the sort menu,
 *    then prints the different	options	stored in the Menu array.
 * ============================================================================
 *)
PROCEDURE BoxInit;
VAR
    Color: BYTE;
    i: INTEGER;
    Factor: LSTRING(3);

BEGIN
    Color := WHITE;
    DrawFrame(1, LEFTCOLUMN - 3, WIDTH + 3, 22);

    FOR i := 1 TO NLINES DO
        ret := VioWrtCharStrAtt(ads Menu[i,1], Menu[i].len,
			       FIRSTMENU + i, LEFTCOLUMN, Color, 0);

   (* Print the	current	value for Sound:
    *)
    IF (Sound) THEN
        ret := VioWrtCharStrAtt(ads 'ON ',3, 11, LEFTCOLUMN + 14, Color, 0)
    ELSE
        ret := VioWrtCharStrAtt(ads 'OFF',3, 11, LEFTCOLUMN + 14, Color, 0);

    EVAL(ENCODE(Factor,Pause DIV 30:3));
    ret := VioWrtCharStrAtt(ads Factor[1], 3, 13, LEFTCOLUMN + 14, Color, 0);

   (* Erase the	speed option if	the length of the Pause	is at a	limit
    *)
    IF (Pause =	900) THEN
        ret := VioWrtCharStrAtt(ads '            ',12,14,LEFTCOLUMN,Color,0)
    ELSE IF (Pause = 0)	THEN
        ret := VioWrtCharStrAtt(ads '            ',12,15,LEFTCOLUMN,Color,0);

END;

(* ============================== BubbleSort ==================================
 *    The BubbleSort algorithm cycles through SortArray, comparing adjacent
 *    elements and swapping pairs that are out of order.  It continues to
 *    do this until no pairs are swapped.
 * ============================================================================
 *)
PROCEDURE BubbleSort;
VAR
    Row, Switch, Limit:	INTEGER;

BEGIN
    Limit := MaxBars;
    REPEAT
	Switch := 0;
	FOR Row	:= 1 TO	Limit -	1 DO BEGIN

	   (* Two adjacent elements are	out of order, so swap their values
	    * and redraw those two bars: *)
	    IF (SortArray[Row].Length >	SortArray[Row +	1].Length) THEN	BEGIN
		swaps (SortArray[Row], SortArray[Row + 1]);
		SwapBars (Row, Row + 1);
		Switch := Row;
	    END;
	END;

    (* Sort on next pass only to where the last	switch was made: *)
    Limit := Switch;
    UNTIL Switch = 0;
END;

(* ============================== DrawFrame ===================================
 *   Draws a rectangular frame using the high-order ASCII characters � (201) ,
 *   � (187) , � (200) , � (188) , � (186) , and � (205). The parameters
 *   TopSide, BottomSide, LeftSide, and	RightSide are the row and column
 *   arguments for the upper-left and lower-right corners of the frame.
 * ============================================================================
 *)
PROCEDURE DrawFrame {(Top, Left, Width,	Height)};
CONST
    ULEFT = 201;
    URIGHT = 187;
    LLEFT = 200;
    LRIGHT = 188;
    VERTICAL = 186;
    HORIZONTAL = 205;
    SPACE = ' ';

VAR
    Attr: BYTE;
    CellAttr, i, bottom, right:	INTEGER;
    TempStr: STRING(80);

BEGIN
      Attr := WHITE;
      CellAttr := Attr * 256;
      bottom :=	Top+Height-1;
      right := Left+Width-1;

      ret := VioWrtNCell(ads (CellAttr OR ULEFT),1,Top,Left,0);
      ret := VioWrtNCell(ads (CellAttr OR HORIZONTAL),Width-2,Top,Left+1,0);
      ret := VioWrtNCell(ads (CellAttr OR URIGHT),1,Top,right,0);

      FILLSC(ads Tempstr,Width,CHR(SPACE));
      Tempstr[1] := CHR(VERTICAL);
      Tempstr[Width] :=	CHR(VERTICAL);
      FOR i := 1 TO Height-2 DO
          ret := VioWrtCharStrAtt(ads Tempstr,Width,i+Top,Left,Attr,0);

      ret := VioWrtNCell(ads (CellAttr OR LLEFT),1,bottom,Left,0);
      ret := VioWrtNCell(ads (CellAttr OR HORIZONTAL),Width-2,bottom,Left+1,0);
      ret := VioWrtNCell(ads (CellAttr OR LRIGHT),1,bottom,right,0);
END;

(* ============================= ElapsedTime ==================================
 *    Prints seconds elapsed since the given sorting routine started.
 *    Note that	this time includes both	the time it takes to redraw the
 *    bars plus	the pause while	the SOUND statement plays a note, and
 *    thus is not an accurate indication of sorting speed.
 * ============================================================================
 *)
PROCEDURE ElapsedTime {(CurrentRow)};
VAR
    Color: BYTE;
    Timing: LSTRING(80);

BEGIN
    Color := WHITE;

    ret := DosGetDateTime(ads wTime);

    nTime := (wTime.hours * 360000) +
	    (wTime.minutes * 6000) +
	    (wTime.seconds * 100) +
	     wTime.hundredths;

    EVAL(ENCODE(Timing,(nTime -	oTime) / 100:7:2));

    (* Print the number	of seconds elapsed *)
    ret := VioWrtCharStrAtt(ads Timing[1], 7, curSelect + FIRSTMENU + 3,
	LEFTCOLUMN + 15, Color,	0);

    IF (Sound) THEN
        ret := DosBeep(60 * CurrentRow, 32);    (* Play a note. *)
    ret := DosSleep(Pause);                     (* Pause. *)

END;

(* ============================= ExchangeSort =================================
 *   The ExchangeSort compares each element in SortArray - starting with
 *   the first element - with every following element.	If any of the
 *   following elements	is smaller than	the current element, it	is exchanged
 *   with the current element and the process is repeated for the next
 *   element in	SortArray.
 * ============================================================================
 *)
PROCEDURE ExchangeSort;
VAR
    Row, SmallestRow, j: INTEGER;

BEGIN
    FOR	Row := 1 TO MaxBars - 1	DO BEGIN
	SmallestRow := Row;
	FOR j := Row + 1 TO MaxBars DO BEGIN
	    IF (SortArray[j].Length < SortArray[SmallestRow].Length) THEN BEGIN
		SmallestRow := j;
		ElapsedTime(j);
	    END;
	END;
       (* Found	a row shorter than the current row, so swap those
	* two array elements: *)
	IF (SmallestRow	> Row) THEN BEGIN
	    swaps (SortArray[Row], SortArray[SmallestRow]);
	    SwapBars (Row, SmallestRow);
	END;
    END;

END;

(* =============================== HeapSort ==================================
 *  The	HeapSort procedure works by calling two	other procedures - PercolateUp
 *  and	PercolateDown.	PercolateUp turns SortArray into a "heap," which has
 *  the	properties outlined in the diagram below:
 *
 *				 SortArray(1)
 *				 /	    \
 *		      SortArray(2)	     SortArray(3)
 *		     /		\	     /		\
 *	   SortArray(4)	  SortArray(5)	 SortArray(6)  SortArray(7)
 *	    /	   \	   /	   \	   /	  \	 /	\
 *	  ...	   ...	 ...	   ...	 ...	  ...  ...	...
 *
 *
 *  where each "parent node" is	greater	than each of its "child nodes";	for
 *  example, SortArray(1) is greater than SortArray(2) or SortArray(3),
 *  SortArray(3) is greater than SortArray(6) or SortArray(7), and so forth.
 *
 *  Therefore, once the	first FOR...NEXT loop in HeapSort is finished, the
 *  largest element is in SortArray(1).
 *
 *  The	second FOR...NEXT loop in HeapSort swaps the element in	SortArray(1)
 *  with the element in	MaxRow,	rebuilds the heap (with	PercolateDown) for
 *  MaxRow - 1,	then swaps the element in SortArray(1) with the	element	in
 *  MaxRow - 1,	rebuilds the heap for MaxRow - 2, and continues	in this	way
 *  until the array is sorted.
 * ===========================================================================
 *)
PROCEDURE HeapSort;
VAR
    i: INTEGER;

BEGIN
    FOR	i := 2 TO MaxBars DO
	PercolateUp (i);

    FOR	i := MaxBars DOWNTO 2 DO BEGIN
	swaps (SortArray[1], SortArray[i]);
	SwapBars (1, i);
	PercolateDown (i - 1);
    END;
END;

(* ============================== Initialize =================================
 *    Initializes the SortBackup array.	 It also calls the BoxInit procedure.
 * ===========================================================================
 *)
PROCEDURE Initialize;
VAR
    iTime: _DATETIME;
    i, MaxIndex, Index,	BarLength: INTEGER;
    TempArray: ARRAY [1..43] OF	INTEGER;

BEGIN
    FOR	i := 1 TO MaxBars DO
        TempArray[i] := i;

   (* If monochrome or color burst disabled, use one color *)
    IF (((wMode.fbType AND VGMT_OTHER) <> 0) AND
        ((wMode.fbType AND VGMT_DISABLEBURST) = 0))
        MaxColors := 15;
    ELSE
        MaxColors := 1;

   (* Seed the random-number generator. *)
    ret := DosGetDateTime(ads iTime);
    RandSeed :=	(iTime.hours * 3600) +
		(iTime.minutes * 60) +
		 iTime.seconds;
    RandSeed :=	TRUNC4(RandSeed	/ 86400.0 * 259199.0);

    MaxIndex :=	MaxBars;
    FOR	i := 1 TO MaxBars DO BEGIN

       (* Find a random	element	in TempArray between 1 and MaxIndex,
	* then assign the value	in that	element	to BarLength: *)
	Index := RandInt(1,MaxIndex);
	BarLength := TempArray[Index];

       (* Overwrite the	value in TempArray[Index] with the value in
	* TempArray[MaxIndex] so the value in TempArray[Index] is
	* chosen only once: *)
	TempArray[Index] := TempArray[MaxIndex];

       (* Decrease the value of	MaxIndex so that TempArray[MaxIndex] can't
	* be chosen on the next	pass through the loop: *)
	MaxIndex := MaxIndex - 1;

	SortBackup[i].Length :=	BarLength;

        IF (MaxColors = 1) THEN
            SortBackup[i].ColorVal := 7;
        ELSE
            SortBackup[i].ColorVal := (BarLength MOD MaxColors) + 1;
    END;

    cls;
    Reinitialize;      (* Assign values	in SortBackup to SortArray and draw *)
			 (* unsorted bars on the screen. *)
    Sound := TRUE;
    Pause := 30;	   (* Initialize Pause.	*)
    BoxInit;	       (* Draw frame for the sort menu and print options. *)

END;

(* ============================= InsertionSort ===============================
 *   The InsertionSort procedure compares the length of	each successive
 *   element in	SortArray with the lengths of all the preceding	elements.
 *   When the procedure	finds the appropriate place for	the new	element, it
 *   inserts the element in its	new place, and moves all the other elements
 *   down one place.
 * ===========================================================================
 *)
PROCEDURE InsertionSort;
VAR
    j, Row, TempLength:	INTEGER;
    TempVal: SortType;

BEGIN
    FOR	Row := 2 TO MaxBars DO BEGIN
	TempVal	:= SortArray[Row];
	TempLength := TempVal.Length;
	FOR j := Row DOWNTO 2 DO BEGIN

	   (* As long as the length of the j-1st element is greater than the
	    * length of	the original element in	SortArray(Row),	keep shifting
	    * the array	elements down: *)
	    IF (SortArray[j - 1].Length	> TempLength) THEN BEGIN
		SortArray[j] :=	SortArray[j - 1];
		PrintOneBar(j);		    (* Print the new bar. *)
		ElapsedTime(j);		    (* Print the elapsed time. *)

	     (*	Otherwise, exit: *)
	     END
             ELSE
		break;
	END;

	(* Insert the original value of	SortArray(Row) in SortArray(j):	*)
	SortArray[j] :=	TempVal;
	PrintOneBar(j);
	ElapsedTime(j);
    END;
END;

(* ============================	PercolateDown ================================
 *   The PercolateDown procedure restores the elements of SortArray from 1 to
 *   MaxLevel to a "heap" (see the diagram with	the HeapSort procedure).
 * ===========================================================================
 *)
PROCEDURE PercolateDown	{(MaxLevel)};
VAR
    i, Child: INTEGER;

BEGIN
    i := 1;
   (* Move the value in	SortArray(1) down the heap until it has	reached
    * its proper node (that is,	until it is less than its parent node
    * or until it has reached MaxLevel,	the bottom of the current heap): *)
    WHILE TRUE DO BEGIN
	Child := 2 * i;		   (* Get the subscript	for the	child node. *)

	(* Reached the bottom of the heap, so exit this	procedure: *)
	IF (Child > MaxLevel) THEN
	    break;

	(* If there are	two child nodes, find out which	one is bigger: *)
	IF (Child + 1 <= MaxLevel) THEN
	    IF (SortArray[Child	+ 1].Length > SortArray[Child].Length) THEN
		Child := Child+1;

       (* Move the value down if it is still not bigger	than either one	of
	* its children:	*)
	IF (SortArray[i].Length	< SortArray[Child].Length) THEN	BEGIN
	    swaps (SortArray[i], SortArray[Child]);
	    SwapBars (i, Child);
	    i := Child;

       (* Otherwise, SortArray has been	restored to a heap from	1 to
	* MaxLevel, so exit: *)
	END
        ELSE
	    break;
    END;
END;

(* ============================== PercolateUp ================================
 *   The PercolateUp procedure converts	the elements from 1 to MaxLevel	in
 *   SortArray into a "heap" (see the diagram with the HeapSort	procedure).
 * ===========================================================================
 *)
PROCEDURE PercolateUp {(MaxLevel)};
VAR
    i, Parent: INTEGER;

BEGIN
    i := MaxLevel;
   (* Move the value in	SortArray(MaxLevel) up the heap	until it has
    * reached its proper node (that is,	until it is greater than either
    * of its child nodes, or until it has reached 1, the top of	the heap): *)
    WHILE (i <>	1) DO BEGIN
	Parent := i DIV	2;	     (*	Get the	subscript for the parent node. *)

       (* The value at the current node	is still bigger	than the value at
	* its parent node, so swap these two array elements: *)
	IF (SortArray[i].Length	> SortArray[Parent].Length) THEN BEGIN
	    swaps (SortArray[Parent], SortArray[i]);
	    SwapBars (Parent, i);
	    i := Parent;

       (* Otherwise, the element has reached its proper	place in the heap,
	* so exit this procedure: *)
	END
	ELSE
	    break;
    END;
END;

(* ============================== PrintOneBar ================================
 *  Prints SortArray(Row).BarString at the row indicated by the	Row
 *  parameter, using the color in SortArray(Row).ColorVal.
 * ===========================================================================
 *)
PROCEDURE PrintOneBar {(Row)};
VAR
    Cell: CELLINFO;
    NumSpaces: INTEGER;

BEGIN
    Cell.Attr := SortArray[Row].ColorVal;
    Cell.Char := BLOCK;
    ret := VioWrtNCell(ads Cell,SortArray[Row].Length,Row,1,0);
    NumSpaces := MaxBars - SortArray[Row].Length;
    IF NumSpaces > 0 THEN
	Cell.Char := SPACE;
        ret := VioWrtNCell(ads Cell,NumSpaces,Row,SortArray[Row].Length+1,0);
END;

(* ============================== QuickSort ==================================
 *   QuickSort works by	picking	a random "pivot" element in SortArray, then
 *   moving every element that is bigger to one	side of	the pivot, and every
 *   element that is smaller to	the other side.	 QuickSort is then called
 *   recursively with the two subdivisions created by the pivot.  Once the
 *   number of elements	in a subdivision reaches two, the recursive calls end
 *   and the array is sorted.
 * ===========================================================================
 *)
PROCEDURE QuickSort {(Low, High)};
VAR
    i, j, RandIndex, Partition:	INTEGER;

BEGIN
    IF (Low < High) THEN BEGIN

       (* Only two elements in this subdivision; swap them if they are out of
	* order, then end recursive calls: *)
	IF ((High - Low) = 1) THEN BEGIN
	    IF (SortArray[Low].Length >	SortArray[High].Length)	THEN BEGIN
		swaps (SortArray[Low], SortArray[High]);
		SwapBars (Low, High);
	    END;
	END
        ELSE BEGIN
	    Partition := SortArray[High].Length;
	    i := Low;
	    j := High;
	    WHILE i < j	DO BEGIN

		(* Move	in from	both sides towards the pivot element: *)
		WHILE ((i < j) AND (SortArray[i].Length	<= Partition)) DO
		    i := i + 1;

		WHILE ((j > i) AND (SortArray[j].Length	>= Partition)) DO
		    j := j - 1;

	       (* If we	haven't reached the pivot element, it means that two
		* elements on either side are out of order, so swap them: *)
		IF (i <	j) THEN	BEGIN
		    swaps (SortArray[i], SortArray[j]);
		    SwapBars (i, j);
		END;
	    END;

	   (* Move the pivot element back to its proper	place in the array: *)
	    swaps (SortArray[i], SortArray[High]);
	    SwapBars (i, High);

	   (* Recursively call the QuickSort procedure (pass the smaller
	    * subdivision first	to use less stack space): *)
	    IF ((i - Low) < (High - i))	THEN BEGIN
		QuickSort (Low,	i - 1);
		QuickSort (i + 1, High);
	    END
	    ELSE BEGIN
		QuickSort (i + 1, High);
		QuickSort (Low,	i - 1);
	    END;
	END;
    END;
END;

(* =============================== RandInt ===================================
 *   Returns a random integer greater than or equal to the Lower parameter
 *   and less than or equal to the Upper parameter.
 * ===========================================================================
 *)
FUNCTION RandInt {(Lower, Upper)};
BEGIN
    RandSeed :=	(RandSeed*7141+54773) MOD 259200;
    RandInt := ORD(Lower + ((Upper - Lower + 1)	* RandSeed) DIV	259200);
END;

(* ============================== Reinitialize ===============================
 *   Restores the array	SortArray to its original unsorted state, then
 *   prints the	unsorted color bars.
 * ===========================================================================
 *)
PROCEDURE Reinitialize;
VAR
    Row: INTEGER;
BEGIN
    FOR	Row := 1 TO MaxBars DO BEGIN
	SortArray[Row] := SortBackup[Row];
	PrintOneBar(Row);
    END;

    ret := DosGetDateTime(ads sTime);
    oTime := (sTime.hours * 360000) +
	    (sTime.minutes * 6000) +
	    (sTime.seconds * 100) +
	     sTime.hundredths;
END;

FUNCTION Screen	{(ACTION)};
VAR [STATIC]
    Mode: _VIOMODEINFO;
    CellStr: ADSMEM;
    Row,Col,Length: WORD;
BEGIN
    if(ACTION=1) THEN BEGIN
	Mode.cb	:= sizeof(Mode);
        ret := VioGetMode(ads Mode,0);
	Length := 2*Mode.row*Mode.col;
	CellStr	:= GETMQQ(Length);
	if(CellStr.r = NULL) THEN BEGIN	Screen := FALSE;return;END;
        ret := VioReadCellStr(CellStr,Length,0,0,0);
        ret := VioGetCurPos(Row,Col,0);
    END
    ELSE BEGIN
        ret := VioSetMode(ads Mode,0);
	if(CellStr.r = NULL) THEN BEGIN	Screen := FALSE;return;END;
        ret := VioWrtCellStr(CellStr,Length,0,0,0);
        ret := VioSetCurPos(Row,Col,0);
    END;
    Screen := TRUE;
END;

(* =============================== ShellSort =================================
 *  The	ShellSort procedure is similar to the BubbleSort procedure.  However,
 *  ShellSort begins by	comparing elements that	are far	apart (separated by
 *  the	value of the Offset variable, which is initially half the distance
 *  between the	first and last element), then comparing	elements that are
 *  closer together (when Offset is one, the last iteration of this procedure
 *  is merely a	bubble sort).
 * ===========================================================================
 *)
PROCEDURE ShellSort;
VAR
    Offset, Switch, Limit, Row:	INTEGER;

BEGIN
    (* Set comparison offset to	half the number	of records in SortArray: *)
    Offset := MaxBars DIV 2;

    WHILE (Offset>0) DO	BEGIN	  (* Loop until	offset gets to zero. *)
	Limit := MaxBars - Offset;
	REPEAT
	    Switch := 0;	(* Assume no switches at this offset. *)

	 (* Compare elements and switch	ones out of order: *)
	    FOR	Row := 1 TO Limit DO
		IF (SortArray[Row].Length > SortArray[Row + Offset].Length) THEN BEGIN
		    swaps (SortArray[Row], SortArray[Row + Offset]);
		    SwapBars (Row, Row + Offset);
		    Switch := Row;
		END;

	    (* Sort on next pass only to where last switch was made: *)
	    Limit := Switch - Offset;
	UNTIL Switch = 0;

       (* No switches at last offset, try one half as big: *)
	Offset := Offset DIV 2;
    END;
END;

(* =============================== SortMenu ==================================
 *   The SortMenu procedure first calls	the Reinitialize procedure to make
 *   sure the SortArray	is in its unsorted form, then prompts the user to
 *   make one of the following choices:
 *
 *		 * One of the sorting algorithms
 *		 * Toggle sound	on or off
 *		 * Increase or decrease	speed
 *		 * End the program
 * ===========================================================================
 *)
PROCEDURE SortMenu;
BEGIN
    WHILE TRUE DO BEGIN

        ret := VioSetCurPos(FIRSTMENU + NLINES, LEFTCOLUMN + Menu[NLINES].len, 0);

        ret := KbdCharIn(ads KeyInfo, 0, 0);
        IF (CHR(KeyInfo.chChar) >= 'a') AND (CHR(KeyInfo.chChar) <= 'z') THEN
	    KeyInfo.chChar := KeyInfo.chChar - 32;

	(* Branch to the appropriate procedure depending on the	key typed: *)
	CASE CHR(KeyInfo.chChar) OF

	    'I': BEGIN
		    curSelect := 0;
		    Reinitialize;
		    InsertionSort;
		    ElapsedTime(0);	(* Print final time. *)
		 END;

	    'B': BEGIN
		    curSelect := 1;
		    Reinitialize;
		    BubbleSort;
		    ElapsedTime(0);	(* Print final time. *)
		 END;

	    'H': BEGIN
		    curSelect := 2;
		    Reinitialize;
		    HeapSort;
		    ElapsedTime(0);	(* Print final time. *)
		 END;

	    'E': BEGIN
		    curSelect := 3;
		    Reinitialize;
		    ExchangeSort;
		    ElapsedTime(0);	(* Print final time. *)
		 END;

	    'S': BEGIN
		    curSelect := 4;
		    Reinitialize;
		    ShellSort;
		    ElapsedTime(0);	(* Print final time. *)
		 END;

	    'Q': BEGIN
		    curSelect := 5;
		    Reinitialize;
		    QuickSort (1, MaxBars);
		    ElapsedTime(0);	(* Print final time. *)
		 END;

	    '>': BEGIN
		   (* Decrease pause length to speed up	sorting	time,
		    * then redraw the menu to clear any	timing results
		    * (since they won't compare with future results): *)
                    IF (Pause <> 0) THEN
                        Pause := Pause - 30;
		    BoxInit;
		 END;

	    '<': BEGIN
		   (* Increase pause length to slow down sorting time,
		    * then redraw the menu to clear any	timing results
		    * (since they won't compare with future results): *)
                    IF (Pause <> 900) THEN
                        Pause := Pause + 30;
		    BoxInit;
		 END;

	    'T': BEGIN
		    Sound := NOT Sound;
		    BoxInit;
		 END;

	    ESC: return; (* User pressed ESC, so exit and return to main: *)

	    OTHERWISE

	END;
    END;
END;

(* =============================== SwapBars ==================================
 *   Calls PrintOneBar twice to	switch the two bars in Row1 and	Row2,
 *   then calls	the ElapsedTime	procedure.
 * ===========================================================================
 *)
PROCEDURE SwapBars {(Row1, Row2)};
BEGIN
    PrintOneBar	(Row1);
    PrintOneBar	(Row2);
    ElapsedTime	(Row1);
END;

PROCEDURE cls;
BEGIN
    ret := VioScrollDn (0, 0, -1, -1, -1, ads 1824, 0);
END;

PROCEDURE swaps	{(one, two)};
VAR
    temp: SortType;

BEGIN
    temp := one;
    one	:= two;
    two	:= temp;

END;

(* Main	program	*)
BEGIN
    if(NOT Screen(1)) THEN cls;
    ret := VioGetMode(ads wMode,0);
    IF (wMode.row <> 43) THEN BEGIN     (* Use 43-line mode if available *)
        wMode.row := 43;
        wmode.hres := 640;              (* Try EGA *)
        wmode.vres := 350;
        IF (VioSetMode(ads wMode,0) <> 0) THEN BEGIN
            wmode.hres := 720;          (* Try VGA *)
            wmode.vres := 400;
            IF (VioSetMode(ads wMode,0) <> 0) THEN BEGIN
                ret = VioGetMode(ads wMode,0)
                wMode.row := 25;        (* Use 25 lines *)
                ret = VioSetMode(ads wMode,0)
            END
        END;
    END;
    MaxBars := ORD(wMode.row);
    Initialize;		 (* Initialize data values. *)
    SortMenu;		 (* Print sort menu. *)
    if(NOT Screen(0)) THEN cls;
END.
