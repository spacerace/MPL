
#include  "libc.h"
#include  "dbase.h"


/*************************************************************************
* db_seek positions the read/write pointer to the ith record in the data *
* base.  It also works with an offset so that the pointer can be moved   *
* back or forward n records from the current position, the beginning  or *
* end of the file.  If db_seek is sucessful it returns the current posi- *
* tion in the file; otherwise, it returns a -1.  The offset codes are as *
* follows:                                                               *
*                                                                        *
*          OFFSET                             MEANING                    *
*          ------                   ------------------------------       *
*                                                                        *
*            0                       offset from the beginning of the    *
*                                    data base. zero or + offset only    *
*                                                                        *
*            1                       offset from the current position    *
*                                    in the database. zero, + or - offst *
*                                    allowed                             *
*                                                                        *
*            2                       offset from the end of the database *
*                                    zero or - offset only               *
*                                                                        *
*************************************************************************/
db_seek(db_ptr,rcrds,origin)
   DBASE_FILE  *db_ptr ;
   int         rcrds, origin ;
   {long       offset, char_pos, lseek();

      /*----- calculate the character position offset for the "records" position -----*/ 
      switch ( origin  )
        {
         case 0 :
           if( rcrds < 1 )
              return( ERROR );
           else {
              offset = (long) (DATA_START + (rcrds-1) * db_ptr->rec_len) ;     
              break;
           }

         case 1 :
           offset =  (long) ( rcrds * db_ptr->rec_len ) ;
           if( (offset > 0) && (db_ptr->curr_rec + offset/db_ptr->rec_len) > db_ptr->rec_num )
              return( ERROR );          /* tried to get past end of file      */
           else if( (offset < 0) && (db_ptr->curr_rec + offset/db_ptr->rec_len) < 0 )
                   return( ERROR );     /* tried to get past beginning of file */
           break;

         case 2 :
           if( rcrds > 0 )
             return( ERROR );               /* attempt to offset past end-of-file */
           else {
             offset = (long) DATA_START + (db_ptr->rec_num + rcrds ) * db_ptr->rec_len ;  
             origin = 0 ;                   /* reset origin since offset is from beginning of file */
             break;
             }

         default :
           return ( ERROR );                /* undefined origin value */
        }

      /*----- return the record number -----*/
      if( (char_pos = lseek(db_ptr->file_ptr,offset,origin )) == ERROR ){
         db_seek(db_ptr,db_ptr->curr_rec,0);          /* reset the file pointer */
         return(char_pos);
         }
      else{
         /*----- reset the current record number pointer -----*/
         db_ptr->curr_rec = (char_pos-DATA_START)/(db_ptr->rec_len)  ;
         return( db_ptr->curr_rec );
         }
   }
