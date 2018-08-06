#include <ctype.h>
#include <conio.h>

#define VIDEO_START         0xb8000000

#define BLACK_ON_CYAN       48
#define RED_ON_CYAN         52
#define BRIGHT_WHITE_ON_RED 79

#define ENTER               13
#define RIGHT_ARROW         77
#define LEFT_ARROW          75

/* Definition of the QuickBASIC string descriptor structure */
struct bas_str
    {
    int  sd_len;
    char *sd_addr;
    };

/***********************************************
**  Name:         BitShiftLeft%               **
**  Type:         Function                    **
**  Module:       CTOOLS2.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* Shifts all bits in a QuickBASIC string one bit
* to the left.  The leftmost bit is returned, and
* the rightmost bit is set to zero.
*
* EXAMPLE OF USE:  bit% = BitShiftLeft%(bit$)
* PARAMETERS:      bit$       String containing a bit pattern
* VARIABLES:       len        Length of the string (number of bytes)
*                  str        Pointer to string contents
*                  i          Looping index to each byte of the string
*                  carry      Bit carried over from byte to byte
*                  the_byte   Working copy of each byte of the string
*
* Definition of the QuickBASIC string descriptor structure
*    struct bas_str
*        {
*        int  sd_len;
*        char *sd_addr;
*        };                                    */


int bitshiftleft (basic_string)
struct bas_str *basic_string;
    {
    int len = basic_string->sd_len;
    unsigned char *str = basic_string->sd_addr;
    int i, carry;
    unsigned int the_byte;
   
    for (i=len-1, carry=0; i>=0; i--)
        {
        the_byte = *(str + i);
        *(str + i) = (the_byte << 1) + carry;
        carry = the_byte >> 7;
        }
   
    return (carry);
    }

/***********************************************
**  Name:         BitShiftRight%              **
**  Type:         Function                    **
**  Module:       CTOOLS2.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* Shifts all bits in a QuickBASIC string one bit to
* the right.  The rightmost bit is returned, and the
* leftmost bit is set to zero.
*
* EXAMPLE OF USE:  bit% = BitShiftRight%(bit$)
* PARAMETERS:      bit$       String containing a bit pattern
* VARIABLES:       len        Length of the string (number of bytes)
*                  str        Pointer to string contents
*                  i          Looping index to each byte of the string
*                  carry      Bit carried over from byte to byte
*                  the_byte   Working copy of each byte of the string
*
* Definition of the QuickBASIC string descriptor structure
*    struct bas_str
*        {
*        int  sd_len;
*        char *sd_addr;
*        };                                             */


int bitshiftright (basic_string)
struct bas_str *basic_string;
    {
    int len = basic_string->sd_len;
    unsigned char *str = basic_string->sd_addr;
    int i, carry;
    unsigned int the_byte;
   
    for (i=0, carry=0; i<len; i++)
        {
        the_byte = *(str + i);
        *(str + i) = (the_byte >> 1) + carry;
        carry = (the_byte & 1) << 7;
        }
   
    if (carry)
        return(1);
    else
        return(0);
    }

/***********************************************
**  Name:         MenuString%                 **
**  Type:         Function                    **
**  Module:       CTOOLS2.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
*  Displays a horizontal bar menu and waits for a
*  response from the user.  Returns the number of
*  the word selected from the string. 
*
* EXAMPLE OF USE:  choice% = MenuString%(row%, col%, menu$)
* PARAMETERS:      row%       Row location to display the menu string
*                  col%       Column location to display the menu string
*                  menu$      String containing list of words representing
*                             choices
* VARIABLES:       len        Length of the menu string
*                  str        Pointer to string contents
*                  vidptr     Pointer to video memory
*                  attribute  Index into string
*                  character  Character from keyboard press
*                  both       Combination of a character and its attribute
*                  i          Looping index
*                  j          Looping index
*                  k          Looping index
*                  c          Looping index
*                  choice     Menu selection number
*                  wordnum    Sequential count of each word in the menu string
*                  refresh    Signals to redraw the menu string
* #include <ctype.h>
* #include <conio.h>
* #define VIDEO_START         0xb8000000
* #define BLACK_ON_CYAN       48
* #define RED_ON_CYAN         52
* #define BRIGHT_WHITE_ON_RED 79
* #define ENTER               13
* #define RIGHT_ARROW         77
* #define LEFT_ARROW          75
*
* Definition of the QuickBASIC string descriptor structure
*    struct bas_str
*        {
*        int  sd_len;
*        char *sd_addr;
*        };                                        */


int menustring (row, col, basic_string)
int *row, *col;
struct bas_str *basic_string;
    {
    int len;
    char * str;
    int far * vidptr;
    int attribute, character, both;
    int i, j, k, c;
    int choice, wordnum;
    int refresh;
    void packword();

    /* Initialize variables */
    len = basic_string->sd_len;
    str = basic_string->sd_addr;
    vidptr = (int far *) VIDEO_START + (*row - 1) * 80 + (*col - 1);
    choice = 1;
    refresh = 1;
   
    /* Loop until return() statement */
    while (1)
        {
       
        /* Display the string only if refresh is non-zero */
        if (refresh)
            {
            refresh = 0;
           
            /* Loop through each character of the string */
            for (wordnum = 0, i=0; i<len; i++)
                {
               
                /* Set the character and default attribute */
                character = str[i];
                attribute = BLACK_ON_CYAN;
               
                /* Uppercase? */
                if (isupper(character))
                    {
                    wordnum++;
                    attribute = RED_ON_CYAN;
                    }
               
                /* In the middle of the current selection? */
                if (wordnum == choice && character != ' ')
                    attribute = BRIGHT_WHITE_ON_RED;
               
                /* Move data to video */
                packword(&both, &attribute, &character);
                vidptr[i] = both;
                }
            }
       
        /* Check for any key presses */
        if (kbhit())
            {
           
            /* Get the key code and process it */
            switch (c = getch())
                {
               
                /* Return the choice when Enter is pressed */
                case ENTER:
                    return (choice);
               
                /* Highlight next choice if Right arrow is pressed */
                case RIGHT_ARROW:
                    if (choice < wordnum)
                        {
                        choice++;
                        refresh = 1;
                        }
                    break;
               
                /* Highlight previous choice if Left arrow is pressed */
                case LEFT_ARROW:
                    if (choice > 1)
                        {
                        choice--;
                        refresh = 1;
                        }
                    break;
               
                /* Check for match on first character of each word */
                default:
                    c = _toupper(c);
                    for (k=0, j=0; j<len; j++)
                        {
                       
                        /* Each choice starts at an uppercase char */
                        if (isupper(str[j]))
                            k++;
                   
                        /* Match if same char and not current choice */
                        if (str[j] == c && k != choice)
                            {
                            choice = k;
                            refresh = 1;
                            break;
                            }
                        }
                    break;
                }
            }
        }
    }

/***********************************************
**  Name:         NumberOfBits&               **
**  Type:         Function                    **
**  Module:       CTOOLS2.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* Counts the 1 bits in a QuickBASIC string.
*
* EXAMPLE OF USE:  count& = NumberOfBits&(a$)
* PARAMETERS:      a$         String containing bits to be counted
* VARIABLES:       len        Length of the string
*                  str        Pointer to string contents
*                  i          Looping index to each byte
*                  the_byte   Working copy of each byte of the string
*                  count      Count of the bits
*
* Definition of the QuickBASIC string descriptor structure
*    struct bas_str
*        {
*        int  sd_len;
*        char *sd_addr;
*        };                                           */


long numberofbits (basic_string)
struct bas_str *basic_string;
    {
    int len = basic_string->sd_len;
    unsigned char *str = basic_string->sd_addr;
    int i,the_byte;
    long count = 0;

    for (i=0; i<len; i++)
        {
        the_byte = *(str+i);
        while (the_byte)
            {
            count += (the_byte & 1);
            the_byte >>= 1;
            }
        }
    return (count);
    }

/***********************************************
**  Name:         PackWord                    **
**  Type:         Subprogram                  **
**  Module:       CTOOLS2.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
*  Packs two byte values into the high and low
*  bytes of an integer (word).
*
* EXAMPLE OF USE:  PackWord hiloword%, hibyte%, lobyte%
* PARAMETERS:      hiloword%  Integer word to pack the two bytes into
*                  hibyte%    Integer value of the most significant byte
*                  lobyte%    Integer value of the least significant byte
* VARIABLES:       both       A union of a two-byte structure and an integer *  
*                             variable                                */


void packword (hiloword, hibyte, lobyte)
int *hiloword, *hibyte, *lobyte;
    {
    union
        {
        struct
            {
            unsigned char lo;
            unsigned char hi;
            } bytes;
        int hilo;
        } both;
   
    both.bytes.hi = *hibyte;
    both.bytes.lo = *lobyte;
    *hiloword = both.hilo;
    }

/***********************************************
**  Name:         TextGet                     **
**  Type:         Subprogram                  **
**  Module:       CTOOLS2.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* Saves characters and attributes from a rectangular
* area of the screen.
*
* EXAMPLE OF USE:  TextGet r1%, c1%, r2%, c2%, a$
* PARAMETERS:      r1%        Pointer to row at upper left corner
*                  c1%        Pointer to column at upper left corner
*                  r2%        Pointer to row at lower right corner
*                  c2%        Pointer to column at lower right corner
*                  a$         String descriptor, where screen contents
*                             will be stored
* VARIABLES:       len        Length of string
*                  str        Pointer to string contents
*                  video      Pointer to video memory
*                  i          Index into string
*                  row        Looping index
*                  col        Looping index
* #define VIDEO_START         0xb8000000
*
* Definition of the QuickBASIC string descriptor structure
*
*  struct bas_str
*      {
*      int  sd_len;
*      char *sd_addr;
*      };                                           */


void textget (r1,c1,r2,c2,basic_string)
int *r1,*c1,*r2,*c2;
struct bas_str *basic_string;
    {
    int len;
    int * str;
    int far * video;
    int i,row,col;
   
    len = basic_string->sd_len;
    str = (int *) basic_string->sd_addr;
    video = (int far *) VIDEO_START;
   
    if (len == (*r2 - *r1 + 1) * (*c2 - *c1 + 1) * 2)
        for (row = *r1 - 1, i = 0; row < *r2; row++)
            for (col = *c1 - 1; col < *c2; col++)
                str[i++] = video[row * 80 + col];
    }

/***********************************************
**  Name:         TextPut                     **
**  Type:         Subprogram                  **
**  Module:       CTOOLS2.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* Restores characters and attributes to a rectangular
* area of the screen.
*
* EXAMPLE OF USE:  TextPut r1%, c1%, r2%, c2%, a$
* PARAMETERS:      r1%        Pointer to row at upper left corner
*                  c1%        Pointer to column at upper left corner
*                  r2%        Pointer to row at lower right corner
*                  c2%        Pointer to column at lower right corner
*                  a$         String descriptor where screen contents are stored
* VARIABLES:       len        Length of string
*                  str        Pointer to string contents
*                  video      Pointer to video memory
*                  i          Index into string
*                  row        Looping index
*                  col        Looping index
* #define VIDEO_START         0xb8000000
*
* Definition of the QuickBASIC string descriptor structure
*    struct bas_str
*        {
*        int  sd_len;
*        char *sd_addr;
*        };                                        */


void textput (r1,c1,r2,c2,basic_string)
int *r1,*c1,*r2,*c2;
struct bas_str *basic_string;
    {
    int len;
    int * str;
    int far * video;
    int i,row,col;
   
    len = basic_string->sd_len;
    str = (int *) basic_string->sd_addr;
    video = (int far *) VIDEO_START;
   
    if (len == (*r2 - *r1 + 1) * (*c2 - *c1 + 1) * 2)
        for (row = *r1 - 1, i = 0; row < *r2; row++)
            for (col = *c1 - 1; col < *c2; col++)
                video[row * 80 + col] = str[i++];
    }

/***********************************************
**  Name:         UnPackWord                  **
**  Type:         Subprogram                  **
**  Module:       CTOOLS2.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
*  Unpacks two byte values from the high and low
*  bytes of an integer (word).
*
* EXAMPLE OF USE:  UnPackWord hiloword%, hibyte%, lobyte%
* PARAMETERS:      hiloword%  Integer word containing the two bytes
*                  hibyte%    Integer value of the most significant byte
*                  lobyte%    Integer value of the least significant byte
* VARIABLES:       both       A union of a two-byte structure and an integer
*                             variable                               */


void unpackword (hiloword, hibyte, lobyte)
int *hiloword, *hibyte, *lobyte;
    {
    union
        {
        struct
            {
            unsigned char lo;
            unsigned char hi;
            } bytes;
        int hilo;
        } both;
   
    both.hilo = *hiloword;
    *hibyte = both.bytes.hi;
    *lobyte = both.bytes.lo;
    }

