/* area.c   11/26/83
   Modified 3-18-86 by Charles Ransom
   [update area codes -CR]

        Telephone area code search program
                (C) 1983 Martin Smith
        Compuserve 72155,1214   Source ST2259
        310 Cinnamon Oak Lane   (713) 661-1241 [Office]
        Houston, Texas  77079   (713) 464-6737 [Home]

        Find area codes by entering an area code or state name on the
        command line.  Multiple codes, names are permitted up to 20,
        the limit set forth for C86 Computer Innovations(tm) compiler.
        Program written for IBM-PC with PC-DOS 2.0, but no machine-specific
        code is involved.

        The table is near the top of this file to make it easier to maintain.
        Area codes don't change much, but just this year in Houston our
        area code was broken off from the surrounding area.
        The format of the table is simple, so adding cities to the city list
        or even new states or regions won't upset the program if a few easy
        rules are followed:

        1) The table format is one array of string pointers.
        2) A state name consists of a two character postal abbreviation,
           like CA for California, and then the actual name,
           a)  All data on this line is lowercase. A multiple word name,
               like New Jersey, would be entered as "njnew*jersey" , .
               This prevents argv from making two arguments out of the name.
        3) An area code is the three digit code, followed by whatever text.
           a)  As many area codes, cities as you want are ok.
        4) All entries need to be enclosed in quotes (") and followed by a
           comma.
*/

#include "stdio.h"
areacode(s,x)
char *s;
int x;
{
unsigned strlen();

static char *area[] =
{  "alalabama"    , "205 All locations",
   "akalaska"     , "907 All locations",
   "azarizona"    , "602 All locations",
   "ararkansas"   , "501 All locations",
   "cacalifornia" ,
        "805 Bakersfield" , "209 Fresno" , "213 Los Angeles" ,
        "213 Burbank" , "714 Camp Pendelton" , "415 Palo Alto",
        "916 Folsom" , "714 Fullerton" , "805 Oxnard"
        "916 Sacramento" , "619 San Diego" , "415 San Francisco" ,
        "707 Santa Rosa" , "408 San Jose" , "213 Santa Monica" ,
   "cncanada" ,
        "519 London, Ont." , "514 Montreal, Quebec" , "403 Alberta" ,
        "613 Ottawa, Ont." , "418 Quebec, Quebec" , "306 Saskatchewan" ,
        "416 Toronto, Ont." , "604 British Columbia" , "204 Manitoba" ,
        "807 East Ontario" , "705 West Ontario" , "819 NW Quebec" ,
        "709 Newfoundland" , "506 New Brunswick" ,
        "902 Nova Scotia, Prince Edward Island" ,
   "cocolorado"   , "303 All locations" ,
   "ctconnecticut" , "203 All locations" ,
   "dedelaware"   , "302 All locations" ,
   "dcdistrict of*columbia" , "202 Washington" ,
   "flflorida" ,
        "813 Ft. Myers, Winter Haven" , "904 Jacksonville" ,
        "305 Miami, Key West, Ft. Lauderdale" ,
   "gageorgia" ,
        "404 Atlanta, Rome" , "912 Waycross" ,
   "hihawaii"  , "808 All locations" ,
   "ididaho"   , "208 All locations" ,
   "ilillinois" ,
        "618 Alton, Mt. Vernon" , "312 Chicago, Aurora, Waukegan" ,
        "815 Rockford" , "217 Springfield" , "309 Peoria" ,
   "inindiana" ,
        "812 Evansville" , "219 Gary, South Bend, Warsaw" ,
        "317 Indianapolis, Kokomo" ,
   "iaiowa" ,
        "712 Council Bluffs" , "515 Des Moines" , "319 Dubuque" ,
   "kskansas" ,
        "316 Wichita, Dodge City" ,
        "913 Topeka, Lawrence, Manhattan, Salina" ,
   "kykentucky" ,
        "502 Louisville, Frankfort, Paducah, Shelbyville" , "606 Winchester" ,
   "lalouisiana" ,
        "504 Baton Rouge, New Orleans" , "318 Lake Charles" ,
   "memaine"      , "207 All locations" ,
   "mdmaryland"   , "301 All locations" ,
   "mamassachusetts" ,
        "617 Boston, New Bedford, Plymouth, Worchester" ,
        "413 Springfield" ,
   "mxmexico" ,
        "905 Mexico City" , "706 NW Mexico" ,
   "mimichigan" ,
        "313 Detroit, Ann Arbor, Flint" ,
        "616 Battle Creek, Grand Rapids, Kalamazoo" ,
        "517 Lansing" , "906 Escanaba" ,
   "mnminnesota" ,
        "218 Duluth" , "612 Minneapolis, St. Paul" , "507 Rochester" ,
   "msmississippi" , "601 All locations" ,
   "momissouri" ,
        "816 Belton, Independence, Kansas City, Marshall, St. Joseph, Sedalia" ,
        "314 St. Louis, Cape Girardeau, Columbia, Fulton, Hannibal, \n\tJefferson City, Mexico, Poplar Bluff, Rolla" ,
        "417 Joplin, Springfield" ,
   "mtmontana"    , "406 All locations" ,
   "nenebraska" ,
        "402 Omaha, Lincoln" , "308 North Platte" ,
   "nvnevada"     , "702 All locations" ,
   "nhnew*hampshire" , "603 All locations" ,
   "njnew*jersey" ,
        "609 Atlantic City, Camden, Trenton" ,
        "201 Newark, Hackensack, New Brunswick, Patterson" ,
   "nmnew*mexico" , "505 All locations" ,
   "nynew*york" ,
        "518 Albany, Schenectady" , "607 Binghamton" ,
        "716 Buffalo, Niagara Falls, Rochester" , "914 White Plains" ,
        "212 New York City" , "315 Syracuse" ,
   "ncnorth*carolina" ,
        "704 Charlotte, Salisbury" ,
        "919 Greenville, Raleigh, Winston-Salem" ,
   "ndnorth*dakota" , "701 All locations" ,
   "ohohio" ,
        "216 Akron, Cleveland, Youngstown" ,
        "513 Cincinnati, Dayton" , "614 Columbus" ,
        "419 Toledo" ,
   "okoklahoma" ,
        "918 Tulsa, Bartlesville, McAlester, Muskogee" ,
        "405 Oklahoma City, Enid, Norman, Ponca City, Stillwater" ,
   "ororegon"      , "503 All locations" ,
   "papennsylvania" ,
        "215 Philadelphia, Allentown" , "814 Erie" ,
        "412 Pittsburgh" , "717 Harrisburg, Scranton" ,
   "prpuerto*rico"  ,
"809 Anguilla, Antigua, Bahamas, Barbados, Bermuda, Cayman Islands, \n\tDominican Republic, Jamaica, St. Lucia, Trinidad" ,
   "rirhode*island" , "401 All locations" ,
   "scsouth*carolina" , "803 All locations" ,
   "sdsouth*dakota" , "605 All locations" ,
   "tntennessee" ,
        "615 Nashville, Chattanooga" , "901 Memphis" ,
   "txtexas" ,
        "915 Abilene, Alpine, Big Spring, El Paso, Midland, Odessa" ,
        "512 Austin, Brownsville, Corpus Christi, Del Rio, Eagle Pass, \n\tLaredo, McAllen, San Antonio, Victoria" ,
        "806 Amarillo, Dalhart, Lubbock" ,
        "713 Houston, Baytown, Pasadena" ,
        "409 Bay City, Beaumont, Bryan, College Station, Galveston, Huntsville" ,
        "214 Dallas, Ennis, Greenville, Jefferson, Longview, Sherman, Tyler" ,
        "817 Fort Worth, Denton, Temple, Waco, Wichita Falls" ,
   "ututah"        , "801 All locations" ,
   "vtvermont"     , "802 All locations" ,
   "vivirgin*islands" , "809 All locations" ,
   "vavirginia" ,
        "804 Charlottesville, Newport News, Norfolk, Richmond" ,
        "703 Roanoke, Winchester" ,
   "wawashington" ,
        "206 Seattle, Olympia, Vancouver" , "509 Walla Walla" ,
   "  wide area"   , "800 All locations" ,
   "wvwest*virginia" , "304 All locations" ,
   "wiwisconsin" ,
        "414 Milwaukee, Green Bay, Racine" ,
        "608 Madison" , "715 Wausau" ,
   "wywyoming"     , "307 All locations" ,
   NULL  } ;

    int i,j,found;
    unsigned k;
    char temp[64];
    char *t;

    if (x==2)   /* a number area code */
    {
        found=0;
        for (i=0; area[i] != NULL; i++)
           {
           if (strncmp(area[i],s,3)==0)  /* test for match */
                {
                for (j=i-1; satoi(area[j]) != 0; --j)  /* then go back for state */
                        ;
                stprint(area[j]);       /* special print out for state name */
                putchar('\n');
                printf("    %s.\n\n",area[i]);
                found=-1;
                break;                  /* stop when found */
                }
           }
        if (found==0)
                printf("Not found, %s.\n\n",s);

     }
    else
    if (x==1)   /* state name or two letter code is input */
    {
        for (i=0; s[i] != '\0'; i++)    /* make everything lowercase */
                s[i]=tolower(s[i]);
        if (strlen(s)==2)               /* two letter code assumed */
                {
                found=0;
                for (i=0; area[i] != NULL; i++)
                        {
                        if (strncmp(s,area[i],2)==0)
                                {
                                stprint(area[i]);
                                putchar('\n');
                                for (j=i+1; satoi(area[j]) != 0;j++)
                                        printf("    %s.\n",area[j]);
                                found=-1;
                                putchar('\n');
                                break;
                                }
                        }
                if (found==0)
                        printf("Not found, %s.\n",s);
                }
        else                            /* otherwise try for state name */
        {
        k=strlen(s);                    /* ok if partial name */
        found=0;
                for (i=0; area[i] != NULL; i++)
                        {
                        if (satoi(area[i])==0)
                                {
                                t=area[i];
                                t=t+2;
                                strcpy(temp,t);
                                if (strncmp(temp,s,k) == 0)
                                        {
                                        stprint(area[i]);
                                        putchar('\n');
                                        for (j=i+1; satoi(area[j]) != 0;j++)
                                                printf("    %s.\n",area[j]);
                                        found=-1;
                                        putchar('\n');
                                        break;
                                        }
                                }
                        }
                        if (found==0)
                                printf("Not found, %s.\n",s);
        }
   }
   else         /* print entire list */
   {
     for (i=0; area[i] != NULL; i++)
     {
        if (satoi(area[i])==0)
                {
                putchar('\n');
                stprint(area[i]);
                putchar('\n');
                }
        else
                printf("    %s.\n",area[i]);
     }
   }
}

/* special printout for state name */
stprint(s)
char s[];
{
        int i;
        putchar(toupper(s[0]));
        putchar(toupper(s[1]));
        printf("  ");
        putchar(toupper(s[2]));
        for (i=3; s[i] != '\0'; i++)
                {
                if (s[i]=='*')
                        {
                        putchar(' ');
                        putchar(toupper(s[++i]));
                        }
                else
                        putchar(s[i]);
                }
        printf(" area code(s): ");
}

main(argc,argv)
int  argc;
char *argv[];
{
        int i,satoi();
        printf("   ** Area Code Finder **\n");
        printf("    (C) Marty Smith 1983 \n\n");
        printf("    Modified by Charles Ransom -1986\n\n");
        if (argc==1)
        {
        printf("Program searches for telephone area codes,\n");
        printf("   as area xxx xxx xxx etc.\n");
        printf("   xxx is an Area Code or State name.\n");
        printf("   Two letter state postal codes like TX for Texas, CA for California\n");
        printf("      can be used, otherwise type in the state name.\n");
        printf("   Enter two word state names like this: New*Jersey.\n");
        printf("   Enter area * for a list of all Area Codes.\n");
        }
        else
        if (*argv[1]=='*')
                areacode(argv[1],0);
        else
        {
        for (i=1; i < argc; ++i)
                {
                if (satoi(argv[i])==0)
                      areacode(argv[i],1);
                else
                      areacode(argv[i],2);
                }
        }
}

/* integer convert to number */
int satoi(s)
char s[];
{
        int i, n, sign;

        for (i=0; s[i]==' ' || s[i]=='\n' || s[i]=='\t'; i++)
                ;  /* skip white space */

        sign = 1;
        if (s[i] == '+' || s[i] == '-')
                sign = (s[i++]=='+') ? 1 : -1;
        for (n=0; s[i] >= '0' && s[i] <= '9'; i++)
                n = 10 * n + s[i] - '0';
        return(sign * n);

}

                                                                          