/* Example program demonstrating the use of glob() */

#include "glob.h"

 main() {
 	char **names;
 	names = glob("*.?");
 	if (names) {
	    printf("expansion of *.? is:\n");
 	    print(names);
 	} else
 	    printf("glob() error number %d (see glob.h)\n",globerror);
        recover(names);
 }
 
 recover(names)		/* free() all the space used up by **names; */
 char **names;
 {
     int i = 0;
     while (names[i] != (char *)0) {
 	free(names[i]);
 	i++;
     }
     free(names);
 }
 
 print(names)		/* print out all the filenames returned by glob() */
 char **names;
 {
     int i;
     i = 0;
     while (names[i] != (char *)0) {
 	printf("%s\n",names[i]);
 	i++;
     }
 }
