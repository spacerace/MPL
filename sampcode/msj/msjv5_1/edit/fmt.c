/*****************************************************************************/
/*                                                                           */
/* FMT.C                                                                     */
/*   Formatted edit routines for Presentation Manager.                       */
/*                                                                           */
/*                                                                           */
/* (C) Copyright 1989  Marc Adler and Magma Systems    All Rights Reserved   */
/* This source code is strictly for personal use. Commercial users must      */
/* obtain a commercial license from Magma Systems.  Contact us at :          */
/*   15 Bodwell Terrace, Millburn, New Jersey  07041                         */
/*   (201) 912 - 0192                                                        */
/*****************************************************************************/

#define INCL_WIN
#define INCL_DOS
#define INCL_GPI
#include <os2.h>
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <stdarg.h>

#include "fmt.h"

#define SimpleMessage(hWnd, msg)  WinMessageBox(hWnd, hWnd, \
                                 (PSZ) msg, (PSZ) "Message", 0, MB_OK);

#define LCID_COURIER   1L
FONTMETRICS FMCourier;
FATTRS      FontAttrs;
LONG        IDCourier = 0;
HPS         hMyPS;

HWND hWndFrame,
     hWndClient;
HAB  hAB;

PFNWP  pfnOldEditWndProc = (PFN) NULL;
PFNWP  pfnOldStaticWndProc = (PFN) NULL;


MRESULT EXPENTRY ClientWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY FmtDlgProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY FmtWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY MonoStaticWndProc(HWND, USHORT, MPARAM, MPARAM);
void pascal EditSetCursor(HWND hWnd);
extern PSZ lstrcpy(PSZ s, PSZ t);


int pascal ValidDigit();
int pascal ValidDigitSignSpace();
int pascal ValidAlpha();
int pascal ValidAlphaNum();
int pascal ValidLogical();
int pascal ValidAny();
int pascal ConvertToUpper();
int pascal ConvertToLower();


typedef struct format
{
  ULONG fFormatFlags;
#define FLD_HASPICTURE  0x0001L
#define FLD_AUTONEXT    0x0002L  /* Advance to next field when all filled */
#define FLD_NOECHO      0x0004L  /* Don't echo the chars (for passwds) */
#define FLD_PROTECT     0x0008L  /* Can't enter data into this field   */
#define FLD_IGNORE      0x0010L  /* The cursor skips over this field   */
#define FLD_REQUIRED    0x0020L  /* user MUST enter data in this field */
#define FLD_TOUPPER     0x0040L  /* convert characters to upper-case   */
#define FLD_TOLOWER     0x0080L  /* convert characters to lower-case   */
#define FLD_CENTER      0x0100L  /* center the data in the field       */
#define FLD_RJUST       0x0200L  /* right justify the data in the field*/
#define FLD_NUMERIC     0x0400L
#define FLD_SIGNEDNUMERIC 0x0800L
#define FLD_ALPHA       0x1000L
#define FLD_ALPHANUMERIC 0x2000L
#define FLD_LOGICAL     0x4000L
#define FLD_MIXEDPICTURE 0x10000L
  NPFN pfnValidChar;             /* function to validate each character*/
  PSZ  szPicture;                /* picture string */
} FORMAT, FAR *PFORMAT;

struct mask_to_func
{
  ULONG mask;
  NPFN  pfnFunc;
} MaskToFunc[] =
{
  FLD_NUMERIC,          ValidDigit,
  FLD_SIGNEDNUMERIC,    ValidDigitSignSpace,
  FLD_ALPHA,            ValidAlpha,
  FLD_ALPHANUMERIC,     ValidAlphaNum,
  FLD_LOGICAL,          ValidLogical,
};

typedef struct picinfo
{
  NPFN pfnPicFunc;     /* validation function corresponding to the mask */
  BYTE chPic;          /* the mask character */
} PICINFO;
extern PICINFO *CharToPicInfo(int c);

PICINFO PicInfo[] =
{
  ValidDigit,          '9',
  ValidDigitSignSpace, '#',
  ValidAlpha,          'A',
  ValidLogical,        'L',
  ValidAlphaNum,       'N',
  ValidAny,            'X',
  ConvertToUpper,      '!',
  NULL,                '\0'
};


/*****************************************************************************/
/* main()                                                                    */
/*   Let's begin at the beginning...                                         */
/*                                                                           */
/*****************************************************************************/
int main(void)
{
  HMQ   hMQ;
  QMSG  qMsg;
  ULONG flCreateFlags = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER |
                        FCF_MINMAX   | FCF_TASKLIST | FCF_SHELLPOSITION |
                        FCF_MENU;

  PSZ  szClassName = "ClientClass";
  CLASSINFO clsInfo;

  /*
    Initialize the window and create the message queue
  */
  hAB = WinInitialize(0);
  hMQ = WinCreateMsgQueue(hAB, 0);

  /*
    Get the address of the default window proc for edit controls. We
    will call this proc to do most of the processing for the new
    formatted edit controls.
  */
  if (!WinQueryClassInfo(hAB, WC_ENTRYFIELD, (PCLASSINFO) &clsInfo))
  {
    DEBUG("Could not get class info for WC_ENTRYFIELD");
    goto bye;
  }
  pfnOldEditWndProc = clsInfo.pfnWindowProc;

  /*
    Register the main window class
  */
  WinRegisterClass(hAB, szClassName, ClientWndProc, 
                   CS_SIZEREDRAW, sizeof(PVOID));

  if (!WinRegisterClass(hAB, "Formatted", FmtWndProc, CS_SIZEREDRAW, 
                        clsInfo.cbWindowData + sizeof(PVOID)))
  {
    DEBUG("Could not register class Formatted");
    goto bye;
  }


  /*
    We have our own static class too!
  */
  if (!WinQueryClassInfo(hAB, WC_STATIC, (PCLASSINFO) &clsInfo))
  {
    DEBUG("Could not get class info for WC_STATIC");
    goto bye;
  }
  pfnOldStaticWndProc = clsInfo.pfnWindowProc;

  if (!WinRegisterClass(hAB, "MonoStatic", MonoStaticWndProc, CS_SIZEREDRAW, 
                        clsInfo.cbWindowData + sizeof(PVOID)))
  {
    DEBUG("Could not register class MonoStatic");
    goto bye;
  }

  /*
    Create the main window
  */
  hWndFrame = WinCreateStdWindow(HWND_DESKTOP,     /* parent        */
                                 WS_VISIBLE,       /* window styles */
                                 &flCreateFlags,   /* frame styles  */
                                 szClassName,      /* class name    */
                                 (PSZ) "Fmt",      /* window title  */
                                 CS_SIZEREDRAW,    /* Client style  */
                                 NULL,             /* Resource ID   */
                                 DLG_FORMAT,       /* frame window id */
                                 &hWndClient);     /* client handle */
  /*
    Message Processing Loop...
  */
  while (WinGetMsg(hAB, &qMsg, NULL, 0, 0))
  {
    WinDispatchMsg(hAB, &qMsg);
  }

  /*
    End of the program. Destroy the window and message queue.
  */
  WinReleasePS(hMyPS);
  WinDestroyWindow(hWndFrame);
bye:
  WinDestroyMsgQueue(hMQ);
  WinTerminate(hAB);

  return 0;
}


/*****************************************************************************/
/* ClientWndProc()                                                           */
/*   Main window procedure for this app. All messages to the client window   */
/* get sent here.                                                            */
/*****************************************************************************/
MRESULT EXPENTRY ClientWndProc(HWND hWnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
  HPS hPS;

  switch (msg)
  {
    case WM_PAINT :
      /*
        Erase the window.
      */
      hPS = WinBeginPaint(hWnd, NULL, NULL);
      GpiErase(hPS);
      WinEndPaint(hPS);
      return MRFROMSHORT(TRUE);

    case WM_COMMAND :
      switch (COMMANDMSG(&msg)->cmd)
      {
        case ID_DLG  :
          WinDlgBox(HWND_DESKTOP, hWnd, FmtDlgProc, NULL, DLG_FORMAT, NULL);
          break;
        case ID_EXIT :
          WinPostMsg(hWnd, WM_CLOSE, 0L, 0L);
          break;
      }
      return MRFROMSHORT(FALSE);

  } /* end switch */

  return WinDefWindowProc(hWnd, msg, mp1, mp2);
}


/*****************************************************************************/
/* FmtDlgProc()                                                              */
/*   Driver for the sample dialog box.                                       */
/*                                                                           */
/*****************************************************************************/
MRESULT EXPENTRY FmtDlgProc(HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
{
  HWND    hEdit;
  struct  fldfmtinfo *pFld;
  PFNWP   pfn;
  PICINFO *pi;
  int     i;

  switch (msg)
  {
    case WM_INITDLG :
      /*
        Get the monospaced font (Courier)
      */
      hMyPS = WinGetPS(hDlg);
      SetMonospacedFont(hMyPS);

/*
      for (i = 256;  i <= 258;  i++)
        if ((hEdit = WinWindowFromID(hDlg, i)))
        {
          pfn = WinSubclassWindow(hEdit, MonoStaticWndProc);
          if (!pfnOldStaticWndProc)
            pfnOldStaticWndProc = pfn;
        }
*/
      return MRFROMSHORT(FALSE);


    case WM_COMMAND :
      switch (COMMANDMSG(&msg)->cmd)
      {
        case ID_OK :
          WinDismissDlg(hDlg, TRUE);
          return MRFROMSHORT(TRUE);
        default :
          break;
      }
      return MRFROMSHORT(TRUE);
  }

  return WinDefDlgProc(hDlg, msg, mp1, mp2);
}


/*****************************************************************************/
/* FmtWndProc()                                                              */
/*   Window Proc for the new formatted edit control class. We will process   */
/* some of the messages, but for the most part, we rely on the standard      */
/* edit control window proc to handle the hard stuff.                        */
/*****************************************************************************/
MRESULT EXPENTRY FmtWndProc(HWND hWnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
  PSZ     szFmtMask;
  PSZ     szPicture = NULL;
  char    buf[128], *s;
  char    chFmt;
  MRESULT mr;
  PICINFO *pi;
  HPS     hPS;
  int     rc, iPos, picch, piclen;
  int     incr;
  int     cnt;
  PFORMAT pFmt;
  struct fldfmtinfo *pFld;

  switch (msg)
  {
    /*
      WM_CREATE
        We create a format information structure and associate it with
      this control.
    */
    case WM_CREATE     :
    {
      PCREATESTRUCT pCr;

      /*
        Get the control ID from the create-structure
      */
      pCr = (PCREATESTRUCT) PVOIDFROMMP(mp2);

      /*
        See if we have a picture clause in the edit control's title.
        If so, save the picture and set the title to NULL so that
        the edit control will be initially empty.
      */
      if (pCr->pszText && pCr->pszText[0])
      {
        szPicture = pCr->pszText;
        pCr->pszText = NULL;
      }

      /*
        Let the normal edit proc do its thing....
      */
      pfnOldEditWndProc(hWnd, msg, mp1, mp2);

      /*
        Search the list of formatted controls for this one
      */

      /*
        Allocate a format structure and have the window point to it
      */
      pFmt = (PFORMAT) malloc(sizeof(FORMAT));
      WinSetWindowPtr(hWnd, 0, (PVOID) pFmt);

      /*
        Tbere is an address passed in mp1 - this signifies a char mask.
      */
      if (szPicture)
      {
        /*
          Set a bit to signify that we have a character mask, and copy
          the mask into some private storage area.
        */
        pFmt->fFormatFlags = FLD_HASPICTURE;
        pFmt->szPicture = lstrcpy((PSZ) malloc(lstrlen(szPicture)+1), szPicture);

        /*
          We want to display the edit control's "protected" characters,
          but we don't want to display the mask characters. We copy
          the mask into a character array, translating the mask chars
          into blanks. If we have protected characters in the mask, then
          set a bit to signify this.
        */
        lstrcpy((PSZ) buf, szPicture);
        for (s = buf;  *s;  s++)
          if (CharToPicInfo(*s))
            *s = ' ';
          else
            pFmt->fFormatFlags |= FLD_MIXEDPICTURE;
        WinSetWindowText(hWnd, (PSZ) buf);
      }

      if (mp1)
      {
        /*
          We got a bitmask instead of a character mask.
          We search the mask table for this mask, and set the corresponding
          character validation function.
        */
        struct mask_to_func *mf;
        pFmt->fFormatFlags |= (long) (* (PUSHORT) mp1);
        for (mf = MaskToFunc;
             mf < MaskToFunc + sizeof(MaskToFunc)/sizeof(MaskToFunc[0]);  mf++)
          if (pFmt->fFormatFlags & mf->mask)
          {
            pFmt->pfnValidChar = mf->pfnFunc;
            break;
          }
      }

      /*
        Set the cursor position to the first character
      */
      pfnOldEditWndProc(hWnd, EM_SETSEL, 0L, 0L);
      return MRFROMSHORT(FALSE);
    }

    /*---------------------------------------------------------------------*/

    /*
      WM_CHAR
    */
    case WM_CHAR :
      if ((pFmt = (PFORMAT) WinQueryWindowPtr(hWnd, 0)) == NULL)
        break;

      if (CHARMSG(&msg)->fs & KC_CHAR)
      {
        BYTE c = (BYTE) CHARMSG(&msg)->chr;

        if (c == '\t' || c == '\n' || c == 27)
          return MRFROMSHORT(FALSE);

        if (pFmt->szPicture)
        {
          iPos = SHORT1FROMMR(pfnOldEditWndProc(hWnd, EM_QUERYSEL, 0L, 0L));
          piclen = strlen(pFmt->szPicture);
          /*
            Don't let the user type beyond the last picture char
          */
          if (iPos >= piclen)
            return MRFROMSHORT(TRUE);

          /*
            Validate the character
          */
          pi = CharToPicInfo(picch = pFmt->szPicture[iPos]);
          if (picch == '!')
            c = ConvertToUpper(c);
          if (pi && pi->pfnPicFunc && (*pi->pfnPicFunc)(c) == FALSE)
            return MRFROMSHORT(FormatError());

          /*
            Implement overstrike mode by deleting the next character
          */
          pfnOldEditWndProc(hWnd, EM_SETSEL, MPFROM2SHORT(iPos, iPos+1), 0L);
          pfnOldEditWndProc(hWnd, EM_CLEAR,  0L, 0L);

          /*
            Let the edit win proc handle the insertion of the character
          */
          pfnOldEditWndProc(hWnd, WM_CHAR, mp1, mp2);

          /*
            Find out what position we are in
          */
          iPos = SHORT1FROMMR(pfnOldEditWndProc(hWnd, EM_QUERYSEL, 0L, 0L));

advance:
          /*
            If the char we typed was the last one in the picture, go back
            to the left.
          */
          if (iPos >= piclen)
          {
            c = VK_LEFT;
            mp1 = MPFROMSH2CH(KC_VIRTUALKEY, 1, 0);
            mp2 = MPFROM2SHORT(0, VK_LEFT);
            goto rightleft;
          }

          /*
            Figure out how many "protected" columns to skip
          */
          cnt = 0;
          while (iPos < piclen && (picch = pFmt->szPicture[iPos]) && 
                                             CharToPicInfo(picch) == NULL)
            iPos++, cnt++;

          /*
            Skip 'em if there is a valid column to go to
          */
          if (picch)
            while (cnt--)
            {
              mp1 = MPFROMSH2CH(KC_VIRTUALKEY, 1, 0);
              mp2 = MPFROM2SHORT(0, VK_RIGHT);
              pfnOldEditWndProc(hWnd, WM_CHAR, mp1, mp2);
            }

          return MRFROMSHORT(TRUE);
        }

        /*
           NO MASK - we just have a validation function
        */
        else
        {
          if (pFmt->pfnValidChar && (*pFmt->pfnValidChar)(c) == FALSE)
            return MRFROMSHORT(FormatError());
          if (pFmt->fFormatFlags & FLD_TOUPPER)
            c = toupper(c);
          else if (pFmt->fFormatFlags & FLD_TOLOWER)
            c = tolower(c);
        }
      } /* end if KC_CHAR */


      else if (CHARMSG(&msg)->fs & KC_VIRTUALKEY)
      {
        BYTE c = (BYTE) CHARMSG(&msg)->vkey;

        if (c == VK_TAB || c == VK_BACKTAB || c == VK_NEWLINE || c == VK_ESC)
          return MRFROMSHORT(FALSE);

        if (pFmt->szPicture)
        {
          iPos = SHORT1FROMMR(pfnOldEditWndProc(hWnd, EM_QUERYSEL, 0L, 0L));
          piclen = strlen(pFmt->szPicture);
          /*
            Don't let the user type beyond the last picture char
          */
          if (iPos >= piclen)
            return MRFROMSHORT(TRUE);

          switch (c)
          {
            case VK_LEFT  :
            case VK_RIGHT :
            {
  rightleft:
              incr = (c == VK_LEFT) ? -1 : 1;
              do
              {
                /* BUG - we don't know what rc is */
                rc = SHORT1FROMMR(pfnOldEditWndProc(hWnd,WM_CHAR,mp1,mp2));
              } while (rc && !CharToPicInfo(pFmt->szPicture[iPos += incr]));

              EditSetCursor(hWnd);
              return MRFROMSHORT(TRUE);
            }

            case VK_HOME :
              /*
                Pass the HOME key onto the edit control & set position to 0
              */
              pfnOldEditWndProc(hWnd, WM_CHAR, mp1, mp2);
              iPos = 0;
              goto advance;

            case VK_END  :
              /*
                Pass the END key onto the edit control, then get the position
              */
              pfnOldEditWndProc(hWnd, WM_CHAR, mp1, mp2);
              iPos = SHORT1FROMMR(pfnOldEditWndProc(hWnd, EM_QUERYSEL, 0L, 0L));

              /*
                Move left until we hit a maskable character
              */
              while (iPos >= piclen ||
                    (picch = pFmt->szPicture[iPos]) && !CharToPicInfo(picch))
              {
                mp1 = MPFROMSH2CH(KC_VIRTUALKEY, 1, 0);
                mp2 = MPFROM2SHORT(0, VK_LEFT);
                if (!pfnOldEditWndProc(hWnd, WM_CHAR, mp1, mp2))
                  break;
                iPos--;
              }

              EditSetCursor(hWnd);
              return MRFROMSHORT(TRUE);

            case VK_DELETE   :
            case VK_INSERT   :
            case VK_BACKSPACE:
              /*
                 If we have protected chars in this field, don't let the
                 user delete or toggle insert mode.
              */
              if (pFmt->fFormatFlags & FLD_MIXEDPICTURE)
                return MRFROMSHORT(TRUE);
              break;

            default :
              break;
              return MRFROMSHORT(FALSE);

          } /* end switch (c) */
        } /* if picture */
      } /* end if KC_VIRTUALKEY */
      return MRFROMSHORT(TRUE);

    /*---------------------------------------------------------------------*/

    /*
      WM_SETFOCUS
        We intercept this to insure that the cursor is not placed
      over a protected mask character.
    */
    case WM_SETFOCUS :
      if (SHORT1FROMMP(mp2) == FALSE)   /* losing focus? */
        break;

      /*
        Get the window's format structure
      */
      if ((pFmt = (PFORMAT) WinQueryWindowPtr(hWnd, 0)) == NULL ||
          !pFmt->szPicture)
        break;

      /*
        Let the normal edit proc do its thing for setting focus...
      */
      rc = pfnOldEditWndProc(hWnd, msg, mp1, mp2);

      /*
        Get the current position. Move past all protected mask chars.
      */
      iPos = SHORT1FROMMR(pfnOldEditWndProc(hWnd, EM_QUERYSEL, 0L, 0L));
      while ((picch = pFmt->szPicture[iPos]) && CharToPicInfo(picch) == NULL)
      {
        /*
          We move past a mask character by simulating the user pressing
          the RIGHT arrow key.
        */
        mp1 = MPFROMSH2CH(KC_VIRTUALKEY, 1, 0);
        mp2 = MPFROM2SHORT(0, VK_RIGHT);
        if (!pfnOldEditWndProc(hWnd, WM_CHAR, mp1, mp2))
          break;
        iPos++;
      }
      return MRFROMSHORT(rc);

    /*---------------------------------------------------------------------*/

    /*
      WM_PAINT
        We do the drawing and shadowing ourselves.
    */
    case WM_PAINT :
    {
      USHORT iLen;
      HPS    hPS;
      char   szText[128];
      SWP    swp;
      POINTL ptL;
      CURSORINFO cursorInfo;

      /*
        Erase the edit control
      */
      hPS = WinBeginPaint(hWnd, NULL, NULL);
      GpiErase(hPS);
      WinEndPaint(hPS);

      /*
        Get the text and the coordinates of the control.
      */
      iLen = WinQueryWindowText(hWnd, sizeof(szText), (PCH) szText);
      WinQueryWindowPos(hWnd, (PSWP) &swp);

      /*
        Draw the border around the control.
      */
      ptL.x = swp.x;  ptL.y = swp.y;
      GpiMove(hMyPS, (PPOINTL) &ptL);
      ptL.x += swp.cx;
      GpiLine(hMyPS, (PPOINTL) &ptL);
      ptL.y += swp.cy;
      GpiLine(hMyPS, (PPOINTL) &ptL);
      ptL.x -= swp.cx;
      GpiLine(hMyPS, (PPOINTL) &ptL);
      ptL.y -= swp.cy;
      GpiLine(hMyPS, (PPOINTL) &ptL);

      /*
        Draw the shadow
      */
      ptL.x = swp.x + 4;
      ptL.y = swp.y;
      GpiMove(hMyPS, (PPOINTL) &ptL);

      ptL.x += swp.cx;
      ptL.y -= 4;
      GpiBox(hMyPS, DRO_FILL, (PPOINTL) &ptL, 0L, 0L);

      GpiMove(hMyPS, (PPOINTL) &ptL);

      ptL.x = swp.x + swp.cx;
      ptL.y += swp.cy;
      GpiBox(hMyPS, DRO_FILL, (PPOINTL) &ptL, 0L, 0L);

      /*
        Draw the edit text in a monospaced font.
      */
      ptL.x = swp.x;
      ptL.y = swp.y + (swp.cy - FMCourier.lMaxBaselineExt) / 2 
                                           + FMCourier.lInternalLeading;
      GpiCharStringAt(hMyPS, (PPOINTL) &ptL, (LONG) iLen, (PCH) szText);

      EditSetCursor(hWnd);
      /*
        Return FALSE to signify that we processed the message ourselves.
      */
      return MRFROMSHORT(FALSE);
    }

    /*---------------------------------------------------------------------*/

    case WM_DESTROY :
      if ((pFmt = (PFORMAT) WinQueryWindowPtr(hWnd, 0)))
        free(pFmt);
      break;
  } /* switch */

  /*
    We got a message that we weren't interested in. Let the normal edit
    procedure process the message.
  */
  return pfnOldEditWndProc(hWnd, msg, mp1, mp2);
}


/*****************************************************************************/
/* EditSetCursor()                                                           */
/*   Attempts to position the editing cursor correctly within a window       */
/*****************************************************************************/
void pascal EditSetCursor(HWND hWnd)
{
  CURSORINFO cursorInfo;
  SHORT      iPos;

  /*
    Find out the 0-based logical position of the cursor within the edit field
  */
  iPos = SHORT1FROMMR(pfnOldEditWndProc(hWnd, EM_QUERYSEL, 0L, 0L));
  /*
    Get a copy of the cursor information
  */
  WinQueryCursorInfo(HWND_DESKTOP, (PCURSORINFO) &cursorInfo);
  /*
    The cursor is placed to the right of the current character.
  */
  WinCreateCursor(hWnd,
          (SHORT) (iPos+1) * FMCourier.lAveCharWidth - FMCourier.lMaxCharInc,
          cursorInfo.y, 0, 0, CURSOR_SETPOS, (PRECTL) NULL);
}


/*****************************************************************************/
/* CharToPicInfo()                                                           */
/*   Returns a pointer to the PICINFO structure associated with character c. */
/*****************************************************************************/
PICINFO *CharToPicInfo(c)
{
  PICINFO *p;

  c = toupper(c);

  for (p = PicInfo;  p->chPic && p->chPic != c;  p++)
    ;
  return p->chPic ? p : NULL;
}

/*****************************************************************************/
/*                                                                           */
/*                       VALIDATION FUNCTIONS                                */
/*                                                                           */
/*****************************************************************************/

int pascal ValidDigit(c)
{
  return isdigit(c);
}

int pascal ValidDigitSignSpace(c)
{
  return isdigit(c) || isspace(c) || c == '+' || c == '-';
}

int pascal ValidAlpha(c)
{
  return isalpha(c);
}

int pascal ValidAlphaNum(c)
{
  return isalnum(c);
}

int pascal ValidLogical(c)
{
  return strchr("TtFfYyNn", c) != NULL;
}

int pascal ValidAny(c)
{
  return TRUE;
}

int pascal ConvertToUpper(c)
{
  return toupper(c);
}


/*****************************************************************************/
/* MonoStaticWndProc()                                                       */
/*   Used to "front-end" the normal behavior of a static text control so     */
/* that we could print it in a mono-spaced font.                             */
/*                                                                           */
/*****************************************************************************/
MRESULT EXPENTRY MonoStaticWndProc(HWND hWnd,USHORT msg,MPARAM mp1,MPARAM mp2)
{
  HPS    hPS;
  POINTL ptL;
  SHORT  iLen;
  SWP    swp;
  char   szText[128];

  switch (msg)
  {
    case WM_PAINT :
      /*
        Call Begin/EndPaint in order to satisfy PM.
      */
      hPS = WinBeginPaint(hWnd, NULL, NULL);
      WinEndPaint(hPS);

      /*
        Get the text of the static control and its position
      */
      iLen = WinQueryWindowText(hWnd, sizeof(szText), (PCH) szText);
      WinQueryWindowPos(hWnd, (PSWP) &swp);

      /*
        Write the string using our own presentation space
      */
      ptL.x = swp.x;  ptL.y = swp.y;
      GpiCharStringAt(hMyPS, (PPOINTL) &ptL, (LONG) iLen, (PCH) szText);

      /*
        Return FALSE if we processed the message ourselves
      */
      return MRFROMSHORT(FALSE);
  }

  return pfnOldStaticWndProc(hWnd, msg, mp1, mp2);
}



/*****************************************************************************/
/* SetMonospacedFont()                                                       */
/*   Sets the font of the current presentation space to a mono-spaced        */
/* font. Returns TRUE if we found the font, FALSE if not.                    */
/*****************************************************************************/
SetMonospacedFont(hPS)
  HPS hPS;
{
  FONTMETRICS *pMetrics;
  LONG   nFonts, nRequestFonts;
  int    i;
  LONG   rcMatch;
  ERRORID errID;
  PSZ    szFont = "Courier";

  static BOOL bFirstTime = TRUE;

  /*
    Load in the Courier font
  */
  if (bFirstTime)
  {
    GpiLoadFonts(hAB, (PSZ) "C:\\os2\\dll\\courier.fon");
    bFirstTime = FALSE;
  }

  /*
    Make a dummy call to GpiQueryFonts() in order to find out how many
    courier fonts are available.
  */
  nRequestFonts = 0L;
  nFonts = GpiQueryFonts(hPS, QF_PUBLIC | QF_PRIVATE, (PSZ) szFont,
                         &nRequestFonts, 0L, NULL);
  if (nFonts == 0)
    return FALSE;

  /*
    Allocate temp space to hold the font-metrics info
  */
  if ((pMetrics = malloc((unsigned int) nFonts * sizeof(FONTMETRICS))) == NULL)
    return FALSE;

  /*
    Get the font-metric info for all Courier fonts
  */
  GpiQueryFonts(hPS, QF_PUBLIC | QF_PRIVATE, (PSZ) szFont, &nFonts,
                                (LONG) sizeof(FONTMETRICS), pMetrics);

  for (i = 0;  i < nFonts;  i++)
  {
    /*
      Set up the FontAttrs structure in preparation for GpiCreateLogFont()
    */
    FontAttrs.usRecordLength  = sizeof(FontAttrs);
    FontAttrs.fsSelection     = pMetrics[i].fsSelection;
    FontAttrs.lMatch          = pMetrics[i].lMatch;
    strcpy(FontAttrs.szFacename, szFont);
    FontAttrs.idRegistry      = pMetrics[i].idRegistry;
    FontAttrs.usCodePage      = 850;
    FontAttrs.lMaxBaselineExt = pMetrics[i].lMaxBaselineExt;
    FontAttrs.lAveCharWidth   = pMetrics[i].lAveCharWidth;
    FontAttrs.fsType          = FATTR_TYPE_FIXED;
    FontAttrs.fsFontUse       = 0;

    /*
      Given the above attribures, try to find the best match in a Courier font
    */
    rcMatch = GpiCreateLogFont(hPS, (PSTR8) szFont,
                               (LONG) ++IDCourier, (PFATTRS) &FontAttrs);
    /*
      We got a match if rcMatch is 2
    */
    if (rcMatch == 2)
    {
      GpiSetCharSet(hPS, IDCourier);
      GpiQueryFontMetrics(hPS, (long) sizeof(FONTMETRICS), &FMCourier);
      /*
        If the matched font is 16x9, then let's use it.
      */
      if (FMCourier.lAveCharWidth == 9L 
          && FMCourier.lMaxBaselineExt >= 16L)
        break;
    }
  }

  free(pMetrics);
  return (i < nFonts);
}


/*****************************************************************************/
/* FormatError()                                                             */
/*   Sounds a little beep when the user presses a bad key.                   */
/*****************************************************************************/
FormatError()
{
  WinAlarm(HWND_DESKTOP, WA_WARNING);
  return TRUE;
}


/*****************************************************************************/
/* lstrlen(), lstrcpy()                                                      */
/*   Miscellaneous far-pointer routines                                      */
/*****************************************************************************/
int lstrlen(PSZ s)
{
  int len = 0;
  while (*s++)
    len++;
  return len;
}

PSZ lstrcpy(PSZ s, PSZ t)
{
  PSZ orig_s = s;
  while (*s++ = *t++) ;
  return orig_s;
}


/*****************************************************************************/
/* DEBUG()                                                                   */
/*   Simple diagnostic routine which displays a message box.                 */
/*****************************************************************************/
char szDebug[82];

DEBUG(char *fmt, ...)
{
  va_list arg;

  va_start(arg, fmt);
  vsprintf(szDebug, fmt, arg);         /* format the message */
  va_end(arg);
  SimpleMessage(hWndClient, szDebug);  /* put it in a message box */
}

