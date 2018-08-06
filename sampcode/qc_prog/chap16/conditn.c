/* conditn.c -- attempt to use conditional op */

main()
{ 
    int n, m; 
 
    n = 2; 
    m = (n != 2) : 0 ? 1;  /* almost right */ 
    printf("%d\n", m); 
} 
