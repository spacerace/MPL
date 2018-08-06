/*
                AUTHOR          Jon Wesener
                DATE            7 / 19 / 85
                PURPOSE           These routines are used for conversions
                                between integers and strings.
*/
 
/*
        ATOI converts a string of specified base to an integer
         err= atoi(string, &number, base);
        RETURNS:        0= Error        1= Success
*/
 
atoi(str,num,base)
char    *str;
unsigned     *num,base;
{
        int     digit;          /* holds value of ASCII converted character */
 
        *num= 0;
        while (*str)
        {
                if ( (*str >= '0') && (*str <= '9') )
                        digit= *(str++) - '0';
 
                else if ( (*str >= 'A') && (*str <= 'Z') )
                        digit= *(str++) - 'A' + 10;
 
                else if ( (*str >= 'a') && (*str <= 'z') )
                        digit= *(str++) - 'a' + 10;
 
                else return(0);         /* digit was unrecognized */
 
                if (digit < base)
                        *num= *num * base + digit;
                else return(0);         /* digit was invalid in base */
        }
        return(1);
}
 
/*
        ITOA converts an integer to a string of specified base
         itoa(string, number, base);
        RETURNS:        nothing
*/
 
itoa(str,num,base)
char    *str;
unsigned     num, base;
{
        int     digit;
        char    temp[30], *ptr;
 
        ptr=temp + 29;
        *ptr= '\0';
        while (num)
        {
                digit= num % base;      /* get last character */
                *(--ptr)=  (digit < 10)? digit+'0' :digit+'A'- 10;
                num= num / base;
        }
        strcpy(str,(*ptr)?ptr:"0");
}
