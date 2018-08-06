program mtest(output);

procedure mouses(vars m1,m2,m3,m4:word);extern;
procedure chkdrv;extern;
procedure graf;extern;

var
  m1,m2,m3,m4:word;
  Cursor : array [0..31]of word;
  bound  : array [0..3] of word;
  ptradd : array [1..2] of word;
  i, j : integer;

begin

  for i := 0 to 15  do cursor[i] := 16#ffff;
  Cursor[16] := 16#8000;
  Cursor[17] := 16#E000;
  Cursor[18] := 16#F800;
  Cursor[19] := 16#FE00;  {initialize curosr array}
  Cursor[20] := 16#D800;
  Cursor[21] := 16#0C00;
  Cursor[22] := 16#0600;
  Cursor[23] := 16#0300;
  for j := 24 to 31 do Cursor[j] := 16#0000;

  chkdrv;			    {Check for mouse	 }
				    { driver installation}
  m1:=0;			    {Function call 0	 }
  mouses(m1,m2,m3,m4);		    {initialize mouse	 }
  if ( m1 = 0 ) then		    {No, output message  }
     writeln('Microsoft mouse NOT found')
  else
     begin			    {Yes, demo function 9}
				    { and funciton 16	 }
       graf;			    {set to graphics mode}

       m1:=9;			    {Function call 9	 }
       m2:=1;			    { set graphics cursor}
       m3:=1;
       ptradd[1] := (ads Cursor).r; {offset  of the array}
       ptradd[2] := (ads Cursor).s; {segment of the array}
       mouses(m1,m2,m3,ptradd[1]);

       writeln('Mouse cursor will disappear within this area.');
       writeln('Press the right mouse button to EXIT.........');

       m1:=1;			    {Function call 1	 }
       mouses(m1,m2,m3,m4);	    { show mouse cursor  }

       m1 := 16;		    {Function call 16	 }
       bound[0] := 0;		    {left  x coordinate  }
       bound[1] := 0;		    {upper y coordinate  }
       bound[2] := 390; 	    {right x coordinate  }
       bound[3] := 25;		    {lower y coordinate  }
       ptradd[1] := (ads bound).r;  {offset  of the array}
       ptradd[2] := (ads bound).s;  {segment of the array}
       mouses(m1,m2,m3,ptradd[1]);

       m2 := 999;		    {dummy value for loop}
       repeat			    {until .		 }
	 m1 := 3;		    {Function call 3, get}
	 mouses( m1, m2, m3, m4 );  {current mouse status}
       until m2 = 2;		    {left button pressed }

     end
end.

