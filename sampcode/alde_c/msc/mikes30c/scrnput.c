/**************************************************************************
  This program demonstrates the program NOSNOW.ASM which significantly
  increases the speed of full screen displays and updates as opposed to
  using printf.  This program and NOSNOW was written for Microsoft 3.0
  C and displays a screen without the usual snow assosiated with movedata.
  The following command and switches where used to compile:

     cl -Ox -Ze scrnput.c nosnow

        Enjoy!!   Mike Elkins    Sysop Mike's "C" Board
***************************************************************************/

#include <stdio.h>
#include <dos.h>

void flash(unsigned int, unsigned int);
void scrnsave(unsigned int, unsigned int);
char *malloc();

main()
{
   char far *hold;
   unsigned  i;

   hold = (char far * ) malloc(4001);       /* Allocate memory for save */
   scrnsave(FP_SEG(hold), FP_OFF(hold));    /* Save current screen */
   screen0();                               /* Display new screen */
   for(i=0; i<60000; i++);                  /* Stall */
   for(i=0; i<60000; i++); 
   for(i=0; i<60000; i++); 
   flash(FP_SEG(hold), FP_OFF(hold));       /* Put orig. screen back */
   free(hold);                              /* Free space */
}

screen0() /* This screen was created with SG3 and the C code edited in */
{
    char	*scrn0;

    scrn0 = malloc(2001);
    strcpy(scrn0, "��������������������������������������������������������������������������������");
    strcat(scrn0, "۲������������������������������������������������������������������������������");
    strcat(scrn0, "۲������������������������������������������������������������������������������");
    strcat(scrn0, "۲������������������������������������������������������������������������������");
    strcat(scrn0, "۲��                                                                        ����");
    strcat(scrn0, "۲��                                                                        ����");
    strcat(scrn0, "۲��                             W E L C O M E                              ����");
    strcat(scrn0, "۲��                                                                        ����");
    strcat(scrn0, "۲��                                  T O                                   ����");
    strcat(scrn0, "۲��                                                                        ����");
    strcat(scrn0, "۲��                           MIKE'S  'C'  BOARD                           ����");
    strcat(scrn0, "۲��                                                                        ����");
    strcat(scrn0, "۲��              �����������������������������������������Ŀ               ����");
    strcat(scrn0, "۲��              �           FIDO  NODE  102/201           �               ����");
    strcat(scrn0, "۲��              �            619 - 722 - 8724             �               ����");
    strcat(scrn0, "۲��              �               24 HRS DAY                �               ����");
    strcat(scrn0, "۲��              �             300/1200 BAUD               �               ����");
    strcat(scrn0, "۲��              �           SYSOP  MIKE ELKINS            �               ����");
    strcat(scrn0, "۲��              �������������������������������������������               ����");
    strcat(scrn0, "۲��                                                                        ����");
    strcat(scrn0, "۲��                                                                        ����");
    strcat(scrn0, "۲������������������������������������������������������������������������������");
    strcat(scrn0, "۲������������������������������������������������������������������������������");
    strcat(scrn0, "۲������������������������������������������������������������������������������");
    strcat(scrn0, "��������������������������������������������������������������������������������");

    quik_scrn(scrn0);
    free(scrn0);
}

/*********************************************************************
  These routines take an array containing a screen layout and injects
  attribute bytes preparing it for direct move to screen memory.
*********************************************************************/
quik_scrn(strng)
char	*strng;
{
    char	far *scrn;
    int	i, j;

    scrn = (char far * ) malloc(4001);
    build_scrn(scrn,strng);             /* Insert attribute bytes */
    flash(FP_SEG(scrn), FP_OFF(scrn));  /* Display new screen */
    free(scrn);

}

build_scrn(hld, scrn) /* Convert scrn to screen memory format and store in hld */
char	far *hld;
char	*scrn;
{
    int	i, j;

    for (i = 1; i < 4001; i += 2) /* Fill every other byte with std. screen */
        hld[i] = 0x07;            /* attribute starting with second */
    for (i = 0, j = 0; hld[i] = scrn[j]; i += 2, j++) /* Put above screen in*/
        ;                         /* in every other byte */
}

