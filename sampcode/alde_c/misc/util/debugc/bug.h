/*  Dr. Bob's Utilities  */

#define COMMA ,
#define HERE(name) DB_PRINT("function: %-14s file: %-14s line: %-4d \n"  \
        COMMA name COMMA __FILE__ COMMA __LINE__)
#ifndef debug
#    define DEBUG(msg) /* nothing */
#    define DB_PRINT(stuff) /* nothing */
#    define N_DEBUG(stuff) stuff
#    define NDB_PRINT(msg) fprintf(stderr,msg)
#else
#    define DB_PRINT(msg) fprintf(stderr,msg)
#    define DEBUG(stuff) stuff
#    define N_DEBUG(msg) /* nothing */
#    define NDB_PRINT(stuff) /* nothing */
#endif

    DEBUG(
       printf("\n\nIf debug is defined only two calls to ");
       printf("sortfile() will be