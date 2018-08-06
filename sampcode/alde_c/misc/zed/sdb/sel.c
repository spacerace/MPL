/* SDB - select data from the database */

#include "sdbio.h"

extern int dbv_token;
extern char dbv_tstring[];
extern int dbv_tvalue;

/* db_select - select a set of tuples from a set of relations */
struct sel *db_select(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9)
  char *fmt;
{
    struct sel *slptr;

    /* check for a command line */
    if (fmt != NULL)
        db_scan(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9);

    /* allocate a sel structure */
    if ((slptr = malloc(sizeof(struct sel))) == NULL)
        return (db_nerror(INSMEM));

    /* initialize the structure */
    slptr->sl_rels = NULL;
    slptr->sl_attrs = NULL;
    slptr->sl_where = NULL;
    slptr->sl_bindings = NULL;

    /* parse the list of selected attributes */
    if (!get_sattrs(slptr)) {
        db_done(slptr);
        return (NULL);
    }

    /* check for "from" clause */
    if (db_token() == FROM) {
        db_ntoken();
        if (!get_srels(slptr)) {
            db_done(slptr);
            return (NULL);
        }
    }
    else {
        if (!srelation(slptr,"sdbcur",NULL)) {
            db_done(slptr);
            return (NULL);
        }
    }

    /* check the list of selected attributes */
    if (!check_attrs(slptr)) {
        db_done(slptr);
        return (NULL);
    }

    /* check for the existance of a "where" clause */
    if (db_token() == WHERE) {
        db_ntoken();

        /* parse the boolean expression */
        if (!db_compile(slptr)) {
            db_done(slptr);
            return (FALSE);
        }
    }

    /* return the new selection structure */
    return (slptr);
}

/* db_retrieve - retrieve a set of tuples from a set of relations */
struct sel *db_retrieve(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9)
  char *fmt;
{
    struct sel *slptr;

    /* check for a command line */
    if (fmt != NULL)
        db_scan(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9);

    /* allocate a sel structure */
    if ((slptr = malloc(sizeof(struct sel))) == NULL)
        return (db_nerror(INSMEM));

    /* initialize the structure */
    slptr->sl_rels = NULL;
    slptr->sl_attrs = NULL;
    slptr->sl_where = NULL;
    slptr->sl_bindings = NULL;

    /* check for selected relations clause */
    if (db_token() == ID) {
        if (!get_srels(slptr)) {
            db_done(slptr);
            return (NULL);
        }
    }
    else {
        if (!srelation(slptr,"sdbcur",NULL)) {
            db_done(slptr);
            return (NULL);
        }
    }

    /* check the list of selected attributes */
    if (!check_attrs(slptr)) {
        db_done(slptr);
        return (NULL);
    }

    /* check for the existance of a "where" clause */
    if (db_token() == WHERE) {
        db_ntoken();

        /* parse the boolean expression */
        if (!db_compile(slptr)) {
            db_done(slptr);
            return (FALSE);
        }
    }

    /* return the new selection structure */
    return (slptr);
}

/* db_done(slptr) - finish a selection */
db_done(slptr)
  struct sel *slptr;
{
    struct sattr *saptr,*nxtsa;
    struct srel *srptr,*nxtsr;
    struct binding *bdptr,*nxtbd;

    /* free the selected attribute blocks */
    for (saptr = slptr->sl_attrs; saptr != NULL; saptr = nxtsa) {
        nxtsa = saptr->sa_next;
        if (saptr->sa_rname != NULL)
            free(saptr->sa_rname);
        free(saptr->sa_aname);
        if (saptr->sa_name != NULL)
            free(saptr->sa_name);
        free(saptr);
    }

    /* close the scans and free the selected relation blocks */
    for (srptr = slptr->sl_rels; srptr != NULL; srptr = nxtsr) {
        nxtsr = srptr->sr_next;
        if (srptr->sr_name != NULL)
            free(srptr->sr_name);
        db_rclose(srptr->sr_scan);
        free(srptr);
    }

    /* free the where clause */
    db_fcode(slptr);

    /* free the user bindings */
    for (bdptr = slptr->sl_bindings; bdptr != NULL; bdptr = nxtbd) {
        nxtbd = bdptr->bd_next;
        free(bdptr);
    }

    /* free the selection structure */
    free(slptr);
}

/* db_fetch(slptr) - fetch the next tuple from a selection */
int db_fetch(slptr)
  struct sel *slptr;
{
    struct srel *srptr;
    struct binding *bdptr;

    /* clear the update flags */
    for (srptr = slptr->sl_rels; srptr != NULL; srptr = srptr->sr_next)
        srptr->sr_update = FALSE;

    /* find a matching tuple */
    while (process(slptr->sl_rels))
        if (db_interpret(slptr)) {
            for (bdptr = slptr->sl_bindings; bdptr != NULL; bdptr = bdptr->bd_next)
                db_aget(bdptr->bd_attr,bdptr->bd_vtuple,bdptr->bd_vuser);
            return (TRUE);
        }

    /* no matches, failure return */
    return (FALSE);
}

/* db_update - update modified tuples */
int db_update(slptr)
  struct sel *slptr;
{
    struct srel *srptr;

    /* check each selected relation for updates */
    for (srptr = slptr->sl_rels; srptr != NULL; srptr = srptr->sr_next)
        if (srptr->sr_update)
            if (!db_rupdate(srptr->sr_scan))
                return (FALSE);

    /* return successfully */
    return (TRUE);
}

/* db_store - store tuples */
int db_store(slptr)
  struct sel *slptr;
{
    struct srel *srptr;

    /* check each selected relation for stores */
    for (srptr = slptr->sl_rels; srptr != NULL; srptr = srptr->sr_next)
        if (srptr->sr_update)
            if (!db_rstore(srptr->sr_scan))
                return (FALSE);

    /* return successfully */
    return (TRUE);
}

/* db_bind - bind a user buffer to the value of an attribute */
int db_bind(slptr,rname,aname,avalue)
  struct sel *slptr; char *rname,*aname,*avalue;
{
    struct binding *newbd;
    struct srel *srptr;

    /* allocate and initialize a binding structure */
    if ((newbd = malloc(sizeof(struct binding))) == NULL)
        return (db_ferror(INSMEM));
    newbd->bd_vuser = avalue;

    /* find the attribute */
    if (!find_attr(slptr,rname,aname,&newbd->bd_vtuple,&srptr,&newbd->bd_attr))
        return (FALSE);

    /* link the new binding into the binding list */
    newbd->bd_next = slptr->sl_bindings;
    slptr->sl_bindings = newbd;

    /* return successfully */
    return (TRUE);
}

/* db_get - get the value of an attribute */
int db_get(slptr,rname,aname,avalue)
  struct sel *slptr; char *rname,*aname,*avalue;
{
    struct srel *srptr;
    struct attribute *aptr;
    char *vptr;

    /* find the attribute */
    if (!find_attr(slptr,rname,aname,&vptr,&srptr,&aptr))
        return (FALSE);

    /* get the attribute value */
    db_aget(aptr,vptr,avalue);

    /* return successfully */
    return (TRUE);
}

/* db_put - put the value of an attribute */
int db_put(slptr,rname,aname,avalue)
  struct sel *slptr; char *rname,*aname,*avalue;
{
    struct srel *srptr;
    struct attribute *aptr;
    char *vptr;

    /* find the attribute */
    if (!find_attr(slptr,rname,aname,&vptr,&srptr,&aptr))
        return (FALSE);

    /* put the attribute value */
    db_aput(aptr,vptr,avalue);

    /* mark the tuple as updated */
    srptr->sr_update = TRUE;

    /* return successfully */
    return (TRUE);
}

/* db_sattr - get selected attribute type, pointer, and length */
int db_sattr(slptr,rname,aname,ptype,pptr,plen)
  struct sel *slptr; char *rname,*aname;
  int *ptype; char **pptr; int *plen;
{
    struct srel *srptr;
    struct attribute *aptr;

    if (!find_attr(slptr,rname,aname,pptr,&srptr,&aptr))
        return (FALSE);
    *ptype = aptr->at_type;
    *plen = aptr->at_size;
    return (TRUE);
}

/* get_sattrs(slptr) - get selected attributes */
static get_sattrs(slptr)
  struct sel *slptr;
{
    struct sattr *newsattr,*lastsattr;

    /* check for "*" or blank field meaning all attributes are selected */
    if (db_token() == '*') {
        db_ntoken();
        return (TRUE);
    }
    else if (db_token() != ID)
        return (TRUE);

    /* parse a list of attribute names */
    lastsattr = NULL;
    while (TRUE) {

        /* get attribute name */
        if (db_ntoken() != ID)
            return (db_ferror(SYNTAX));

        /* allocate a selected attribute structure */
        if ((newsattr = malloc(sizeof(struct sattr))) == NULL)
            return (db_ferror(INSMEM));

        /* initialize the selected attribute structure */
        newsattr->sa_next = NULL;

        /* save the attribute name */
        if ((newsattr->sa_aname = malloc(strlen(dbv_tstring)+1)) == NULL) {
            free(newsattr);
            return (db_ferror(INSMEM));
        }
        strcpy(newsattr->sa_aname,dbv_tstring);

        /* check for "." meaning "<rel-name>.<att-name>" */
        if (db_token() == '.') {
            db_ntoken();

            /* the previous ID was really the relation name */
            newsattr->sa_rname = newsattr->sa_aname;

            /* check for attribute name */
            if (db_ntoken() != ID) {
                free(newsattr->sa_aname); free(newsattr);
                return (db_ferror(SYNTAX));
            }

            /* save the attribute name */
            if ((newsattr->sa_aname = malloc(strlen(dbv_tstring)+1)) == NULL) {
                free(newsattr->sa_aname); free(newsattr);
                return (db_ferror(INSMEM));
            }
            strcpy(newsattr->sa_aname,dbv_tstring);
        }
        else
            newsattr->sa_rname = NULL;

        /* check for alternate attribute name */
        if (db_token() == ID) {
            db_ntoken();

            /* allocate space for the alternate name */
            if ((newsattr->sa_name = malloc(strlen(dbv_tstring)+1)) == NULL) {
                if (newsattr->sa_rname != NULL)
                    free(newsattr->sa_rname);
                free(newsattr->sa_aname);
                free(newsattr);
                return (db_ferror(INSMEM));
            }
            strcpy(newsattr->sa_name,dbv_tstring);
        }
        else
            newsattr->sa_name = NULL;

        /* link the selected attribute structure into the list */
        if (lastsattr == NULL)
            slptr->sl_attrs = newsattr;
        else
            lastsattr->sa_next = newsattr;
        lastsattr = newsattr;

        /* check for more attributes */
        if (db_token() != ',')
            break;
        db_ntoken();
    }

    /* return successfully */
    return (TRUE);
}

/* get_srels(slptr) - get selected relations */
static get_srels(slptr)
  struct sel *slptr;
{
    char rname[KEYWORDMAX+1],*aname;

    /* get the list of selected relations */
    while (TRUE) {

        /* check for relation name */
        if (db_ntoken() != ID)
            return (db_ferror(SYNTAX));
        strcpy(rname,dbv_tstring);

        /* check for alternate relation name */
        if (db_token() == ID) {
            db_ntoken();
            aname = dbv_tstring;
        }
        else
            aname = NULL;

        /* add the relation name to the list */
        if (!srelation(slptr,rname,aname))
            return (FALSE);

        /* check for more selected relations */
        if (db_token() != ',')
            break;
        db_ntoken();
    }

    /* return successfully */
    return (TRUE);
}

/* srelation - select a relation */
static srelation(slptr,rname,aname)
  struct sel *slptr; char *rname,*aname;
{
    struct srel *srptr,*newsrel;

    /* allocate a new selected relation structure */
    if ((newsrel = malloc(sizeof(struct srel))) == NULL)
        return (db_ferror(INSMEM));

    /* initialize the new selected relation structure */
    newsrel->sr_ctuple = FALSE;
    newsrel->sr_update = FALSE;
    newsrel->sr_next = NULL;

    /* open the relation */
    if ((newsrel->sr_scan = db_ropen(rname)) == NULL) {
        free(newsrel);
        return (FALSE);
    }

    /* check for alternate relation name */
    if (aname != NULL) {

        /* allocate space for the alternate name */
        if ((newsrel->sr_name = malloc(strlen(aname)+1)) == NULL) {
            free(newsrel);
            return (db_ferror(INSMEM));
        }
        strcpy(newsrel->sr_name,aname);
    }
    else
        newsrel->sr_name = NULL;

    /* find the end of the list of relation names */
    for (srptr = slptr->sl_rels; srptr != NULL; srptr = srptr->sr_next)
        if (srptr->sr_next == NULL)
            break;

    /* link the new selected relation structure into the list */
    if (srptr == NULL)
        slptr->sl_rels = newsrel;
    else
        srptr->sr_next = newsrel;

    /* return successfully */
    return (TRUE);
}

/* check_attrs(slptr) - check the list of selected attributes */
static int check_attrs(slptr)
  struct sel *slptr;
{
    struct sattr *saptr;

    /* check for all attributes selected */
    if (slptr->sl_attrs == NULL)
        return (all_attrs(slptr));

    /* check each selected attribute */
    for (saptr = slptr->sl_attrs; saptr != NULL; saptr = saptr->sa_next)
        if (!find_attr(slptr,saptr->sa_rname,saptr->sa_aname,
                        &saptr->sa_aptr,&saptr->sa_srel,&saptr->sa_attr))
            return (FALSE);

    /* return successfully */
    return (TRUE);
}

/* all_attrs(slptr) - create a list of all attributes */
static int all_attrs(slptr)
  struct sel *slptr;
{
    struct sattr *newsattr,*lastsattr;
    struct srel *srptr;
    struct attribute *aptr;
    int i,astart;

    /* loop through each selected relation */
    lastsattr = NULL;
    for (srptr = slptr->sl_rels; srptr != NULL; srptr = srptr->sr_next) {

        /* loop through each attribute within the relation */
        astart = 1;
        for (i = 0; i < NATTRS; i++) {

            /* get a pointer to the current attribute */
            aptr = &srptr->sr_scan->sc_relation->rl_header.hd_attrs[i];

            /* check for last attribute */
            if (aptr->at_name[0] == 0)
                break;

            /* allocate a new selected attribute structure */
            if ((newsattr = malloc(sizeof(struct sattr))) == NULL)
                return (db_ferror(INSMEM));

            /* initialize the new selected attribute structure */
            newsattr->sa_name = NULL;
            newsattr->sa_srel = srptr;
            newsattr->sa_aptr = srptr->sr_scan->sc_tuple + astart;
            newsattr->sa_attr = aptr;
            newsattr->sa_next = NULL;

            /* save the relation name */
            if ((newsattr->sa_rname = malloc(RNSIZE+1)) == NULL) {
                free(newsattr);
                return (db_ferror(INSMEM));
            }
            stccpy(newsattr->sa_rname,
                    srptr->sr_scan->sc_relation->rl_name,
                    RNSIZE);
            newsattr->sa_rname[RNSIZE] = 0;

            /* save the attribute name */
            if ((newsattr->sa_aname = malloc(ANSIZE+1)) == NULL) {
                free(newsattr->sa_rname);
                free(newsattr);
                return (db_ferror(INSMEM));
            }
            stccpy(newsattr->sa_aname,
                    srptr->sr_scan->sc_relation->rl_header.hd_attrs[i].at_name,
                    ANSIZE);
            newsattr->sa_aname[ANSIZE] = 0;

            /* link the selected attribute into the list */
            if (lastsattr == NULL)
                slptr->sl_attrs = newsattr;
            else
                lastsattr->sa_next = newsattr;
            lastsattr = newsattr;

            /* update the attribute start */
            astart += aptr->at_size;
        }
    }

    /* return successfully */
    return (TRUE);
}

/* find_attr - find a named attribute */
static int find_attr(slptr,rname,aname,paptr,psrel,pattr)
  struct sel *slptr; char *rname,*aname;
  char **paptr; struct attribute **pattr;
{
    /* check for unqualified or qualified attribute names */
    if (rname == NULL)
        return (uattr(slptr,aname,paptr,psrel,pattr));
    else
        return (qattr(slptr,rname,aname,paptr,psrel,pattr));
}

/* uattr - find an unqualified attribute name */
static int uattr(slptr,aname,paptr,psrel,pattr)
  struct sel *slptr; char *aname;
  char **paptr; struct srel **psrel; struct attribute **pattr;
{
    struct srel *srptr;
    struct attribute *aptr;
    int i,astart;

    /* loop through each selected relation */
    *pattr = NULL;
    for (srptr = slptr->sl_rels; srptr != NULL; srptr = srptr->sr_next) {

        /* loop through each attribute within the relation */
        astart = 1;
        for (i = 0; i < NATTRS; i++) {

            /* get a pointer to the current attribute */
            aptr = &srptr->sr_scan->sc_relation->rl_header.hd_attrs[i];

            /* check for last attribute */
            if (aptr->at_name[0] == 0)
                break;

            /* check for attribute name match */
            if (db_sncmp(aname,aptr->at_name,ANSIZE) == 0) {
                if (*pattr != NULL)
                    return (db_ferror(ATAMBG));
                *paptr = srptr->sr_scan->sc_tuple + astart;
                *psrel = srptr;
                *pattr = aptr;
            }

            /* update the attribute start */
            astart += aptr->at_size;
        }
    }

    /* check whether attribute was found */
    if (*pattr == NULL)
        return (db_ferror(ATUNDF));

    /* return successfully */
    return (TRUE);
}

/* qattr - find a qualified attribute name */
static int qattr(slptr,rname,aname,paptr,psrel,pattr)
  struct sel *slptr; char *rname,*aname;
  char **paptr; struct srel **psrel; struct attribute **pattr;
{
    struct srel *srptr;
    struct attribute *aptr;
    char *crname;
    int i,astart;

    /* loop through each selected relation */
    for (srptr = slptr->sl_rels; srptr != NULL; srptr = srptr->sr_next) {

        /* get relation name */
        if ((crname = srptr->sr_name) == NULL)
            crname = srptr->sr_scan->sc_relation->rl_name;

        /* check for relation name match */
        if (db_sncmp(rname,crname,RNSIZE) == 0) {

            /* loop through each attribute within the relation */
            astart = 1;
            for (i = 0; i < NATTRS; i++) {

                /* get a pointer to the current attribute */
                aptr = &srptr->sr_scan->sc_relation->rl_header.hd_attrs[i];

                /* check for last attribute */
                if (aptr->at_name[0] == 0)
                    break;

                /* check for attribute name match */
                if (db_sncmp(aname,aptr->at_name,ANSIZE) == 0) {
                    *paptr = srptr->sr_scan->sc_tuple + astart;
                    *psrel = srptr;
                    *pattr = aptr;
                    return (TRUE);
                }

                /* update the attribute start */
                astart += aptr->at_size;
            }

            /* attribute name not found */
            return (db_ferror(ATUNDF));
        }
    }

    /* relation name not found */
    return (db_ferror(RLUNDF));
}

/* process(srptr) - process each tuple in a relation cross-product */
static int process(srptr)
  struct srel *srptr;
{
    /* always get a new tuple if this is the last relation in the list */
    if (srptr->sr_next == NULL) {

        /* check for beginning of new scan */
        if (!srptr->sr_ctuple)
            db_rbegin(srptr->sr_scan);

        /* return the next tuple in the relation */
        return (srptr->sr_ctuple = db_rfetch(srptr->sr_scan));
    }

    /* check for beginning of new scan */
    if (!srptr->sr_ctuple) {
        db_rbegin(srptr->sr_scan);

        /* get the first tuple */
        if (!db_rfetch(srptr->sr_scan))
            return (FALSE);
    }

    /* look for a match with the remaining relations in list */
    while (!process(srptr->sr_next))

        /* get the next tuple in the scan */
        if (!db_rfetch(srptr->sr_scan))
            return (srptr->sr_ctuple = FALSE);

    /* found a match at this level */
    return (srptr->sr_ctuple = TRUE);
}

/* db_aget - get the value of an attribute */
db_aget(aptr,vptr,avalue)
  struct attribute *aptr; char *vptr,*avalue;
{
    int i;

    /* get the attribute value */
    for (i = 0; i < aptr->at_size; i++)
        *avalue++ = vptr[i];
    *avalue = EOS;
}

/* db_aput - put the value of an attribute */
db_aput(aptr,vptr,avalue)
  struct attribute *aptr; char *vptr,*avalue;
{
    int i;

    /* initialize counter */
    i = 0;

    /* right justify numbers */
    if (aptr->at_type == TNUM)
        for (; i < aptr->at_size - strlen(avalue); i++)
            vptr[i] = ' ';

    /* put the attribute value */
    for (; i < aptr->at_size; i++)
        if (*avalue == 0)
            vptr[i] = 0;
        else
            vptr[i] = *avalue++;
}

