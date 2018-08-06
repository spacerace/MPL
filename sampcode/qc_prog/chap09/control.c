/* control.c  --  demonstrate string justification */
/*                using printf()                   */

char Some_text[] = "Some Text";
char Left_control[] =    "<<%-15s>>";
char Right_control[] =    "<<%15s>>";

main()
{
    char ch;

    while (1)
        {
        printf("Select l)eft r)ight or q)uit: ");
        ch = getch();
        putch( ch );

        printf("\n\n");
        switch((int) ch)
            {
            case 'l':
            case 'L':
                printf(Left_control, Some_text);
                break;
            case 'r':
            case 'R':
                printf(Right_control, Some_text);
                break;
            case 'q':
            case 'Q':
                exit (0);
            default:
                printf("Huh?");
                break;
            }
        printf("\n\n");
        }
}
