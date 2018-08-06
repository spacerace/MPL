/* COM.C illustrates serial port access using function:
 *      _bios_serialcom
 */

#include <bios.h>
#include <stdio.h>

main()
{
    unsigned status, port;

    for( port = 0; port < 3; port++ )
    {
        status = _bios_serialcom( _COM_STATUS, port, 0 );

        /* Report status of each serial port and test whether there is a
         * modem for each. If data-set-ready and clear-to-send, modem exists.
         */
        printf( "COM%c status: %.4X\tModem: %s\n",
                (char)port + '1', status,
                (status & 0x0030) ? "YES" : "NO" );
    }
}
