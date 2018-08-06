/* local.c -- local variables defined */
/*            within functions        */

main()
    {
    int n = 12;
    int func1(), funct2();
    printf("n in main(): val %d ", n);
    printf("address %d\n", &n);

    printf("Calling func1()\n");
    func1();
    printf("Calling func2()\n");
    func2();
    }

int func1()
    {
    int n = 8; /* local variable */
    printf("n in func1(): val %d ", n);
    printf("address %d\n", &n);
    }

int func2()
    {
    int n = 20; /* local variable */
    printf("n in func2(): val %d ", n);
    printf("address %d\n", &n);
    }
