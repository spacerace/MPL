/* scrange.c  --  illustrates scanf()'s control */
/*                directives                    */

main()
{
    char buf[512],    /* should be big enough */
         dummy[2];    /* for \n and \0        */
    int  num;

    do
        {
        printf("Running:\n");
        printf("\tscanf(\"%%d\", &num);\n");
        printf("\tscanf(\"%%[^\\n]\", buf);\n");
        printf("\tscanf(\"%%[\\n]\", dummy);\n");

        printf("\nType enough to satisfy this:\n");
        printf("(Set num equal to zero to quit)\n");

        scanf("%d", &num);
        scanf("%[^\n]", buf);
        scanf("%[\n]", dummy);

        printf("\n\tnum = %d\n", num);
        printf("\tbuf[] = \"%s\"\n", buf);
        printf("\n\n");

        } while (num != 0) ;

}
