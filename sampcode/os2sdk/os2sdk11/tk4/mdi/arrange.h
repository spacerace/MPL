/***************************************************************************\
* arrange.h - public interface of a window rearrangement module.
*
* Created by Microsoft Corporation, 1989
\***************************************************************************/
 


#define AWP_TILED     1
#define AWP_CASCADED  2

 
BOOL ArrangeWindowPositions(PRECTL, SHORT, PSWP, USHORT);
