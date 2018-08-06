/*
 * atol();
 *
 * The atol() function is commonly missing from most compilers.
 * The complete source to a atol() is listed below.
 * This fucntion was copied from 'C Programmer's Library'
 *
 * Spelling corrected, L. Paper, 5/6/85
 */

long atol(s)
char *s;
{

        long value;
        int minus;

        while(isspace(*s))
            s++;
                if(*s == '-')
                {
                        s++;
                        minus = 1;
                }
                else
                        minus = 0;
                value = 0;
                while(isdigit(*s))
                        value = value * 10 + *s++ - '0';
                return (minus ? -value : value);
}
