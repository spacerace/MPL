/* bug.c  --  shows how different levels of debugging  */
/*            output can be produced using #if         */

#define DEBUG_LEVEL 2      /* 0 = none, 1-2 for debug  */
#include <stdio.h>

main()
{
    int ret;

#if (DEBUG_LEVEL == 2)
    fprintf(stderr, "Entering main()\n");
#endif

#if (DEBUG_LEVEL == 1)
    fprintf(stderr, "Calling sub()\n");
#endif

    ret = sub();

#if (DEBUG_LEVEL == 1)
    fprintf(stderr, "sub() returned %d\n", ret);
#endif

#if (DEBUG_LEVEL == 2)
    fprintf(stderr, "Leaving main()\n");
#endif

}

sub()
{
    return (5);
}
