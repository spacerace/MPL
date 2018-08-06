typedef double far *LPDOUBLE;
typedef char   far *LPSTR;


/* ------------------------------ AA */
int far pascal TestBool(a)
int a;
{
   return (!a);
}

/* ------------------------------ */
LPDOUBLE far pascal TestDouble(a)
double a;
{
   static double temp;

   temp = a*2.0;
   return (LPDOUBLE)&temp;
}

/* ------------------------------ */
LPSTR far pascal TestChar(s)
LPSTR s;
{
     static char buffer[128];
     char *t;

     t = buffer;
     while (*s++) {
          *t++ = '$';
     }
     *t = '\0';
     return (LPSTR) buffer;
}

/* ------------------------------ */
LPSTR far pascal TestByte()
{
     static char buff[24];

     buff[0] = 9;    /* a contains the # of characters to return */
     buff[1] = 'H';
     buff[2] = 'i';
     buff[3] = ' ';
     buff[4] = 'T';
     buff[5] = 'h';
     buff[6] = 'e';
     buff[7] = 'r';
     buff[8] = 'e';
     buff[9] = '.';

   return (LPSTR) buff;
}

/* ------------------------------ */
double far pascal TestFloatBuff(a)
double *a;
{

   *a = 2.0;
   return (double) *a;
}

/* ------------------------------ */
LPSTR far pascal TestZBuff(a)
LPSTR a;
{

   a[0] = 'G';
   a[1] = 'r';
   a[2] = 'e';
   a[3] = 'e';
   a[4] = 't';
   a[5] = 'i';
   a[6] = 'n';
   a[7] = 'g';
   a[8] = 's';
   a[9] = '\0';

   return (LPSTR) a;
}

/* ------------------------------ */
LPSTR far pascal TestByteBuff(a)
LPSTR a;
{

   a[0] = 8;
   a[1] = 'G';
   a[2] = 'o';
   a[3] = 'o';
   a[4] = 'd';
   a[5] = ' ';
   a[6] = 'd';
   a[7] = 'a';
   a[8] = 'y';

   return (LPSTR) a;
}

/* ------------------------------ */
unsigned int far pascal TestUInt(a)
unsigned int a;
{
   return (2*a);
}

/* ------------------------------ */
int far pascal TestInt(a)
int a;
{
   return (2*a);
}

/* ------------------------------ */
long far pascal TestLong(a)
long a;
{
   return (2*a);
}



/* ============================== */

typedef unsigned short  WORD;
typedef struct fp {
     WORD rows;
     WORD columns;
     double Array[1];
} FP;
typedef struct gp {
     WORD rows;
     WORD columns;
     double Array[6];
} GP;
typedef FP far *LPFP;
typedef GP far *LPGP;


/* ------------------------------ */
LPDOUBLE far pascal TestAddK(a)
LPFP a;
{
     int i,items;
     static double value;

     value = 0.0;
     items = a->rows * a->columns;
     for (i=0 ; i<items ; i++)
        value += a->Array[i];
     return (LPDOUBLE)&value;
}

LPGP far pascal ChangeK(a)
LPFP a;
{
     int i,items;
     static GP b;

     items = a->rows * a->columns;
     b.rows = 2; b.columns = 3;
     for (i=0 ; i<items ; i++)
	     b.Array[i] = a->Array[i] + 1;
     return (LPGP)&b;
}

