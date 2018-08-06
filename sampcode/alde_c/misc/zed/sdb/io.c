/* SDB - relation file I/O routines */

#include "stdio.h"
#include "sdbio.h"

/* global error code variable */
int dbv_errcode;

/* list of currently loaded relation definitions */
static struct relation *relations = NULL;

/* rfind - find the specified relation */
static struct relation *rfind(rname)
  char *rname;
{
    int fd;
    char filename[RNSIZE+5];
    struct relation *rptr;

    /* look for relation in list currently loaded */
    for (rptr = relations; rptr != NULL; rptr = rptr->rl_next)
        if (db_sncmp(rname,rptr->rl_name,RNSIZE) == 0)
            return (rptr);

    /* create a file name */
    make_fname(filename,rname);

    /* lookup the relation file */
#ifdef Lattice
    if ((fd = open(filename,0x8000)) == -1)  /*NAs*/
#else
    if ((fd = open(filename,0)) == -1)
#endif
        return (db_nerror(RELFNF));

    /* allocate a new relation structure */
    if ((rptr = malloc(sizeof(struct relation))) == NULL) {
        close(fd);
        return (db_nerror(INSMEM));
    }

    /* initialize the relation structure */
    rptr->rl_scnref = 0;

    /* read the header block */
    if ( read(fd,&rptr->rl_header,512) != 512) {
        free(rptr);
        close(fd);
        return (db_nerror(BADHDR));
    }

    /* close the relation file */
    close(fd);

    /* extract header information */
    rptr->rl_tcnt = db_cvword(rptr->rl_header.hd_tcnt);
    rptr->rl_tmax = db_cvword(rptr->rl_header.hd_tmax);
    rptr->rl_data = db_cvword(rptr->rl_header.hd_data);
    rptr->rl_size = db_cvword(rptr->rl_header.hd_size);

    /* store the relation name */
    stccpy(rptr->rl_name,rname,RNSIZE);

    /* link new relation into relation list */
    rptr->rl_next = relations;
    relations = rptr;

    /* return the new relation structure pointer */
    return (rptr);
}

/* db_ropen - open a relation file */
struct scan *db_ropen(rname)
  char *rname;
{
    struct relation *rptr;
    struct scan *sptr;
    char filename[RNSIZE+5];

    /* find the relation definition */
    if ((rptr = rfind(rname)) == NULL)
        return (NULL);

    /* allocate a new scan structure */
    if ((sptr = malloc(sizeof(struct scan))) == NULL)
        return (db_nerror(INSMEM));

    /* allocate a tuple buffer */
    if ((sptr->sc_tuple = malloc(rptr->rl_size)) == NULL) {
        free(sptr);
        return (db_nerror(INSMEM));
    }

    /* initialize the scan structure */
    sptr->sc_relation = rptr;           /* store the relation struct addrs */
    sptr->sc_dtnum = 0;                 /* desired tuple (non-existant) */
    sptr->sc_atnum = 0;                 /* actual tuple (non-existant) */
    sptr->sc_store = FALSE;             /* no store done since open */

    /* open relation file if necessary */
    if (rptr->rl_scnref++ == 0) {

        /* create the relation file name */
        make_fname(filename,rname);

        /* open the relation file */
#ifdef Lattice
        if ((rptr->rl_fd = open(filename,0x8002)) == -1) {  /*dns*/
#else
        if ((rptr->rl_fd = open(filename,2)) == -1) {
#endif
            rptr->rl_scnref--;
            free(sptr->sc_tuple); free(sptr);
            return (db_nerror(RELFNF));
        }
    }

    /* return the new scan structure pointer */
    return (sptr);
}

/* db_rclose - close the relation file */
int db_rclose(sptr)
  struct scan *sptr;
{
    struct relation *rptr,*lastrptr;

    /* close relation file if this is the last reference */
    if (--sptr->sc_relation->rl_scnref == 0) {

        /* rewrite header if any stores took place */
        if (sptr->sc_store) {

            /* store the tuple count back in the header */
            db_cvbytes(sptr->sc_relation->rl_tcnt,
                       sptr->sc_relation->rl_header.hd_tcnt);

            /* write the header block */
            lseek(sptr->sc_relation->rl_fd,0L,0);
            if (write(sptr->sc_relation->rl_fd,
                      &sptr->sc_relation->rl_header,512) != 512) {
                close(sptr->sc_relation->rl_fd);
                free(sptr->sc_tuple); free(sptr);
                return (db_ferror(BADHDR));
            }
        }

        /* close the relation file */
        close(sptr->sc_relation->rl_fd);

        /* free the relation header */
        lastrptr = NULL;
        for (rptr = relations; rptr != NULL; rptr = rptr->rl_next) {
            if (rptr == sptr->sc_relation) {
                if (lastrptr == NULL)
                    relations = rptr->rl_next;
                else
                    lastrptr->rl_next = rptr->rl_next;
            }
            lastrptr = rptr;
        }
        free(sptr->sc_relation);
    }

    /* free the scan structure */
    free(sptr->sc_tuple); free(sptr);

    /* return successfully */
    return (TRUE);
}

/* db_rcompress - compress a relation file */
int db_rcompress(sptr)
  struct scan *sptr;
{
    unsigned int next,nextfree,tcnt;

    /* get the last used tuple */
    tcnt = sptr->sc_relation->rl_tcnt;

    /* loop through all of the tuples */
    for (next = nextfree = 1; next <= tcnt; next++) {

        /* read the tuple */
        seek(sptr,next);
        if (read(sptr->sc_relation->rl_fd,
                 sptr->sc_tuple,sptr->sc_relation->rl_size)
                        != sptr->sc_relation->rl_size)
            return (db_ferror(TUPINP));

        /* rewrite the tuple if it is active */
        if (sptr->sc_tuple[0] == ACTIVE) {

            /* rewrite it only if it must move */
            if (next != nextfree) {

                /* write the tuple */
                seek(sptr,nextfree);
                if (write(sptr->sc_relation->rl_fd,
                          sptr->sc_tuple,sptr->sc_relation->rl_size)
                                != sptr->sc_relation->rl_size)
                    return (db_ferror(TUPOUT));
            }

            /* update the next free tuple number */
            nextfree += 1;
        }
    }

    /* update the tuple count */
    sptr->sc_relation->rl_tcnt = nextfree - 1;

    /* remember which tuple is in the buffer */
    sptr->sc_atnum = sptr->sc_relation->rl_tcnt;

    /* reset the desired tuple */
    sptr->sc_dtnum = 0;

    /* remember that the index needs rewriting */
    sptr->sc_store = TRUE;

    /* return successfully */
    return (TRUE);
}

/* db_rbegin - begin scan at first tuple in relation */
db_rbegin(sptr)
  struct scan *sptr;
{
    /* begin with the first tuple in the file */
    sptr->sc_dtnum = 0;
}

/* db_rfetch - fetch the next tuple from the relation file */
int db_rfetch(sptr)
  struct scan *sptr;
{
    /* look for an active tuple */
    while (TRUE) {

        /* check for this being the last tuple */
        if (!db_rget(sptr,sptr->sc_dtnum + 1))
            return (FALSE);

        /* increment the tuple number */
        sptr->sc_dtnum += 1;

        /* return if the tuple found is active */
        if (sptr->sc_tuple[0] == ACTIVE)
            return (TRUE);
    }
}

/* db_rupdate - update the current tuple */
int db_rupdate(sptr)
  struct scan *sptr;
{
    /* make sure the status byte indicates an active tuple */
    sptr->sc_tuple[0] = ACTIVE;

    /* write the tuple */
    return (db_rput(sptr,sptr->sc_atnum));
}

/* db_rdelete - delete the current tuple */
int db_rdelete(sptr)
  struct scan *sptr;
{
    /* make sure the status byte indicates a deleted tuple */
    sptr->sc_tuple[0] = DELETED;

    /* write the tuple */
    return (db_rput(sptr,sptr->sc_atnum));
}

/* db_rstore - store a new tuple */
int db_rstore(sptr)
  struct scan *sptr;
{
    /* make sure there's room for this tuple */
    if (sptr->sc_relation->rl_tcnt == sptr->sc_relation->rl_tmax)
        return (db_ferror(RELFUL));

    /* make sure the status byte indicates an active tuple */
    sptr->sc_tuple[0] = ACTIVE;

    /* write the tuple */
    if (!db_rput(sptr,sptr->sc_relation->rl_tcnt + 1))
        return (FALSE);

    /* update the tuple count */
    sptr->sc_relation->rl_tcnt += 1;

    /* remember that a tuple was stored */
    sptr->sc_store = TRUE;

    /* return successfully */
    return (TRUE);
}

/* db_rget - get a tuple from the relation file */
int db_rget(sptr,tnum)
  struct scan *sptr; unsigned int tnum;
{
    /* check to see if the tuple is already in the buffer */
    if (tnum == sptr->sc_atnum)
        return (TRUE);

    /* check for this being beyond the last tuple */
    if (tnum > sptr->sc_relation->rl_tcnt)
        return (db_ferror(TUPINP));

    /* read the tuple */
    seek(sptr,tnum);
    if (read(sptr->sc_relation->rl_fd,
             sptr->sc_tuple,sptr->sc_relation->rl_size)
                != sptr->sc_relation->rl_size)
        return (db_ferror(TUPINP));

    /* remember which tuple is in the buffer */
    sptr->sc_atnum = tnum;

    /* return successfully */
    return (TRUE);
}

/* db_rput - put a tuple to a relation file */
int db_rput(sptr,tnum)
  struct scan *sptr; unsigned int tnum;
{
    /* check for this being beyond the maximum tuple */
    if (tnum > sptr->sc_relation->rl_tmax)
        return (db_ferror(TUPOUT));

    /* write the tuple */
    seek(sptr,tnum);
    if (write(sptr->sc_relation->rl_fd,
              sptr->sc_tuple,sptr->sc_relation->rl_size)
                != sptr->sc_relation->rl_size)
        return (db_ferror(TUPOUT));

    /* remember which tuple is in the buffer */
    sptr->sc_atnum = tnum;

    /* return successfully */
    return (TRUE);
}

/* seek - seek a tuple in a relation file */
static seek(sptr,tnum)
  struct scan *sptr; unsigned int tnum;
{
    long offset;

    offset = (long) sptr->sc_relation->rl_data +
                ((long) (tnum - 1) * (long) sptr->sc_relation->rl_size);
    lseek(sptr->sc_relation->rl_fd,offset,0);
}

/* make_fname - make a relation name into a file name */
static make_fname(fname,rname)
  char *fname,*rname;
{
    stccpy(fname,rname,RNSIZE); fname[RNSIZE] = 0;
    strcat(fname,".sdb");
}

/* db_nerror - store the error code and return NULL */
int db_nerror(errcode)
  int errcode;
{
    dbv_errcode = errcode;
    return (NULL);
}

/* db_ferror - store the error code and return FALSE */
int db_ferror(errcode)
  int errcode;
{
    dbv_errcode = errcode;
    return (FALSE);
}

/* db_cvword - convert 2 bytes to a word */
int db_cvword(bytes)
  char bytes[2];
{
    return (((bytes[1] & 0377) << 8) + (bytes[0] & 0377));
}

/* db_cvbytes - convert a word to 2 bytes */
db_cvbytes(word,bytes)
  int word; char bytes[2];
{
    bytes[0] = word;
    bytes[1] = word >> 8;
}

