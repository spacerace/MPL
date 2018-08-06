#include <osbind.h>
#include <stdio.h>
#include <io.h>

extern char	*_base;
extern int	_argc;
extern char	**_argv;
extern char	*_envp;

FILE	_iob[MAXFILES] =			/* stream buffers */
{
/*in*/	{0, F_READ|F_DEVICE, NULL, NULL, 0, 0, '\0', '\0'},
/*out*/	{1, F_WRITE|F_DEVICE, NULL, NULL, 0, 0, '\0', '\0'},
/*err*/	{-1, F_WRITE|F_DEVICE, NULL, NULL, 0, 0, '\0', '\0'},
/*prn*/	{-3, F_WRITE|F_DEVICE, NULL, NULL, 0, 0, '\0', '\0'},
/*aux*/	{-2, F_READ|F_WRITE|F_DEVICE, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'},
	{0, 0, NULL, NULL, 0, 0, '\0', '\0'}
};

void _main(cmdline, cmdlen)
char *cmdline;
int cmdlen;
/*
 *	Parses command line, opens standard files, plus other assorted
 *	general setup. Then calls user's main() function.  If main()
 *	returns, exit() is called with a 0 return value.  The following
 *	standard streams are initialized by _main():
 *		stdin		Standard input, may be redirected
 *		stdout		Standard output, may be redirected
 *		stderr		Usually the system console
 *		stdprn		The standard printer (output only)
 *		stdaux		The communication port (input/output)
 *	The _initargs() function is called to process the command line.
 *	The global variables "_argc", "_argv" and "_envp" are used to
 *	store the values to be passed to the user's main().
 */
{
	void exit();

	_argc = 0;
	_argv = NULL;
	_envp = *((char **) (_base + 0x2C));
	_initargs(cmdline, cmdlen);
	main(_argc, _argv, _envp);
	exit(0);
}

void exit(status)
int status;
/*
 *	Flushes and closes all open streams.  Returns <status> value to
 *	the operating system.
 */
{
	register int i, f;

	for(i=0; i<MAXFILES; ++i) {
		f = _iob[i].F_stat;
		if((f & (F_READ | F_WRITE)) && !(f & F_DEVICE))
			fclose(&_iob[i]);
	}
	_exit(status);
}
