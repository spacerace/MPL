/*-----------------------------------------------------------------*/
/* Machine.h                                                       */
/* Machine dependent declarations.                                 */
/*-----------------------------------------------------------------*/

#ifdef PM_MACINTOSH  /* no prototypes in Lightspeed headers! */

typedef unsigned int  size_t;

void    exit( int );
void*   malloc( size_t );
void*   realloc( void*, size_t );

int     vsprintf( char*, char*, void* );

#endif

/*-----------------------------------------------------------------*/
