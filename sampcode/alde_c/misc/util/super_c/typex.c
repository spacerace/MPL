/*              Examples of Types of Data & Code in C
*/

int a,b;                        /* Uninitialized global data. */

int x = {5};                    /* Initialized global data. */

static int i;                   /* Uninitialized static data local
                                   to this module. */

static int y = {9};             /* Initialized static data local to
                                   this module. */

/* retNext(inc): returns the current value of a count maintained by
   the routine, and then adds inc to the count. */

retNext(inc)

int inc;        /* Parameter data. */

{
 static int lastinc;            /* Uninitialized static data local
                                   to this function. */
 static int count = {0};        /* Initialized static data local to
                                   this function. */
 int retValue;                  /* Automatic data. */

 lastinc = inc;                 /* Code. */
 retValue = count;              /* Code. */
 count += inc;                  /* Code. */
 return(retValue);              /* Code. */
}

