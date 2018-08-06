/* phone.c  -- a telephone number mini-database that  */
/*             demonstrates fseek()                   */

#include <stdio.h>          /* for FILE, BUFSIZ, NULL */

#define MAXL (128)
char Name[MAXL];
char Number[MAXL];
char File[] = "C:\\TMP\\PHONE.DB";
int  Count;
FILE *Fp;
int  Distance = (MAXL * MAXL);

main(argc, argv)
int argc;
char *argv[];
{
    if (argc == 1)
        Ask();
    else
        Find(argv[1]);

    return (0);
    }

Find(char *str)
    {
    int i;

    if ((Fp = fopen(File, "r")) == NULL)
        {
        fprintf(stderr, "\"%s\": Can't Read\n", File);
        exit (1);
        }
    if (fread(&Count, 1, sizeof(int), Fp) != sizeof(int))
        {
        fprintf(stderr,"\"%s\": Error Reading\n", File);
        exit (1);
        }
    for (i = 0; i < Count; i++)
        {
        fread(Name, 1, MAXL, Fp);
        fread(Number, 1, MAXL, Fp);
        if (ferror(Fp))
            {
            fprintf(stderr, "\"%s\": Error Reading.\n", File);
            exit (1);
            }
        if (strcmp(*str, *Name) == 0)
            {
            printf("Name: %s\n", Name);
            printf("Number: %s\n", Number);
            return;
            }
        }
    fprintf(stderr, "\"%s\": Not in database.\n", str);
    return;
}

Ask()
{
    if ((Fp = fopen(File, "r+")) == NULL)
        Make();
    else if (fread(&Count, 1, sizeof(int),Fp) != sizeof(int))
        {
        fprintf(stderr, "\"%s\": Error Reading\n", File);
        exit (1);
        }
    printf("Name: ");
    if (gets(Name) == NULL || *Name == '\0')
        return;
    printf("Number: ");
    if (gets(Number) == NULL || *Number == '\0')
        return;
    if (fseek(Fp, (long)(Distance * Count), SEEK_CUR) != 0)
        {
        fprintf(stderr, "\"%s\": Error Seeking.\n", File);
        exit (1);
        }
    fwrite(Name, 1, MAXL, Fp);
    fwrite(Number, 1, MAXL, Fp);
    if (ferror(Fp))
        {
        fprintf(stderr, "\"%s\": Error Writing.\n", File);
        exit (1);
        }
    if (fseek(Fp, 0L, SEEK_SET) != 0)
        {
        fprintf(stderr, "\"%s\": Error Seeking.\n", File);
        exit (1);
        }
    ++Count;
    if (fwrite(&Count, 1, sizeof(int),Fp) != sizeof(int))
        {
        fprintf(stderr, "\"%s\": Error Writing\n", File);
        exit (1);
        }
    return;
}

Make()
{
    if ((Fp = fopen(File, "w+")) == NULL)
        {
        fprintf(stderr, "\"%s\": Can't Create\n", File);
        exit (1);
        }
    Count = 0;
    if (fwrite(&Count, 1, sizeof(int), Fp) != sizeof(int))
        {
        fprintf(stderr," \"%s\": Error Creating\n", File);
        exit (1);
        }
}
