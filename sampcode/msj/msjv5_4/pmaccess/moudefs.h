/* moudefs.h RHS
 *
 * macros and definitions for simplifying mouse handling
 */

#define MOU_B1       (MOUSE_MOTION_WITH_BN1_DOWN | MOUSE_BN1_DOWN)
#define MOU_B2       (MOUSE_MOTION_WITH_BN2_DOWN | MOUSE_BN2_DOWN)
#define MOU_B3       (MOUSE_MOTION_WITH_BN3_DOWN | MOUSE_BN3_DOWN)

#define MOUNOMOVE  (MOUSE_BN1_DOWN | MOUSE_BN2_DOWN | MOUSE_BN3_DOWN)
#define MOUMOVED   (MOUSE_MOTION | MOUSE_MOTION_WITH_BN1_DOWN | \
                    MOUSE_MOTION_WITH_BN2_DOWN | \
                    MOUSE_MOTION_WITH_BN3_DOWN)

#define MouNoMove(event)            (MOUNOMOVE & event.fs)
#define MouMoved(event)             (MOUMOVED & event.fs)
#define MouButtonPressed(e,b)       (e.fs & b)
#define MouB1Pressed(event)         (event & MOU_B1)
#define MouB2Pressed(event)         (event & MOU_B2)
#define MouB3Pressed(event)         (event & MOU_B3)
#define MouButtons(event)           (event.fs & (MOU_B1 | MOU_B2 | \
                                                 MOU_B3))

#define IsMouButtonPressed(event)   (event.fs & 0xffff)
#define MouEventDropLowBit(event)   (event.fs &= ~MOUSE_MOTION)
