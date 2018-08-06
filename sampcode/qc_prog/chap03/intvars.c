/* intvars.c -- declare, define, and print */
/*              some integer variables     */

main()
{
    /* declare variables */
    int length, beam;
    unsigned int displacement;
    /* assign values to variables */
    length = 824;
    beam = 118;
    displacement = 41676;

    /* print out values */
    printf("The battleship Bismarck was %d feet long",
            length);
    printf(" with a beam of %d feet,\n", beam);
    printf("and displaced %u tons.\n", displacement);
}
