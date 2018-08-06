
#include  "libc.h"
#include  "dbase.h"


/***************************************************************
*  db_dupl(dp_ptr) duplicates a DBASE file  description  block *
* and returns a pointer to the block.  It aborts the processing*
* if there is insufficient memory to create the block.         *
***************************************************************/
db_dupl(filename,db_ptr)
   DBASE_FILE  *db_ptr ;
   char        filename[] ;    
   {DBASE_FILE *db_new ;
   
   /*----- open the new file and allocate file descriptor space -----*/
   if( (db_new=db_open(filename)) == ERROR ) {
      printf("\nInsufficient HEAP space to create the block");
      printf("\nCalled from db_dupl");
   }

   /*----- there was enough space, copy the old block and set */
   /*----- the change indicator                               */
   blockmv(db_ptr,db_new,DB_HDR_SZ);
   db_new->chng_ind = TRUE ;

   return(db_new) ;

}
