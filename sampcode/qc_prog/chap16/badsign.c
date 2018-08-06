/* badsign.c -- uncaught typo                 */

main()
{ 
    int i; 
    int j = 1; 
 
    for (i = 0; i < 10; i++)
	{
        j =+ 10;         /* transposed += */ 
        printf("%4d ", j); 
	}
    printf("\n"); 
} 

