/* fmenu.c  --  demonstrates file renaming, etc. */

#include <direct.h>
#include <stdio.h>
#include <string.h>

#define MAXPATH (80)
char From_name[MAXPATH],
     To_name[MAXPATH];

int Input(char *prompt, char buf[])
{
    printf("%s: ", prompt);
    if (gets(buf) == NULL || *buf == '\0')
        return (0);
    return (1);
}
void Rename(void)
{
    printf("->Rename/move\n");
    if (!Input("From", From_name)) return;
    if (!Input("To", To_name)) return;
    if (rename(From_name, To_name) != 0)
        perror("RENAME");
    else
        printf("Renamed: \"%s\" -> \"%s\"\n", 
                From_name, To_name);
}
void Remove(void)
{
    printf("->Remove\n");
    if (!Input("Remove", From_name)) return;
    if (!Input("Are You Sure", To_name)) return;
    if (*To_name != 'y' && *To_name != 'Y')
        return;
    if (remove(From_name) != 0)
        perror(From_name);
    else
        printf("Removed: \"%s\"\n", From_name);
}
void Maketemp(void)
{
    printf("->Maketemp\n");
    if (!Input("In What Directory", From_name))
        return;
    (void)strcat(From_name, "\\XXXXXX");
    if (mktemp(From_name) == NULL)
        printf("Can't create a unique name.\n");
    else
        printf("Created: \"%s\"\n", From_name);
}
void Quit(void)
{
    printf("->Quit\n");
    if (!Input("Are You Sure", From_name))
        return;
    if (*From_name != 'y' && *From_name != 'Y')
        return;
    exit(0);
}

main()
{
    static void (*doit[])() = {Rename, Remove, Maketemp, Quit};
    int ch;

    while (1)
        {
        printf("--------------------------------------------\n");
        printf("1) Rename/move a file or rename a directory.\n");
        printf("2) Remove a file.\n");
        printf("3) Make a unique temporary file.\n");
        printf("4) Quit.\n");
        printf("--------------------------------------------\n");
        printf("Select: ");

        do
            {
            ch = getchar();
            } while (ch < '1' || ch > '4');
        getchar();    /* gobble trailing newline */
        printf("%c\n\n", ch);
        ch -= '1';
        doit[ch]();
        }
}
