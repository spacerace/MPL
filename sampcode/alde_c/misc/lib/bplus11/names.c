/*******************************************************************/
/*                              NAMES.C                             */
/*                                                                  */
/* This example shows how easy it is to write a program for an      */
/* online address book using the B-PLUS file indexing toolkit.      */
/* The program creates a file of names, addresses, and telephone    */
/* numbers.  A record is displayed on the screen by entering part   */
/* or all of the name.  Although the program is usefully as written */
/* it has purposely been kept simple.  You may want to add new      */
/* features to the progran.                                         */
/*                                                                  */
/********************************************************************/


#include <stdio.h>
#include <string.h>
#include "bplus.h"


typedef struct              /* Here is the address record definition */
  {
     char lastname[16];     /* last name           */
     char firstname[16];    /* first name          */
     char address1[31];     /* first address line  */
     char address2[31];     /* second address line */
     char city[21];         /* the city            */
     char state[3];         /* the state           */
     char zipcode[6];       /* postal zip code     */
     char telephone[14];    /* telephone number    */
  }  ADDRESS;


IX_DESC  nameindex;             /* index file variable  */
FILE     *namefile;             /* data file pointer    */
ADDRESS  person;                /* data record variable */


void openfiles(void);
void closefiles(void);
int addrecord(void);
void getstring(char*, int);
void newaddress(void);
void printname(ENTRY*);
void getname(void);
void nextname(void);
void listnames(void);


void openfiles()
  /* If the file NAMES.DAT already exist, open the index and data */
  /* file.  Otherwise, these files are created.                   */
  {
    if ((namefile = fopen("names.dat","r+")) != NULL)
      open_index("names.idx", &nameindex, 1);      /* open index file  */
    else
    {
      namefile = fopen("names.dat","w+");          /* create data file */
      if (namefile == NULL)
        {
          printf("Unable to open namefile\n");
          exit(1);
        }
      make_index("names.idx", &nameindex, 1);   /* creat index file     */
    }                                           /* allow duplicate keys */
  } /* openfiles */


void closefiles()
  /* close all files and exit */
  {
    fclose(namefile);
    close_index(&nameindex);
    exit(0);
  } /* closefiles */


int addrecord()
  /* add a new address to the data file - add index to index file */
  {
    ENTRY ee;
    char name[32];
    int ret;
    ret = fseek(namefile, 0L, SEEK_END);      /* seek to end of datafile  */
    if (ret == 0)
      {
        strcpy(ee.key, person.lastname);      /* key is last name followed */
        strcat(ee.key, person.firstname);     /* first name.  Capitalize   */
        strupr(ee.key);                       /*    and copy to ee.key.    */
        ee.recptr = ftell(namefile);          /* get position in datafile  */
        if (ee.recptr != -1L)
          {
            if (add_key(&ee, &nameindex) == IX_OK)     /* add key to index */
              {
                fwrite(&person,sizeof(person),1,namefile);  /* add address */
                return (IX_OK);
              }
          }
      }
    else printf("Seek error - data file");
    return (IX_FAIL);
  } /* addrecord */


void getstring(mes, length)
  char *mes;
  int length;
  /* input a string and check that it is not too long   */
  {
    char message[80];
    gets(message);
    if (strlen(message) > length) message[length] = '\0';
    strcpy(mes,message);
  } /* getstring */


void newaddress()
  /* add new address records */
  {
    while (1)
     {
       printf("\n\nLast Name      : ");
       getstring(person.lastname,15);
       if ( strlen(person.lastname) > 0)       /* quit if no last name */
         {
           printf("First Name     : ");
           getstring(person.firstname,15);
           printf("Address Line 1 : ");
           getstring(person.address1,30);
           printf("Address Line 2 : ");
           getstring(person.address2,30);
           printf("City           : ");
           getstring(person.city,20);
           printf("State          : ");
           getstring(person.state,2);
           printf("Zip Code       : ");
           getstring(person.zipcode,5);
           printf("Telephone      : ");
           getstring(person.telephone,13);
           addrecord();                     /* update data and index files */
           printf("\n");
         }
         else return ;
     }
  } /* newaddress */


void printname(e)
  ENTRY *e;
  /* retrieve a data record and print it on the screen */
  {
    int ret;

    /* seek to the record address stored in ENTRY e->recptr */
    ret = fseek(namefile, e->recptr, SEEK_SET);

    if (ret == 0)     /* if OK read the record and display */
      {
        fread(&person,sizeof(person),1,namefile);
        printf("\n\n            %s %s" , person.firstname,person.lastname);
        printf("\n            %s", person.address1);
        if (strlen(person.address2) > 0)
           printf("\n            %s", person.address2);
        printf("\n            %s, %s  %s", person.city,person.state,person.zipcode);
        printf("\n            %s\n", person.telephone);
      }
    else printf("Seek error - data file");
  } /* printname */


void getname()
  /* Get an address record by entering part or all of name */
  /* Enter last name first then first name with no spaces  */
  {
    ENTRY ee;
    printf("\n\nEnter name: ");
    gets(ee.key);

    /* make all upper case letters and copy to ee.key */	
    strupr(ee.key);

    /* use locate_key instead of find_key so an exact match not required */
    if (locate_key(&ee, &nameindex) != EOIX) printname(&ee);
    else printf("No key this large in index file\n");
  } /* getname */


void nextname()
  /* display the next address in the address file */
  {
    ENTRY ee;
    if (next_key(&ee, &nameindex) == IX_OK) printname(&ee);
    else printf("\nEnd of index file\n");
  } /* nextname */


void listnames()
  /* list all the names in the address file */
  {
    ENTRY ee;
    first_key(&nameindex);
    while (next_key(&ee, &nameindex) == IX_OK) printname(&ee);
  } /* listnames */

main()
  /* Here is the main program loop */
  {
    char cmd;
    int  done;
    done = 0;
    openfiles();
    do
     {
       printf("\nCommand: A (Add Name), F (Find), N (Next), L (List), Q (Quit): ");
       cmd = toupper(getche());
       switch (cmd)
        {
          case 'A': newaddress(); break;   /* add a name to address file   */
          case 'F': getname(); break;      /* find an address              */
          case 'N': nextname(); break;     /* display next address in file */
          case 'L': listnames(); break;    /* display all addresses        */
          case 'Q': closefiles();          /* quit and close files         */
        }
      }
    while (!done);
  } /* main */

