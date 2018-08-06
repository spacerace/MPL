main()
{
  printf("The following program acesses the IBM Graphics printer");

  lprintf("\nThis is a test program to check the IBM Graphics printer\n");
  lprintf("and the Unicorn1 Library\n\n\n");

  prtwidon();
  lprintf("This is the wide print\n");
  lprintf("A simple carriage return has turned it off\n");
  prtwidof();
  prtdubon();
  lprintf("This is double strike printing\n");
  lprintf("A carriage return did NOT turn it off\n");
  prtdubof();
  prtempon();
  lprintf("This is emphasized print\n");
  prtdubon();
  lprintf("This is both emphasized and double strike\n");
  prtdubof();
  prtempof();
  lprintf("Following this line the printer buzzer should sound\n");
  prtbuzz();
  prtcomon();
  lprintf("This is compressed print\n");
  lprintf("A carriage return does NOT turn it off\n");
  prtcomof();
  lprintf("There should be a backspace character");
  prtbs();
  lprintf("right after the word character\n");
  lprintf("There should be a tab");
  prttab();
  lprintf("right after the word tab\n");
  lprintf("The line feed character here");
  prtlf();
  lprintf("moves the line up one and moves the carriage back\n");
  lprintf("\n\nThis ");
  prtwidon();
  prtdubon();
  prtempon();
  lprintf("is a");
  prtwidof();
  prtdubof();
  prtempof();
  lprintf("simple test line ");
  prtcomon();
  lprintf("With different type styles\n");
  prtcomof();
  lprintf("Next is a Form Feed character to eject the page\n");
  prtff();







  printf("\nAll done now!");

}

