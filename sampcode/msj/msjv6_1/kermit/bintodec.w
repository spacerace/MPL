#include <stdio.h>

int state, s = 1, m = 0, d;
float f;
char *b;

/* Declare wart states */
%states sign mantissa fraction

/* Begin state table */

%%                                        
<sign>-      { s = -1; BEGIN mantissa; }  /* Look for sign */
<sign>0      { m = 0;  BEGIN mantissa; }  /* Got digit, 
                                             start mantissa */
<sign>1      { m = 1;  BEGIN mantissa; }
<sign>.      { fatal("bad input"); }    /* Detect bad format */
<mantissa>0  { m *= 2; }                /* Accumulate mantissa */
<mantissa>1  { m = 2 * m + 1; }
<mantissa>$  { printf("%d\n", s * m); return; }
<mantissa>.  { f = 0.0; d = 1; BEGIN fraction; } /* Start fraction */
<fraction>0  { d *= 2; }                    /* Accumulate fraction */
<fraction>1  { d *= 2; f += 1.0 / d; }
<fraction>$  { printf("%f\n", s * (m + f) ); return; }
<fraction>.  { fatal("bad input\n"); }
%%

input(void) {       /* Define input() function */
    int x;
    return(((x = *b++) == '\0') ? '$' : x );
}

fatal(char *s) {                    /* Error exit */

    fprintf(stderr,"fatal - %s\n",s);
    exit(1);
}

main(int argc,char **argv) {    /* Main program */
    if (argc < 2) {
        fprintf(stderr, "Not enough input\n");
        exit(1);
    }
    b = *++argv;
    state = sign;           /* Initialize state */
    wart();                 /* Invoke state switcher */
    exit(0);                /* Done */
}

