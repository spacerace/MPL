/* SDB - table output routines */

#include "stdio.h"
#include "sdbio.h"

static char buffer[TABLEMAX+1];
int bndx;

/* db_thead - print a table header */
db_thead(fp,slptr)
  FILE *fp; struct sel *slptr;
{
    struct sattr *saptr;
    int twidth,fwidth,i;
    char *aname;

    /* compute the table width */
    twidth = 1;
    for (saptr = slptr->sl_attrs; saptr != NULL; saptr = saptr->sa_next)
        twidth += saptr->sa_attr->at_size + 3;

    /* print the top line of the table */
    bstart();
    for (i = 0; i < twidth; i++)
        binsert('-');
    bprint(fp);

    /* print the label line of the table */
    bstart();
    for (saptr = slptr->sl_attrs; saptr != NULL; saptr = saptr->sa_next) {
        fwidth = saptr->sa_attr->at_size;
        binsert('|'); binsert(' ');
        if ((aname = saptr->sa_name) == NULL)
            aname = saptr->sa_aname;
        for (i = 0; i < fwidth; i++)
            if (*aname != 0)
                binsert(*aname++);
            else
                binsert(' ');
        binsert(' ');
    }
    binsert('|');
    bprint(fp);

    /* print the line under the labels */
    bstart();
    for (i = 0; i < twidth; i++)
        binsert('-');
    bprint(fp);
}

/* db_tfoot - print a table foot */
db_tfoot(fp,slptr)
  FILE *fp; struct sel *slptr;
{
    struct sattr *saptr;
    int twidth,i;

    /* compute the table width */
    twidth = 1;
    for (saptr = slptr->sl_attrs; saptr != NULL; saptr = saptr->sa_next)
        twidth += saptr->sa_attr->at_size + 3;

    /* print the line at the foot of the table */
    bstart();
    for (i = 0; i < twidth; i++)
        binsert('-');
    bprint(fp);
}

/* db_tentry - print a table entry */
db_tentry(fp,slptr)
  FILE *fp; struct sel *slptr;
{
    struct sattr *saptr;
    int fwidth,i;

    /* print a table entry */
    bstart();
    for (saptr = slptr->sl_attrs; saptr != NULL; saptr = saptr->sa_next) {
        fwidth = saptr->sa_attr->at_size;
        binsert('|'); binsert(' ');
        for (i = 0; i < fwidth; i++)
            if (saptr->sa_aptr[i] != 0)
                binsert(saptr->sa_aptr[i]);
            else
                binsert(' ');
        binsert(' ');
    }
    binsert('|');
    bprint(fp);
}

/* bstart - start building a line */
static bstart()
{
    bndx = 0;
}

/* binsert - insert a character into the buffer */
static binsert(ch)
  int ch;
{
    if (bndx < TABLEMAX)
        buffer[bndx++] = ch;
}

/* bprint - print the current line */
static bprint(fp)
  FILE *fp;
{
    buffer[bndx] = EOS;
    fprintf(fp,"%s\n",buffer);
}

