#include <dos.h>
prtscn()
{
	struct HREG REG;
	int86(0x05,&REG,&REG);
}
                                                  