#include <dos.h>

struct DESC
	{
	unsigned limit;
	unsigned base_lo;
	unsigned char base_hi;
	unsigned char rights;
	unsigned reserved;
	};

struct GDT
	{
	struct DESC
		dummy,
		gdt_desc,
		src,
		tgt,
		bios_cs,
		st_reg;
	};

static struct GDT gdt = {   { 0,0,0,0x93,0 },
							{ 0,0,0,0x93,0 },
							{ 0,0,0,0x93,0 },
							{ 0,0,0,0x93,0 },
							{ 0,0,0,0x93,0 },
							{ 0,0,0,0x93,0 }
						};

unsigned copy( unsigned long source, unsigned long dest, unsigned nbytes )
{

	gdt.src.limit = nbytes;
	gdt.src.base_lo = source % 65536;
	gdt.src.base_hi = source / 65536;

	gdt.tgt.limit = nbytes;
	gdt.tgt.base_lo = dest % 65536;
	gdt.tgt.base_hi = dest / 65536;

	_ES = _DS;
	_SI = (unsigned)&gdt;
	_CX = nbytes/2;
	_AH = 0x87;
	geninterrupt( 0x15 );
	return( _AH );
}
