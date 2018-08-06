/* SDB - sort routines */

#include "stdio.h"
#include "sdbio.h"

extern int dbv_token;
extern char dbv_tstring[];
extern int dbv_tvalue;

/* get_skeys - get sort key list */
static struct skey *get_skeys(sptr)
  struct scan *sptr;
{
    struct skey *skeys,*newskey,*lastskey;

    /* parse a list of attribute names */
    skeys = lastskey = NULL;
    while (TRUE) {

        /* get attribute name */
        if (db_ntoken() != ID)
            return (db_nerror(SYNTAX));

        /* allocate a sort key structure */
        if ((newskey = malloc(sizeof(struct skey))) == NULL)
            return (db_nerror(INSMEM));

        /* initialize the sort key structure */
        newskey->sk_next = NULL;

        /* lookup the attribute name */
        if (!find_attr(sptr,newskey,dbv_tstring)) {
            free(newskey);
            return (NULL);
        }

        /* check for ascending or descending */
        if (db_token() == ASCENDING || dbv_token == DESCENDING) {
            newskey->sk_type = dbv_token;
            db_ntoken();
        }
        else
            newskey->sk_type = ASCENDING;

        /* link the sort key structure into the list */
        if (lastskey == NULL)
            skeys = newskey;
        else
            lastskey->sk_next = newskey;
        lastskey = newskey;

        /* check for more attributes */
        if (db_token() != ',')
            break;
        db_ntoken();
    }

    /* return successfully */
    return (skeys);
}

/* db_sort - sort tuples in a relation */
int *db_sort(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9)
  char *fmt;
{
    struct scan *sptr1,*sptr2,*sptr3;   /*dns*/
    struct skey *skeys;
    int result;

    /* check for a command line */
    if (fmt != NULL)
        db_scan(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9);

    /* checks for relation name */
    if (db_token() == ID)
        db_ntoken();
    else
        strcpy(dbv_tstring,"sdbcur");

    /* open the relation */
    if ((sptr1 = db_ropen(dbv_tstring)) == NULL)
        return (FALSE);
    if ((sptr2 = db_ropen(dbv_tstring)) == NULL) {
        db_rclose(sptr1);
        return (FALSE);
        }
    if ((sptr3 = db_ropen(dbv_tstring)) == NULL) {   /*dns*/
        db_rclose(sptr1);                            /*dns*/
        db_rclose(sptr2);                            /*dns*/
        return (FALSE);                              /*dns*/
        }

    /* checks for "<relation-name> by <sort-list>" */
    if (db_ntoken() != BY)
        return (db_ferror(SYNTAX));
    if ((skeys = get_skeys(sptr1)) == NULL) {
        db_rclose(sptr1);
        db_rclose(sptr2);
        db_rclose(sptr3);  /*dns*/
        return (FALSE);
       }

    /* do the sort */
    result = sort(skeys,sptr1,sptr2,sptr3);  /*dns*/

    /* close the relation */
    db_rclose(sptr1);
    db_rclose(sptr2);
    db_rclose(sptr3);     /*dns*/

    /* free the sort keys */
    free_skeys(skeys);

    return (result);
}

/* free_skeys - free a list of sort keys */
static free_skeys(skeys)
  struct skey *skeys;
{
    struct skey *thisskey;

    for (thisskey = skeys; skeys != NULL; thisskey = skeys) {
        skeys = skeys->sk_next;
        free(thisskey);
    }
}

/* find_attr - find an attribute */
static int find_attr(sptr,newskey,aname)
  struct scan *sptr; struct skey *newskey; char *aname;
{
    struct attribute *aptr;
    int i,astart;

    /* loop through each attribute within the relation */
    astart = 1;
    for (i = 0; i < NATTRS; i++) {

        /* get a pointer to the current attribute */
        aptr = &sptr->sc_relation->rl_header.hd_attrs[i];

        /* check for last attribute */
        if (aptr->at_name[0] == 0)
            break;

        /* check for attribute name match */
        if (db_sncmp(aname,aptr->at_name,ANSIZE) == 0) {
            newskey->sk_start = astart;
            newskey->sk_aptr = aptr;
            return (TRUE);
        }

        /* update the attribute start */
        astart += aptr->at_size;
    }

    /* attribute name not found */
    return (db_ferror(ATUNDF));
}

/* sort - sort the relation */
static int sort(skeys,sptr1,sptr2,sptr3)
  struct skey *skeys; struct scan *sptr1,*sptr2,*sptr3;
{
/*  unsigned int j,k,l,r;          dns */
    long int passes,swaps;           /*dns*/
    int i, j, m, n;         /*dns*/
    int rec1 = 0;           /*dns*/
    int rec2 = 0;           /*dns*/
    int rec3 = 0;           /*dns*/
    int dns = 0;            /*dns*/
    FILE *test;             /*dns*/

    passes = 0L;
    swaps = 0L;

    /*dns --->   */
    test = fopen("sort.dat", "w");
    n = sptr1->sc_relation->rl_tcnt;
    m = n;

    while( m>1 )   {
       passes++;
       if ((m/=3.14159) < 1)  m = 1;
       for ( j=1; j<=n-m; j++ )  {
          if( rec1 != j+m )  {
             if(dns) fprintf(test,"Read1: %d\n", j+m);
             if (!db_rget(sptr1, rec1=j+m))  return (FALSE);
             }
          for ( i=j; i>=1; i-=m ) {
             if( rec2 != i )   {
                if(dns) fprintf(test,"Read2: %d\n", i);
                if (!db_rget(sptr2, rec2=i))  return (FALSE);
                }
             if (compare(skeys,sptr1,sptr2) > 0)
                break;
             if(rec3 != i+m)  {
                if(dns) fprintf(test,"Read3: %d\n", i+m);
                if (!db_rget(sptr3, rec3=i+m))  return (FALSE);
                }
             if(dns) fprintf(test,"Write 3,2: %d from %d\n", i+m, i);
             assign( sptr3, sptr2 );
             swaps++;
             }
          if(rec1 != i+m)  {
             if(rec3 != i+m)  {
                if(dns) fprintf(test,"Read 3: %d\n", i+m);
                if (!db_rget(sptr3, rec3=i+m))  return (FALSE);
                }
             if(dns) fprintf(test,"Write 3,1: %d from %d\n", i+m, j+m);
             assign( sptr3, sptr1 );
             swaps++;
             }
          }
       }
       fclose(test);

/*
    l = 2;
    r = sptr1->sc_relation->rl_tcnt;
    k = r;

    do {
        for (j = r; j >= l; j--) {
            if (!db_rget(sptr1,j-1))
                return (FALSE);
            if (!db_rget(sptr2,j))
                return (FALSE);
            if (compare(skeys,sptr1,sptr2) > 0) {
                swap(sptr1,sptr2);
                k = j;
                swaps++;
            }
        }
        l = k + 1;
        for (j = l; j <= r; j++) {
            if (!db_rget(sptr1,j-1))
                return (FALSE);
            if (!db_rget(sptr2,j))
                return (FALSE);
            if (compare(skeys,sptr1,sptr2) > 0) {
                swap(sptr1,sptr2);
                k = j;
                swaps++;
            }
        }
        r = k - 1;
        passes++;
    } while (l <= r);
*/

    printf("[ Passes: %ld  Swaps: %ld ]\n",passes,swaps);

    return (TRUE);
}

/* compare - compare two tuples */
static int compare(skeys,sptr1,sptr2)
  struct skey *skeys; struct scan *sptr1,*sptr2;
{
    struct skey *cskey;
    int result;

    for (cskey = skeys; cskey != NULL; cskey = cskey->sk_next)
        if ((result = cattr(cskey,sptr1,sptr2)) != 0)
            break;

    return (result);
}

/* cattr - compare two attributes */
static int cattr(cskey,sptr1,sptr2)
  struct skey *cskey; struct scan *sptr1,*sptr2;
{
    int astart,aend,i;

    astart = cskey->sk_start;
    aend = astart + cskey->sk_aptr->at_size;

    for (i = astart; i < aend; i++)
        if (sptr1->sc_tuple[i] != sptr2->sc_tuple[i])
            break;

    if (i == aend)
        return (0);

    if (sptr1->sc_tuple[i] < sptr2->sc_tuple[i])
        if (cskey->sk_type == ASCENDING)
            return (-1);
        else
            return (1);
    else
        if (cskey->sk_type == ASCENDING)
            return (1);
        else
            return (-1);
}

/* swap - swap two tuples */
/* dns
static int swap(sptr1,sptr2)
  struct scan *sptr1,*sptr2;
{
    unsigned int tnum1,tnum2;

    tnum1 = sptr1->sc_atnum;
    tnum2 = sptr2->sc_atnum;

    if (!db_rput(sptr1,tnum2))
        return (FALSE);
    if (!db_rput(sptr2,tnum1))
        return (FALSE);

    return (TRUE);
}
  dns  */


/* assign - assign one tupple to another */
static int assign(sptr1,sptr2)
  struct scan *sptr1,*sptr2;
{
    unsigned int tnum1,tnum2;

    tnum1 = sptr1->sc_atnum;

    if (!db_rput(sptr2,tnum1))
        return (FALSE);

    return (TRUE);
}

