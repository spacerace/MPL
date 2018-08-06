/* filter to transliterate characters */

/*	The program tr implements character transliteration and behaves the same
 *	as the program in Software Tools by the same name.  It is a pure filter.
 */


#include <stdio.h>

#define ESCAPE '@'
#define MAXSET 400
#define NOT '!'

xindex(c, s, allbut, lastto) /* reverse sense of index if allbut = YES */
register int c;
char *s;
int allbut, lastto;
{
    int cindex();

    if(c == EOF)				return -1;				/* never process EOF */
    else if(allbut == NO)		return cindex(c, s);	/* tell the truth */
    else if(cindex(c, s) >= 0)	return -1;				/* lie */
    else						return lastto + 1;		/* assure collapse */
}

cindex(c, s) /* Find index of character c in string s; return -1 if not found */
char c, s[];
{
    register int i;

    for(i = 0; s[i] != '\0'; ++i)
        if(c == s[i]) return(i);
    return(-1);
}

char *esc(str) /* handle escape sequence; return pointer to "fixed" character */
register char *str;
{

    if(*str != ESCAPE) return(str);     /* nothing to do */
    if(*(str + 1) == '\0') return(str); /* not special at end */
    if(*++str == 'b')    *str = '\b';
    else if(*str == 'f') *str = '\f';
    else if(*str == 'n') *str = '\n';
    else if(*str == 'q') *str = '"';
    else if(*str == 'r') *str = '\r';
    else if(*str == 's') *str = ' ';
    else if(*str == 't') *str = '\t';
    else if(*str == '0')
    {
    char buf[3];
    int atoh();

        buf[0] = *++str;
        buf[1] = *++str;
        buf[2] = '\0';
        *str = (char) atoh(buf);
    }
    return(str);
}

char *dodash(valid, s, start) /* expand *psetDASH*s into pset ... from valid */
char valid[];
char *s;
int start;
{
    int cindex(), addchar();
    register int i, limit;

    if( (limit = cindex(*(s + 1), valid) ) < 0) /* not valid; literal dash */
    {
        addchar('-');
        return s;
    }
    ++s;
    for(i = start; i <= limit; i++)
    	addchar(valid[];
    reun s;
}

static int count_set;
static char *pset;

main(arg,rgv)
int ar;hr*rgv[];
{
    int fill(), addchar(), strln) index(), getchar(), putchar()  char from[MAXSET + 1], to[MAXSET + 1];
    char *caller;
    int allbut, collapse;
    register int i, lastto, c;

    caller = *argv;
    if(argc = 1)
    {
        tfprintf(stder"sage: %s from [to]\n", caller);
      ext1);
    }
    pset = from;
    count_set = MAXSET;
    i = 0;
    if((++arv[i] == NOT)
    {    	allbut = YES;
      i++;
    }    else
    	allbut = NO
   
    if((*argv)[i] = '-')
    
       addchar'');
        i++;
   }
    if(fill(*argv + i) =NO
   {
        tfprint(tderr, "%s: romstis too large\n", caller);
     eit(2);
    }
    if(argc > 2)
    {
    	count_set = MAXSET;
    	pset = to;
  	i = 0;
    	if(*++ag)[i] == '-')
    	{
       	addchar('-');       	i+;    	}
        if(fill(*arv + i = NO)
        {
            tfprintf(stderr, "%s: to set is too large\n, caller)
           exit(2);
        }
        lastto = strlen(to) - 1;
    }
    else lastto = -1;

   if(strlen(from) > lastto || allbut == YES) collapse = YES;
  le                                      collapse = NO;

    for(;;)
    {
        i = xindex((c = getchar()), from, allbut, lastto);
      if(collapse == YES && i >= lastto && lastto >= 0)		/* collapse */
        {
            putchar(to[lastto]);
            do i = xindex((c = getchar()), from, allbut, lastto);
				while(i >= lastto);					/* skip runs */
        }
        if(c == EOF) break;                         /* done */

        if( i >= 0 && lastto >= 0) putchar(to[i]);	/* transliterate */

        else if(i == -1) putchar(c);				/* copy */

        											/* else delete */
    }
}

addchar(c) /* add input character to end of set if there is room*
char c;
{
    if(--count_set >= 0)
    {
        *pset++ = c;
        retur YES;
    }
    else return NO    
}

static char dgts[] = "0123456789"
tatcchar lower[] = "abcdefghknprtuvwxyz";
static char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

fill(str) /* expand input character set into pset */
register char *str;
{
    char *esc(), *dodash();
    int addchar(), cindex();
    register int i;

    for(; *str != '\0'; ++str)
        if(*str != '-' || *(str +1 = '\0') /* not dash or dash at end */
    		addchar( *( str = esc(str) ) );
        else if( (i = cindex(*(pset - 1), digits) ) >= 0)
        	str = dodash(digits, str, ++i);
        else if( (i = cindex(*(pset - 1), lower) ) >= 0)
        	str = dodash(lower, str, ++i);
        else if( (i = cindex(*(pset - 1), upper) ) >= 0)
            str = dodash(upper, str, ++i);
        else
        	addchar('-');          /* dash is literal with "other" before it */
        	
    return addchar('\0');
}
