/*  profile.h - definitions for profile.dll */

extern	unsigned far pascal PROFCLEAR (int);
extern	unsigned far pascal PROFDUMP (int, char far *);
extern	unsigned far pascal PROFFREE (int);
extern	unsigned far pascal PROFINIT (int, char far *);
extern	unsigned far pascal PROFOFF (int);
extern	unsigned far pascal PROFON (int);

#define     PROF_SHIFT	    2	    /* Power of 2 profile granularity */

#define     MOD_NAME_SIZE  10	    /* size of module name */

/*  Profile type */
#define     PT_SYSTEM	    0	    /* System profiling 	*/
#define     PT_USER	    1	    /* User   profiling 	*/
#define     PT_USEKP	    4	    /* Kernel support profiling */
