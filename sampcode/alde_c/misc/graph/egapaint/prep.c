#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <bios.h>
#include <process.h>
#include <io.h>

#define LENGTH 80

FILE *infile;
FILE *outfile;

void main(int argc, char *argv[])
{

    register int i, count = 0;
    int flag = 1;

    char c;
    char tmp_file[LENGTH], *temptr;
    char src_file[LENGTH], *srcptr;
    char bak_file[LENGTH], *bakptr;

    if(argc != 2) {
        printf("\nUsage: prep <argv[1]>\n");
        exit(0);
    }
    strcpy(src_file, argv[1]);            /* copy the filename  */
    temptr = tmp_file;                    /* point to the files */
    srcptr = src_file;
    bakptr = bak_file;
    while(*srcptr) {                    /* copy the name til a . appears */
        if(*srcptr == '.') break;        /*   or we run out of stuff      */
        *bakptr++ = *srcptr;
        *temptr++ = *srcptr++;
    }
    strcpy(temptr, ".$$$");                /* fill in the file types        */
    strcpy(bakptr, ".BAK");
    if((infile = fopen(src_file, "r")) == NULL) {
        printf("\nCannot open file: %s\n", src_file);
        exit(0);
    }
    if((outfile = fopen(tmp_file, "w")) == NULL) {
        printf("\nCannot open file: %s\n", tmp_file);
        exit(0);
    }
    while(flag) {                        /* copy characters until End-Of-File */
        c = fgetc(infile);
        if(c == EOF) flag = 0;
        if(c != '\t') {
			fputc(c, outfile); /* expand the tab */
			count++;
		}
        else {
			i = 4 - (count % 4);
			while(i--) {
				fputc(' ', outfile);
				count++;
			}
		}
		if(c == '\n')  count = 0;
    }
    fcloseall();
    unlink(bak_file);                    /* delete any previous .BAK files */
    rename(src_file, bak_file);            /* rename the files */
    rename(tmp_file, src_file);

}
