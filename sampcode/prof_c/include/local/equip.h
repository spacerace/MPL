/*
 *	equip.h -- header for equipment determination/inventory
 */

struct EQUIP {
	short	disksys,	/* 1 if disks installed */
		game_io,	/* 1 if game i/o adapter installed */
		nprint,		/* number of printer ports */
		nrs232,		/* number of serial ports */
		vmode,		/* initial video mode (from switches) */
		ndrive,		/* number of disk drives (from switches) */
		basemem;	/* amount of base memory in Kbytes */
};
