{	    Mouse Function Calls and Microsoft Pascal	    }
{							    }
{	    To Run :	PAS1 PASEXAMP;			    }
{			PAS2				    }
{			LINK PASEXAMP+SUBS,,,MOUSE;	    }
{			     or 			    }
{			MAKE PASEXAMP.MAK		    }
{							    }

program mtest (output);

procedure mouses(vars m1, m2, m3, m4:word);extern;
procedure chkdrv;extern;
procedure graf;extern;

var
  m1, m2, m3, m4: word;

begin {demo}

  chkdrv;			{check mouse driver, if not }
				{installed, Exit.	    }
  m1:=0;			{installed, initialize mouse}
  mouses( m1, m2, m3, m4);
  if ( m1 = 0 ) then
     writeln('Microsoft Mouse NOT found')
  else
     begin

       m1 := 4; 		{function call 4, set mouse }
       m3 := 200;		{horizontal position	    }
       m4 := 100;		{vertical position	    }
       mouses( m1, m2, m3, m4 );


       m1 := 7; 		{function call 4, set mouse }
       m3 := 150;		{minimum horizontal position}
       m4 := 450;		{maximum horizontal position}
       mouses( m1, m2, m3, m4 );


       m1 := 8; 		{function call 4, set mouse }
       m3 := 50;		{minimum vertical position  }
       m4 := 150;		{maximum vertical position  }
       mouses( m1, m2, m3, m4 );

       graf;			{change into graphics mode  }

       writeln('Graphics cursor limited to center of the screen.');
       writeln('Press the left mouse button to EXIT.');

       m1:=1;			{function call 1	    }
       mouses( m1, m2, m3, m4 );{show mouse cursor	    }

       m2 := 999;		{dummy value for loop	    }
       repeat			{until ....		    }
	 m1 := 3;		   {function call 3	    }
	 mouses( m1, m2, m3, m4 ); {get current mouse status}
       until m2 = 1;		{left mouse button pressed  }

     end

end. {demo}
