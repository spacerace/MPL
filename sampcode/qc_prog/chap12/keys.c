/* keys.c  --   The keyboard input-handling routines  */
/*              for the texed editor.                 */

Edit_file()
{
    char ch;

    printf("\nYou are now in the editor.\n");
    printf("Press 'Q' to exit back to main menu.\n");

    do
        {
        ch = getch();
        putch(ch);
        } while (ch != 'Q');

    printf("\n\n");
}
