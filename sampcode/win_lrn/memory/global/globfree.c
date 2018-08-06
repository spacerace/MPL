/*
 *   This program demonstrates the use of the GlobalFree function. The
 *   GlobalFree function frees memory allocated from the Global heap.
 *   GlobalFree returns NULL if the memory is free. GlobalFree was called
 *   from WinMain in this sample application.
 *
 */

#include "windows.h"

int sprintf();

typedef struct {     /* structure we are going */
      int x;         /* to allocate and lock   */
      int y;
        } MYSTRUCT;

typedef MYSTRUCT far *lpMyPtr;	/* far pointer to MYSTRUCT type */

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HANDLE hMemBlock;   /* Handle to memory block       */
    lpMyPtr  ThisPtr;   /* Pointer to myStruct          */
    char szBuff[30];	   /* buffer for message box       */
    BOOL freed; 	      /* return value from GlobalFree */

    /* allocate space in global heap for a MYSTRUCT structure */
    hMemBlock = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
                            (long)sizeof(MYSTRUCT));

    /* if memory allocated properly */
    if (hMemBlock != NULL)
       {
       /* lock memory into current address */
       ThisPtr = (lpMyPtr)GlobalLock(hMemBlock);

    /* if lock worked */
    if (ThisPtr != NULL)
       {
       /* use memory from global heap and output results*/
       ThisPtr->x = 4;
       ThisPtr->y = 4;
       ThisPtr->x = ThisPtr->x*ThisPtr->y;
       sprintf(szBuff,"ThisPtr->x * ThisPtr->y = %d",ThisPtr->x);
       MessageBox(GetFocus(),(LPSTR)szBuff,
  	               (LPSTR)"Info from GlobalAlloc'ed memory",
        	         MB_OK);
       GlobalUnlock(hMemBlock);	      /* unlock memory */

       freed = GlobalFree(hMemBlock);    /* free memory */
       if (!freed)  /* if memory freed */
       MessageBox(GetFocus(),(LPSTR)"The memory was freed properly",
                 (LPSTR)" ",MB_OK);
       else      /* if memory not freed */
       MessageBox(GetFocus(),(LPSTR)"The memory WAS NOT freed",
                  (LPSTR)" ",MB_OK);
       }
   }
    return 0;
}
