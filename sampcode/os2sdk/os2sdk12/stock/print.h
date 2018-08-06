/***	print.h
 *
 */

//  index values for PrintQueryInfo

#define PQPI_NAME		1
#define PQPI_LOG_ADDRESS	2
#define PQPI_DRIVER		3
#define PQPI_MODEL		4
#define PQPI_TYPE		5
#define PQPI_PRINTER		6
#define PQPI_DESCRIPTION	7


typedef VOID *HPRINTERLIST; /* hprtlist */
typedef VOID *HPRINTER;     /* hprt */

HPRINTERLIST PrintCreatePrinterList(HAB hab);
BOOL	  PrintDestroyPrinterList(HPRINTERLIST hprtlist);
HPRINTER  PrintQueryNextPrinter(HPRINTERLIST hprtlist,HPRINTER hprt);
HPRINTER  PrintQueryDefaultPrinter(HPRINTERLIST hprtlist);
ULONG	  PrintQueryPrinterInfo(HPRINTER hprt,USHORT index);

HDC	  PrintOpenDC(HAB hab,HPRINTER hprt,char *pszDataType);

BOOL	  PrintQueryJobProperties(HPRINTER hprt,USHORT *pcbData,BYTE *pbData);
BOOL	  PrintChangeJobProperties(HPRINTER hprt);
VOID	  PrintResetJobProperties(HPRINTER hprt);

HPRINTER  PrintMatchPrinter(HPRINTERLIST hprtlist,USHORT cbData,BYTE *pbData);
