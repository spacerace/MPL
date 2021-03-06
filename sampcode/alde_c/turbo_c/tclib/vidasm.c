/* These routines are for Turbo-C and provide some basic but
   valuable screen access to the IBM PC/AT and Compatibles.
   Unfortunately these cannot be used when in the Turbo-C
   development environment.

   by Mike Elkins - Mike's C Board (619)-722-8724 1200/2400
     */

cls()         /* Clear the screen  */
{

    asm  mov   ax,0600h
    asm  mov   bh,7
    asm  mov   cx,0
    asm  mov   dx,184fh
    asm  int   10h

}

del_cur()  /* Make cursor disappear */
{

    asm  mov   ah,01
    asm  mov   ch,26h
    asm  mov   cl,7
    asm  int   10h
}

restor_cur()  /* Put cursor back on screen */
{
    unsigned	st, en;

    if (get_mode() == 7)
        st = 12, en = 13;
    else
        st = 6, en = 7;

    asm  mov   ah,01
    asm  mov   bh,0
    asm  mov   ch,st
    asm  mov   cl,en
    asm  int   10h
}

get_mode()   /* Check for Monocrome or graphics  */
{

    asm  mov   ah,15
    asm  mov   bh,0
    asm  mov   ch,0
    asm  mov   cl,0
    asm  int   10h
    asm  xor   ah,ah
}

gotoxy(x, y)
int x, y;
{

    asm  mov   ah,02
    asm  mov   bh,0
    asm  mov   dh,x
    asm  mov   dl,y
    asm  int   10h
}

          Mike Johnson ........... 1-918-493-2137 2400  N81N          Met Chem ...........