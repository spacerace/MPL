/***	ea.h - layer on EA APIs
 *
 *	History
 *	    08-Feb-1990 bens	Initial version (subset of ea.exe)
 *	    02-May-1990 bens	Added SetEAValue (copied from ea.exe)
 */

char *EAQueryValue(char *pszFile,char *pszName,USHORT *pcbValue);
BOOL EASetValue(char *pszFile,char *pszName,USHORT cbValue,char *pszValue);
