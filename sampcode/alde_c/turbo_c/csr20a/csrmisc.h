/*
**  C S R M I S C . H
**
**  Miscellaneous C Spot Run macros etc.
**
**  Created: 04/20/86   Last Updated: 12/05/86
**
*/

#define dummy()

			/* min() max(), missing in MSC 3.0 */

#define min(a,b)	(( (a) < (b) ) ? (a) : (b) )
#define max(a,b)	(( (a) > (b) ) ? (a) : (b) )

#define iswild(c)	( (c) == '?' || (c) == '*' )
