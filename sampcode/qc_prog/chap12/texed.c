/* texed.c  --  main entry point to the editor; the   */
/*              menu and signal handlers are here     */

main(argc, argv)
int argc;
char *argv[];
{
    char ch;

    while (1)
        {
        printf("\nTexEd Main Menu\n");
        printf("Select from:\n");
        printf("0) Quit\n\n");
        printf("1) Load File\n");
        printf("2) Save File\n");
        printf("3) Edit File\n");
        printf("Which: ");
        do
            {
            ch = getch();
            ch -= '0';
            } while (ch < 0 || ch > 3);
        printf("%d\n\n", (int)ch);
        switch(ch)
            {
            case 0: exit(0);
            case 1: Load_file(); break;
            case 2: Save_file(); break;
            case 3: Edit_file(); break;
            }
        }
}
