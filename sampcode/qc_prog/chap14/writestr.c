/*  writestr.c -- writes string and attribute using DMA */
/*  write the string str with attribute attr at         */
/*  location pstart -- uses pointer notation.           */

typedef unsigned int (far * VIDMEM); 

void Write_str(pstart, attr, str)
VIDMEM pstart;
unsigned short attr;
char *str;
{
    while (*str != '\0')
        *pstart++ = (attr << 8) | (*str++ & 0x00FF);
}
