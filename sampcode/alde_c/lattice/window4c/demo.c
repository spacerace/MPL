#define	foreground(A)	((color_mon==1)?(A):(7))


	int	u_row[14]={2,2,2,2,2,8,14,20,20,20,20,20,14,8};
	int	u_col[14]={6,21,36,51,66,66,66,66,51,36,21,6,6,6};
	int	l_row[14]={6,6,6,6,6,12,18,24,24,24,24,24,18,12};
	int	l_col[14]={15,30,45,60,75,75,75,75,60,45,30,15,15,15};
	int	wind[14];

	int	color_mon=0;


main()

{

	int	temp,key,i,j,k,l;

	color(7,0);

	temp=open_window(9,15,13,61);

	locate(11,20);printf("Do you have a color monitor (Y/N) [ ]");
	locate(11,55);
	putch(key=getch());
	if(key=='Y' || key=='y')color_mon=1;

	close_window(temp);

	color(foreground(1),0);
	temp=open_window(3,5,23,75);
	color_window(temp,foreground(14),0);
	cls_window(temp);
	print_window(temp,"ึฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ ฑฑฑฑÛÛÛÛÛ\n");
	print_window(temp,"บ                            ฐฐฐฐฐฐ              ÛÛÛÛÛÛÛÜ\n");
	print_window(temp,"บ    ÛÛ   Û   ÛÛ             ฐ____               ÛÛÛÛOÛÛÛÛ\n");
	print_window(temp,"บ     ÛÛ ÛÛÛ ÛÛ                   ฐ              ฿฿฿฿฿฿฿฿฿\n");
	print_window(temp,"บ      ÛÛ   ÛÛ   I N D O W   ฐฐฐฐฐฐ   O F T        ณ\n");
	print_window(temp,"บ                                                  ณ\n");
	print_window(temp,"ศออออออออออัออออออออออออออออออออออออออัออออออออออออพ\n");
	print_window(temp,"           ณ   Fast - Small - Easy &  ณ   ษออออออออออออออออออออป\n");
	print_window(temp,"           ณ        C H E A P         ณ   บ  Glen Boyd         บ\n");
	print_window(temp,"     ษอออออณ                          ณอออน  21606 Criptana    บ\n");
	print_window(temp,"     บ     ณ  W I N D O W S  For  'C' ณ   บ  Mission Viejo, CA บ\n"); 
	print_window(temp,"     บ     ณ                          ณ   บ         92692      บ\n");
	print_window(temp,"     บ     ิออออออออออออออออออออออออออพ   ศออหอออออออออออออออออผ\n");
	print_window(temp,"     บ                                       บ\n");
	print_window(temp,"     บ If you use these functions and find   บ\n");
	print_window(temp,"     บ them of value, a contribution of $30  บ\n");
	print_window(temp,"     บ would be greatly appreciated.         บ\n");
	print_window(temp,"     บ                                       บ\n");
	print_window(temp,"     ศอออออออออออออออออออออออออออออออออออออออผ");

	key=getch();

	close_window(temp);


	for(i=0;i<=13;i++){

		color(foreground(i+1),0);
		wind[i]=open_window(u_row[i],u_col[i],l_row[i],l_col[i]);

	}

	for(i=13;i>=0;i--){

		close_window(wind[i]);

	}

	for(i=13;i>=0;i--){

		color(foreground(i+1),0);
		wind[i]=open_window(u_row[i],u_col[i],l_row[i],l_col[i]);

	}

	for(i=0;i<=13;i++){

		close_window(wind[i]);

	}

	for(i=0;i<14;i++){

		color(foreground(i+1),0);
		wind[i]=open_window(u_row[i],u_col[i],l_row[i],l_col[i]);
		color_window(wind[i],foreground(13),0);
		print_window(wind[i],"C-WINDOWC-WINDOWC-WINDOW");
		color_window(wind[i],foreground(i+1),0);

	}




	subject(-1);


/*  Move box 14 into place */

	for(i=15;i<=35;i++){

		move_window(wind[13],2);
		slower();

	}
	close_window(wind[13]);


	subject(13);

/* Move box 13 into place */

	for(i=14;i>=11;i--){

		move_window(wind[12],1);
		slower(13);

	}

	for(i=15;i<=40;i++){

		move_window(wind[12],2);
		slower();

	}
	close_window(wind[12]);

	subject(12);


/* Move box 12 into place */

	for(i=11;i>=7;i--){

		for(j=u_col[i];j>6;j--){

			move_window(wind[i],4);
			slower();
		}

		for(j=u_row[i];j>11;j--){

			move_window(wind[i],1);
			slower();
		}

		l_col[i]=11;

		for(j=l_col[i];j<=35;j++){

			move_window(wind[i],2);
			slower();
		}

		close_window(wind[i]);

		subject(i);

	}

	for(i=6;i>=4;i--){

		for(j=l_row[i];j<24;j++){
			move_window(wind[i],3);
			slower();
		}
		l_row[i]=24;
			

		for(j=u_col[i];j>6;j--){

			move_window(wind[i],4);
			slower();
		}
		

		for(j=l_row[i];j>15;j--){

			move_window(wind[i],1);
			slower();
		}

		l_col[i]=6;

		for(j=l_col[i];j<=35;j++){

			move_window(wind[i],2);
			slower();
		}

		close_window(wind[i]);

		subject(i);

	}


	for(i=3;i>=0;i--){

		for(j=l_col[i];j<75;j++){

			move_window(wind[i],2);
			slower();
		}

		l_col[i]=75;


		for(j=l_row[i];j<24;j++){
			move_window(wind[i],3);
			slower();
		}
		l_row[i]=24;
			

		for(j=l_col[i];j>15;j--){

			move_window(wind[i],4);
			slower();
		}

		for(j=l_row[i];j>15;j--){

			move_window(wind[i],1);
			slower();
		}

		l_col[i]=11;

		for(j=l_col[i];j<=35;j++){

			move_window(wind[i],2);
			slower();
		}

		close_window(wind[i]);

		subject(i);

	}

	subject(-2);

}

subject(wind)

int	wind;

{

static	int	temp,
		temp2;
	int	key,
		wind1,
		wind2,
		wind3,
		wind4,i,j;

	char	string[100];

	switch(wind){


		case (-1):

			color(foreground(14),0);
			temp2=open_window(8,17,18,64);
			color_window(temp2,foreground(2),0);
			print_window(temp2," C-WINDOWS is a comprehensive screen and");
			print_window(temp2,"\n window handling package.  C-WINDOWS relieves");
			print_window(temp2,"\n the programmer of handling extensive screen");
			print_window(temp2,"\n and window manipulation.  C-WINDOWS is a");
			print_window(temp2,"\n library of pre-written 'C' and ASSEMBLY");
			print_window(temp2,"\n functions which do printing, coloring,");
			print_window(temp2,"\n cursor positioning, and window handling.");
			locate_window(temp2,9,9);
			print_window(temp2,"Press any key to continue...");
			color_window(temp2,foreground(14),0);
			key=getch();

			break;

		case(13):

			color(foreground(14),0);
			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);

			print_window(temp," C-WINDOWS is supplied in a MS-DOS object library.  To obtain ");
			print_window(temp,"\n the source for C-WINDOWS send a $30 contribution.  \n");

			print_window(temp,"\n C-WINDOWS provides you with a extensive set  of  pre-written ");
			print_window(temp,"\n functions   for   manipulation  of  the  screen.   C-WINDOWS ");
			print_window(temp,"\n provides functions for handling standard screen I/O such  as ");
			print_window(temp,"\n printing, locating the cursor, setting color, scrolling part ");
			print_window(temp,"\n of the screen, printing boxes, saving and restoring parts of ");
			print_window(temp,"\n the  screen,  and  printing  horizontal and veritcal bars of ");
			print_window(temp,"\n characters. \n");

			print_window(temp,"\n C-WINDOWS also contains a extensive  set  of  functions  for ");
			print_window(temp,"\n handling  windows.  C-WINDOWS treats each seperate window as ");
			print_window(temp,"\n a seperate virtual screen.  C-WINDOWS allows you to  scroll, ");
			print_window(temp,"\n locate in,  color, blank, print in, move, open, close and re ");
			print_window(temp,"\n activate each window by only specifying the window you  want \n");


			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);
			key=getch();
			close_window(temp);

			break;
		case(12):

			color(foreground(14),0);
			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);

			print_window(temp," this to take place in. \n");

			print_window(temp,"\n C-WINDOWS  has  a  default maximum setting of 255 windows at ");
			print_window(temp,"\n one time.  This can be changed easily to a greater or lessor ");
			print_window(temp,"\n amount by recompiling the source.  C-WINDOWS treats  windows ");
			print_window(temp,"\n in  the same manor as the Macintosh and LISA computers.  You ");
			print_window(temp,"\n can open windows in layers on top of  each  other  and  then ");
			print_window(temp,"\n re-activate  a  window  which  is  in  the  middle of or the ");
			print_window(temp,"\n bottom.  C-WINDOWS will restore any text under  the  windows ");
			print_window(temp,"\n when they are opened, closed or re-activated.\n"); 

			print_window(temp,"\n To  use  the C-WINDOWS functions,  all you have to do is put ");
			print_window(temp,"\n the code in your program which makes the call to the funtion ");
			print_window(temp,"\n and then at link time specify the appropriate library.  Here ");
			print_window(temp,"\n is a example of how this program 'demo.c' was  compiled  and ");
			print_window(temp,"\n linked. \n");



			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);
			key=getch();
			close_window(temp);

			break;
		case(11):

			color(foreground(14),0);
			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);

			print_window(temp,"            A>lc1 b:demo");
			print_window(temp,"\n            A>lc2 b:demo");
			print_window(temp,"\n            A>link a:cs+b:demo,demo,,b:windows+a:lcs\n");

			print_window(temp,"\n C-WINDOWS  was  written  using the LATTICE 2.12 'C' compiler ");
			print_window(temp,"\n and IBM macro assembler ver 1.0.  For those who are familiar ");
			print_window(temp,"\n with this compiler you can make sense out of the above.  For ");
			print_window(temp,"\n those  who  are  not:  lc1 and lc2 are the two passes of the ");
			print_window(temp,"\n compiler,  and when you link,  the file cs is a header  file ");
			print_window(temp,"\n which   is   linked   to  your  'C'  program.   The  library ");
			print_window(temp,"\n 'windows.lib'  must  be  specified  before  'lcs.lib'   (the ");
			print_window(temp,"\n LATTICE   library)   because   'windows.lib'  contains  some ");
			print_window(temp,"\n replacements for the LATTICE library. \n");


 			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);
			key=getch();
			close_window(temp);

			break;
		case(10):

			color(foreground(14),0);
			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);

			print_window(temp," To give the support of color and full  screen  operation  to ");
			print_window(temp,"\n standard  'C'  some changes had to be made to how the screen ");
			print_window(temp,"\n is handled.  C-WINDOWS handles  color  and  cursor  location ");
			print_window(temp,"\n similiar  to  that  of  IBM BASIC.  C-WINDOWS adds color and ");
			print_window(temp,"\n cursor location support to the standard  printf()  function. "); 
			print_window(temp,"\n Printf()  looks  at  the last color specified with a color() ");
			print_window(temp,"\n function call and then prints using that color.  If no color ");
			print_window(temp,"\n has been specified it uses the default setting of  white  on ");
			print_window(temp,"\n black.  Printf()  was rewritten in assembly language to give ");
			print_window(temp,"\n blazing speed increase and to support  color.  This  version ");
			print_window(temp,"\n of  printf() is not compatable with the standard redirection ");
			print_window(temp,"\n I/O supported by DOS,  but most  programs,  at  least  those ");
			print_window(temp,"\n which are screen intensive,  never use this technique anyway ");
			print_window(temp,"\n so there is on real loss. ");


 			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);
			key=getch();
			close_window(temp);

			break;
		case(9):

			color(foreground(14),0);
			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);

			print_window(temp," The files provided with this package are:\n");

			print_window(temp,"\n             WINDOWS.LIB        |  The library of functions");
			print_window(temp,"\n             WINDOWS.DOC        |  Documentation for each");
			print_window(temp,"\n                                   fuction in the library");
			print_window(temp,"\n             DEMO.EXE           |  This demonstration file");
			print_window(temp,"\n             DEMO.C             |  The source to this demo");
			print_window(temp,"\n                                   file, so you can see ");
			print_window(temp,"\n                                   C-WINDOWS in use.\n");

			print_window(temp,"\n As mentioned earlier.  To obtain  the  source  to  C-WINDOWS ");
			print_window(temp,"\n please send a contribution of $30 to: \n");

			print_window(temp,"\n                  Glen Boyd");
			print_window(temp,"\n                  21606 Criptana");
			print_window(temp,"\n                  Mission Veijo, CA 92692");


			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);
			key=getch();
			close_window(temp);

			break;

		case(8):

			color(foreground(14),0);
			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);

			print_window(temp," When  you  send in a contribution I will send you a diskette ");
			print_window(temp,"\n which contains the source and any new  revisions  to  the  C ");
			print_window(temp,"\n WINDOWS  library,  any new functions which I have added to C ");
			print_window(temp,"\n WINDOWS and documentation for  the  whole  package.  I  will ");
			print_window(temp,"\n also  send  you any new programs which I have written.  Your ");
			print_window(temp,"\n name will go on a mailing list and you will receive any news ");
			print_window(temp,"\n about a new version or new programs which I have  available. "); 
			print_window(temp,"\n You  will also receive a telephone # to call for support and ");
			print_window(temp,"\n or questions about C-WINDOWS or any other related  questions ");
			print_window(temp,"\n you might have. \n");

			print_window(temp,"\n This  demo  you  are watching is written entirely in 'C' and ");
			print_window(temp,"\n uses the C-WINDOWS functions to do the  windows  and  screen ");
			print_window(temp,"\n manipulation.  I  will take you through a tour of what these ");
			print_window(temp,"\n functions are and how they can be used to write some  useful ");
			print_window(temp,"\n applications. ");

			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);
			key=getch();
			close_window(temp);

			break;
		case(7):

			color(foreground(14),0);
			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);

			print_window(temp," color()                   /* Allows setting of");
			print_window(temp,"\n                              the default color */");
			print_window(temp,"\n locate()                  /* Position cursor on");
			print_window(temp,"\n                              the screen */");
			print_window(temp,"\n place()                   /* Print single char */");
			print_window(temp,"\n cls()                     /* Clear entire screen */");
			print_window(temp,"\n v_bar()                   /* Print vertical bar of");
			print_window(temp,"\n                              a specified char */");
			print_window(temp,"\n h_bar()                   /* Print horizontial bar");
			print_window(temp,"\n                              of a specified char */");
			print_window(temp,"\n scroll()                  /* Scroll any part of the");
			print_window(temp,"\n                              screen */");
			print_window(temp,"\n printf()                  /* just like standard printf");
			print_window(temp,"\n                              except written in assembly");
			print_window(temp,"\n                              for speed and color */");


			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);
			key=getch();
			close_window(temp);

			break;
		case(6):

			color(foreground(14),0);
			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);
			print_window(temp," box()                     /* Draws a box using double");
			print_window(temp,"\n                              line drawing characters */");
			print_window(temp,"\n minor_box( )              /* Draws a box using single");
			print_window(temp,"\n                              line drawing characters */");
			print_window(temp,"\n window()                  /* Makes an exploding box */");
			print_window(temp,"\n save_restore()            /* Save and restore portions");
			print_window(temp,"\n                              of the screen */\n");

			print_window(temp,"\n Are  functions  which operate with the screen no matter what ");
			print_window(temp,"\n is on it.  These function can be used by themselves to write ");
			print_window(temp,"\n your  own  window  or  screen   handling   routines.   These ");
			print_window(temp,"\n functions  are  the  primitives used in the functions listed ");
			print_window(temp,"\n below to  handle  the  windows.  Windows  are  handled  like ");
			print_window(temp,"\n individual screens, for example if I open a window up, which ");
			print_window(temp,"\n is  ten  rows  by  thirty columns then C-WINDOWS treats this ");
			print_window(temp,"\n like a  seperate  screen.  You  can  scroll,  print,  color, ");



			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);
			key=getch();
			close_window(temp);

			break;

		case(5):

			color(foreground(14),0);
			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);


			print_window(temp," locate the cursor, and do a host of other functions directly ");
			print_window(temp,"\n to this virtual screen by just passing it a window #.  I can ");
			print_window(temp,"\n have  up to 255 virtual screens open at the same time,  that ");
			print_window(temp,"\n is if I have that much memory.  \n");

			print_window(temp,"\n activate()              /* Make a window active on");
			print_window(temp,"\n                            the screen */");
			print_window(temp,"\n open_window()           /* Open a window */");
			print_window(temp,"\n close_window()          /* Close a previously opened");
			print_window(temp,"\n                            window */");
			print_window(temp,"\n scroll_window()         /* Scroll the text inside of");
			print_window(temp,"\n                            a window */");
			print_window(temp,"\n locate_window()         /* Locate the cursor inside");
			print_window(temp,"\n                            of a window */");
			print_window(temp,"\n print_window()          /* Allows printing in a");
			print_window(temp,"\n                            specified window */");


			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);

			key=getch();
			close_window(temp);
			break;
		case(4):

			color_window(temp,foreground(2),0);

			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);

			print_window(temp," cls_window()            /* Clears out a specified");
			print_window(temp,"\n                            window */");
			print_window(temp,"\n move_window()           /* Move a window around");
			print_window(temp,"\n color_window()          /* Set color in a window */");
			print_window(temp,"\n                            on the screen */\n");

			print_window(temp,"\n When a window is opened all text under the  window  will  be ");
			print_window(temp,"\n saved,  then the window will open up using the current color ");
			print_window(temp,"\n specified with the last  color()  function  call,  then  the ");
			print_window(temp,"\n virtual  screen  (the  inside of the window) will be cleared ");
			print_window(temp,"\n and the cursor will locate in the HOME postion (row 1 column ");
			print_window(temp,"\n 1).  Any printing,  locating,  and  coloring  that  is  done ");
			print_window(temp,"\n inside  this  window  will  be relative to its HOME postion. "); 
			print_window(temp,"\n C-WINDOWS allows overlapping windows, which can be opened on ");
			print_window(temp,"\n top of each other.  When a  window  is  opened,  it  is  the \n");



			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);

			key=getch();
			close_window(temp);

			break;

		case(3):

			color(foreground(14),0);
			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);


			print_window(temp," current (active) window,  that is, it is the window which is ");
			print_window(temp,"\n on top.  When another window is opened it becomes the active ");
			print_window(temp,"\n window  and  the  previous  window  becomes  inactive.  This ");
			print_window(temp,"\n mechanism  allows  for  windows  to be layered on top of one ");
			print_window(temp,"\n another.  You can do anything to any window at any  time  by ");
			print_window(temp,"\n just  specifying  the window #,  and C-WINDOWS will sort out ");
			print_window(temp,"\n what is to be done to  whom.  For  example  if  I  have  ten ");
			print_window(temp,"\n windows  opened up and I want to print to the third one then ");
			print_window(temp,"\n I  would  call  the  print_window()  fucntion  passing   the ");
			print_window(temp,"\n appropriate  window  #  and  data  to  be  printed in it.  C ");
			print_window(temp,"\n WINDOWS will make it the activate window,  by bringing it to ");
			print_window(temp,"\n the top and then printing in it. \n");

			print_window(temp,"\n I  am  going  to  take  you through a short demonstration of ");
			print_window(temp,"\n opening windows and printing to them.  This  demo  will  use ");
			print_window(temp,"\n four windows which are opened,  closed,  activated,  printed ");

			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);

			key=getch();
			close_window(temp);

			break;
		case(2):

			color(foreground(14),0);
			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);

			print_window(temp," in,  move around,  cleared,  and scrolled text in.  First we ");
			print_window(temp,"\n will  start  off  by  opening  a window in ten rows tall and ");
			print_window(temp,"\n thirty five columns wide.  We will position this  window  in ");
			print_window(temp,"\n our  upper left corner of this window.  The function call to ");
			print_window(temp,"\n do this looks like this. \n");

			print_window(temp,"\n            wind1=open_window(7,14,18,50);\n");

			print_window(temp,"\n Ok,  lets examine this statement  before  we  do  the  call. ");
			print_window(temp,"\n First  of  all  the  function  open_window()  will  return a ");
			print_window(temp,"\n integer which is a window ID #,  this # is  a  ID  which  is ");
			print_window(temp,"\n assigned  when  the  window  is  opened.  This  ID  does not ");
			print_window(temp,"\n represent the coresponding place which it  was  opened,  but ");
			print_window(temp,"\n represents  a  ID  given  to  that  window.  Each  window is ");
			print_window(temp,"\n assigned an unique ID number which is used in identifying it ");


			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);

			key=getch();
			close_window(temp);

			break;
		case(1):

			color(foreground(14),0);
			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);
			print_window(temp," with the rest of the windows.  Ok,  the firt argument in the ");
			print_window(temp,"\n function call is the row which the upper left hand corner of ");
			print_window(temp,"\n the window will be placed in,  the next is the column of the ");
			print_window(temp,"\n upper left hand corner, the next two arguments are the lower ");
			print_window(temp,"\n left  hand  corners  row  and  column.   Well  now  that  we ");
			print_window(temp,"\n understand that lets do the call. \n\n");

			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);

			key=getch();

			color(foreground(1),0);

		wind1=open_window(7,14,18,50);


/*  do the open  */

			print_window(wind1," Now you can see  we  are  inside");
			print_window(wind1,"\n this   window   and  it  is  the ");
			print_window(wind1,"\n current active window.  Now lets ");
			print_window(wind1,"\n open up some more windows so  we ");
			print_window(wind1,"\n can see some more. ");
			print_window(wind1,"\n Press any key when ready..");
			key=getch();

			color(foreground(4),0);


		wind2=open_window(1,23,12,60);	

/* open up another window */

  			print_window(wind2," This is our second window");
			print_window(wind2,"\n\n Press any key when ready..");
			key=getch();

			color(foreground(15),0);


		wind3=open_window(9,43,20,80);


			print_window(wind3," This is our third window");
			print_window(wind3,"\n\n Press any key when ready..");
			key=getch();


			color(foreground(10),0);

		wind4=open_window(14,1,25,37);

/* open up another window */

			print_window(wind4," This  is  our   fourth   window. ");
			print_window(wind4,"\n Notice   that  I  specified  the ");
			print_window(wind4,"\n window    locations     in     a ");
			print_window(wind4,"\n overlapping    fashion.     Each ");
			print_window(wind4,"\n window  is  placed  some   where ");
			print_window(wind4,"\n overlapping   the   one   under- ");
			print_window(wind4,"\n neath.  Now that we  have  these ");
			print_window(wind4,"\n windows  open we can call up any ");
			print_window(wind4,"\n window at any  time.  Lets  call ");
			print_window(wind4,"\n window two. Press any key... ");

			key=getch();


/* Now activate window two */

			cls_window(wind2);
			print_window(wind2," Notice how this  window  becomes ");
			print_window(wind2,"\n active   restoring   all   stuff ");
			print_window(wind2,"\n underneath  it  and     anything ");
			print_window(wind2,"\n inside    the    window.    Lets ");
			print_window(wind2,"\n demonstrate  some  other  things ");
			print_window(wind2,"\n C-WINDOWS  can  do.   Lets  move ");
			print_window(wind2,"\n this window  to  the  right  ten ");
			print_window(wind2,"\n spaces.  Press any key ten times ");


			for(i=0;i<10;i++){

				key=getch();
				move_window(wind2,2);
			}
				
			key=getch();


/* do the move */

			cls_window(wind2);

			print_window(wind2," I am going  to  demonstrate  the ");
			print_window(wind2,"\n scrolling capabilities  of  each ");
			print_window(wind2,"\n window.\n\n   Press any key when ");
			print_window(wind2,"ready... ");

			key=getch();

 
/* Do scrolling on each window */

			cls_window(wind1);

			for(j=1;j<=25;j++){

				sprintf(string," This is scrolling up %d\n",j);
				print_window(wind1,string);
			
			}

			cls_window(wind4);

			for(j=1;j<=25;j++){

				sprintf(string," This is scrolling up %d\n",j);
				print_window(wind4,string);
			
			}
			for(j=25;j>=10;j--){

				scroll_window(wind4,1,1);
				locate_window(wind4,1,1);
				sprintf(string,"  This is a scrolling down %d",j-9);
				print_window(wind4,string);
			
			}

			locate_window(wind4,10,1);
			print_window(wind4,"\n Press any key to continue...");
			key=getch();


/* open window 1 and */

/* clear window one */
			cls_window(wind1);

			print_window(wind1," Now we are back  to  window  one ");
			print_window(wind1,"\n and you can start to see some of ");
			print_window(wind1,"\n the power we have by just having ");
			print_window(wind1,"\n four windows, and you have up to ");
			print_window(wind1,"\n 255! \n");

			print_window(wind1,"\n Press any key when ready...");
			key=getch();
			close_window(wind1);
/* close window one */

			cls_window(wind3);

/* clear window 3 */
			print_window(wind3,"\n Now lets close them all\n");

			print_window(wind3,"\n Press any key when ready...");
			key=getch();

			close_window(wind3);
/* close window 3 */
			cls_window(wind4);

/* clear window 4 */
			print_window(wind4," Now lets close window 4");

			print_window(wind4,"\n Press any key when ready...");
			key=getch();

			close_window(wind4);
/* close window 4 */
			cls_window(wind2);
/* clear window 2 */

			print_window(wind2," This  is  the  last  one so here ");
			print_window(wind2,"\n goes.  Watch  how  the  previous ");
			print_window(wind2,"\n window,  which  is the one below ");
			print_window(wind2,"\n becomes active as we  close  all ");
			print_window(wind2,"\n the windows on top of it. ");

			print_window(wind2,"\n Press any key when ready...");

			key=getch();

			close_window(wind2);

/* close window 2 */

			cls_window(temp);

			color_window(temp,foreground(2),0);
			print_window(temp,"\n We made it back here and thats all folks...");
			print_window(temp,"\n Thank you and have fun, writting applications.");

			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");

			key=getch();
			close_window(temp);

			break;
		case(0):

			color(foreground(14),0);
			temp=open_window(4,9,23,73);
			color_window(temp,foreground(2),0);

			print_window(temp," To get the latest copy of this program or to leave a");
			print_window(temp,"\n message for me, you can call");
			print_window(temp,"\n\n                   The Consultants Exchange");
			print_window(temp,"\n                   714-842-6348");
			print_window(temp,"\n                   Huntington Beach, CA");
			print_window(temp,"\n\n And leave a message on it for Glen Boyd");
			print_window(temp,"\n This is a Electronic BBS.  You must have a");
			print_window(temp,"\n modem to call this number.");

			locate_window(temp,18,15);
			print_window(temp,"Press any key to continue...");
			color_window(temp,foreground(14),0);

			key=getch();
			close_window(temp);

			break;

		case(-2):

			close_window(temp2);

			break;

	}

}

slow()
{

	int	i;
	for(i=0;i<=5000;i++);

}

slower()

{

	int	i;

	for(i=0;i<=350;i++);

}



	for(i=0;i<=350;i++);

}

    