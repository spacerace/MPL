/*
** BYTE Small-C Graphics Display Benchmark
** Version 1 for 8088/8086/80286 PC Machines
** March, 1988
** Written in BYTE Small-C
** Based on Small-C by J.E. Hendrix
**
** Operation:
** 1. Prompt for which mode the user wants to test.
** 2. Save current graphics mode, flip to new mode.
** 3. Calculate center of screen based on new mode.
** 4. Turn on timer.
** 5. Draw an initial circle of radius 20.
** 6. Draw NUMCIRC circles, performing flood fill of doughnut
**    area formed for each circle.
** 7. Turn off stopwatch.
** 8. Flip back to original mode.
** 9. Report results and exit.
**
*/

#include stdio.h

#define NUMCIRC 7	/* Number of circles to draw */

int tblock[4];		/* Timer block */
int vblock[3];		/* Video block */
int mode;		/* Global variable for mode */
int esseg;		/* ES register */

extern svmode();
extern gvmode();
extern setp();

main()
{
	int cx,cy;	/* Center of screen */
	int color;	/* color */
	int i,radius;
int j;
/*
** Save current video mode
*/
	gvmode(vblock);
/*
** Prompt for new mode
*/
	printf("BYTE Graphics Benchmark\n");
	printf("What video mode?\n");
	printf("(Enter 255 for Hercules Monochrome):");
	fscanf(stdin,"%d",&mode);
/*
** Verify valid mode
*/
	switch(mode) {

	case 4:
	case 5:
	case 6:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 255: break;
	default: {
	  printf("Not a valid mode\n");
	  exit(0);
	}
	}
/*
** Set the new video mode
*/
	svmode(mode);
/*
** Set up foreground colors and cx,cy based on mode
*/

	switch(mode) {
	  case 4:
	  case 5:  cx=160;
		   cy=100;
		   color=3;	/* Set to white */
		   esseg=-18432; /* b800h for es */
		   if(mode==4) setcga4(); /* Set the color palettes */
		   break;

	  case 6:  cx=320;
		   cy=100;
		   color=1;	/* Set to white */
		   esseg=-18432; /* b800h for es */
		   setcga6(15); /* Set color palette */
		   break;

	  case 13: cx=160;
		   cy=100;
		   color=7;	/* Set to white */
		   esseg=-24576; /* a000h for es */
		   break;

	  case 14: cx=320;
		   cy=100;
		   color=15;	/* Set to white */
		   esseg=-24576; /* a000h for es */
		   break;

	  case 15: cx=320;
		   cy=175;
		   color=1;
		   esseg=-24576; /* a000h for es */
		   break;

	  case 16: cx=320;
		   cy=175;
		   color=7;	/* Mid-intensity white */
		   esseg=-24576; /* a000h for es */
		   break;

	  case 17:
	  case 18: cx=320;
		   cy=240;
		   color=1;	/* Whatever */
		   esseg=-24576;  /* a000h for es */
		   break;

	  case 19: cx=160;
		   cy=100;
		   color=15;	/* CGA default white */
		   esseg=-24576; /* a000h for es */
		   break;
	   case 255: cx=360;	/* Hercules */
		     cy=174;
		     color=1;
		     esseg=-20480; /* b000h for es */
		     break;
	}
/*
** Set the ES register
*/
#asm
	MOV	AX,_ESSEG
	MOV	ES,AX
#endasm
/*
** Start the timer
*/
	gtime(tblock);
/*
** Draw a circle of radius 20
*/
	radius=20;
	circ(cx,cy,radius,color);

/*
** Alternately draw new circles of radius 30,40,50,60,70, 80 and 90,
** and flood the region in between.
*/
	for(i=0;i<NUMCIRC;++i) {
	  radius+=10;
	  circ(cx,cy,radius,color);
	  flood(cx+radius-5,cy,color);
	}
/*
** Shut off timer
*/
	calctim(tblock);

/*
** Flip back to original mode
*/
	svmode(vblock[0]);

/*
** End of test -- report results
*/
	printf("Elapsed time (HH:MM:SS:1/100ths)\n");
	printf("%d:%d:%d:%d\n",tblock[0],tblock[1],tblock[2],tblock[3]);

	exit(0);
}

/*
** Set cga palette for mode 4
*/
setcga4()
{
#asm
	MOV	AH,0BH
	MOV	BX,257
	INT	10H		;Select cyan-violet-white
#endasm
}
/*
** Set cga palette for mode 6
*/
setcga6(color)  int color;
{
#asm
	MOV	AH,0BH
	MOV	BP,SP
	MOV	BL,2[BP]
	MOV	BH,0
	INT	10H
#endasm
}

/*
** Draw a circle of color, radius r.  Centered at cx,cy.
** Note that we assume (ha!) square pixels and global variable
** mode is set.
** This routine uses an algorithm given by David Troendle
** in the May-June 1984 issue of Sextant Magazine
*/
circ(cx,cy,r,color)
int cx,cy,r,color;
{
	int x,y,dv;
int ch;
/*
** Do initial 4 corners
*/
	setp(cx+r,cy,color,mode);
	setp(cx,cy+r,color,mode);
	setp(cx-r,cy,color,mode);
	setp(cx,cy-r,color,mode);
/*
** Do arc from 0 to 45 degrees
*/
	x=r;
	y=0;
	dv=0;
	do {
	  dv=dv+y+y+1;	/* Deviation change due to y */
	  y+=1;
	  if (dv>x) {
	  	  dv=dv-x-x+1;	/* Reduce deviation */
		  x-=1;
		  }
/*
** Now do reflections
*/
	  setp(cx+x,cy+y,color,mode);
	  setp(cx-x,cy+y,color,mode);
	  setp(cx+x,cy-y,color,mode);
	  setp(cx-x,cy-y,color,mode);
	  if(x!=y) {
	    setp(cx+y,cy+x,color,mode);
	    setp(cx-y,cy+x,color,mode);
	    setp(cx+y,cy-x,color,mode);
	    setp(cx-y,cy-x,color,mode);
	  }
	} while (y<x);
   return 0;
}

/*
** flood(sx,sy,color)
** Flood a region with color, starting at sx,sy.
** This is a fill until ... it fills until it encounters
** color.
** NOTE: This routine assumes that global variable mode is
** set, and the ES register is loaded with the proper value.
** Also...this version does not check for hitting the edge of
** the screen.  So your region BETTER be closed.
*/
flood(sx,sy,color)
int sx,sy,color;
{
	setp(sx,sy,color,mode);
	if(getp(sx+1,sy,mode)!=color) flood(sx+1,sy,color);
	if(getp(sx-1,sy,mode)!=color) flood(sx-1,sy,color);
	if(getp(sx,sy+1,mode)!=color) flood(sx,sy+1,color);
	if(getp(sx,sy-1,mode)!=color) flood(sx,sy-1,color);
	return;
}

