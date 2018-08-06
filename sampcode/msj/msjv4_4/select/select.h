/*

Author: Gregg Spaulding
        Tencor Instruments
        2400 Charleston Road
        Mountain View, Ca. 94043

    The following defines apply to the "select" control type.

    Both a mouse and a keyboard interface are provided.  Clicking
    the left mouse button advances the control to the next string.
    Clicking the right button moves to the previous string.  The
    control may obtain the input focus via the normal dialog box 
    method of the tab and back tab keys.  Once highlighted, the 
    arrow keys will advance through the strings.

    Notes:
        (My development was done with an EGA)
        when using this control in a dialog box, it must be 18 units
        in height (as defined in the CONTROL statement).  It can be 
        most easily placed using an "edit" control in the DIALOG.EXE 
        utility and stretching it to the 18 units necessary.  The width
        is arbitrary but should be large enough to accomodate the largest
        string which may appear in it.  The circular arrows are part of
        the control and occupy approximately two character widths.
        Once the .DLG file is created, it should be edited and the 
        CONTROL type "edit" replaced with "select".  The window styles
        should be modified to read only 'WS_TABSTOP | WS_CHILD'.
*/
/* messages processed by control */
#define SL_ADDSTRING        LB_ADDSTRING
#define SL_SETCURSEL        LB_SETCURSEL
#define SL_GETCURSEL        LB_GETCURSEL
#define SL_GETTEXT          LB_GETTEXT
#define SL_GETCOUNT         LB_GETCOUNT

/* notification messages */
#define SLN_SELCHANGE       LBN_SELCHANGE

/*
    SL_ADDSTRING
    wParam:        not used
    lParam:        long pointer to zero terminated string to be added
    Comments:      strings take on ordinal numbers in the sequentially as
                   they are added.  i.e. the first string is 0, the second
                   is 1, the third is 2, and so on.  These are the numbers
                   used by SL_SETCURSEL and SL_GETCURSEL.

    SL_SETCURSEL
    wParam:        ordinal number of string to set as the current selection
    lParam:        not used
    Comments:      has no effect if wParam is not a legel value

    SL_GETCURSEL
    wParam:        not used
    lParam:        not used
    Comments:      returns ordinal number of current selection as return
                   value of SendMessage or SendDlgItemMessage.

    SL_GETTEXT
    wParam:        ordinal number of string to copy
    lParam:        long pointer to location where string should be copied to
    Comments:      copies the null string if wParam is not a legal value

    SL_GETCOUNT
    wParam:        not used
    lParam:        not used
    Comments:      returns the number of currently defined strings for the 
                   control as the return value of SendMessage or 
                   SendDlgItemMessage


    WM_COMMAND
    wParam:        control ID
    lParam:        LOWORD = window handle, HIWORD = SLN_SELCHANGE
    Comments:      this message is sent BY the control TO the dialog function
                   when the selection has been changed
*/

