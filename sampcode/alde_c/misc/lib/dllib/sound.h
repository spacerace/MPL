/*
**          SM.LIB  function source code
**  Copyright 1986, S.E. Margison
**
**  FUNCTION: sound.h
** defines for IBM speaker control
*/

/* defines for mktone() update parameter: */
#define ON 0          /* turn the speaker on */
#define UPDATE 1      /* sound is on, just change freq */
#define TOGGLE 2      /* for delay use, turn on, then off */

/* port equates */
#define SCNTRL 97     /* sound control port */
#define SOUNDON 0x03  /* bit mask to enable speaker */
#define SOUNDOFF 0xfc /* bit mask to disable speaker */
#define C8253 67      /* port address to control 8253 */
#define SETIMER 182   /* tell 8253 to expect freq data next */
#define F8253 66      /* frequency address on 8253 */
