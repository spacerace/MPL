/*
 *  This module contains the winprocs for the Graphics Exchange server
 *  application window as well as the DDE conversation windows.
 *  A supplementary routine, InitWindowWordVars, is also included.
 */

#include <os2.h>
#include <stdio.h>      /* sprintf */
#include <stdlib.h>     /* memcpy,itoa */
#include <string.h>     /* strcpy,strcmp,strlen */
#include "st.h"
#include "server.h"

HWND    hwndDDE;
HPS     hpsGraphics;

/*
 *  Function GraphicWndProc
 *
 *  This function processes the system and application menu messages.
 *  When a WM_DDE_INITIATE message contains the correct parameters,
 *  a DDE conversation window is created to manage the conversation.
 *  The WM_TIMER message simulates the arrival of phone messages.  This
 *  message is processed by sending a WM_DDE_REQUEST message to all
 *  conversations which are ADVISING a client application.  The WM_CLOSE
 *  message is processed by terminating ALL conversations related to this
 *  instance of the application.
 */

MRESULT APIENTRY   GraphicWndProc(hwnd, message, lParam1, lParam2)
HWND    hwnd;
USHORT  message;
MPARAM  lParam1;
MPARAM  lParam2;
{
      HPS         hps;           /* window micro-ps                   */
      RECTL       lRect;         /* rectangle for sizing operations   */
      SIZEL       lSize;         /* size structure for ps creation    */
      HWND        hwndConv;      /* window handle for conversation    */
      HWND        hwndEnum;      /* current enumeration handle        */
      HENUM       hEnum;         /* enumeration handle                */
      char        szMsgStr[13];  /* temporary work strings            */
      char        szTemp[24];    /* temporary work strings            */
      PDDEINIT    pDDEInit;      /* DDEINIT structure                 */
      PDDESTRUCT  pDDEStruct;    /* DDESTRUCT for DDE messages        */
      PWWVARS     pWWVar;        /* pointer to window data            */
      PWWVARS     pWWChild;      /* pointer to children's window data */
      static int  nConvID = 0;   /* conversation id for dde windows   */
      static char szNameStr[][10] = { "Carl","Tony","Susan","Ellen","David","Patrick",
                                      "Bob","Cliff","Linda","Steve","Dagny" };
      int         currmsg;       /* phone message to be inserted      */

      /* pointer to window data block is always queried */
      pWWVar = (PWWVARS)WinQueryWindowPtr(hwnd, QWL_USER);

      switch(message) {

          case WM_CREATE:    /* create the anchor window,
                                initialize the PS, and initiate the conversation */
               hwndDDE  = WinCreateWindow(hwnd, (PSZ)"DDEManager", (PSZ)NULL,
                                          NULL, 0,0,0,0, hwnd, HWND_TOP,
                                          NULL, (PVOID)NULL, (PVOID)NULL);
               pWWVar = InitWindowWordVars();
               WinSetWindowPtr(hwnd, QWL_USER, pWWVar);

               /* initialize colors array   */
               InitPhoneColors(&pWWVar->PhColors[0], 0);
               InitPhoneColors(&pWWVar->PhColors[1], 1);

               /* create presentation space */
               lSize.cx=(LONG)1000; lSize.cy=(LONG)1000;
               hpsGraphics = GpiCreatePS(hab, WinOpenWindowDC(hwnd),
                                         (PSIZEL)&lSize, PU_PELS | GPIA_ASSOC);
               GpiSetDrawingMode(hpsGraphics,DM_RETAIN);
               GpiSetInitialSegmentAttrs(hpsGraphics,ATTR_CHAINED,DCTL_ON);
               GpiSetInitialSegmentAttrs(hpsGraphics,ATTR_VISIBLE,DCTL_ON);
               /* create graphical segment */
               InitPhoneSeg(hpsGraphics, pWWVar->nMsgCnt, &pWWVar->PhColors[0]);

               /* initiate conversation */
               WinDdeInitiate(hwnd, "Client", "Graphics_Exchange");
               break;

          case WM_DDE_INITIATE:  /* respond if app and topic strings match */
               pDDEInit = (PDDEINIT)lParam2;

               /* check for null strings or "Graphics Exchange" */
               if ((HWND)lParam1 != hwnd) {
                  itoa((int)hwnd, szTemp, 10);
                  if(((!strlen(pDDEInit->pszAppName)) &&
                       (!strcmp("Graphics_Exchange", pDDEInit->pszTopic))) ||
                     ((!strcmp(szTemp, pDDEInit->pszAppName)) &&
                       (!strcmp("Graphics_Exchange", pDDEInit->pszTopic))) ||
                     ((!strlen(pDDEInit->pszTopic)) &&
                       (!strlen(pDDEInit->pszAppName))))    {

                     /* create conversation window and respond */
                     hwndConv = WinCreateWindow(hwndDDE, (PSZ)"DDEConversation", (PSZ)NULL, WS_VISIBLE,
                                                0,0,0,0, hwnd, HWND_TOP, ++nConvID, (PVOID)NULL, (PVOID)NULL);
                     pWWVar->ConvCnt++;
                     WinDdeRespond(lParam1, hwndConv, "Client", "Graphics_Exchange");
                  }
               }
               DosFreeSeg(PDDEITOSEL(pDDEInit));
               break;

          case WM_CLOSE:  /* enumerate all conversations for termination and quit */
               if (pWWVar->ConvCnt) {
                   pWWVar->bClose = TRUE;
                   hEnum = WinBeginEnumWindows(hwndDDE);
                   while((hwndEnum = WinGetNextWindow(hEnum))) {
                      pWWChild = (PWWVARS)WinQueryWindowULong(hwndEnum, QWL_USER);
                      pWWChild->bClose = TRUE;
                      WinDdePostMsg(pWWChild->hwndLink, hwndEnum, WM_DDE_TERMINATE, NULL, TRUE);
                      WinLockWindow(hwndEnum, FALSE);
                   }
                   WinEndEnumWindows(hEnum);
               }
               else {   /* quit if all conversations have terminated */
                    WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
               }
               break;

          case APPM_CONV_CLOSE:  /* decrement conversation count and quit if ready */
               pWWVar->ConvCnt--;
               if (!pWWVar->ConvCnt && pWWVar->bClose) {
                  WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
               }
               break;

          case WM_TIMER:  /* insert phone message into listbox, update graphics,
                             and generate new REQUESTS for all ADVISING windows */
               if(pWWVar->nMsgCnt > 10) {
                  pWWVar->bInsert = !pWWVar->bInsert;
                  pWWVar->nMsgCnt = 0;
               }
               pWWVar->nMsgCnt++;
               GpiDeleteSegment(hpsGraphics, (LONG)IDSEG_PHONE);
               if(pWWVar->bInsert){
                  currmsg = pWWVar->nMsgCnt;
                  sprintf(szMsgStr, "Message No. %d from %s", currmsg, szNameStr[currmsg-1]);
                  WinSendMsg(hStdLB, LM_INSERTITEM, MPFROMSHORT(LIT_END), (MPARAM)szMsgStr);
                  InitPhoneSeg(hpsGraphics, currmsg, &pWWVar->PhColors[0]);
               }
               else {
                  currmsg = 11-pWWVar->nMsgCnt;
                  WinSendMsg(hStdLB, LM_DELETEITEM, MPFROMSHORT(currmsg), (MPARAM)NULL);
                  InitPhoneSeg(hpsGraphics, currmsg, &pWWVar->PhColors[1]);
               }

               hEnum = WinBeginEnumWindows(hwndDDE);
               while((hwndEnum = WinGetNextWindow(hEnum))) {
                  pWWChild = (PWWVARS)WinQueryWindowULong(hwndEnum, QWL_USER);
                  if (pWWChild->bAdvise) {
                      pDDEStruct = st_DDE_Alloc(sizeof(DDESTRUCT) + strlen("Graphics") + 1, "DDEFMT_graphics_data");
                      pDDEStruct->offszItemName = (USHORT)sizeof(DDESTRUCT);
                      strcpy(DDES_PSZITEMNAME(pDDEStruct), "Graphics");
                      WinDdePostMsg(hwndEnum, hwnd, WM_DDE_REQUEST, pDDEStruct, TRUE);
                  }
                  WinLockWindow(hwndEnum, FALSE);
               }
               WinEndEnumWindows(hEnum);
               if (pWWVar->nMode == PHONE_MODE) {
                   WinQueryWindowRect(hwnd, &lRect);
                   WinInvalidateRect(hwnd, &lRect, FALSE);
               }
               break;

          case WM_PAINT:  /* repaint the window */
               hps = WinBeginPaint(hwnd, NULL, &lRect);
               WinFillRect(hps, &lRect, CLR_WHITE);
               if(pWWVar->nMode == PHONE_MODE)
                  GpiDrawChain(hpsGraphics);
               WinEndPaint(hps);
               break;

          case WM_SIZE: /* adjust size of child */
               WinQueryWindowRect(hwnd, &lRect);
               WinSetWindowPos(hStdLB, HWND_TOP, LOUSHORT(lRect.xLeft),
                               LOUSHORT(lRect.yBottom),
                               LOUSHORT(lRect.xRight - lRect.xLeft),
                               LOUSHORT(lRect.yTop - lRect.yBottom),
                               SWP_SIZE | SWP_MOVE);
               GpiSetPageViewport(hpsGraphics, &lRect);
               break;

          case WM_COMMAND:  /* menu processing */
               switch (SHORT1FROMMP(lParam1)) {

                  case IDM_PHONE:  /* display phone segment */
                       pWWVar->nMode = PHONE_MODE;
                       WinShowWindow(hStdLB, FALSE);
                       break;

                  case IDM_STD_LB:  /* display message list box */
                       pWWVar->nMode = STD_LB_MODE;
                       WinShowWindow(hStdLB, TRUE);
                       break;

                  case IDM_TIMERON:  /* start system timer */
                       WinStartTimer(hab, hwnd, (USHORT)1, (USHORT)5000);
                       break;

                  case IDM_TIMEROFF: /* stop system timer */
                       WinStopTimer(hab, hwnd, (USHORT)1);
                       break;

                  default:
                       break;
               }
               break;

          case WM_DESTROY:  /* destroy resources and window structure memory */
               GpiAssociate(hpsGraphics, NULL);
               GpiDestroyPS(hpsGraphics);
               free(pWWVar);
               break;

          default:
              return(WinDefWindowProc(hwnd, message, lParam1, lParam2));
      }

   return((MRESULT)0L);

}

/*
 *  Function ConversationWndProc
 *
 *  This function is the wndproc for the conversational DDE window
 *  and processes the WM_DDE_REQUEST, WM_DDE_ADVISE, WM_DDE_UNADVISE,
 *  and WM_DDE_TERMINATE messages.  WM_DDE_REQUEST results in the
 *  packaging of relevant data and posting of the WM_DDE_DATA message
 *  to the client application.  WM_DDE_ADVISE and WM_DDE_UNADVISE are
 *  processed by setting an advise bit in the DDE window structure.
 *  WM_DDE_TERMINATE posts a corresponding WM_DDE_TERMINATE to the
 *  client as well as a WM_CONVCLOSE to the main server application.
 *  The conversational window destroys itself when a WM_DDE_TERMINATE
 *  is received.
 */

MRESULT APIENTRY ConversationWndProc(hwnd, message, lParam1, lParam2)
HWND    hwnd;
USHORT  message;
MPARAM  lParam1;
MPARAM  lParam2;
{
      PWWVARS       pWWVar;         /* pointer to window data    */
      char          szTemp[24];     /* temporary work string     */
      LONG          lOffset = 0;    /* offset for GpiGetData     */
      SHORT         nNumBytes;      /* data length variable      */
      PDDESTRUCT    pDDEStruct;     /* pointer for DDESTRUCTS    */
      PGDEDATA      pGDEData;       /* pointer for graphics data */

      /* pointer to window data block is always queried */
      pWWVar = (PWWVARS)WinQueryWindowPtr(hwnd, QWL_USER);

      switch(message) {

          case  WM_CREATE: /* initialize window structure and register data format */
                pWWVar = InitWindowWordVars();
                WinSetWindowULong(hwnd, QWL_USER, (ULONG)pWWVar);
                pWWVar->usFormat = st_Register_DDEFMT("DDEFMT_graphics_data");
                break;

          case WM_DDE_REQUEST: /* allocate DDESTRUCT and dump graphics data */
               pDDEStruct = (PDDESTRUCT)lParam2;
               strcpy(szTemp, "Graphics");
               if((pDDEStruct->usFormat == pWWVar->usFormat) &&
                   (!strcmp(szTemp, DDES_PSZITEMNAME(pDDEStruct)))) {
                    if(!pWWVar->bNoData) {
                       nNumBytes = (strlen(szTemp) + 1 + sizeof(GDEDATA) + LOUSHORT(lPhFigCnt));
                       pDDEStruct = st_DDE_Alloc((sizeof(DDESTRUCT) + nNumBytes), "DDEFMT_graphics_data");
                       pDDEStruct->cbData  = sizeof(GDEDATA) + lPhFigCnt;
                       pDDEStruct->offszItemName = (USHORT)sizeof(DDESTRUCT);
                       pDDEStruct->offabData     = (USHORT)((sizeof(DDESTRUCT) + strlen(szTemp)) + 1);
                       pGDEData = (PGDEDATA)DDES_PABDATA(pDDEStruct);
                       st_Init_GDEData(pGDEData);
                       pGDEData->cBytes = lPhFigCnt;
                       strcpy(pGDEData->szItem, "phone");
                       pGDEData->pGpi = (unsigned char far *)((LONG)pGDEData + sizeof(GDEDATA));
                       GpiGetData(hpsGraphics, (LONG)IDSEG_PHONE,
                                  (PLONG)&lOffset, DFORM_NOCONV,
                                  (LONG)lPhFigCnt, (PBYTE)pGDEData->pGpi);
                       memcpy(DDES_PSZITEMNAME(pDDEStruct), szTemp, (strlen(szTemp) + 1));
                       if(lParam1 != WinQueryWindow(hwnd, QW_OWNER, FALSE)) {
                          pDDEStruct->fsStatus |= DDE_FRESPONSE;
                          pWWVar->hwndLink = (HWND)lParam1;
                       }
                       WinDdePostMsg(pWWVar->hwndLink, hwnd, WM_DDE_DATA, pDDEStruct, TRUE);
                    }
                    else {
                       WinDdePostMsg(pWWVar->hwndLink, hwnd, WM_DDE_DATA, NULL, TRUE);
                    }
                    DosFreeSeg(PDDESTOSEL(lParam2));
               }
               else {  /* post negative ack using their DDESTRUCT */
                    pDDEStruct->fsStatus &= (~DDE_FACK);
                    WinDdePostMsg(lParam1, hwnd, WM_DDE_ACK, pDDEStruct, TRUE);
               }
               break;

          case WM_DDE_ADVISE:  /* set ADVISE bit in window data and ACK */
               pDDEStruct = (PDDESTRUCT)lParam2;
               if(pDDEStruct->usFormat == pWWVar->usFormat) {
                  pWWVar->hwndLink = (HWND)lParam1;
                  pWWVar->bAdvise = TRUE;
                  if(pDDEStruct->fsStatus & DDE_FNODATA) {
                    pWWVar->bNoData = TRUE;
                  }
                  if(pDDEStruct->fsStatus & DDE_FACKREQ) {
                     pDDEStruct->fsStatus |= DDE_FACK;
                     WinDdePostMsg(pWWVar->hwndLink, hwnd, WM_DDE_ACK, pDDEStruct, TRUE);
                  }
                  else {
                     DosFreeSeg(PDDESTOSEL(lParam2));
                  }
               }
               else {    /* Send a Negative ACK using their DDEStruct */
                  pDDEStruct->fsStatus &= (~DDE_FACK);
                  WinDdePostMsg(lParam1, hwnd, WM_DDE_ACK, pDDEStruct, TRUE);
               }
               break;

          case WM_DDE_UNADVISE: /* turn off ADVISE bit in window data and ACK */
               pDDEStruct = (PDDESTRUCT)lParam2;
               if((lParam1 == pWWVar->hwndLink) &&
                  (pDDEStruct->usFormat == pWWVar->usFormat)) {
                    pWWVar->bAdvise       = FALSE;
                    pWWVar->bNoData       = TRUE;
                    pDDEStruct->fsStatus |= DDE_FACK;
                    WinDdePostMsg(lParam1, hwnd, WM_DDE_ACK, pDDEStruct, TRUE);
               }
               else {
                  pDDEStruct->fsStatus &= (~DDE_FACK);
                  WinDdePostMsg(lParam1, hwnd, WM_DDE_ACK, pDDEStruct, TRUE);
               }
               break;

          case WM_DDE_TERMINATE: /* destroy conversation window and confirm TERMINATE */
                  if (!pWWVar->bClose) {
                     WinDdePostMsg((HWND)lParam1, hwnd, WM_DDE_TERMINATE, NULL, TRUE);
                  }
                  WinPostMsg(WinQueryWindow(hwnd, QW_OWNER, FALSE), APPM_CONV_CLOSE, MPFROMLONG(hwnd), (MPARAM)NULL);
                  WinDestroyWindow(hwnd);
                  break;

          case WM_DESTROY:  /* free window data structure */
                  free(pWWVar);
                  break;

          default:
              return(WinDefWindowProc(hwnd, message, lParam1, lParam2));

      }

      return((MRESULT)0L);

}

/*
 *  Function InitWindowWordVars
 *
 *  This function allocates memory for the window structure and
 *  initializes the values of the fields.  It returns a pointer to be
 *  stored in the window word.
 */

PWWVARS  InitWindowWordVars()
{
   PWWVARS   pWWVar;   /* pointer to window structure */

   pWWVar = (PWWVARS)malloc(sizeof(WWVARS));
   pWWVar->nMsgCnt    = 0;
   pWWVar->ConvCnt    = 0;
   pWWVar->nMode      = STD_LB_MODE;
   pWWVar->bInsert    = TRUE;
   pWWVar->bClose     = FALSE;
   pWWVar->hwndLink   = NULL;
   pWWVar->bAdvise    = FALSE;
   pWWVar->bNoData    = FALSE;
   pWWVar->usFormat   = NULL;
   return(pWWVar);
}
