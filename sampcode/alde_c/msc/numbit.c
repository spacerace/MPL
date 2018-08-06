#include <stdio.h>
#define  MSOFT

/*
 *  numbit.c
 *  For those times when you need to know the values of a number in another
 *  base.  As we all need Decimal, Octal, Hex and Binary forms from time to
 *  time.
 *  This will accept an entry in the following formats:
 *     Decimal   255
 *     Octal    0377
 *     Hex      0xff
 *
 *  The output will be given in all formats.  Numbers larger than 0xffff
 *  will be handled as 32 bit long integers.  Negative numbers will cause
 *  the upper bit to be set and thus will be erroneous.
 */

#ifndef MSOFT
long    stoi();                                  /* declare conversion */
#else
#include <stdlib.h>                              /* get library declaration */
#endif

main()
{
char    *endptr, buf[80];
union  {
      long value;
      struct  {
        unsigned int lw_bit0:1;
        unsigned int lw_bit1:1;
        unsigned int lw_bit2:1;
        unsigned int lw_bit3:1;
        unsigned int lw_bit4:1;
        unsigned int lw_bit5:1;
        unsigned int lw_bit6:1;
        unsigned int lw_bit7:1;
        unsigned int lw_bit8:1;
        unsigned int lw_bit9:1;
        unsigned int lw_bit10:1;
        unsigned int lw_bit11:1;
        unsigned int lw_bit12:1;
        unsigned int lw_bit13:1;
        unsigned int lw_bit14:1;
        unsigned int lw_bit15:1;

        unsigned int hw_bit0:1;
        unsigned int hw_bit1:1;
        unsigned int hw_bit2:1;
        unsigned int hw_bit3:1;
        unsigned int hw_bit4:1;
        unsigned int hw_bit5:1;
        unsigned int hw_bit6:1;
        unsigned int hw_bit7:1;
        unsigned int hw_bit8:1;
        unsigned int hw_bit9:1;
        unsigned int hw_bit10:1;
        unsigned int hw_bit11:1;
        unsigned int hw_bit12:1;
        unsigned int hw_bit13:1;
        unsigned int hw_bit14:1;
        unsigned int hw_bit15:1;
      } bits;
    } num;

   do {
      printf("enter number to try   ");

      gets(buf);

#ifndef MSOFT
      num.value = stoi(buf);
#else
      num.value = strtol(buf, &endptr, 0);
#endif

      printf("Decimal = %lu  Hex = %lx  Octal = %lo\n",
               num.value,
               num.value,
               num.value   );

      if( num.value > 0xffff )
      {
         printf("%d%d%d%d%d%d%d%d %d%d%d%d%d%d%d%d ",
                num.bits.hw_bit15,
                num.bits.hw_bit14,
                num.bits.hw_bit13,
                num.bits.hw_bit12,
                num.bits.hw_bit11,
                num.bits.hw_bit10,
                num.bits.hw_bit9,
                num.bits.hw_bit8,
                num.bits.hw_bit7,
                num.bits.hw_bit6,
                num.bits.hw_bit5,
                num.bits.hw_bit4,
                num.bits.hw_bit3,
                num.bits.hw_bit2,
                num.bits.hw_bit1,
                num.bits.hw_bit0);
      }

      printf("%d%d%d%d%d%d%d%d %d%d%d%d%d%d%d%d\n",
             num.bits.lw_bit15,
             num.bits.lw_bit14,
             num.bits.lw_bit13,
             num.bits.lw_bit12,
             num.bits.lw_bit11,
             num.bits.lw_bit10,
             num.bits.lw_bit9,
             num.bits.lw_bit8,
             num.bits.lw_bit7,
             num.bits.lw_bit6,
             num.bits.lw_bit5,
             num.bits.lw_bit4,
             num.bits.lw_bit3,
             num.bits.lw_bit2,
             num.bits.lw_bit1,
             num.bits.lw_bit0);
   } while (num.value != 0);

}
/*=============================================================*/
#ifndef MSOFT                                    /* cant use the library */
                                                 /* function strtol      */
#define  islower(c)     ( 'a' <= (c) && (c) <= 'z' )
#define  toupper(c)     ( islower(c) ? (c) - ('a' - 'A') : (c) )

/* stoi.c       more powerful version of atoi.
 *
 *      copyright 1985 by Allen Holub.
 *      modified 5/86 by Jerry Pendergraft
 *          to return a long value, and work on a char pointer rather
 *          than a pointer to char pointer.
 */

long    stoi(str)
char    *str;
{
long    num = 0;
int     sign = 1;

        while(*str == ' ' || *str == '\t' || *str == '\n' )
                str++;

        if( *str == '-' )
           {
           sign = -1;
           str++;
           }

        if( *str == '0' )                        /* Octal or Hex form        */
           {
                ++str;
                if( *str == 'x' || *str == 'X' ) /* Hex form                 */
                   {
                        str++;
                        while( ('0' <= *str && *str <= '9') ||
                               ('a' <= *str && *str <= 'f') ||
                               ('A' <= *str && *str <= 'F')  )
                           {
                           num *= 16;
                           num += ( '0' <= *str && *str <= '9' ) ?
                                    *str - '0'                   :
                                    toupper( *str ) - 'A' + 10   ;
                           str++;
                           }
                   }
                else
                   {
                        while( '0' <= *str && *str <= '7' )
                           {
                           num *= 8;
                           num += *str++ - '0';
                           }
                   }
           }
        else                                     /* Decimal entry */
           {
                while( '0' <= *str && *str <= '9' )
                   {
                   num *= 10;
                   num += *str++ - '0';
                   }
           }

return( num * sign );
}
#endif
