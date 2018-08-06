
/* a function that accepts a string from the keyboard                   */

inputs(s)
char s[];
{
        int i;

        for(i=0;(s[i]=getchar()) != '\n' && i<MAXLIN-1; ++i)
        ;
        s[i]='\0';
}
har s[];
{
        int i;

     