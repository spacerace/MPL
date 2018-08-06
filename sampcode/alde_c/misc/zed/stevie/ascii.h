/*
 * STEVIE - ST Editor for VI Enthusiasts   ...Tim Thompson...twitch!tjt...
 *
 * Extensive modifications by:  Tony Andrews       onecom!wldrdg!tony
 * Turbo C 1.5 port by: Denny Muscatelli 061988
 */

/*
 * Definitions of various common control characters
 */

#define	NUL	'\0'
#define	BS	'\010'
#define	TAB	'\011'
#define	NL	'\012'
#define	CR	'\015'
#define	ESC	'\033'

#define	CTRL(x)	((x) & 0x1f)
