#include  "libc.h"
#include  "dbase.h"



/***************************************************************************
*  db_open(filename) opens the DBASE .DBF file contained in "filename"     *
* and returns a pointer to the block containing the DBASE file discription *
* block.  A value of -1 is returned if the file doesn't exist and execution*
* is terminated if there is insufficient memory availible to allocate the  *
* file descriptor block                                                    *
***************************************************************************/
db_open(filename){          
   int         i, *inptr, variables ;
   DBASE_FILE  *db_pointer ;

   /*----- allocate the storage area -----*/
   if( (db_pointer = alloc(DB_HDR_SZ+5)) == 0 ){
     puts("Insufficient HEAP space to open DBASE file description block");
     puts("Called from db_open");
     exit();
     }

   if( (inptr=open(filename,O_RDWR)) == ERROR)  /* open the file */
      return(inptr);                            /* return the errror if not found */
   read(inptr,db_pointer,DB_HDR_SZ);            /* read in the header information */
                                                /* reset the number of variables */
   variables = 0;
   while(db_pointer->field_desc[variables].var_len != 0 ) variables++;

   /*----- fill in the rest of the file descriptor and return -----*/
   db_pointer->file_ptr = inptr;
   db_pointer->var_num  = variables ;
   db_pointer->chng_ind = FALSE ;
   lseek(inptr,DATA_START,0);                   /* set to the beginning of data */
   db_pointer->curr_rec = 0 ;  

   return(db_pointer);
   }

