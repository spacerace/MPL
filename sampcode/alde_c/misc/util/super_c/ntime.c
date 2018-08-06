main(argc,argv)

int argc;
char *argv[];

{
 int i, j;

 i = atoi(argv[1]);
 j = atoi(argv[2]);
 printf("Starting...");
 while (i--) delay(j);
 printf("done.\n");
}

delay(cnt)

{
 int i;

 for (i = 0; i < cnt; i++);
}

