/*
 *	sound.h -- header for sound routines
 */

#define PPI	  0x61
#define SPKR	  0x03
#define SPKR_ON	  outp(PPI, inp(PPI) | SPKR)
#define SPKR_OFF  outp(PPI, inp(PPI) & ~SPKR)
