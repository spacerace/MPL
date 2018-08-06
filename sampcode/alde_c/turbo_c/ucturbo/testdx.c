main()
{
   lprintf("The following is a test of the Unicorn #1 Library with a\n");
   lprintf("daisy wheel printer.  The library was designed using an\n");
   lprintf("Epson DX-10 printer.  This printer is designed to be\n");
   lprintf("Diablo 630 compatible therefore the commands in the\n");
   lprintf("library should be applicable to many Daisy wheels.\n\n\n");

   lputch('A');
   lputch('B');
   lputch('C');
   lputch('D');
   lputch('E');
   lputch('F');
   lputch('G');
   lputch('H');
   lputch('I');

   dxlf();

   lputch('a');
   lputch('b');
   lputch('c');
   lputch('d');
   lputch('e');
   lputch('f');
   lputch('g');
   lputch('h');
   lputch('i');

   dxrlf();

   lputch('A');
   lputch('B');
   lputch('C');
   lputch('D');
   lputch('E');
   lputch('F');
   lputch('G');
   lputch('H');
   lputch('I');

   dxcr();
   dxlf();
   dxcr();
   dxlf();
   dxcr();
   dxlf();
   dxcr();
   dxlf();

   lprintf("\nNotice that in the above a LF was sent to the printer. This\n");
   lprintf("did advance the paper one line but did NOT do a carriage return.\n");
   lprintf("The reverse line feed performed in a similar manner.");

   lprintf("\n\nNow lets do some ");
   dxhlf();
   lprintf("sub");
   dxrhlf();
   lprintf(" and ");
   dxrhlf();
   lprintf("super");
   dxhlf();
   lprintf("Scripting with the printer.\n\n");
   dxshast();
   lprintf("Now we will do shadow printing\n");
   lprintf("Now let's ");
   dxundlst();
   lprintf("underline");
   dxundend();
   lprintf(" a bit of print\n");
   dxboldst();
   lprintf("This line is printed using bold print\n");
   dxbsend();
   dxchwide(15);
   lprintf("\nNow let's use 8 pitch print\n");
   dxchwide(12);
   lprintf("\nNow let's use 10 pitch print\n");
   dxchwide(10);
   lprintf("\nNow let's use 12 pitch print\n");
   lprintf("\n\n\nThis is the end of the daisy wheel print test\n");
   dxff();
}
