/*
 * STEVIE - ST Editor for VI Enthusiasts   ...Tim Thompson...twitch!tjt...
 *
 * Extensive modifications by:  Tony Andrews       onecom!wldrdg!tony
 * Turbo C 1.5 port by: Denny Muscatelli 061988
 */

/*
 * Settable parameters
 */

struct	param {
	char	*fullname;	/* full parameter name */
	char	*shortname;	/* permissible abbreviation */
	int	value;		/* parameter value */
	int	flags;
};

extern	struct	param	params[];

/*
 * Flags
 */
#define	P_BOOL		0x01	/* the parameter is boolean */
#define	P_NUM		0x02	/* the parameter is numeric */
#define	P_CHANGED	0x04	/* the parameter has been changed */

/*
 * The following are the indices in the params array for each parameter
 */

/*
 * Numeric parameters
 */
#define	P_TS		0	/* tab size */
#define	P_SS		1	/* scroll size */
#define	P_RP		2	/* report */
#define	P_LI		3	/* lines */

/*
 * Boolean parameters
 */
#define	P_VB		4	/* visual bell */
#define	P_SM		5	/* showmatch */
#define	P_WS		6	/* wrap scan */
#define	P_EB		7	/* error bells */
#define	P_MO		8	/* show mode */
#define	P_BK		9	/* make backups when writing out files */
#define	P_CR		10	/* use cr-lf to terminate lines on writes */
#define	P_LS		11	/* show tabs and newlines graphically */

#if 0
#define	P_AI		12	/* auto-indent (not really in yet) */
#endif

/*
 * Macro to get the value of a parameter
 */
#define	P(n)	(params[n].value)
