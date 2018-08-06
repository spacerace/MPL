/*
                AUTHOR          Jon Wesener
                DATE            7 / 18 / 85
                PURPOSE         This routine will give interactive input
                                from the screen with editing functions.
*/
 
/*
        KEYGETS will get a string of max size length from the keyboard.
                The passed in string should be buffered out maximally.
 
        keygets(string, length, row, col, page, upper);
 
        RETURN:         nothing
*/
 
#define         ESC     27
#define         BKSP    8
#define         CR      13
#define         NORM    15
#define         RESET   7
#define         INV     112
#include        "key.h"
 
keygets(str,len,row,col,page,upper)
char    *str;
int     len, row, col, page, upper;
{
        int     bol, eol, cnt, ins=0, tmp;
        char    *ptr, ch, *cpy;
 
        bol=col;
        eol=col+len-2;
        ptr=str;
        atputs(ptr,page,row,col,NORM);
        atputc(*ptr,page,row,col,INV);
        while ( (ch=getcw()) != CR)
        {
                atputc(*ptr,page,row,col,NORM);
                switch(ch){
                case    LFTCH : if (col > bol)
                                        {ptr--;col--;}
                                break;
 
                case    RGTCH : if (col < eol)
                                        {ptr++;col++;}
                                break;
 
                case    DEL   : strcpy(ptr,ptr+1);
                                str[len-2]=' ';
                                atputs(ptr,page,row,col,NORM);
                                break;
 
                case    BKSP  : if (col > bol)
                                {       ptr--; col--;
                                        strcpy(ptr,ptr+1);
                                        str[len-2]=' ';
                                        atputs(ptr,page,row,col,NORM);
                                }
                                break;
                case    BOL   : ptr=str;
                                col=bol;
                                break;
 
                case    EOL   : ptr=str+len-2;
                                col=eol;
                                break;
 
                case    INS   : ins= !ins;
                                break;
 
                default       : if ( (ch >= ' ') && (ch <= '~') )
                                {
                                if ( (upper) && (ch >= 'a') && (ch <= 'z') )
                                        ch=ch-'a'+'A';
                                if (ins)
                                  {for(tmp=eol,cpy=str+len-2;tmp>col;tmp--)
                                        *(cpy--)=*cpy;
                                  atputs(ptr,page,row,col,NORM);
                                  }
                                *ptr=ch;
                                atputc(ch,page,row,col,NORM);
                                if (col < eol)
                                        {ptr++;col++;}
                                }
                                break;
                }
                atputc(*ptr,page,row,col,INV);
        }
        atputs(str,page,row,bol,RESET);
}
