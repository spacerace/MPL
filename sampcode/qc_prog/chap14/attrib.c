/* attrib.c -- this program illustrates attributes */
/* program list: attrib.c, scrfun.c                */
/* user include files: scrn.h                      */
/* Note: activate Screen Swapping On in Debug menu */
#include <stdio.h>
#include <conio.h>
#include "scrn.h"
#define PAGE 0
#define ESC '\033'
char *Format = "This message is displayed using an "
               "attribute value of %2X hex (%s).";
int Get_attrib(char *);
void Print_attr(char *, unsigned char, unsigned char);

main()
{

    int attribute;       /* value of attribute */
    char attr_str[9];    /* attr. in string form */
    char mesg[80];

    Clearscr();
    Home();
    printf("Enter an attribute as an 8-digit binary "
           "number, such as 00000111, and see a\n"
           "message displayed using that attribute."
           "Hit <Esc> to quit.\n"
           "Attribute = ");
    while ((attribute = Get_attrib(attr_str)) != -1)
        {
        Setcurs(10,0,PAGE);
        sprintf(mesg, Format, attribute, attr_str);
        Print_attr(mesg, attribute, PAGE);
        Setcurs(2, 12, PAGE);
        printf("         ");  /* clear old display */
        Setcurs(2, 12, PAGE);
        }
    Clearscr();
}

/* The following function reads in a binary number    */
/* as a sequence of 1s and 0s. It places the 1 and 0  */
/* characters in a string whose address is passed as  */
/* an argument. It returns the numeric value of the   */
/* binary number. Bad input is summarily rejected.    */
/* The function returns -1 when you press Esc.        */

int Get_attrib(a_str)
char a_str[];     /* attribute as binary string */
{
    int attrib[8];
    int index = 7;
    int ch;
    int attribute = 0; /* attrib. as numeric value */
    int pow;

    a_str[8] = '\0';  /* terminate string */
    while ((index >= 0) &&  (ch = getch()) != ESC)
        {
        if (ch != '0' && ch != '1')  /* bad input */
            putch('\a');
        else
            {
            putch(ch);
            a_str[index] = ch;      /* string form */
            attrib[index--] = ch - '0'; /* numeric */
            }
        }
    if (ch == ESC)
        return (-1);
    else            /* convert numeric array to a number */
        {
        for(index = 0, pow = 1; index < 8;
                                  index++, pow *= 2)
            attribute += attrib[index] * pow;
        return attribute;
        }
}

/* The following function prints the string str using */
/* attribute attr on the indicated page.              */
/* It uses functions from the scrfun.c file.          */

void Print_attr(str, attr, page)
char *str;
unsigned char attr, page;
{
    while (*str != '\0')
        {
        Write_ch_atr(*str++, attr , page, 1);
        Cursrt();
        }
}
