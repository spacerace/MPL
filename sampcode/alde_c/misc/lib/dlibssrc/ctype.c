/*
 *  CTYPE.C	Character classification and conversion
 */

#include <d:\usr\stdlib\stdio.h>
#include <d:\usr\stdlib\ctype.h>

#undef	toupper
#undef	tolower

unsigned char _ctype[128] = {
	/* 0x00 */	_CTc,
	/* 0x01 */	_CTc,
	/* 0x02 */	_CTc,
	/* 0x03 */	_CTc,
	/* 0x04 */	_CTc,
	/* 0x05 */	_CTc,
	/* 0x06 */	_CTc,
	/* 0x07 */	_CTc,
	/* 0x08 */	_CTc,
	/* 0x09 */	_CTc|_CTs,
	/* 0x0A */	_CTc|_CTs,
	/* 0x0B */	_CTc|_CTs,
	/* 0x0C */	_CTc|_CTs,
	/* 0x0D */	_CTc|_CTs,
	/* 0x0E */	_CTc,
	/* 0x0F */	_CTc,

	/* 0x10 */	_CTc,
	/* 0x11 */	_CTc,
	/* 0x12 */	_CTc,
	/* 0x13 */	_CTc,
	/* 0x14 */	_CTc,
	/* 0x15 */	_CTc,
	/* 0x16 */	_CTc,
	/* 0x17 */	_CTc,
	/* 0x18 */	_CTc,
	/* 0x19 */	_CTc,
	/* 0x1A */	_CTc,
	/* 0x1B */	_CTc,
	/* 0x1C */	_CTc,
	/* 0x1D */	_CTc,
	/* 0x1E */	_CTc,
	/* 0x1F */	_CTc,

	/* 0x20 */	_CTs,
	/* 0x21 */	_CTp,
	/* 0x22 */	_CTp,
	/* 0x23 */	_CTp,
	/* 0x24 */	_CTp,
	/* 0x25 */	_CTp,
	/* 0x26 */	_CTp,
	/* 0x27 */	_CTp,
	/* 0x28 */	_CTp,
	/* 0x29 */	_CTp,
	/* 0x2A */	_CTp,
	/* 0x2B */	_CTp,
	/* 0x2C */	_CTp,
	/* 0x2D */	_CTp,
	/* 0x2E */	_CTp,
	/* 0x2F */	_CTp,

	/* 0x30 */	_CTd|_CTx,
	/* 0x31 */	_CTd|_CTx,
	/* 0x32 */	_CTd|_CTx,
	/* 0x33 */	_CTd|_CTx,
	/* 0x34 */	_CTd|_CTx,
	/* 0x35 */	_CTd|_CTx,
	/* 0x36 */	_CTd|_CTx,
	/* 0x37 */	_CTd|_CTx,
	/* 0x38 */	_CTd|_CTx,
	/* 0x39 */	_CTd|_CTx,
	/* 0x3A */	_CTp,
	/* 0x3B */	_CTp,
	/* 0x3C */	_CTp,
	/* 0x3D */	_CTp,
	/* 0x3E */	_CTp,
	/* 0x3F */	_CTp,

	/* 0x40 */	_CTp,
	/* 0x41 */	_CTu|_CTx,
	/* 0x42 */	_CTu|_CTx,
	/* 0x43 */	_CTu|_CTx,
	/* 0x44 */	_CTu|_CTx,
	/* 0x45 */	_CTu|_CTx,
	/* 0x46 */	_CTu|_CTx,
	/* 0x47 */	_CTu,
	/* 0x48 */	_CTu,
	/* 0x49 */	_CTu,
	/* 0x4A */	_CTu,
	/* 0x4B */	_CTu,
	/* 0x4C */	_CTu,
	/* 0x4D */	_CTu,
	/* 0x4E */	_CTu,
	/* 0x4F */	_CTu,

	/* 0x50 */	_CTu,
	/* 0x51 */	_CTu,
	/* 0x52 */	_CTu,
	/* 0x53 */	_CTu,
	/* 0x54 */	_CTu,
	/* 0x55 */	_CTu,
	/* 0x56 */	_CTu,
	/* 0x57 */	_CTu,
	/* 0x58 */	_CTu,
	/* 0x59 */	_CTu,
	/* 0x5A */	_CTu,
	/* 0x5B */	_CTp,
	/* 0x5C */	_CTp,
	/* 0x5D */	_CTp,
	/* 0x5E */	_CTp,
	/* 0x5F */	_CTp,

	/* 0x60 */	_CTp,
	/* 0x61 */	_CTl|_CTx,
	/* 0x62 */	_CTl|_CTx,
	/* 0x63 */	_CTl|_CTx,
	/* 0x64 */	_CTl|_CTx,
	/* 0x65 */	_CTl|_CTx,
	/* 0x66 */	_CTl|_CTx,
	/* 0x67 */	_CTl,
	/* 0x68 */	_CTl,
	/* 0x69 */	_CTl,
	/* 0x6A */	_CTl,
	/* 0x6B */	_CTl,
	/* 0x6C */	_CTl,
	/* 0x6D */	_CTl,
	/* 0x6E */	_CTl,
	/* 0x6F */	_CTl,

	/* 0x70 */	_CTl,
	/* 0x71 */	_CTl,
	/* 0x72 */	_CTl,
	/* 0x73 */	_CTl,
	/* 0x74 */	_CTl,
	/* 0x75 */	_CTl,
	/* 0x76 */	_CTl,
	/* 0x77 */	_CTl,
	/* 0x78 */	_CTl,
	/* 0x79 */	_CTl,
	/* 0x7A */	_CTl,
	/* 0x7B */	_CTp,
	/* 0x7C */	_CTp,
	/* 0x7D */	_CTp,
	/* 0x7E */	_CTp,
	/* 0x7F */	_CTc
};

int toupper(c)
int c;
{
	return(islower(c) ? (c)^0x20 : (c));
}

int tolower(c)
int c;
{
	return(isupper(c) ? (c)^0x20 : (c));
}
