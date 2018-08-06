/* FUNKEY0.C - Typed from Microsoft C Programming for the IBM   */
/* by Robert Lafore, page 274. Assigns function key to string   */
/* typed by user, uses command line arguments.                  */
/****************************************************************/

main(argc,argv)
int argc;
char *argv[];
{
int key;

   if(argc != 3) {
      printf("Example usage: A>funkey0 2 dir");
      exit();
   }
   key = atoi(arcv[1]);
   printf("\x1B[0;%d;\"%s\";13p", key+58, argv[2]);
}

