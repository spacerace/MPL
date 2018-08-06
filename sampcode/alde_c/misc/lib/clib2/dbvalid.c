/**********************************************
* valid_eof(i) determines whether or not the  *
* CP/M end of file mark encountered in the    *
* status field of the ith record is valid.  It*
* uses the following set of rules :           *
*                                             *
* (i)   If the record is the maximum record in*
*       the file, return TRUE.                *
*                                             *
* (ii)  If the record is not within a DBASE   *
*       buffer size of the end of the file, a *
*       FALSE value is returned               *
*                                             *
* (iii) If no other end of file mark is en-   *
*       countered in the file, return TRUE    *
*                                             *
* (iv)  If two potentially valid EOF marks are*
*       found, the  user is prompted for a    *
*       decision.                             *
**********************************************/
valid_eof(i,buffer) 
     int  i;
     char buffer[];

     {int   cur_rec;
      char  db_buffer[1000], c;

     if( i >= maxrec-1 )
        return( TRUE );
     else if( (i*db_ptr->rec_len+DATA_START) < (char_size-DB_SECT_SZ) )
        return( FALSE );
     else if( strip(buffer,db_ptr->rec_len) > 2)
        return( TRUE );
      else {
         printf("\nUnable to determine whether the end of file mark");
         printf("\nat record #%d is reasonable",i);
         printf("\nIs this the end of the file[Y/N]:");
         c = keyin();
         if( (c == 'y') || (c == 'Y') )
            return( TRUE );
         else
           return( FALSE );
      } 
}
/* end of dbvalid.c */
