/* SDB - relation creation routines */

#include "sdbio.h"

/* db_rcreate(rname) - begin the creation of a new relation */
struct relation *db_rcreate(rname)
  char *rname;
{
    struct relation *rptr;

    /* allocate the relation structure */
    if ((rptr = calloc(1,sizeof(struct relation))) == NULL)
        return (db_nerror(INSMEM));

    /* initialize the relation structure */
    stccpy(rptr->rl_name,rname,RNSIZE);
    rptr->rl_tcnt = 0;
    rptr->rl_tmax = 0;
    rptr->rl_data = 512;
    rptr->rl_size = 1;
    rptr->rl_header.hd_attrs[0].at_name[0] = 0;

    /* return the new relation structure pointer */
    return (rptr);
}

/* db_rcheader - create the relation header */
int db_rcheader(rptr)
  struct relation *rptr;
{
    char rname[RNSIZE+1],filename[RNSIZE+13];

    /* initialize the relation file header */
    db_cvbytes(rptr->rl_tcnt,rptr->rl_header.hd_tcnt);
    db_cvbytes(rptr->rl_tmax,rptr->rl_header.hd_tmax);
    db_cvbytes(rptr->rl_data,rptr->rl_header.hd_data);
    db_cvbytes(rptr->rl_size,rptr->rl_header.hd_size);

    /* create the relation file name */
    stccpy(rname,rptr->rl_name,RNSIZE); rname[RNSIZE] = 0;
    sprintf(filename,"%s.sdb",rname);

    /* create the relation file */
    if ((rptr->rl_fd = creat(filename,0)) == -1) {
        free(rptr);
        return (db_ferror(RELCRE));
    }

    /* write the header to the relation file */
    if (write(rptr->rl_fd,&rptr->rl_header,512) != 512) {
        close(rptr->rl_fd);
        free(rptr);
        return (db_ferror(BADHDR));
    }

    /* return successfully */
    return (TRUE);
}

/* db_rctuples - create the relation tuples */
int db_rctuples(rptr,tcnt)
  struct relation *rptr; unsigned int tcnt;
{
    unsigned int i;
    char *tbuf;

    /* store the number of tuples */
    rptr->rl_tmax = tcnt;

    /* allocate a tuple buffer */
    if ((tbuf = calloc(1,rptr->rl_size)) == NULL)
        return (db_ferror(INSMEM));

    /* write null tuples into the file */
    for (i = 0; i < tcnt; i++)
        if (write(rptr->rl_fd,tbuf,rptr->rl_size) != rptr->rl_size) {
            free(tbuf);
            return (db_ferror(INSBLK));
        }

    /* free the tuple buffer */
    free(tbuf);

    /* return successfully */
    return (TRUE);
}

/* db_rcdone(rptr) - finish the creation of a new relation */
int db_rcdone(rptr)
  struct relation *rptr;
{
    /* initialize the relation file header */
    db_cvbytes(rptr->rl_tcnt,rptr->rl_header.hd_tcnt);
    db_cvbytes(rptr->rl_tmax,rptr->rl_header.hd_tmax);

    /* write the header to the relation file */
    lseek(rptr->rl_fd,0L,0);
    if (write(rptr->rl_fd,&rptr->rl_header,512) != 512) {
        close(rptr->rl_fd);
        free(rptr);
        return (db_ferror(BADHDR));
    }

   /* close the relation file */
    close(rptr->rl_fd);

    /* free the relation structure */
    free(rptr);

    /* return successfully */
    return (TRUE);
}

/* db_rcattr(rptr,aname,type,size) - add an attribute to relation being created
*/
int db_rcattr(rptr,aname,type,size)
  struct relation *rptr; char *aname; int type,size;
{
    int i;

    /* look for attribute name */
    for (i = 0; i < NATTRS; i++)
        if (rptr->rl_header.hd_attrs[i].at_name[0] == 0)
            break;
        else if (db_sncmp(aname,rptr->rl_header.hd_attrs[i].at_name,ANSIZE) == 0
)
            return (db_ferror(DUPATT));

    /* check for too many attributes */
    if (i == NATTRS)
        return (db_ferror(MAXATT));

    /* store the new attribute */
    stccpy(rptr->rl_header.hd_attrs[i].at_name,aname,ANSIZE);
    rptr->rl_header.hd_attrs[i].at_type = type;
    rptr->rl_header.hd_attrs[i].at_size = size;

    /* terminate the attribute table */
    if (++i != NATTRS)
        rptr->rl_header.hd_attrs[i].at_name[0] = 0;

    /* update the tuple size */
    rptr->rl_size += size;

    /* return successfully */
    return (TRUE);
}

