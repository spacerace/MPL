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
     char buff[];
     int  len;
{
     int      bad_char;
     bad_char = 0;

     /*----- process the last n-1 charaters in the string -----*/
     while ( --len > 0 ) 
       {
        if( buff[len] & 128 ) 
          {
           buff[len] = buff[len] & 127;
           bad_char++;
          }
        if( buff[len] < 32 )
          {
           buff[len] = 32;
           bad_char++;
          }
        }

     if( buff[0] != '*' && buff[0] != ' ' && buff[0] != CPM_EOF ) 
       {
        buff[0] = ' ';
        bad_char++;
       }

     return( bad_char );
}

/* end of dbstrip.c */
