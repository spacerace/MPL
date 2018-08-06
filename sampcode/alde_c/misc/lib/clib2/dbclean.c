/****************************************************************
* db_clean is a utility program for DBASE II files.  It performs*
* the following functions.                                      *
*                                                               *
*    (i)   It strips all non-printing ASCII characters from the *
*          data fields by changing them to blanks.              *
*                                                               *
*    (ii)  It strips all non-printing characters from the status*
*          field except for the CP/M end of file marker (^Z)    *
*          which it replace with a blank unless it is a valid   *
*          end of file mark.                                    *
*                                                               *
*    (iii) It reset the record count in the DBASE file header.  *
*                                                               *
*    (C) Richard Hoffbeck                                 1983  *
****************************************************************/

#include   "libc.h"
#include   "dbase.h"
#define     DB_SECT_SZ   512        /* DBASE uses a 512 byte file buffer */

int         maxrec ;
long        char_size ;
DBASE_FILE  *db_ptr ;

main(argc,argv)
     char *argv[] ;
     int  argc ;

     {char          DONE = FALSE, db_buffer[1000] ;
      int           i = 0, chng = FALSE, bad_eof = 0, bad_char = 0 ;
      long          lseek() ;

      if( argc != 2 ){
         help() ;
         exit() ;
      }

      else if( (db_ptr = db_open(argv[1])) == ERROR ){
         printf("\n %s NOT FOUND",argv[1]) ;
         exit() ;
      }

      char_size = lseek(db_ptr->file_ptr,0L,2) ;
      maxrec = (char_size-DATA_START) / db_ptr->rec_len  ;
      db_seek(db_ptr,1,0) ;
      printf("\nThere are a maximum of %d records",maxrec) ;
      printf("\nProcessing ");
      /*---------- Start of main loop ----------*/
      while( ! DONE  &&  (db_read(db_ptr,db_buffer,1) != ERROR) ) {
         chng = FALSE ;
         if( ++i == maxrec )
           DONE = TRUE ;
         if( (i % 25) == 0 ) 
            putchar('.') ;
         if( db_buffer[0] == CPM_EOF ) {
            if(  valid_eof(i,db_buffer) != FALSE )
               DONE = TRUE ;
            else  {                            /*invalid end of file */
               chng = TRUE ;
               db_buffer[0] = ' ' ;
               bad_eof++ ;
            }
         }
         if( strip(db_buffer,db_ptr->rec_len) > 0 ) {       /*invalid characters reset */
            chng = TRUE ;
            bad_char++ ;
         }
         if( chng == TRUE ) {                                     /*if there were corrections, then */
            db_seek(db_ptr,i,0) ;                                 /* rewrite the record             */
            db_write(db_ptr,db_buffer,1) ;
         }
      }

     /*----- finish up the book keeping -----*/
     if( i != db_recnum(db_ptr) ) {
         db_ptr->rec_num = i ;
         db_ptr->chng_ind = TRUE ;
         db_close(db_ptr) ;
      }
      printf("\n\n\n\tThere were %d bad end of file markers removed",bad_eof) ;
      printf("\n\tand %d records with illegal characters were rewritten",bad_char) ;
}



/*********************************************************
* strip(str,len) strips the non-printing ascii characters*
* from the string "str".  First the 8th bit is stripped  *
* from  each  character and then the  following  rule is *
* applied                                                *
*                                                        *
* (i)   If the character is <= 32 (space), except for ^Z *
*       CP/M end of file, it is converted to a space     *
*                                                        *
* (ii)  Any character in the first position other than a *
*       '*' (DBASE deleted record marker), a space or a  *
*       ^Z is reset to a space (NORMAL status)           *
*********************************************************/
strip(buff,len) 
     char buff[] ;
     int  len ;

     {int      bad_char ;
     bad_char = 0 ;

     /*----- process the last n-1 charaters in the string -----*/
     while ( --len > 0 ) {
        if( buff[len] & 128 ) {
           buff[len] = buff[len] & 127 ;
           bad_char++;
        }
        if( buff[len] < 32 ){
           buff[len] = 32 ;
           bad_char++ ;
         }
     }

     if( buff[0] != '*' && buff[0] != ' ' && buff[0] != CPM_EOF ) {
        buff[0] = ' ' ;
        bad_char++ ;
     }

     return( bad_char ) ;
}


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
     int  i ;
     char buffer[] ;

     {int   cur_rec ;
      char  db_buffer[1000], c ;

     if( i >= maxrec-1 )
        return( TRUE ) ;
     else if( (i*db_ptr->rec_len+DATA_START) < (char_size-DB_SECT_SZ) )
        return( FALSE ) ;
     else if( strip(buffer,db_ptr->rec_len) > 2)
        return( TRUE ) ;
      else {
         printf("\nUnable to determine whether the end of file mark");
         printf("\nat record #%d is reasonable",i) ;
         printf("\nIs this the end of the file[Y/N]:");
         c = keyin() ;
         if( (c == 'y') || (c == 'Y') )
            return( TRUE ) ;
         else
           return( FALSE ) ;
      } 
}

help() {
     puts("DBCLEAN is a utility program for cleaning up DBASE II files.");
     puts("Because of some bugs, DBASE will occasionally place invalid ");
     puts("characters in the record status field or in the data field that");
     puts("efffectively lock up the database.  DBASE is also known to mis-");
     puts("count the records which causes problems with some of the com-");
     puts("mands.  DBCLEAN attempts to repair these damaged DBASE files and");
     puts("the interested user should see the source file DBCLEAN.C for ");
     puts("details."); puts(" ");
     puts("     To use DBCLEAN simply type"); puts(" ");
     puts("       DBCLEAN filename.dbf") ; puts(" ") ;
     puts(">>>>> Caution: DBCLEAN rewrites the database in place so it <<<<<");
     puts(">>>>> should only be used on a copy of your working dataset <<<<<");
     puts(">>>>> until you have a corrected version.                   <<<<<");
}




