/**********************************************************************
*             DBASE II UTILITIES FOR AZTEC C                          *
*_____________________________________________________________________*
*    These routine are intended to provide the C language programmer  *
* with a set of I/O utilities to manipulate DBase II DBF files.  This *
* will eventually include sorting, editting and  the likes.           *
*                                                                     *
*    (C) Copyright 1983                 Richard Hoffbeck              *
**********************************************************************/

#include  "libc.h"
#include  "dbase.h"

main(){
   DBASE_FILE  *db_header, *db_open() ;
   char        infile[14], db_buffer[100], name[10] , var_buf[30] ;
   int         i, record ;   

   /*----- get the input file name -----*/
   printf("Enter the input file name: "); scanf("%s",infile);
   if( (db_header=db_open(infile)) == -1 ){
      printf("\nUnable to open the input file\n\n\n\n");
      exit();
      }

   /*----- display the information -----*/
   printf("\n\t\tDBase II file: %s",infile);
   printf("\n\t\tModified on  : %u %u %u",db_header->month,db_header->day,db_header->year);
   printf("\n\t\tContains %u records of length %u\n\n",db_header->rec_num,db_header->rec_len);
   for( i=0; i<db_header->var_num ; i++) 
      printf("\n\t\t\t %10s :  %u",db_header->field_desc[i].name,db_header->field_desc[i].var_len);

   /*----- test the lookup function -----*/
   while( 1 ) {
      printf("\nEnter the variable name: ") ;  scanf("%s", name) ;
      printf("\nEnter the record number: ") ;  scanf("%d", &record ); 
      if( strncmp(name,"quit",4) == 0 )
         exit();
      db_seek(db_header,record,0) ;
      db_read(db_header,db_buffer,1) ;
      if( lookup(name,db_header,db_buffer,var_buf) == ERROR )
         printf("\nVariable not found in the table");
      else {
         printf("\n %s is ",name) ;
         printf("%s",var_buf) ;
      }
   }
 }
