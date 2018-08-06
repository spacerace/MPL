/*                      Fractal Generation Program
*/

#include <stdio.h>
#include <math.h>

#define FALSE 0
#define TRUE 1

/* Movement directions: */

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

/* Table to convert from the direction of a segment being drawn (first
   index) and the direction of a subsegment (second index) to absolute
   screen up/down/left/right: */

int dirDir[4][4] = {
        LEFT, RIGHT, DOWN, UP,  /* Line up. */
        RIGHT, LEFT, UP, DOWN,  /* Line down. */
        DOWN, UP, RIGHT, LEFT,  /* Line left. */
        UP, DOWN, LEFT, RIGHT   /* Line right. */
};

/* X-axis multiplier for up/down/left/right movement: */

int xDir[4] = {
        0, 0, -1, 1     /* Line right. */
};

/* Y-axis multiplier for up/down/left/right movement: */

int yDir[4] = {
        -1, 1, 0, 0     /* Line right. */
};

int boxPatt[100];       /* The box pattern. */
int boxPSz;             /* It's size. */

int linePatt[100];      /* The line pattern. */
int linePSz;            /* It's size. */
int lineLen;            /* Width of line pattern in line segments. */

int level;              /* The level of detail to go to. */

float x, y;             /* The current pen position. */

int color;              /* The current pen color. */

/*      main()

        Function: Get box and line patterns, the line size, and the
        level to draw to from the user. Then draw the fractal. When
        done, ask if another one should be drawn. Repeat the sequence
        as long as the user wants.

        Algorithm: Use getPatt to read in the patterns for the box and
        the line pattern. Use gets/atoi for the other, numeric input.
        Then call doPatt to actually draw the fractal.
*/

main()

{
        char str[100];  /* Input buffer for user responses. */
        int side;       /* Size of line in screen dots. */

        /* Repeat while the user wants more. */
        do {
                /* Set the display mode; use 6 for CGA, 14 for EGA with
                   color display, or 16 for EGA with enhanced color disp. */
                setMod(6);
                /* Get the box pattern. */
                getPatt("box",boxPatt,&boxPSz,TRUE,TRUE);
                /* Get the size of the box line. */
                printf("Side size: ");
                gets(str);
                side = atoi(str);
                /* Get the line pattern. */
                lineLen = getPatt("line",linePatt,&linePSz,TRUE,FALSE);
                /* Get the level to display to. */
                printf("Level: ");
                gets(str);
                level = atoi(str);
                /* Set the pen position and color to start with. */
                x = 270; y = 50; color = 7;
                /* Draw the fractal. */
                doPatt(boxPatt,boxPSz,RIGHT,(float) side,0);
                /* Ask if the user wants to do another one. */
                printf("Do another one? ");
                gets(str);
        } while ((str[0] == 'y') || (str[0] == 'Y'));
}

/*      getPatt(prmpt,patt,size,noUp,noLeft)

        Function: Get a pattern from the user. Issue the prompt
        string pointed to by prmpt. Then read the pattern into
        the pattern array pointed to by patt, and set the size
        pointed to by size. If noUp is TRUE, don't allow patterns
        that end at a different up/down level than they began.
        If noLeft is TRUE, don't allow patterns that end at a
        different left/right level than they started. getPatt
        returns the number of segments to the right the pattern
        ends at from its starting position.

        Algorithm: Prompt the user and then read in the pattern
        string. Scan through it, converting from 'u'/'d'/'l'/'r'
        to UP/DOWN/LEFT/RIGHT. When done, check it for validity.
        If it fails, tell the user and do it all over again.
*/

getPatt(prmpt,patt,size,noUp,noLeft)

char *prmpt;
int *patt;
int *size;
int noUp;
int noLeft;

{
        char str[100];          /* Input buffer for user input. */
        char *cp;               /* Char pointer to scan buffer with. */
        int *pPtr;              /* Pattern array pointer. */
        int upCnt, rightCnt;    /* Up/down and left/right offsets. */
        int badPatt;            /* TRUE if pattern is bad. */

        /* Repeat until he gets it right. */
        do {
                /* Prompt him for a pattern string. */
                printf("Enter %s pattern: ",prmpt); gets(str);
                /* Assume the pattern's OK. */
                badPatt = FALSE;
                /* Scan through the pattern string. */
                for (cp = str, pPtr = patt, upCnt = rightCnt = 0, *size = 0;
                     *cp != 0; cp++, pPtr++, (*size)++) {
                        switch (*cp) {
                                case 'u':
                                case 'U':
                                        /* Up one. */
                                        *pPtr = UP;
                                        upCnt++;
                                        break;
                                case 'd':
                                case 'D':
                                        /* Down one. */
                                        *pPtr = DOWN;
                                        upCnt--;
                                        break;
                                case 'l':
                                case 'L':
                                        /* Left one. */
                                        *pPtr = LEFT;
                                        rightCnt--;
                                        break;
                                case 'r':
                                case 'R':
                                        *pPtr = RIGHT;
                                        rightCnt++;
                                        break;
                                default:
                                        badPatt = TRUE;
                                        break;
                        };
                        if (badPatt) {
                                printf("Only U, D, L, or R allowed!\n");
                                break;
                        };
                };
                if (noUp && (upCnt != 0)) {
                        printf("Pattern must end at the same up/down level ");
                        printf("it began at.\n");
                        badPatt = TRUE;
                };
                if (noLeft && (rightCnt != 0)) {
                        printf("Pattern must end at the same right/left ");
                        printf("level it began at.\n");
                        badPatt = TRUE;
                };
        } while (badPatt);
        return(rightCnt);
}

/*      doPatt(patt,pattSize,dir,size,lev)

        Function: Draw the pattern pointed to by patt, of size pattSize,
        in the direction dir (UP, DOWN, LEFT, or RIGHT), with each line
        segment of size size; this will be level lev. If lev doesn't yet
        match the global variable level, then recursively call doPatt to
        draw each segment as a series of segments from the line pattern.

        Algorithm: For each segment in the pattern, if we're at the drawing
        level (lev == level), then figure out the proper direction to
        draw and draw the segment; otherwise, call doPatt to draw the
        sub-pattern.
*/

doPatt(patt,pattSize,dir,size,lev)

int *patt;
int pattSize;
int dir;
float size;
int lev;

{
        int *pPtr;                      /* A pointer into the pattern. */
        int pCnt;                       /* Count of pattern segments left. */
        int xPlusMinus, yPlusMinus;     /* The x and y drawing increments. */
        register int i, j;              /* Row and column drawing indices. */
        int iEnd, jEnd;                 /* Where to stop drawing. */
        int temp;
        int subDir;                     /* The absolute up/down/left/right. */
        int dotVal;                     /* Color to draw. */

        /* For each segment in the pattern... */
        for (pPtr = patt, pCnt = 0; pCnt++ < pattSize; pPtr++) {
                /* If we're at drawing level... */
                if (lev == level) {
                        /* Find the absolute direction. */
                        subDir = dirDir[dir][*pPtr];
                        /* Convert the pen position to row/column ints. */
                        i = x; j = y;
                        /* Get the pen increment for drawing. */
                        xPlusMinus = xDir[subDir]; 
                        yPlusMinus = yDir[subDir];
                        /* Compute the new pen position after drawing. */
                        x += xPlusMinus*size; y += yPlusMinus*size;
                        /* Convert the end point to row/column ints. */
                        iEnd = x; jEnd = y;
                        /* We always want to draw in ascending coordinates.
                           Therefore, we must swap i/iEnd and j/jEnd if
                           we'd be drawing descending. */
                        if (iEnd < i) {
                                temp = i; i = iEnd; iEnd = temp;
                        };
                        if (jEnd < j) {
                                temp = j; j = jEnd; jEnd = temp;
                        };
                        /* Similarly, we need the abs of the increments. */
                        xPlusMinus = abs(xPlusMinus); 
                        yPlusMinus = abs(yPlusMinus);
                        /* Set the color to draw. For the EGA, this should
                           be the global variable color; for CGA, it should
                           be 1. */
                        dotVal = 1 /*color*/;
                        /* Decide whether we're drawing a vertical or
                           horizontal line. */
                        if (xPlusMinus == 0) {
                                /* If vertical, draw it. */
                                do putDot(0,j++,i,dotVal); while (j < jEnd);
                        } else {
                                /* If horizontal, draw it. */
                                do putDot(0,j,i++,dotVal); while (i < iEnd);
                        };

                /* If we're not down to drawing level yet, call doPatt to
                   draw the next level down. */
                } else doPatt(linePatt,linePSz,dirDir[dir][*pPtr],
                              size/((float) lineLen),lev+1);

                /* If this is the top level, switch to the next color for 
                   drawing the next segment. */
                if (lev == 0) {
                        color--;
                        if (color == 0) color = 15;
                };
        };
}

