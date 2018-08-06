/* pmaccess.h RHS 7/15/89
 *
 * macros and typedefs for dipop.c
 */


#define    BTOGGLE  0x0001
#define    BPRESS   0x0002
#define    INPUT    0X0003

typedef    struct _button
    {
    char        *text;                          // text to be displayed
    USHORT      startrow;                       // upper row
    USHORT      startcol;                       // left column
    USHORT      endrow;                         // lower row
    USHORT      endcol;                         // right column
    BYTE        attribute;                      // color attribute
    USHORT      type;                           // type of object
    USHORT      left_button_val;                // mouse left button event
    USHORT      right_button_val;               // mouse right button event
    USHORT      accelerator;                    // keyboard event
    USHORT      state;                          // button state: on or off
    } BUTTON;


#define    MOUMSG(ptr)    ((MOUEVENTINFO *)ptr)
#define    KBDMSG(ptr)    ((KBDKEYINFO *)ptr)

