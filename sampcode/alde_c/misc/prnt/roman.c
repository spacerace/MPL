
/*
** Print numbers fron 1 to 25 as Roman numerals.
*/
main()
        {
        int x = 1;
        printf("This program prints out Roman Numerals\n");
        while (x <= 25)  {
                roman (x);
                x = x + 1 ;
                }
        printf("\n\nI hope you liked it!\n");
        }

/*
** Print an Arabic number in Roman numerals.
*/
roman(arabic)
int arabic;
        {
        arabic = romanize(arabic,1000,'m');
        arabic = romanize(arabic,500,'d');
        arabic = romanize(arabic,100,'c');
        arabic = romanize(arabic,50,'l');
        arabic = romanize(arabic,10,'x');
        arabic = romanize(arabic,5,'v');
        arabic = romanize(arabic,1,'i');
        romanize(arabic,1,'i');
        printf("\n");
        }

/*
** Print the character c as many times as there are
** j's in the number i, then return i minus the j's
*/
romanize (i,j,c)
char c;
int i,j;
        {
        while (i>=j)
                {
                putch(c);
                i = i - j;
                }
        return i;
        }


