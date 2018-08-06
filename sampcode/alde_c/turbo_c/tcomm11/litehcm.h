/*
**	litehcm.h
**		Communications support for Datalight (tm) C
**		Copyright (c) 1987 - Information Technology, Ltd.,
**							 All Rights Reserved
*/

#ifndef LITEHCM_H
#define LITEHCM_H 1

#include "litecomm.h"

#define NUMRES	0				/* numeric result codes */
#define WRDRES	1				/* word result codes */
#define SPKOFF	0				/* speaker off */
#define SPKON	1				/* speaker on until carrier */
#define SPKSPC	2				/* speaker always on */
#define ONHK	0				/* go on-hook (hangup) */
#define OFFHK	1				/* go off-hook (lift receiver) */
#define OFFHKS	2				/* go off-hook, don't close aux relay */
#define BASIC	0				/* basic result set */
#define EXSET1	1				/* extended results set 1 */
#define EXSET3	2				/* extended results set 3 */
#define EXSET4	3				/* extended results set 4 */
#ifdef HCMAIN
static int	_restype;
static int	_resset;
#endif
#endif							/* ifdef LITEHCM_H */
