/* smerge - merge source and assembly files */

#include <stdio.h>

main(argc,argv)
int argc;
char *argv[];
{
    FILE *csrc, *asmsrc, *mergefile;
    char buffer[128];
    long sline, line, totslines;

    if (argc < 4) {
        printf("usage: smerge asmsrc csrc mergefile\n");
        exit(1);
    }

    sline = 0;

    asmsrc = fopen(argv[1],"r");
    csrc = fopen(argv[2],"r");
    mergefile = fopen(argv[3],"w");

    setvbuf(csrc,NULL,_IOFBF,8192);
    setvbuf(asmsrc,NULL,_IOFBF,8192);
    setvbuf(mergefile,NULL,_IOFBF,8192);

    totslines = 0;
    while (fgets(buffer,128,csrc) != NULL)
        totslines++;
    fseek(csrc,0L,SEEK_SET);

    while ((fgets(buffer,128,asmsrc)) != NULL) {

        /* special hack for MSC -> assembly...kill the "__acrtused" extrn */
        if (strstr(buffer,"__acrtused") != NULL)
            continue;

        if (strncmp(buffer,"; Line",6) == 0) {
            sscanf(buffer,"; Line %ld",&line);
            if (line <= totslines) {
                while (sline < line) {
                    fgets(buffer+1,128,csrc);
                    fputs(buffer,mergefile);
                    ++sline;
                }
            }
        } else
            fputs(buffer,mergefile);
    }
    fclose(asmsrc);
    fclose(csrc);
    fclose(mergefile);
}


