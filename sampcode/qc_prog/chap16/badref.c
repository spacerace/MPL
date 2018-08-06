/* badref.c -- misusing a pointer                     */
main()
{
    char name[81];
    char *pt_ch;

    printf("Enter your first name: -> ");
    scanf("%s", name);
    *pt_ch = name[1];
    printf("The second letter of your name is %c\n",
            *pt_ch );
}

