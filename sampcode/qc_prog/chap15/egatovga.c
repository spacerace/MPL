/* egatovga.c -- converts ega color values to vga      */
/*               color values.                         */

long Ega_to_vga(egacolor)
int egacolor;        /* ega color value */
{
    static long vgavals[6] = {0x2a0000L, 0x002a00L,
                         0x00002aL, 0x150000L,
                         0x001500L, 0x000015L};
     /* array holds VGA equivalents to EGA bits */
    long vgacolor = 0L; /* vga color value */
    int bit;

    /* convert each bit to equivalent, and sum */
    for (bit = 0; bit < 6; bit++)
        vgacolor += ((egacolor >> bit) &1) * vgavals[bit];
    return (vgacolor);
}
