
#define FABS_INIT "12345678901234567890123456789012345678901234567890"

/*            F A B S    K E Y   L E N G T H
              ==============================

    Format is M\<file #>

    This function will get the length of the FABS key.
    The following parameters are required :

    1. The file number > 0 and < 7.

    The program aborts if a FABS error occures.

*/
fabs_key_len( file_num )
unsigned file_num ;
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "M", adr, 0 ) ;
  adr = add_num( file_num, adr, cmd, 1 ) ;

/*  printf("FABS key len string is %s\n", cmd ) ;   */

  if ( err = call_fabs( cmd ) )
    return( fabs_error( "Length", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_key_len */

call_fabs( command )
char *command ;
{ int rec_num, key_adr, err=0 ;
  extern int rec_fabs, key_fabs, err_fabs ;
  char *cmnd ;

  cmnd = "????" ;    /* initialize command pointer */
  rec_num = 0 ;      /* initialize record and address */
  key_adr = 0 ;
/*  printf("FABS string is %s\n",command ) ; */
  callfb( command, strlen( command ), cmnd, &err, &rec_num, &key_adr ) ;
  rec_fabs = rec_num ;
  key_fabs = key_adr ;
  return( err_fabs = err ) ;
}

/*    Check error code for not found. If not found return with a
      1 to indicate not found error. Otherwise print message and
      end program.
*/
fabs_error( txt, cmd, err )
char *txt, *cmd ;
int err ;
{
   if ( ( err > 11 ) & ( err < 17 ) )
     return ;

   switch ( err )
     { case 23 :
       case 24 :
       case 25 :
       case 27 : printf( "\n\t\tFABS %s:Disk error is %d\n", txt, err );
                 printf( "\t\tFABS String was %s\n", cmd ) ;
                 cputs( "\t\tPress any key to continue: " ) ;
                 if ( !getch() ) getch() ;       /* get any key */
                 return ;
     }

   printf( "\n\t\tFABS %s:Fatal error is %d\n", txt, err ) ;
   printf( "\t\tFABS String was %s\n", cmd ) ;
   exit( 1 ) ;
}


add_num( num, adr, cmd, last )
unsigned num  ;
int      adr  ;
char    *cmd  ;
int      last ;
{ char wrk_stg[ 5 ] ;

  stcu_d( wrk_stg, num, sizeof( wrk_stg) ) ;
  return( fill_fabs( cmd, wrk_stg, adr, last ) ) ;
}

fill_fabs( to, from, start, end )
char *to, *from ;
int  start,end ;
{ int k = 0 ;
  while ( to[ start++ ] = from [ k++ ] ) ;

  if ( end )
    to[ start-1 ] = '\0' ;     /* EOS */
  else
    to[ start-1 ] = '\\' ;     /* '\' */

  return( start ) ;
}    /* end of fill_fabs */

fabs_get_key( file_num, key )
unsigned file_num ;
char *key ;
{ int key_adr ;
  extern int key_fabs ;

  key_adr = key_fabs ;            /* save key address */
  fabs_key_len( file_num ) ;      /* get key length */
/*  printf("Key address is %d\n", key_adr );
    printf("Key length is %d\n", rec_fabs );
*/
  callky( key, rec_fabs, key_adr ) ;

}
/*              F A B S    C R E A T E
                ======================

    Format is C\<file name>\<max key len>\<#keys>\<key type>\<file#>

    This function will create a file under fabs. The following
    parameters are required :

    1. The file name as [d:]<filename>[.ext].
    2. The key length > 0 and < 101.
    3. The number of primary keys for this file.
    4. The key type ( I for integer or A for ASCII ).
    5. The file number > 0 and < 7.

    The program aborts if a FABS error occures.

*/
fabs_create( fname, key_length, num_keys, key_type, file_num )
char fname[];
unsigned key_length ;
unsigned num_keys ;
char key_type ;
unsigned file_num ;
{ int err, adr ;
  char wrk_stg[ 5 ] ;
  char cmnd[ 4 ] ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "C", adr, 0 ) ;
  adr = fill_fabs( cmd, fname, adr, 0 ) ;
  adr = add_num( key_length, adr, cmd, 0 ) ;
  adr = add_num( num_keys, adr, cmd, 0 ) ;
  wrk_stg[ 0 ] = key_type ;
  wrk_stg[ 1 ] = '\0' ;
  adr = fill_fabs( cmd, wrk_stg,adr,0) ;
  adr = add_num( file_num, adr, cmd, 1 ) ;

/* printf("FABS create string is %s\n", cmd ) ; */

   if ( err = call_fabs( cmd ) )
    return( fabs_error( "Create", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_create */
/*              F A B S    O P E N
                ==================

    Format is O\<file name>\<file #>

    This function will open a FABS file. The following
    parameters are required :

    1. The file name as [d:]<filename>[.ext].
    2. The file number > 0 and < 7.

    The program aborts if a FABS error occures.

*/
fabs_open( fname, file_num )
char fname[];
unsigned file_num ;
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "O", adr, 0 ) ;
  adr = fill_fabs( cmd, fname, adr, 0 ) ;
  adr = add_num( file_num, adr, cmd, 1 ) ;

/* printf("FABS open string is %s\n",*cmd ) ;  */

  if ( err = call_fabs( cmd ) )
    return( fabs_error( "Open", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_open */
/*                 F A B S    C L O S E
                   ====================

    Format is K\<file #>

    This function will close an open FABS file. The following
    parameters are required :

    1. The file number > 0 and < 7.

    The program aborts if a FABS error occures.

*/
fabs_close( file_num )
unsigned file_num ;
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "K", adr, 0 ) ;
  adr = add_num( file_num, adr, cmd, 1 ) ;

/*  printf("FABS close string is %s\n", cmd ) ;  */

  if ( err = call_fabs( cmd ) )
    return( fabs_error( "Close", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_close */
/*                  F A B S    I N S E R T
                    ======================

    Format is I\<file number>\<key1>\..\<keyn>

    This function will insert a record into a FABS file. The following
    parameters are required :

    1. The file number > 0 and < 7.
    2. The keys seperated by '\'.

    The program aborts if a FABS error occures.

*/
fabs_insert( keys, file_num )
unsigned file_num ;
char keys[];
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "I", adr, 0 ) ;
  adr = add_num( file_num, adr, cmd, 0 ) ;
  adr = fill_fabs( cmd, keys, adr, 1 ) ;

/* printf("FABS insert string is %s\n",cmd ) ;  */

  if ( err = call_fabs( cmd ) )
    return ( fabs_error( "Insert", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_insert */
/*              F A B S    F I R S T
                ====================

    Format is F\<key #>\<file #>

    This function will find the first record in a FABS file.
    The following parameters are required :

    1. The file number > 0 and < 7.
    2. The key number.

    The program aborts if a FABS error occures.

*/
fabs_first( file_num, key_num )
unsigned file_num, key_num ;
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "F", adr, 0 ) ;
  adr = add_num( key_num, adr, cmd, 0 ) ;
  adr = add_num( file_num, adr, cmd, 1 ) ;

/*  printf("FABS first string is %s\n", cmd ) ;   */

  if ( err = call_fabs( cmd ) )
    return( fabs_error( "First", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_first */
/*              F A B S    L A S T
                ==================

    Format is L\<key #>\<file #>

    This function will find the last record in a FABS file.
    The following parameters are required :

    1. The file number > 0 and < 7.
    2. The key number.

    The program aborts if a FABS error occures.

*/
fabs_last( file_num, key_num )
unsigned file_num, key_num ;
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "L", adr, 0 ) ;
  adr = add_num( key_num, adr, cmd, 0 ) ;
  adr = add_num( file_num, adr, cmd, 1 ) ;

/* printf("FABS last string is %s\n", cmd ) ; */

  if ( err = call_fabs( cmd ) )
    return( fabs_error( "Last", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_last */
/*                 F A B S    N E X T
                   ==================

    Format is N\<file #>

    This function will find the next key in a FABS file. The following
    parameters are required :

    1. The file number > 0 and < 7.

    The program aborts if a FABS error occures.

*/
fabs_next( file_num )
unsigned file_num ;
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "N", adr, 0 ) ;
  adr = add_num( file_num, adr, cmd, 1 ) ;

/*  printf("FABS next string is %s\n", cmd ) ;  */

  if ( err = call_fabs( cmd ) )
    return( fabs_error( "Next", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_next */
/*                 F A B S    P R E V I O U S
                   ==========================

    Format is P\<file #>

    This function will find the previous key in a FABS file.
    The following parameters are required :

    1. The file number > 0 and < 7.

    The program aborts if a FABS error occures.

*/
fabs_previous( file_num )
unsigned file_num ;
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "P", adr, 0 ) ;
  adr = add_num( file_num, adr, cmd, 1 ) ;

/* printf("FABS previous string is %s\n", cmd ) ;   */

  if ( err = call_fabs( cmd ) )
    return( fabs_error( "Previous", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_previous */
/*                 F A B S    S E A R C H
                   ======================

    Format is G\<key #>\<file #>\<key>

    This function will search a FABS file for a key. The following
    parameters are required :

    1. The key number .
    2. The file number > 0 and < 7.
    3. The value of the key.

    The program aborts if a FABS error occures.

*/
fabs_search( key_num, file_num, key )
unsigned key_num, file_num ;
char *key ;
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "S", adr, 0 ) ;
  adr = add_num( key_num, adr, cmd, 0 ) ;
  adr = add_num( file_num, adr, cmd, 0 ) ;
  adr = fill_fabs( cmd, key, adr, 1 ) ;

/* printf("FABS search string is %s\n",cmd ) ;  */

  if ( err = call_fabs( cmd ) )
    return( fabs_error( "Specific", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_search */
/*            F A B S    G E N E R I C    S E A R C H
              =======================================

    Format is G\<key #>\<file #>\<key>

    This function will search a FABS file for the first record equal to
    greater than key. The following parameters are required :

    1. The key number .
    2. The file number > 0 and < 7.
    3. The value of the key.

    The program aborts if a FABS error occures.

*/
fabs_generic( key_num, file_num, key )
unsigned key_num, file_num ;
char *key ;
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "G", adr, 0 ) ;
  adr = add_num( key_num, adr, cmd, 0 ) ;
  adr = add_num( file_num, adr, cmd, 0 ) ;
  adr = fill_fabs( cmd, key, adr, 1 ) ;

/* printf("FABS generic string is %s\n", cmd ) ;   */

  if ( err = call_fabs( cmd ) )
    return( fabs_error( "Generic", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_generic */
/*                  F A B S    B U I L D   K E Y
                    ============================

    Format is B\<file number>\<key1>\..\<keyn>

    This function will insert a record into a FABS file without
    updating the map file. A write map FABS call must be executed
    after a series of build calls. The following parameters are
    required :

    1. The file number > 0 and < 7.
    2. The keys seperated by '\'.

    The program aborts if a FABS error occures.

*/
fabs_build( keys, file_num )
unsigned file_num ;
char keys[];
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  ad( keys, file_num )
unsigned file_num ;
char keys[];
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "B", adr, 0 ) ;
  adr = add_num( file_num, adr, cmd, 0 ) ;
  adr = fill_fabs( cmd, keys, adr, 1 ) ;

/* printf("FABS insert string is %s\n",cmd ) ;  */

  if ( err = call_fabs( cmd ) )
    return ( fabs_error( "Build", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_build */
/*                   F A B S    M A P
                     ================

    Format is W\<file #>

    This function will write the map file. This function must be
    executed after a series of build commands. The following
    parameters are required :

    1. The file number > 0 and < 7.

    The program aborts if a FABS error occures.

*/
fabs_map( file_num )
unsigned file_num ;
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "W", adr, 0 ) ;
  adr = add_num( file_num, adr, cmd, 1 ) ;

/*  printf("FABS write string is %s\n", cmd ) ;  */

  if ( err = call_fabs( cmd ) )
    return( fabs_error( "Write", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_map */
/*                  F A B S    D E L E T E
                    ======================

    Format is D\<record number>\<SBDF>\<file number>\<key1>\..\<keyn>

    This function will delete a record from a FABS file. The following
    parameters are required :

    1. The file record number.
    1. The search before delete flag.
    1. The file number > 0 and < 7.
    2. The keys seperated by '\'.

    The program aborts if a FABS error occures.

*/
fabs_delete( rec_num, SBDF, keys, file_num )
unsigned file_num ;
unsigned rec_num ;
char SBDF;
char keys[];
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "D", adr, 0 ) ;
  adr = add_num( rec_num, adr, cmd, 0 ) ;
  if ( SBDF == 'N' )
    adr = fill_fabs( cmd, "N", adr, 0 ) ;
  else
    adr = fill_fabs( cmd, "Y", adr, 0 ) ;
  adr = add_num( file_num, adr, cmd, 0 ) ;
  adr = fill_fabs( cmd, keys, adr, 1 ) ;

/* printf("FABS delete string is %s\n",cmd ) ;  */

  if ( err = call_fabs( cmd ) )
    return ( fabs_error( "Delete", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_delete */
/*                  F A B S    R E P L A C E
                    ========================

    Format is R\<key num>\<rec num>\<file num>\<old key>\<new key>

    This function will replace a single key with another key using the
    same record number and primary key number.The following
    parameters are required :

    1. The key number.
    2. The record number to replace.
    3. The file number > 0 and < 7.
    4. The old key.
    5. The new key.

    The program aborts if a FABS error occures.

*/
fabs_replace( key_num, rec_num,file_num, old_key, new_key )
unsigned key_num, rec_num, file_num ;
char old_key[], new_key[];
{ int adr, err ;
  char *cmd ;

  cmd = FABS_INIT ;
  adr = 0 ;
  adr = fill_fabs( cmd, "R", adr, 0 ) ;
  adr = add_num( key_num, adr, cmd, 0 ) ;
  adr = add_num( rec_num, adr, cmd, 0 ) ;
  adr = add_num( file_num, adr, cmd, 0 ) ;
  adr = fill_fabs( cmd, old_key, adr, 0 ) ;
  adr = fill_fabs( cmd, new_key, adr, 1 ) ;

/* printf("FABS replace string is %s\n",cmd ) ;  */

  if ( err = call_fabs( cmd ) )
    return ( fabs_error( "Replace", cmd, err ) ) ;

  return( err ) ;

}  /* end of fabs_replace */
