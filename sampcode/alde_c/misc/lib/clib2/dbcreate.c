#include	"libc.h"
#include	"dbase.h"

/***************************************************************
*  db_create(filename)  creates a new  DBASE file  header for  *
* output.  A pointer to the DBASE file description block is    *
* returned if there is sufficient memory.  If not, the ERROR   *
* value is returned.                                           *
*                                                              *
* Richard Hoffbeck                                  (c) 1983   *
***************************************************************/
db_create(filename)
   char	        filename[];
   {int         *inptr ;
    DBASE_FILE  *db_ptr ;   

   /*------- allocate memory for the file desc. block -----*/
   if( (db_ptr=alloc(DB_HDR_SZ)) == ERROR ){
      printf("\nInsufficient HEAP space to create file descriptor");
      printf("\nCalled from db_create");
      exit();
   }

   /*----- initialize the file description block -----*/
   if( (inptr=open(filename,O_TRUNC+O_RDWR)) == ERROR ){
      printf("\nUnable to open file %s",filename);
      printf("Called from db_create");
      return( ERROR );
   }

   clear(db_ptr,DB_HDR_SZ,0) ;
   db_ptr->file_ptr = inptr ;
   db_ptr->version  = 02 ;
   db_ptr->chng_ind = TRUE ;

   return( db_ptr ) ;
}
