;***************************************************************************
;									   *
;		Copyright (C) 1984-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************



;	EGA Register Definitions.
;
;	The following definitions are the EGA registers and values
;	used by this driver.  All other registers are set up by
;	the BIOS and never altered (except the palette registers
;	which are set with a BIOS call).
;
;	After careful examination of the Initial EGA registers for
;	the two modes supported by the driver (640x200 8 color and
;	640x350 8 color with at least 128K RAM), all unspecified
;	bits in the following registers were found to be 0.



;	Sequencer Registers Used

rSequencer	equ	03C4h		;Sequencer Address Register

  rMapMask	equ	02h		;Plane Write Enable Mask
    C0		equ	00000001b	;Plane C0
    C1		equ	00000010b	;Plane C1
    C2		equ	00000100b	;Plane C2
    C3		equ	00001000b	;Plane C3

rMisc		equ	03c2h		;Misc Output Register
rCrtc		equ	03d4h		;CRTC controller register
 rSahr		equ	0ch		;start address high register
 rSalr		equ	0dh		;start address low register

rAttread	equ	03dah		;Attribute read register
  mDEnable	equ	1		;display enable
  mLPStrobe	equ	2		;light pen strobe
  mLPSwitch	equ	4		;light pen switch
  mVRetrace	equ	8		;vertical retrace


rAttwrite	equ	03c0h		;Attribute write register
rGraphics1	equ	03cch		;Graphics register 1
rGraphics2	equ	03cah		;Graphics register 2


;	Graphics Controller Registers Used

rGraphics	equ	03CEh		;Graphics Controller Address Register

  rEnableSR	equ	01h		;Set/Reset Enable

  rColorComp	equ	02h		;Color Compare Register
   ccColor	equ	00000111b	;  Color goes in these three bits

  rDataRotate	equ	03h		;Data Rotate Register
   drRotCnt	equ	00000111b	;  Data Rotate Count
   drSET	equ	00000000b	;  Data Unmodified
   drAND	equ	00001000b	;  Data ANDed with latches
   drOR 	equ	00010000b	;  Data ORed  with latches
   drXOR	equ	00011000b	;  Data XORed with latches

 rReadMap	equ	04h		;Read Map Select Register
   rmC0 	equ	00000000b
   rmC1 	equ	00000001b
   rmC2 	equ	00000010b
   rmC3 	equ	00000011b
;  rmRed	equ	00000000b	;  Read red   plane
;  rmGreen	equ	00000001b	;  Read green plane
;  rmBlue	equ	00000010b	;  Read blue  plane

rMode		equ	05h		;Mode Register
  mProcWrite	equ	00000000b	;  Write processor data rotated
  mLatchWrite	equ	00000001b	;  Write latched data
  mColorWrite	equ	00000010b	;  Write processor data as color
  mDataRead	equ	00000000b	;  Read selected plane
  mColorRead	equ	00001000b	;  Read color compare

rColorDontCare	equ	07h		;Color Don't Care Register


;	Note that the Bit Mask Register works by writting 0 bits
;	in the Bit Mask Register with the data in the processor
;	latches.  This will require some critical section code
;	to prevent the cursor from destroying the latched data.

rBitMask	equ	08h		;Bit Mask Register






;	The EGA Flags are used to indiate to the cursor code what
;	information is valid in the shadow registers:
;
;	    All  - All shadow registers need to be restored
;
;	    Most - All but the MapMask needs to be restored.  The
;		   cursor code leaves the MapMask set for all three
;		   planes on exit if this is set.
;
;	    Some - The MapMask and BitMask Register don't need to be
;		   restored.  The MapMask will be set for all three
;		   planes, and the BitMask will be set for all bits
;		   enabled for write.
;
;	    None - Registers don't have to be restored


EGAAll	equ	4
EGAMost equ	3
EGASome equ	2
EGANone equ	0





;	EGA specific flags for cursor coordination
;
;	The structure is set up so a loop can be performed
;	getting the values to output and outputting them.
;	The flag values are set up to be the index of the
;	number of registers that need to be output.  Some
;	routines which leave the map mask as 111b can indicate
;	that that register needn't be restored by the cursor
;	code.  Since the cursor code always uses the MapMask
;	set to 111b, this saves a little time.


EGADef		struc

		dw	rGraphics	;Data Rotate Register
DataRotate	dw	drSet*256+rDataRotate

		dw	rGraphics	;Mode Register
Mode		dw	(mProcWrite+mDataRead)*256+rMode

		dw	rGraphics	;Bit Mask Register
Bitmask 	dw	0FF00h+rBitMask

		dw	rSequencer	;Plane Write Enable Mask
MapMask 	dw	(C0+C1+C2)*256+rmapMask


EGAFlags	db	EGANone 	;Controlling flags
		db	0		;  (msb of flags is always zero)

EGADef		ends
