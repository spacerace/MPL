/*
** Copyright (c) 1987,  Tom Steppe.  All rights reserved.
**
** This module compares two arrays of lines (representing
** files) and reports the sequences of consecutive matching
** lines between them using the "recursive longest matching
** sequence" algorithm.  This is useful for implementing a
** file comparison utility.
**
** Compiler:  Microsoft (R) C Compiler  Version 4.00
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>

/* Boolean type and values. */
typedef int     BOOLEAN;
#define TRUE    1
#define FALSE   0

/* Minimum macro. */
#define min(x, y)   (((x) <= (y)) ? (x) : (y))

/* Value to indicate identical strings with strcmp. */
#define ALIKE   0

/* Result of hashing function for a line of text. */
typedef unsigned int   HASH;

/* Mask for number of bits in hash code.  (12 bits). */
#define MASK   (unsigned int) 0x0FFF

/* Number of possible hash codes. */
#define HASHSIZ   (MASK + 1)

/* Information about an entry in a hash table. */
typedef struct tblentry
{
    int   frst;   /* First line # with this hash code. */
    int   last;   /* Last line # with this hash code. */
} TBLENTRY;

/* Information about a line of text. */
typedef struct lineinf
{
    HASH   hash;    /* Hash code value. */
    int    nxtln;   /* Next line with same hash (or 0). */
} LINEINF;

/* Information about a file. */
typedef struct fileinf
{
    char       **txt;      /* Array of lines of text. */
    LINEINF    *line;      /* Array of line info structs. */
    TBLENTRY   *hashtbl;   /* Hash table. */
} FILEINF;

/* Function declarations. */
BOOLEAN filcmp      (char **, int, char **, int, int);
BOOLEAN get_inf     (char **, int, FILEINF *);
HASH    calc_hash   (char *);
void    fnd_seq     (FILEINF *, int, int,
                          FILEINF *, int, int, int);
BOOLEAN chk_hashes  (LINEINF *, LINEINF *, int);
int     cnt_matches (char **, char **, int);
void    rpt_seq     (int, int, int);

/***********************************************************
** compare compares two arrays of lines and reports the
** sequences of consecutive matching lines.  The zeroth
** element of each array is unused so that the index into
** the array is identical to the associated line number.
**
** RETURNS:  TRUE if comparison succeeded.
**           FALSE if not enough memory.
***********************************************************/

BOOLEAN compare (a1, n1, a2, n2, lngval)

char   **a1;     /* (I) Array of lines of text in #1. */
int    n1;       /* (I) Number of lines in a1.
                        (Does not count 0th element.) */
char   **a2;     /* (I) Array of lines of text in #2. */
int    n2;       /* (I) Number of lines in a2.
                        (Does not count 0th element.) */
int    lngval;   /* (I) "Long enough" value. */
{
    FILEINF   f1;    /* File information for #1. */
    FILEINF   f2;    /* File information for #2. */
    BOOLEAN   rtn;   /* Return value. */

    /* Gather information for each file, then compare. */
    if (rtn =
        (get_inf (a1, n1, &f1) && get_inf (a2, n2, &f2)))
    {
        fnd_seq (&f1, 1, n1, &f2, 1, n2, lngval);
    }

    return (rtn);
}

/***********************************************************
** get_inf calculates hash codes and builds a hash table.
**
** RETURNS:  TRUE if get_inf succeeded.
**           FALSE if not enough memory.
***********************************************************/

static BOOLEAN get_inf (a, n, f)

char      **a;   /* (I) Array of lines of text. */
int       n;     /* (I) Number of lines in a. */
FILEINF   *f;    /* (O) File information. */
{
    unsigned int   size;     /* Size of hash table. */
    register int   i;        /* Counter. */
    TBLENTRY       *entry;   /* Entry in hash table. */

    /* Assign the array of text. */
    f->txt = a;

    /* Allocate and initialize a hash table. */
    size = HASHSIZ * sizeof (TBLENTRY);
    if (f->hashtbl = (TBLENTRY *) malloc (size))
    {
        memset ((char *) f->hashtbl, '\0', size);
    }
    else
    {
        return (FALSE);
    }

    /* If there are any lines: */
    if (n > 0)
    {
        /* Allocate an array of line structures. */
        if (f->line = (LINEINF *)
                malloc ((n + 1) * sizeof (LINEINF *)))
        {
            /* Loop through the lines. */
            for (i = 1; i <= n; i++)
            {
                /* Calculate the hash code value. */
                f->line[i].hash = calc_hash (f->txt[i]);

                /* Locate the entry in the hash table. */
                entry = f->hashtbl + f->line[i].hash;

                /* Update the linked list of lines with */
                /* the same hash code.                  */
                f->line[entry->last].nxtln = i;
                f->line[i].nxtln = 0;

                /* Update the first and last line */
                /* information in the hash table. */
                if (entry->frst == 0)
                {
                    entry->frst = i;
                }
                entry->last = i;
            }
        }
        else
        {
            return (FALSE);
        }
    }
    else
    {
       f->line = NULL;
    }

    return (TRUE);
}

/***********************************************************
** calc_hash calculates a hash code for a line of text.
**
** RETURNS:  a hash code value.
***********************************************************/

static HASH calc_hash (buf)

char   *buf;   /* (I) Line of text. */
{
    register unsigned int   chksum;   /* Checksum. */
    char                    *s;       /* Pointer. */
    HASH                    hash;     /* Hash code value. */

    /* Build up a checksum of the characters in the text. */
    for (chksum = 0, s = buf; *s; chksum ^= *s++)
    {
        ;
    }

    /* Combine the 7-bit checksum and as much of the */
    /* length as is possible.                        */
    hash = ((chksum & 0x7F) | ((s - buf) << 7)) & MASK;

    return (hash);
}

/***********************************************************
** Given starting and ending line numbers, fnd_seq finds a
** "good sequence" of lines within those ranges.  fnd_seq
** then recursively finds "good sequences" in the sections
** of lines above the "good sequence" and below it.
***********************************************************/

static void fnd_seq (f1, beg1, end1, f2, beg2, end2, lngval)

FILEINF   *f1;      /* (I) File information for #1. */
int       beg1;     /* (I) First line # to compare in #1. */
int       end1;     /* (I) Last line # to compare in #1. */
FILEINF   *f2;      /* (I) File information for #2. */
int       beg2;     /* (I) First line # to compare in #2. */
int       end2;     /* (I) Last line # to compare in #2. */
int       lngval;   /* (I) "Long enough" value. */
{
    LINEINF      *line1;   /* Line information ptr in #1. */
    LINEINF      *line2;   /* Line information ptr in #2. */
    register int limit;    /* Looping limit. */
    int          ln1;      /* Line number in #1. */
    int          ln2;      /* Line number in #2. */
    register int ln;       /* Working line number. */
    BOOLEAN      go;       /* Continue to loop? */
    int          most;     /* Longest possible seq. */
    int          most1;    /* Longest possible due to #1. */
    int          most2;    /* Longest possible due to #2. */
    int          cnt;      /* Length of longest seq. */
    int          oldcnt;   /* Length of prev longest seq. */
    int          n;        /* Length of cur longest seq. */
    int          m1;       /* Line of longest seq. in #1. */
    int          m2;       /* Line of longest seq. in #2. */
    
    /* Initialize. */
    go    = TRUE;
    line1 = f1->line;
    line2 = f2->line;

    /* Initialize longest sequence information. */
    cnt    = 0;          /* Length of longest seq. */
    m1     = beg1 - 1;   /* Line # of longest seq. in #1. */
    m2     = beg2 - 1;   /* Line # of longest seq. in #2. */
    oldcnt = 0;          /* Length of prev longest seq. */
    
    /* Calculate maximum possible number of consecutive */
    /* lines that can match (based on line # ranges).   */
    most1 = end1 - beg1 + 1;
    most2 = end2 - beg2 + 1;
    
    /* Scan lines looking for a "good sequence".
    ** Compare lines in the following order of line numbers:
    **
    **                 (1, 1)
    **         (1, 2), (2, 1), (2, 2)
    ** (1, 3), (2, 3), (3, 1), (3, 2), (3, 3)
    ** etc.
    */
    for (ln1 = beg1, ln2 = beg2; TRUE; ln1++, ln2++)
    {
        if (ln2 <= end2 - cnt)
        /* There are enough lines left in #2 such that it */
        /* is possible to find a longer sequence.         */
        {
            /* Determine the limit in #1 that both       */
            /* enforces the order scheme and still makes */
            /* it possible to find a longer sequence.    */
            limit = min (ln1 - 1, end1 - cnt);
            
            /* Calculate first potential match in #1. */
            for (ln = f1->hashtbl[line2[ln2].hash].frst;
                    ln && ln < beg1; ln = line1[ln].nxtln)
            {
               ;
            }

            /* Loop through the lines in #1. */
            for (; ln && ln <= limit; ln = line1[ln].nxtln)
            {
                if (line1[ln].hash == line2[ln2].hash &&
                        line1[ln + cnt].hash ==
                            line2[ln2 + cnt].hash &&
                        !(ln - m1 == ln2 - m2 &&
                        ln < m1 + cnt && m1 != beg1 - 1))
                /* A candidate for a longer sequence has */
                /* been located.  The current lines      */
                /* match, the current lines + cnt match, */
                /* and this sequence is not a subset of  */
                /* the longest sequence so far.          */
                {
                    /* Calculate most possible matches. */
                    most = min (end1 - ln + 1, most2);
                    
                    /* First compare hash codes.  If the  */
                    /* number of matches exceeds the      */
                    /* longest sequence so far, then      */
                    /* compare the actual text.           */
                    if (chk_hashes (line1 + ln,
                                line2 + ln2, cnt) &&
                            (n = cnt_matches (f1->txt + ln,
                                f2->txt + ln2, most)) > cnt)
                    /* This is the longest seq. so far. */
                    {
                        /* Update longest sequence info. */
                        oldcnt = cnt;
                        cnt    = n;
                        m1     = ln;
                        m2     = ln2;
                        
                        /* If it's long enough, end the */
                        /* search.                      */
                        if (cnt >= lngval)
                        {
                            break;
                        }
                        
                        /* Update limit, using new count. */
                        limit = min (ln1 - 1, end1 - cnt);
                    }
                }
            }

            /* If it's long enough, end the search. */
            if (cnt >= lngval)
            {
                break;
            }
            most2--;
        }
        else
        {
            go = FALSE;   /* This file is exhausted. */
        }
    
        /* Repeat the process for the other file. */    
        if (ln1 <= end1 - cnt)
        {
            limit = min (ln2, end2 - cnt);
            
            for (ln = f2->hashtbl[line1[ln1].hash].frst;
                    ln && ln < beg2; ln = line2[ln].nxtln)
            {
               ;
            }

            for (; ln && ln <= limit; ln = line2[ln].nxtln)
            {
                if (line1[ln1].hash == line2[ln].hash &&
                        line1[ln1 + cnt].hash ==
                            line2[ln+ cnt].hash &&
                        !(ln1 - m1 == ln - m2 &&
                        ln1 < m1 + cnt && m2 != beg2 - 1))
                {
                    most = min (end2 - ln + 1, most1);
                    
                    if (chk_hashes (line1 + ln1,
                                line2 + ln, cnt) &&
                            (n = cnt_matches (f1->txt + ln1,
                                f2->txt + ln, most)) > cnt)
                    {
                        oldcnt = cnt;
                        cnt    = n;
                        m1     = ln1;
                        m2     = ln;
                        
                        if (cnt >= lngval)
                        {
                            break;
                        }
                        
                        limit = min (ln2, end2 - cnt);
                    }
                }
            }

            if (cnt >= lngval)
            {
                break;
            }
            most1--;
        }
        else if (!go)
        {
            break;   /* This file is exhausted, also. */
        }
    }

    /* If the longest sequence is shorter than the "long */
    /* enough" value, the "long enough" value can be     */
    /* adjusted for the rest of the comparison process.  */
    if (cnt < lngval)
    {
        lngval = cnt;
    }

    if (cnt >= 1)
    /* Longest sequence exceeds minimum necessary size. */
    {
        if (m1 != beg1 && m2 != beg2 && oldcnt > 0)
        /* There is still something worth comparing */
        /* previous to the sequence.                */
        {
            /* Use knowledge of the previous longest seq. */
            fnd_seq (f1, beg1, m1 - 1,
                    f2, beg2, m2 - 1, oldcnt);
        }
        
        /* Report the sequence. */
        rpt_seq (m1, m2, cnt);
        
        if (m1 + cnt - 1 != end1 && m2 + cnt - 1 != end2)
        /* There is still something worth comparing */
        /* subsequent to the sequence.              */
        {
            fnd_seq (f1, m1 + cnt, end1,
                    f2, m2 + cnt, end2, lngval);
        }
    }
}

/***********************************************************
** chk_hashes determines whether this sequence of matching
** hash codes is longer than cnt.  It knows that the first
** pair of hash codes is guaranteed to match.
**
** RETURNS:  TRUE if this sequence is longer than cnt.
**           FALSE if this sequence is not longer than cnt.
***********************************************************/

static BOOLEAN chk_hashes (line1, line2, cnt)

LINEINF        *line1;   /* (I) Line information for #1. */
LINEINF        *line2;   /* (I) Line information for #2. */
register int   cnt;      /* (I) Count to try to exceed. */
{
    register int   n;   /* Count of consecutive matches. */

    for (n = 1; n <= cnt &&
        ((++line1)->hash == (++line2)->hash); n++)
    {
        ;
    }

    return (n > cnt);
}

/***********************************************************
** cnt_matches counts the number of consecutive matching
** lines of text.
**
** RETURNS:  number of consecutive matching lines.
***********************************************************/

static int cnt_matches (s1, s2, most)

char         **s1;   /* (I) Starting line in file #1. */
char         **s2;   /* (I) Starting line in file #2. */
register int most;   /* (I) Most matching lines possible. */
{
    register int   n;   /* Count of consecutive matches. */

    /* Count the consecutive matches. */
    for (n = 0; n < most && strcmp (*s1++, *s2++) == ALIKE;
            n++)
    {
        ;
    }
    
    return (n);
}

/***********************************************************
** rpt_seq reports a matching sequence of lines.
***********************************************************/

static void rpt_seq (m1, m2, cnt)

int   m1;    /* (I) Location of matching sequence in #1. */
int   m2;    /* (I) Location of matching sequence in #2. */
int   cnt;   /* (I) Number of lines in matching sequence. */
{
    fprintf (stdout,
        "Matched %5d lines:  (%5d - %5d) and (%5d - %5d)\n",
         cnt, m1, m1 + cnt - 1, m2, m2 + cnt - 1);
}
