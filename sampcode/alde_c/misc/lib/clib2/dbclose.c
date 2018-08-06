#include  "libc.h"
#include  "dbase.h"


/***************************************************************
*  db_close(db_ptr) closes the dbase file pointed to by db_ptr *
* if the file has been changed, dbclose updates the dbase head-*
* er before closing the file.  Any  errors are  passed back to *
* the calling routine.                                         *
***************************************************************/
db_close(db_ptr)
   DBASE_FILE  *db_ptr ;
   {int        inptr ;
   
   inptr = db_ptr->file_ptr ;         /* save the file pointer */
   
   /*----- Update the file description block if required -----*/
   if( db_ptr->chng_ind == TRUE ) {
      lseek(db_ptr->file_ptr,0L,0) ;
      write(db_ptr->file_ptr,db_ptr,DB_HDR_SZ) ;
   } 
   return( close(inptr) ) ;                        /* save the file and return */
}
