#include	"libc.h"
#include	"dbase.h"

/**************************************************************************
*  db_read reads the next n records from the database at db_ptr into the  *
* buffer.  It returns a read error if it is unsucessful.                  *
**************************************************************************/
db_read(db_ptr,buffer,n)
  DBASE_FILE   *db_ptr ;
  char         buffer[] ;
  int          n ;
  {    int  char_transfered, length ;
       length = n * db_ptr->rec_len ;
       char_transfered = read(db_ptr->file_ptr,buffer,length) ;  /* read n records             */
       if( char_transfered == length){                           /* mark the end of the string */
          buffer[char_transfered] = '\0' ;                       /* if the read is successful  */
          db_ptr->curr_rec++ ;                                   /* update the file pointer    */
          return( char_transfered );
          }
       else {
          if( char_transfered == EOF) {
             buffer[0] = '\0' ;                           /* clear out the buffer              */
             return(EOF);                                 /* read error number                 */
             }
          else {
             buffer[char_transfered] = '\0' ;             /* set the end of the buffer         */
             return( char_transfered/db_ptr->rec_len);    /* return the number of records read */
             }
          }
  }
