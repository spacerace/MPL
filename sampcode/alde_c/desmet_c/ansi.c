/*************************************************************************
main function to test some of the ansi device driving functions
which follow below.  NOTE:  you need your device set to ansi.sys in the
config.sys for this to work.

written by Rex Jaeschke of Rockville, MD. 1983 (301) 251-8987
compiler used DeSmet v2.2

ci () is a DeSmet compiler specific function which does direct console
input of 1 character without echoing it. It is used in sgr_dsr and the
testing program only. All else should run on any compiler.
*************************************************************************/

#define CTRL_C 3
#define BELL 7
#define NULL '\0'
#define LINEMIN 1  /* Minimum screen line # */
#define LINEMAX 25 /* Maximum screen line # */
#define COLMMIN 1  /* Minimum screen column # */
#define COLMMAX 80 /* Maximum screen column # */
#define L_ARROW 75
#define R_ARROW 77
#define U_ARROW 72
#define D_ARROW 80
#define HOME 71
#define END 79
#define C_HOME 119

main ()
{
    int c;

    scr_ed ();               /* clear screen */
    scr_cup (12,40);         /* move to screen center */
    while ((c = ci()) != CTRL_C) {
         if (c == NULL) {    /* do we have extended code? */
              c = ci();
              switch (c) {
              case L_ARROW:
                   scr_cub (1);
                   break;
              case R_ARROW:
                   scr_cuf (1);
                   break;
              case U_ARROW:
                   scr_cuu (1);
                   break;
              case D_ARROW:
                   scr_cud (1);
                   break;
              case C_HOME:
                   scr_ed ();
                   break;
              case HOME:
                   scr_cup (LINEMIN,COLMMIN);
                   break;
              case END:
                   scr_cup (LINEMAX,COLMMAX);
                   break;
              default:
                   putchar (BELL);
                   break;
              }
         }
         else
              putchar (BELL);
    }         
}


#define ESCAPE 27  /* ASCII ESC character definition */

/*************************************************************************
SCR_CUB - Cursor Backward.

Moves the cursor backward n columns. Current line remains unchanged. If # of
columns exceeds left-of-screen, cursor is left at the left-most column.
*************************************************************************/

scr_cub (ncolms)
int ncolms;
{
    printf ("%c[%dD",ESCAPE,ncolms);
}

/*************************************************************************
SCR_CUD - Cursor Down.

Moves the cursor down n lines. Current column remains unchanged. If # of lines
to move down exceeds bottom-of-screen, cursor is left at the bottom.
*************************************************************************/

scr_cud (nlines)
int nlines;
{
    printf ("%c[%dB",ESCAPE,nlines);
}

/*************************************************************************
SCR_CUF - Cursor Forward.

Moves the cursor forward n columns. Current line remains unchanged. If # of
columns exceeds right-of-screen, cursor is left at the right-most column.
*************************************************************************/

scr_cuf (ncolms)
int ncolms;
{
    printf ("%c[%dC",ESCAPE,ncolms);
}

/*************************************************************************
SCR_CUP - Cursor Position. (same as HVP)

Moves the cursor to the specified position line,colm.
*************************************************************************/

scr_cup (line,colm)
int line,colm;
{
    printf ("%c[%d;%dH",ESCAPE,line,colm);
}

/*************************************************************************
SCR_CUU - Cursor Up.

Moves the cursor up n lines. Current column remains unchanged. If # of lines
to move up exceeds top-of-screen, cursor is left at the top.
*************************************************************************/

scr_cuu (nlines)
int nlines;
{
    printf ("%c[%dA",ESCAPE,nlines);
}

/*************************************************************************
SCR_DSR - Device Status Report.

Returns the Cursor Position Report (CPR) sequence in the form ESC[line;colmR

ci () is a DeSmet compiler specific function which does direct console
input of 1 character without echoing it.
*************************************************************************/

scr_dsr (line,colm)
int *line,*colm;
{
    int i = 0;
    char cpr[10];

    printf ("%c[6n",ESCAPE);
    while ((cpr[i++] = ci ()) != 'R')
         ;
    cpr[i] = '\0';

/* format of cpr[] is ESC[rr;ccR row and colm are always two digits */

    *line = ((cpr[2]-'0')*10)+cpr[3]-'0';
    *colm = ((cpr[5]-'0')*10)+cpr[6]-'0';
}

/*************************************************************************
SCR_ED - Erase in Display.

Erases all of the screen leaving the cursor at home
*************************************************************************/

scr_ed ()
{
    printf ("%c[2J",ESCAPE);
}

/*************************************************************************
SCR_EL - Erase in Line.

Erases from the cursor to the end of the line including the cursor position.
*************************************************************************/

scr_el ()
{
    printf ("%c[2K",ESCAPE);
}

/*************************************************************************
SCR_HVP - Horizontal and Vertical Position. (same as CUP)

Moves the cursor to the specified position line,colm.
*************************************************************************/

scr_hvp (line,colm)
int line,colm;
{
    printf ("%c[%d;%dH",ESCAPE,line,colm);
}

/*************************************************************************
SCR_RCP - Restore Cursor Position.

Restores the cursor to the value it had when previously saved by scr_scp.
*************************************************************************/

scr_rcp ()
{
    printf ("%c[u",ESCAPE);
}

/*************************************************************************
SCR_SCP - Save Cursor Position.

Saves the current cursor position for later restoration by scr_rcp.
*************************************************************************/

scr_scp ()
{
    printf ("%c[s",ESCAPE);
}

/*************************************************************************
SCR_SGR - Set Graphics Rendition.

Sets the character attribute specified by the parameter.
Attributes remain in force until reset or changed.
*************************************************************************/

scr_sgr (attrib)
int attrib;
{
    printf ("%c[%dm",ESCAPE,attrib);
}

/*************************************************************************
SCR_SM - Set Mode.

Sets the screen width or type specified by the parameter.
*************************************************************************/

scr_sm (param)
int param;
{
    printf ("%c[=%dh",ESCAPE,param);
}

/*************************************************************************
SCR_RM - Reset Mode.

Sets the screen width or type specified by the parameter.
*************************************************************************/

scr_rm (param)
int param;
{
    printf ("%c[=%dl",ESCAPE,param);
}
*****************************************************/

