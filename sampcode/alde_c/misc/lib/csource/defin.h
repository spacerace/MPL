/*
FILENAME      FUNCTION    DESCRIPTION OF FUNCTION


DEFIN.C     ----------->  Text file - definition only - this file

VC.C        main()      primarily a menu
            readline()  read an input line from the keyboard
            parse()     parse the input line
            errout()    output error message to display

DATA.C      initdata()  initialize data and fields
            getnames()  get new variable names
            calcdata()  do the calculations
            calcdat()   do the four function calculations
            getnum()    get the value of the number
            getop()     get the math operator
            errchk()    check for error storage

FILE.C      fileout()   store transcript to a file
            filein()    retrieve transcript from a file
            strtrans()  store a transcript message
            transout()  display the current transcript
            movarrow()  move the arrow up or down

VIDEO.C     monitor()   determine type of monitor
            bkgndvid()  display video background
            valusvid()  display all values
            disnew()    display the new changed variable
            helpm()     display mathematics help messages
            helps()     display system help messages
            linedisp()  display a line to video (with attr)
            strngdis()  display a line (add attr)
            blnkline()  outputs blanks to a video line
            chardis()   display a single character
            errdis()    display error message to screen
            clrscrn()   clear the video monitor
            poscurs()   position the cursor on the monitor
            prtprblm()  printer problem check
                                                                */

/*  Prototype definitions for all functions                     */

void readline(void);
void parse(void);
void errout(void);

void initdata(struct vars *pnt);
void getnames(void);
void calcdata(double *newval);
void calcdat(double *number1,char *op1,double *number2);
double getnum(void);
char getop(void);
void errchk(int err);

void fileout(void);
void filein(void);
void strtrans(char line[],int type);
void transout(void);
void movarrow(int where);

void monitor(void);
void bkgndvid(void);
void valusvid(void);
void disnew(int varinuse);
void helpm(void);
void helps(void);
void linedisp(int line);
void strngdis(int row,int col,int attr);
void blnkline(int row,int col);
void chardis(int row,int col,int attr,int ch);
void errdis(char str[]);
void clrscrn(void);
void poscurs(int row,int col);
int prtprblm(void);
