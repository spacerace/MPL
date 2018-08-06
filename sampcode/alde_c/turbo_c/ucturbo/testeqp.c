main()
{
  cls();
  beepsp();
  printf("\n\n\nThis program will read various information concerning your\n");
  printf("computer and display it on the screen using functions from\n");
  printf("the Unicorn #1 library.\n\n\n\n\n");

  printf("The equipment status is %d decimal or %x hex\n\n", equip(), equip());
  printf("There are %d Kilobytes of memory installed\n", memsize());
  printf("The model code number is %d decimal or %x hex\n",getctype(),getctype());
}
