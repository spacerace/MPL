/* kbddefs.h
 *
 * macros and definitions to simplify keyboard handling
 */

#define  LEFTSHIFT       0x0002
#define  LEFTCTRL        0x0100
#define  LEFTALT         0x0200
#define  CAPS            0x0040
#define  INSERT          0x0080
#define  SYSREQ          0x8000
#define  RIGHTALT        0x0800
#define  RIGHTCTRL       0x0400
#define  RIGHTSHIFT      0x0001
#define  ECHO            0x0001
#define  RAW             0x0004
#define  COOKED          0x0008

#define  KbdLeftShift(info)  (info.fsState & LEFTSHIFT)
#define  KbdLeftCtrl(info)   (info.fsState & LEFTCTRL)
#define  KbdLeftAlt(info)    (info.fsState & LEFTALT)
#define  KbdCapsL(info)      (info.fsState & CAPS)
#define  KbdNumL(info)       (info.fsState & NUMLOCK)
#define  KbdScrLock(info)    (info.fsState & SCROLLLOCK)
#define  KbdInsert(info)     (info.fsState & INSERT)
#define  KbdSysReq(info)     (info.fsState & SYSREQ)
#define  KbdRightAlt(info)   (info.fsState & RIGHTALT)
#define  KbdRightCtrl(info)  (info.fsState & RIGHTCTRL)
#define  KbdRightShift(info) (info.fsState & RIGHTSHIFT)

#define  KbdEcho(info)     (info.fsMask & ECHO)
#define  KbdMode(info)     (info.fsMask & RAW)

