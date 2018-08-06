/******************************************************************************
*									      *
*  return pointer to error message, based on bios return code		      *
*									      *
******************************************************************************/
char *dskerrmsg(error_num)
char error_num;
{

int j=0;
static char *ferr [] = { "\xe0Status",
			 "\xccWrite Fault",
			 "\xbbUndefined",
			 "\xaaNot Ready",
			 "\x80Fail Respond",
			 "\x40Bad Seek",
			 "\x20Controller",
			 "\x11ECC Corrected",
			 "\x10CRC",
			 "\x0aBad Sector Flag",
			 "\x09DMA Boundary",
			 "\x08DMA",
			 "\x07Parm Activity",
			 "\x06Media Change",
			 "\x05Reset",
			 "\x04Sector Not Found",
			 "\x03Write Protect",
			 "\x02Addr Mark",
			 "\x01Parameter",
			 "\x00Unknown" };
  while (*ferr[j])
    {
    if (error_num == *ferr[j]) break;
    j++;
    }
  return((ferr[j]+1));
}
