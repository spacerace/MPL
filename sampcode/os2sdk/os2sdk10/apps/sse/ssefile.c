#include <ctype.h>
#include <doscalls.h>
#include <dos.h>
#include <stdlib.h>
#include "ssedefs.h"




/***  readfile - reads file into filebuffer
 *
 *    readfile reads a file using DOSREAD into a file buffer
 *      and then parses out lines and places them into segments
 *      via 'addline'.
 *
 *    readfile (fhandle)
 *
 *    ENTRY    fhandle - handle to file being read
 *
 *    EXIT     rc      - return code of readfile and addline
 *
 *    readfile uses line-feeds('\n') as seperators between lines.
 *      Lines longer than LINESIZE are cut at LINESIZE and continued
 *      on the next line. All non printable characters execpt TABS are
 *      ignored in buildling the line.
 *
 *    EFFECTS  fbuffer    - by reading the file into fbuffer
 *             bytesread  - by setting it to the number of bytes read
 *             TotalLines - by incrementing it for every line
 *             LineTable  - by adding new lines via 'addline'
 *             SegTable   - by using free space via 'addline'
 *                          and by allocating segments via 'alloseg'
 *             TotalSegs  - by incrementing it via 'alloseg'
 *             memory     - by allocating segments via 'alloseg'
 */

short readfile(fhandle)
unsigned short fhandle;
{
    register unsigned short  i, j;
    unsigned char  line[LINESIZE +1];  /* temp buffer for building line */
    short          rc;                 /* return code of DOS */


    for (bytesread = 1, j = 0, rc = 0; (bytesread != 0) && (!rc); ) {

        rc = DOSREAD(fhandle, (char far *)fbuffer,
                      FBUFFSIZE, (unsigned far *)&bytesread);

        if (rc == 0)  {
/*      scan buffer for complete lines */
            for(i =0; (i < bytesread) && (!rc); i++) {
                line[j] = fbuffer[i];

                if (isprint(line[j])) {
                    if (j < (LINESIZE)) {
/*                  continued building line */
                        j++;
                    }
                    else {
/*                  line length greater than line size */
                        rc = (addline(TotalLines, j, line) || !(TotalLines < MAXLINES));
                        ++TotalLines;
                        line[0] = line[j];
                        j = 1;
                    }
                }
                else if (line[j] == '\t') {
/*              fill with spaces to next tab */
                    if ((j/TABSIZE +1) > MAXTABS) {
/*                      TAB makes line greater than LINSIZE */
                        for ( ; j < LINESIZE; j++)
                            line[j] = ' ';
                        rc = (addline(TotalLines, j, line) || !(TotalLines < MAXLINES));
                        ++TotalLines;
                        j = 0;
                    }
                    do {
                        line[j] = ' ';
                        ++j;
                    } while ((j % TABSIZE) != 0);
                }
                else if (line[j] == '\n') {
/*              a complete line */
                    rc = (addline(TotalLines, j, line) || !(TotalLines < MAXLINES));
                    ++TotalLines;
                    j = 0;
                }
            }
            if ( (bytesread < FBUFFSIZE) && (j >0) && (!rc) ) {
/*          line left in buffer */
                rc = (addline(TotalLines, j, line) || !(TotalLines < MAXLINES));
                ++TotalLines;
                j = 0;
            }
        }
    }
    return(rc);
}




/*** savefile - saves file to disk
 *
 *   savefile attempts to write the file to disk.
 *
 *   savefile (fhandle)
 *
 *   ENTRY    fhandle - handle of file to be written.
 *
 *   EXIT     rc      - return code of DOSWRITE and
 *                      (byteswritten != bytes in buffer to write)
 *
 *   savefile fills a buffer with complete lines until the buffer
 *     can not accept anoter complete line, then savefile calls
 *     DOSWRITE with the number of bytes in the buffer. If an
 *     error occurs durning the write or the number bytes in the
 *     buffer does not match the number of bytes written out then
 *     savefile aborts and returns an error code via rc.
 *
 *   EFFECTS  fbuffer  - by filling it with 'lines' from the file
 *            the file - by writing out any changes made to it
 */

short savefile(fhandle)
unsigned short fhandle;
{
    unsigned short byteswritten; /* number written to file - returned by DOS */
    unsigned long  npl;          /* file pointer - not used */
    unsigned short i, j, k;      /* indexes */
    unsigned long  totalbytes;   /* total bytes written to file */
    short          rc;           /* return code of DOS call */

    DOSCHGFILEPTR(fhandle, 0l, 0, &npl); /* move file ptr to start of file */

    totalbytes = rc = 0;

/*  place lines in fbuffer */
    for (i = 0, j = 0; (i < TotalLines) && (!rc); i++) {
        if ((LineTable[i]->linelength) < (FBUFFSIZE - j)) {
            for (k = 0; k < LineTable[i]->linelength; k++, j++)
                fbuffer[j] = LineTable[i]->firstchar[k];
            fbuffer[j] = '\r';
            j++;
            fbuffer[j] = '\n';
            j++;
        }
        else {
/*          fbuffer is full - write to file */
            rc = DOSWRITE(fhandle, (char far *)fbuffer, j,
                     (unsigned far *)&byteswritten);
            rc = ( !(j == byteswritten) || rc);
            totalbytes += (byteswritten);
            j = 0;
        }
    }
    if ((j > 0) && (!rc)) {
/*      lines left in fbuffer - write out to file */
        rc = DOSWRITE(fhandle, (char far *)fbuffer, j,
                 (unsigned far *)&byteswritten);
        rc = ( !(j == byteswritten) || rc);
        totalbytes += (byteswritten);
    }

    if (!rc)   /* no problems writing to the file */
/*         change file size to current file size */
           DOSNEWSIZE(fhandle, totalbytes);
    return(rc);
}




/***  backupfile - make of the file
 *
 *    backupfile creates a backup file for the file being edited
 *      by sse.
 *
 *    backupfile (fname, fhandle)
 *
 *    ENTRY      fhandle  - handle to file to backup
 *               fname    - path name of file to backup
 *
 *    EXIT       rc       - return code of backup
 *
 *    backupfile creates a backup file by replacing the current
 *      extension with .BAK or adding the extension.BAK if none is
 *      given. If the file being edited has the extension .BAK then
 *      backupfile will not create a backup.
 *
 *    EFFECTS    backup file - by creating it
 */

short  backupfile(fname, fhandle)
    char           *fname;
    unsigned short fhandle;
{
    unsigned short bfhandle;    /* handle to backup file */
    unsigned short bytesread;   /* number of bytes read from file fname */
    unsigned short byteswriten; /* number of bytes written to file bfname */
    unsigned char  i, j;        /* indexs */
    unsigned long  npl;         /* file pointer - not used */
    short          rc;          /* return code of backupfile */
    char           bfname[65];  /* pathname of the backup file */

/*  build back up file name - bfname, from fname */
    for (i = 0; ((bfname[i] = fname[i]) != '\0');  i++);
    for (j = 1;
        (bfname[i - j] != '\\') && (bfname[i - j] != '.')
                                && (j < 4) && (j < i);
         j++);
    if ((bfname[i - j] == '.') && ((i - j) != 0))
        i -= j;
    if (bfname[i] != '.')
        bfname[i] = '.';
    bfname[++i] = 'B';
    bfname[++i] = 'A';
    bfname[++i] = 'K';
    bfname[++i] = '\0';


/*  create backup file */
    rc = openfile(bfname, &bfhandle, WCFLAG);
    if (!rc) {
        DOSCHGFILEPTR(fhandle, 0l, 0, &npl); /* file ptr to beginning of fname */
        for (bytesread = 1, byteswriten = 1;
            (bytesread != 0) && !(rc = (bytesread != byteswriten));  )  {
/*         copy fname to bfname */
           DOSREAD(fhandle, (char far *)fbuffer, FBUFFSIZE,
                   (unsigned far *)&bytesread);
           DOSWRITE(bfhandle, (char far *)fbuffer, bytesread,
                    (unsigned far *)&byteswriten);
        }
        closefile(bfhandle);
        if (rc)
/*          delete backup file if there was an error creating it */
            DOSDELETE( (char far *)bfname, 0l);
    }
    return(rc);
}




/***  openfile - opens file to be read
 *
 *    openfile uses DOSOPEN to open or create a file and
 *      return a handle to it.
 *
 *    openfile (fname, fhandle, openflag)
 *
 *    ENTRY    fname    - name of the file
               openflag - is the value of openflag for DOSOPEN
 *
 *    EXIT     fhandle  - file handle
 *             rc       - return code of openfile
 *
 *    openfile has the openmode for DOSOPEN set so it may run as
 *      a bound application. The openflag controls the action
 *      of the open, example: if the file exist then open it
 *      or replace it, or if the file does not exist create it
 *      or fail.
 *
 *    EFFECTS  fhandle - by returning a handle to the file
 *             memory  - by opening a file
 */

short openfile(fname, fhandle, openflag)
char           *fname;
unsigned short *fhandle;
unsigned short openflag;
{
    unsigned short actiontaken = 0;    /* value return by DOS */
    unsigned long  fsize = 0l;         /* set to not change file size (0) */
    unsigned short fattrib  = 0;       /* set to not change file attrib (0) */
    struct flags {
      unsigned access     :3 ;   /* set to read & write access (2) */
      unsigned reserved1  :1 ;   /* must be zero */
      unsigned sharing    :3 ;   /* set to deny write access (2) */
      unsigned inheritance:1 ;   /* set to private (1) */
      unsigned reserved5  :5 ;   /* must be zero */
      unsigned failerrors :1 ;   /* needes to be zero for API */
      unsigned write      :1 ;   /* needes to be zero for API */
      unsigned dasdopen   :1 ;   /* needes to be zero for API */
    };
    static union {
      struct flags   modeflags;
      unsigned short mode;
    } openmode = { { 2,0,2,1,0,0,0,0 } };
    short          rc;         /* return code from DOS call */


/*  Open the file */
    rc = DOSOPEN( (char far *)fname, (unsigned far *)fhandle,
                  (unsigned far *)&actiontaken,
                   fsize, fattrib, openflag,
                   openmode.mode, 0l);
    return(rc);
}




/***  closefile - closes file
 *
 *    closefile uses DOSCLOSE to close the file given by
 *       fhandle.
 *
 *    closefile (fhandle).
 *
 *    ENTRY     fhandle - handle of file to be closed
 *
 *    closefile does not return an error code if one occurs.
 *
 *    EFFECTS   memory   - by closing a file
 *              the file - by closing it
 */

void closefile(fhandle)
unsigned short fhandle;
  {
    short rc;   /* return code of DOS call */

/*  Close the file */
    rc = DOSCLOSE(fhandle);
}
