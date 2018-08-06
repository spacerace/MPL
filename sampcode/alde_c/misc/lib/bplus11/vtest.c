/*                     VTEST.C test program.                        */
/*  This is a simple test program which creates two index files.    */
/*  100 keys are added to each file, some keys are deleted and      */
/*  then the keys are added back to the file.  The keys are listed  */
/*  in ascending and decending order.                               */

#include "bplus.h"
#include <stdio.h>
#include <time.h>
#include <conio.h>

IX_DESC name1, name2;        /* index file variables */

void uplist(name)            /* list keys in ascending order */
  IX_DESC *name;
  {
    ENTRY ee;
    first_key(name);
    while (next_key(&ee, name) == IX_OK) printf("%s   ",ee.key);
    printf("\nPress any key to continue \n");
    getch();
  }

void downlist(name)           /* list keys in decending order  */
  IX_DESC *name;
  {
    ENTRY ee;
    last_key(name);
    while (prev_key(&ee, name) == IX_OK) printf("%s   ",ee.key);
    printf("\nPress any key to continue \n");
    getch();
  }

main()
  {
    long i;
    long ltime;
    ENTRY e;
    printf("Make two index files\n");
    make_index("test1.idx",&name1, 0);
    make_index("test2.idx",&name2, 1);
    printf("Indexing 100 items in two index files:\n");

    /* note the time to index */
    time(&ltime);
    printf("%s",ctime(&ltime));

    /* add 100 keys to each index file */
    for (i = 0L; i < 100L; i++)
      {
        e.recptr = i;
        sprintf(e.key, "VALUE1-%2d",i);
        add_key(&e, &name1);
        sprintf(e.key, "VALUE2-%2d",i);
        add_key(&e, &name2);
      }

    /* print the time required for indexing the two files */
    time(&ltime);
    printf("%s",ctime(&ltime));
    printf("Indexing is complete\n\n");
    printf("List the keys in each index file in ascending order:\n\n");
    uplist(&name1);
    uplist(&name2);

    /* delete some keys and list again */
    printf("\nNow delete all keys from 20 to 90 in each file\n\n");
    for (i = 20L; i < 90L; i++)
      {
        e.recptr = i;
        sprintf(e.key, "VALUE1-%2d",i);
        delete_key(&e, &name1);
        sprintf(e.key, "VALUE2-%2d",i);
        delete_key(&e, &name2);
      }
    printf("List the keys now for each index file in ascending order:\n\n");
    uplist(&name1);
    uplist(&name2);

    /* add the keys back and list again */
    printf("Now add back all items from 20 to 90 to each index\n\n");
    for (i = 20L; i < 90L; i++)
      {
        e.recptr = i;
        sprintf(e.key, "VALUE1-%d",i);
        add_key(&e, &name1);
        sprintf(e.key, "VALUE2-%d",i);
        add_key(&e, &name2);
      }
    printf("List the keys for each index file again in ascending order:\n\n");
    uplist(&name1);
    uplist(&name2);

    /* list both files in decending order */
    printf("List the keys for each index file in decending order\n\n");
    downlist(&name1);
    downlist(&name2);

    /* always close all files */
    close_index(&name1);
    close_index(&name2);
  }
