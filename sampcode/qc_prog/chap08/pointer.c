/* pointer.c  --  demonstrates pointer declaration,   */
/*                assignment, and use                 */

#define WAIT printf("(press any key)"); getch(); \
             printf("\n\n")

main()
{
    int num, *address_var;

    num = 0;
    address_var = &num;

    printf("The address of the variable ");
    printf("\"num\" is:  0x%04X\n", &num );
    printf("The value in the pointer ");
    printf("\"address_var\" is:  0x%04X\n", address_var);
    printf("The value in the variable ");
    printf("\"num\" is: %d\n", num );
    WAIT;
    printf("Since \"address_var\" points to \"num\"\n");
    printf("the value in ");
    printf("\"*address_var\" is: %d\n", *address_var);
    WAIT;
    printf("To verify this, let's store 3 in\n");
    printf("\"*address_var\", then print out ");
    printf("\"num\" and \"*address_var\"\n");
    printf("again.\n");
    WAIT;

    printf("Doing: *address_var = 3;\n\n");
    *address_var = 3;

    printf("The address of the variable ");
    printf("\"num\" is:  0x%04X\n", &num);
    printf("The value in the pointer ");
    printf("\"address_var\" is:  0x%04X\n", address_var);
    printf("The value in the variable ");
    printf("\"num\" is: %d\n", num);
    WAIT;
    printf("Since \"address_var\" points to \"num\"\n");
    printf("the value in ");
    printf("\"*address_var\" is: %d\n", *address_var);
    WAIT;

    printf("Now we will add 15 to \"num\" and print\n");
    printf("\"num\" and \"*address_var\" again.\n");
    WAIT;

    printf("Doing: num += 15;\n\n");
    num += 15;

    printf("The address of the variable ");
    printf("\"num\" is:  0x%04X\n", &num);
    printf("The value in the pointer ");
    printf("\"address_var\" is:  0x%04X\n", address_var);
    printf("The value in the variable ");
    printf("\"num\" is: %d\n", num);
    WAIT;
    printf("Since \"address_var\" points to \"num\"\n");
    printf("the value in ");
    printf("\"*address_var\" is: %d\n", *address_var);
    WAIT;

    printf("Doing: return (*address_var);\n\n");
    return (*address_var);
}
