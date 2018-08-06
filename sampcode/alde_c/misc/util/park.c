#include <stdio.h>

/*
**  park fixed disk heads
*/

main()
{
    int cyl;
    struct {int ax,bx,cx,dx,si,di,ds,es;}srv;

    srv.ax = 0x0800;
    srv.dx = 0x0080;

    /* get fixed disk drive parameters */

    if ( sysint ( 0x13, &srv, &srv ) & 1 )
        abort ("park: Drive parms NOT returned\n");

    srv.ax = 0x0c00;            /* set 'seek' command */
    cyl = ((srv.cx & 0x00c0)<<2) | ((srv.cx & 0xff00)>>8);
    srv.dx = 0x0080;

    if ( sysint ( 0x13, &srv, &srv ) & 1 ) /* park the heads */
        abort ("park: Drive C: FAILED to park - error = %02xh\n",
              (srv.ax & 0xff00)>>8);
    else
        printf("park: Drive C: parked at cylinder %d\n", cyl);
}
