#include <ctype.h>
#include <memory.h>

/***********************************************
**  Name:         IsItAlnum%                  **
**  Type:         Function                    **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* EXAMPLE OF USE:   result% = IsItAlnum%(c%)                        
* PARAMETERS:       c%         ASCII character code                 
* VARIABLES:        (none)                                          
* MODULE LEVEL                                                      
*   DECLARATIONS:   #include <ctype.h>        */
 

int isitalnum (c)
int c;
    {
    return (isalnum(c));
    }

/***********************************************
**  Name:         IsItAlpha%                  **
**  Type:         Function                    **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* EXAMPLE OF USE:   result% = IsItAlpha%(c%)                        
* PARAMETERS:       c%         ASCII character code                 
* VARIABLES:        (none)                                      
* MODULE LEVEL
*   DECLARATIONS:   #include <ctype.h>        */                   
 

int isitalpha (c)
int c;
    {
    return (isalpha(c));
    }

/***********************************************
**  Name:         IsItAscii%                  **
**  Type:         Function                    **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* EXAMPLE OF USE:  result% = IsItAscii%(c%)
* PARAMETERS:      c%         ASCII character code
* VARIABLES:       (none)
* MODULE LEVEL
*   DECLARATIONS:  #include <ctype.h>         */
 

int isitascii (c)
int c;
    {
    return (isascii(c));
    }

/***********************************************
**  Name:         IsItCntrl%                  **
**  Type:         Function                    **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* EXAMPLE OF USE:   result% = IsItCntrl%(c%)
* PARAMETERS:       c%         ASCII character code
* VARIABLES:        (none)
* MODULE LEVEL
*   DECLARATIONS:   #include <ctype.h>        */
 

int isitcntrl (c)
int c;
    {
    return (iscntrl(c));
    }

/***********************************************
**  Name:         IsItDigit%                  **
**  Type:         Function                    **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* EXAMPLE OF USE:  result% = IsItDigit%(c%)
* PARAMETERS:      c%         ASCII character code
* VARIABLES:       (none)
* MODULE LEVEL
*   DECLARATIONS:  #include <ctype.h>         */
 

int isitdigit (c)
int c;
    {
    return (isdigit(c));
    }

/***********************************************
**  Name:         IsItGraph%                  **
**  Type:         Function                    **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* EXAMPLE OF USE:  result% = IsItGraph%(c%)
* PARAMETERS:      c%         ASCII character code
* VARIABLES:       (none)
* MODULE LEVEL
*   DECLARATIONS:  #include <ctype.h>         */
 

int isitgraph (c)
int c;
    {
    return (isgraph(c));
    }

/***********************************************
**  Name:         IsItLower%                  **
**  Type:         Function                    **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* EXAMPLE OF USE:  result% = IsItLower%(c%)
* PARAMETERS:      c%         ASCII character code
* VARIABLES:       (none)
* MODULE LEVEL
*   DECLARATIONS:  #include <ctype.h>         */


int isitlower (c)
int c;
    {
    return (islower(c));
    }

/***********************************************
**  Name:         IsItPrint%                  **
**  Type:         Function                    **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* EXAMPLE OF USE:  result% = IsItPrint%(c%)
* PARAMETERS:      c%         ASCII character code
* VARIABLES:       (none)
* MODULE LEVEL
*   DECLARATIONS:  #include <ctype.h>         */
 

int isitprint (c)
int c;
    {
    return (isprint(c));
    }

/***********************************************
**  Name:         IsItPunct%                  **
**  Type:         Function                    **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* EXAMPLE OF USE:  result% = IsItPunct%(c%)
* PARAMETERS:      c%         ASCII character code
* VARIABLES:       (none)
* MODULE LEVEL
*   DECLARATIONS:  #include <ctype.h>         */
 

int isitpunct (c)
int c;
    {
    return (ispunct(c));
    }

/***********************************************
**  Name:         IsItSpace%                  **
**  Type:         Function                    **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* EXAMPLE OF USE:  result% = IsItSpace%(c%)
* PARAMETERS:      c%         ASCII character code
* VARIABLES:       (none)
* MODULE LEVEL
*   DECLARATIONS:  #include <ctype.h>         */
 

int isitspace (c)
int c;
    {
    return (isspace(c));
    }

/***********************************************
**  Name:         IsItUpper%                  **
**  Type:         Function                    **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* EXAMPLE OF USE:   result% = IsItUpper%(c%)
* PARAMETERS:       c%         ASCII character code
* VARIABLES:        (none)
* MODULE LEVEL
*   DECLARATIONS:   #include <ctype.h>         */
 
int isitupper (c)
int c;
    {
    return (isupper(c));
    }

/***********************************************
**  Name:         IsItXDigit%                 **
**  Type:         Function                    **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
* EXAMPLE OF USE:  result% = IsItXDigit%(c%)
* PARAMETERS:      c%         ASCII character code
* VARIABLES:       (none)
* MODULE LEVEL
*   DECLARATIONS:  #include <ctype.h>         */
 

int isitxdigit (c)
int c;
    {
    return (isxdigit(c));
    }

/***********************************************
**  Name:         MovBytes                    **
**  Type:         Subprogram                  **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
*  Moves bytes from a source segment and offset
*  location in memory to a destination segment and
*  offset location.
*
*  EXAMPLE OF USE:  MovBytes sseg%, soff%, dseg%, doff%, nbytes%
*  PARAMETERS:      sseg%      Source segment address of bytes to be moved
*                   soff%      Source offset address of bytes to be moved
*                   dseg%      Destination segment address of bytes to be moved
*                   doff%      Destination offset address of bytes to be moved
*                   nbytes%    Number of bytes to be moved
* VARIABLES:        (none)
* MODULE LEVEL
*   DECLARATIONS:   #include <memory.h>     */
 

void movbytes(srcseg, srcoff, destseg, destoff, nbytes)
unsigned int *srcseg, *srcoff, *destseg, *destoff, *nbytes;
    {
    movedata(*srcseg, *srcoff, *destseg, *destoff, *nbytes);
    }

/***********************************************
**  Name:         MovWords                    **
**  Type:         Subprogram                  **
**  Module:       CTOOLS1.C                   **
**  Language:     Microsoft QuickC/QuickBASIC **
************************************************
*
*  Moves words from a source segment and offset
*  location in memory to a destination segment and
*  offset location.
*
* EXAMPLE OF USE:  MovWords sseg%, soff%, dseg%, doff%, nbytes%
* PARAMETERS:      sseg%      Source segment address of words to be moved
*                  soff%      Source offset address of words to be moved
*                  dseg%      Destination segment address of words to be moved
*                  doff%      Destination offset address of words to be moved
*                  nwords%    Number of words to be moved
* VARIABLES:       (none)
* MODULE LEVEL
*   DECLARATIONS:  #include <memory.h>        */
 

void movwords(srcseg, srcoff, destseg, destoff, nwords)
unsigned int *srcseg, *srcoff, *destseg, *destoff, *nwords;
    {
    unsigned int nbytes;
    
    nbytes = *nwords + *nwords;
    movedata(*srcseg, *srcoff, *destseg, *destoff, nbytes);
    }

