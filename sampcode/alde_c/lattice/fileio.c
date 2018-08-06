/****************************************************************************/
/*                                                                          */
/* This function was written by Jim Niederriter for the Lattice `C' Compiler*/
/* on the IBM Personal Computer or compatible, running DOS 2.0.             */
/*                                                                          */
/****************************************************************************/
/*  fileio.c  */

/*  This file is a group of several useful I/O functions */

/*  open_fil()  */
/*  this function opens a disk file in untranslated mode. The arguments */
/*  required are the filename pointer, and the mode of operation.  */
/*  It returns the file number.  */
/*  Valid modes are: 0=read, 1=write, 2=read/write  */
/*  NOTE: Untranslated mode is required for binary files, but not for */
/*  ASCII files. Untranslated is specified by turning on bit 15 of    */
/*  the MODE value.  */

int open_fil(filename, mode)
char filename[];
int mode;
{
     int f1;                            /* declare file numberint */
     mode |= 0x8000;                    /* set on bit 15 of mode  */
     f1 = open(filename, mode);
     return(f1);                        /* return file number  */
}Š


/*  get_rec()  */
/*  this function gets a specific record from a random file. The */
/*  required arguments are the file number, file length, relative record */
/*  number, and a pointer to the input data structure.  */
/*  It returns 0 if successful, or 1 if unsuccessful (read past EOF) */

int get_rec(filenum, filelgth, recno, structur)
int filenum, filelgth, recno;
char  structur[];
{
     int stats;
     long offset, rtrn, lseek();

     offset = (long) (recno-1) * filelgth;
     rtrn = lseek(filenum, offset, 0);           
     stats = read(filenum, structur, filelgth);
     if (stats > 0l) {
          return(0);
     }    else {
               return(1);
          }
}

/*  wrt_rec()  */
/*  this function writes a specific record to a random file. The */
/*  required arguments are the file number, file length, relative record */
/*  number, and a pointer to the input data structure.  */
/*  It returns 0 if successful, or 1 if unsuccessful */

int wrt_rec(filenum, filelgth, recno, structur)
int filenum, filelgth, recno;
char structur[];
{
     int stats, c;
     long offset, rtrn, lseek();

     offset = (long) (recno-1) * filelgth;
     rtrn = lseek(filenum, offset, 0);
     stats = write(filenum, structur, filelgth);
     if (stats > 0l) {
          return(0);
     } else {
          return(1);
     }
}

/*  wrt_add()  */
/*  this function appends a record to the end of a sequential file. The  */
/*  required arguments are the file number, file length, and a pointer  */
/*  to the input data structure.  */
/*  It returns 0 if successful, or 1 if unsuccessful */

int wrt_add(filenum, filelgth, structur)
int filenum, filelgth;
char structur[];
{
     int stats, c;
     long rtrn, lseek();

     rtrn = lseek(filenum, 0L, 2);     
     stats = write(filenum, structur, filelgth);
     if (stats > 0l) {
          return(0);
     } else {
          return(1);
     }
}
