/* writechr.c -- writes char and attribute repeatedly */
/*               using DMA                            */
/* write character ch with attribute attr num times   */
/* starting at location pstart -- uses array notation */

typedef unsigned int (far * VIDMEM); 

void Write_chars(pstart, ch, attr, num)
VIDMEM pstart;
unsigned short ch, attr, num;
{
    register count;
    unsigned short pair;

    pair = (attr << 8) | (ch & 0x00FF) ;
    for (count = 0; count < num; count++)
        pstart[count] = pair;
}
