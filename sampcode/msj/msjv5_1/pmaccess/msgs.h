/* msgs.h RHS 9/30/89
 *
 * global message macros and defines for kernel messaging system
 *
 */

#define     SCAN                    0x0100
#define     EVENT                   0x0200
#define     MOUSE                   0x1000

#define     ASCIICODE(val)          (val)
#define     SCANCODE(val)           (val | SCAN)
#define     EVENTCODE(val)          (val | EVENT)

#define     MOUSECODE(val)          (val | MOUSE)
#define     ButtonEvent(event)      (event & MOUSE)
#define     MOUSECODEOFF(val)       (val &= ~MOUSE)


#define     MSG_COPY                EVENTCODE(1)
#define     MSG_CUT                 EVENTCODE(2)
#define     MSG_PASTE               EVENTCODE(3)

#define     MSG_CLR                 EVENTCODE(14)
#define     MSG_ESCAPE              EVENTCODE(15)

#define     MSG_UPARROW             SCANCODE(72)
#define     MSG_LEFTARROW           SCANCODE(75)
#define     MSG_RIGHTARROW          SCANCODE(77)
#define     MSG_DOWNARROW           SCANCODE(80)
#define     MSG_DEL                 SCANCODE(83)
#define     MSG_HOME                SCANCODE(71)
#define     MSG_END                 SCANCODE(79)

#define     MSG_B1UP                EVENTCODE(100)
#define     MSG_B1DOWN              EVENTCODE(101)
#define     MSG_B2UP                EVENTCODE(102)
#define     MSG_B2DOWN              EVENTCODE(103)
#define     MSG_B3UP                EVENTCODE(104)
#define     MSG_B3DOWN              EVENTCODE(105)
#define     MSG_MOUSEMOVED          EVENTCODE(106)
#define     MSG_CHAR                EVENTCODE(107)

#define     ENTER                   13
#define     ESC                     27
#define     MSG_BACKSPACE           8
#define     ALT_C                   SCANCODE(46)
#define     ALT_O                   SCANCODE(24)
#define     ALT_U                   SCANCODE(22)
#define     ALT_P                   SCANCODE(25)

#define     MOUMSG(ptr)             ((MOUEVENTINFO FAR *)ptr)
#define     KBDMSG(ptr)             ((KBDKEYINFO FAR *)ptr)


