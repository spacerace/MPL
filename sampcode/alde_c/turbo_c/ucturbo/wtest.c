#include <stdio.h>
#include <window.h>
#include <border.h>

int insert;

main()
{
   WINDOW *wn, *wn1, *wn2, *wopen();
   char a[80];
   char b[80];


   bdr21.att = 0x014;

insert = 0;

if ((wn = wnopen(&bdr21, 10,2,60,10, 0x01e)) == NULL)
{
   printf("\n--- Error opening window wn\n");
   exit(1);
}
hidecur();
wnputs(wn, "\r\n                Unicorn Software                   \r\n");
wnputs(wn, "                  is proud to present\r\n");
wnputs(wn, "  The Unicorn Window functions.  Many functions are \r\n");
wnputs(wn, "  available for your pleasure.  Type your name please\r\n");
wnputs(wn, "\r\n\Press any key to continue.\r\n");
showcur();
wnputs(wn, "Your name is - ");
gets(a);
wngets(wn, b, 8);

if ((wn2 = wnopen(&bdr12, 15,13,30,5, 0x05f)) == NULL)
{
   printf("\n--- Error opening window wn2\n");
}
hidecur();
wnputs(wn2, "This is window wn2\r\n");
wnputs(wn2, "Hello there  ");
wnputs(wn2, b);
wnputs(wn2, ", How\r\nare you today?");
wnputs(wn2, "\r\n\r\nPress any key to continue.\r\n");
gets(a);


if ((wn1 = wnopen(&bdr1r, 5,5,70,9, 0x04c)) == (WINDOW *) NULL)
{
   printf("\n--- Error opening window wn1\n");
}

hidecur();
wnputs(wn1, "This is window wn1\r\n");
wnputs(wn1, "Press any key to continue.\r\n");
gets(a);
wnclose(wn1);
gets(a);
wnclose(wn2);


wnputs(wn, "\r\nBack in  window wn now press any key to\r\n");
wnputs(wn, "Return to dos\r\n");
gets(a);
wnclose(wn);
showcur();

}


hidecur()
{
  wsetcurt(10,9);
}


showcur()
{
  wsetcurt(6,7);
}



