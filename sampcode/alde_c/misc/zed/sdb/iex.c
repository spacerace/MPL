/* SDB - import/export command routines */

#include "stdio.h"
#include "sdbio.h"

extern int dbv_token;
extern char dbv_tstring[];
extern int dbv_tvalue;

/* db_import - import tuples from a file */
int *db_import(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9)
  char *fmt;
{
    struct scan *sptr;
    struct attribute *aptr;
    char fname[STRINGMAX+1],avalue[STRINGMAX+1];
    int tcnt,astart,i,eofile;
    FILE *fp;

    /* check for a command line */
    if (fmt != NULL)
        db_scan(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9);

    /* checks for "<filename> into <relation-name>" */
    if (db_ntoken() == ID)
        strcat(dbv_tstring,".dat");
    else if (dbv_token != STRING)
        return (db_ferror(SYNTAX));
    strcpy(fname,dbv_tstring);
    if (db_ntoken() != INTO)
        return (db_ferror(SYNTAX));
    if (db_ntoken() != ID)
        return (db_ferror(SYNTAX));

    /* open the relation */
    if ((sptr = db_ropen(dbv_tstring)) == NULL)
        return (FALSE);

    /* open the input file */
    if ((fp = fopen(fname,"r")) == NULL)
        return (db_ferror(INPFNF));

    /* import tuples */
    eofile = FALSE;
    for (tcnt = 0; ; tcnt++) {

        /* get attribute values */
        astart = 1;
        for (i = 0; i < NATTRS; i++) {

            /* get a pointer to the current attribute */
            aptr = &sptr->sc_relation->rl_header.hd_attrs[i];

            /* check for the last attribute */
            if (aptr->at_name[0] == 0)
                break;

            /* input the tuple */
            if (fgets(avalue,STRINGMAX,fp) == 0) {
                eofile = TRUE;
                break;
            }
            avalue[strlen(avalue)-1] = EOS;

            /* store the attribute value */
            db_aput(aptr,&sptr->sc_tuple[astart],avalue);

            /* update the attribute start */
            astart += aptr->at_size;
        }

        /* store the new tuple */
        if (!eofile) {
            if (!db_rstore(sptr)) {
                db_rclose(sptr);
                return (FALSE);
            }
        }
        else
            break;
    }

    /* close the relation */
    db_rclose(sptr);

    /* close the input file */
    fclose(fp);

    /* check number of tuples imported */
    if (tcnt != 0) {

        /* print tuple count */
        printf("[ %d imported ]\n",tcnt);
    }
    else
        printf("[ none imported ]\n");

    /* return successfully */
    return (TRUE);
}

/* db_export - export tuples to a file */
int *db_export(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9)
  char *fmt;
{
    struct scan *sptr;
    struct attribute *aptr;
    char rname[STRINGMAX+1],avalue[STRINGMAX+1];
    int tcnt,astart,i;
    FILE *fp;

    /* check for a command line */
    if (fmt != NULL)
        db_scan(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9);

    /* checks for "<relation-name> [ into <filename> ]" */
    if (db_ntoken() != ID)
        return (db_ferror(SYNTAX));
    strcpy(rname,dbv_tstring);
    if (!db_to(&fp,".dat"))
        return (FALSE);

    /* open the relation */
    if ((sptr = db_ropen(rname)) == NULL)
        return (FALSE);

    /* export tuples */
    for (tcnt = 0; db_rfetch(sptr); tcnt++) {

        /* get attribute values */
        astart = 1;
        for (i = 0; i < NATTRS; i++) {

            /* get a pointer to the current attribute */
            aptr = &sptr->sc_relation->rl_header.hd_attrs[i];

            /* check for the last attribute */
            if (aptr->at_name[0] == 0)
                break;

            /* get the attribute value */
            db_aget(aptr,&sptr->sc_tuple[astart],avalue);

            /* output the tuple */
            fprintf(fp,"%s\n",avalue);

            /* update the attribute start */
            astart += aptr->at_size;
        }
    }

    /* close the relation */
    db_rclose(sptr);

    /* close the output file */
    if (fp != stdout)
        fclose(fp);

    /* check number of tuples exported */
    if (tcnt != 0) {

        /* print tuple count */
        printf("[ %d exported ]\n",tcnt);
    }
    else
        printf("[ none exported ]\n");

    /* return successfully */
    return (TRUE);
}

/* db_squeeze - squeeze deleted tuples from a relation file */
int *db_squeeze(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9)
  char *fmt;
{
    struct scan *sptr;

    /* check for a command line */
    if (fmt != NULL)
        db_scan(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9);

    /* checks for "<relation-name>" */
    if (db_ntoken() != ID)
        return (db_ferror(SYNTAX));

    /* open the relation */
    if ((sptr = db_ropen(dbv_tstring)) == NULL)
        return (FALSE);

    /* compress the relation file */
    if (!db_rcompress(sptr)) {
        db_rclose(sptr);
        return (FALSE);
    }

    /* close the relation */
    db_rclose(sptr);

    /* return successfully */
    return (TRUE);
}

/* db_extract - extract a relation definition */
int *db_extract(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9)
  char *fmt;
{
    struct scan *sptr;
    struct attribute *aptr;
    char rname[STRINGMAX+1],aname[ANSIZE+1],*atype;
    int i;
    FILE *fp;

    /* check for a command line */
    if (fmt != NULL)
        db_scan(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9);

    /* checks for "<relation-name> [ into <filename> ]" */
    if (db_ntoken() != ID)
        return (db_ferror(SYNTAX));
    strcpy(rname,dbv_tstring);
    if (!db_to(&fp,".def"))
        return (FALSE);

    /* open the relation */
    if ((sptr = db_ropen(rname)) == NULL)
        return (FALSE);

    /* output the relation definition */
    fprintf(fp,"create %s (\n",rname);

    /* get attribute values */
    for (i = 0; i < NATTRS; i++) {

        /* get a pointer to the current attribute */
        aptr = &sptr->sc_relation->rl_header.hd_attrs[i];

        /* check for the last attribute */
        if (aptr->at_name[0] == 0)
            break;

        /* get the attribute name */
        stccpy(aname,aptr->at_name,ANSIZE); aname[ANSIZE] = 0;

        /* determine the attribute type */
        switch (aptr->at_type) {
        case TCHAR:
                atype = "char";
                break;
        case TNUM:
                atype = "num";
                break;
        default:
                atype = "<error>";
                break;
        }

        /* output the attribute definition */
        if (strlen(aname) < 8)
            fprintf(fp,"\t%s\t\t%s\t%d\n",aname,atype,aptr->at_size);
        else
            fprintf(fp,"\t%s\t%s\t%d\n",aname,atype,aptr->at_size);
    }

    /* output the relation size */
    fprintf(fp,") %d\n",sptr->sc_relation->rl_tmax);

    /* close the relation */
    db_rclose(sptr);

    /* close the output file */
    if (fp != stdout)
        fclose(fp);

    /* return successfully */
    return (TRUE);
}

