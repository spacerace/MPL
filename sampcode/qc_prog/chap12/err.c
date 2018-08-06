/* err.c  --  illustrates __FILE__ and __LINE__ in */
/*            tracing a small program              */

#define ERR printf("Tracing: \"%s\" line %d\n",\
                    __FILE__, __LINE__);
main()
{
    ERR
    err1();
    ERR
    err2();
    ERR
}

err1()
{
    ERR
    err2();
}

err2()
{
    ERR
}
