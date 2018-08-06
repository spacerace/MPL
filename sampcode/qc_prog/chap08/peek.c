/* peek.c    -- demonstrates how to cast an int to a  */
/*              pointer                               */

main()
{
    char *mem_ptr;
    unsigned int address;

    while (1)
        {
        printf("Examine what memory location?\n");
        printf("Enter location in decimal: ");
        if (scanf("%u", &address) != 1)
            break;

        mem_ptr = (char *)address;   /* cast  */

        printf("The value in %u is 0x%02X\n",
                address, (unsigned char)*mem_ptr);
        }
}
