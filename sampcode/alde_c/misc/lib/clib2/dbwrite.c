
#include  "libc.h"
#include  "dbase.h"


/*************************************************************************
* db_write writes out n records from buffer to the DBASE file mapped at  *
* db_ptr.   It  returns  -1  for an  error and the  number of characters *
* written if the write is sucessful                                      *
*************************************************************************/
db_write(db_ptr,buffer,records)
   DBASE_FILE  *db_ptr;
   int         records;
   char        buffer[];
   {  int      length, char_wrt; 
      char     eofileb = 0x1A ; 

      length = records * db_ptr->rec_len ;
      if( (char_wrt = write(db_ptr->file_ptr,buffer,length )) == length )   /* write successful          */
         db_ptr->curr_rec++ ;                                   /* update the file pointer   */
      if( db_ptr->curr_rec > db_ptr->rec_num ) {
         write(db_ptr->file_ptr,&eofileb,1);                    /* write a new end-of-file    */
         lseek(db_ptr->file_ptr,-1L,1);                         /* and return to position     */
         db_ptr->rec_num = db_ptr->curr_rec ;                   /* update the file descriptor */
         db_ptr->chng_ind = TRUE ;                              /* as necessary               */
      }
      return(char_wrt);
   }
