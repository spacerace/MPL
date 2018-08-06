/* SDB - string arithmetic routines */

#include "ctype.h"  /*dns*/

/* useful definitions */
#define TRUE            1
#define FALSE           0
#define NUMBERMAX       99
#define EOS             0

/* db_cmp - compare two signed numeric strings */
int db_cmp(arg1,arg2)
  char *arg1,*arg2;
{
    int sign1,sign2;

    /* get the signs of the arguments */
    sign1 = getsign(&arg1);
    sign2 = getsign(&arg2);

    /* compute the result */
    if (sign1 == sign2) {
        if (sign1 == 1)
            return (compare(arg1,arg2));
        else
            return (compare(arg2,arg1));
    }

    /* signs are different */
    return (sign1);
}

/* db_add - add two signed numeric strings */
db_add(result,arg1,arg2)
  char *result,*arg1,*arg2;
{
    int signr,sign1,sign2;
    char rtmp[NUMBERMAX+2],*rptr;

    /* get the signs of the arguments */
    sign1 = getsign(&arg1);
    sign2 = getsign(&arg2);

    /* compute the result */
    if (sign1 == sign2) {
        signr = sign1;
        add(rtmp,arg1,arg2);
    }
    else
        switch (compare(arg1,arg2)) {
        case -1:
                signr = sign2;
                sub(rtmp,arg2,arg1);
                break;
        case 0:
                strcpy(result,"0");
                return;
        case 1:
                signr = sign1;
                sub(rtmp,arg1,arg2);
                break;
        }

    /* insert the sign */
    if (signr == -1)
        strcpy(result,"-");
    else
        result[0] = EOS;

    /* eliminate possible leading zeros */
    for (rptr = rtmp; *rptr == '0'; rptr++)
        ;

    /* copy the result */
    if (*rptr != EOS)
        strcat(result,rptr);
    else
        strcat(result,"0");
}

/* db_sub - subtract two signed numeric strings */
db_sub(result,arg1,arg2)
  char *result,*arg1,*arg2;
{
    int signr,sign1,sign2;
    char rtmp[NUMBERMAX+2],*rptr;

    /* get the signs of the arguments */
    sign1 = getsign(&arg1);
    sign2 = getsign(&arg2);

    /* invert the sign of arg2 */
    sign2 = -sign2;

    /* compute the result */
    if (sign1 == sign2) {
        signr = sign1;
        add(rtmp,arg1,arg2);
    }
    else
        switch (compare(arg1,arg2)) {
        case -1:
                signr = sign2;
                sub(rtmp,arg2,arg1);
                break;
        case 0:
                strcpy(result,"0");
                return;
        case 1:
                signr = sign1;
                sub(rtmp,arg1,arg2);
                break;
        }

    /* insert the sign */
    if (signr == -1)
        strcpy(result,"-");
    else
        result[0] = EOS;

    /* eliminate a possible leading zero */
    for (rptr = rtmp; *rptr == '0'; rptr++)
        ;

    /* copy the result */
    if (*rptr != EOS)
        strcat(result,rptr);
    else
        strcat(result,"0");
}

/* add - add two unsigned numeric strings */
static add(result,arg1,arg2)
  char *result,*arg1,arg2;
{
    char *vr,*v1,*v2,*vx;
    int carry,i,nmin,nmax,nd1,nd2;


    /* get number of digits in each argument */
    nd1 = getlen(arg1);
    nd2 = getlen(arg2);

    /* compute the address of the last digit + 1 */
    v1 = arg1 + nd1;
    v2 = arg2 + nd2;

    /* compute minimum and maximum of nd1 and nd2 */
    if (nd1 < nd2) {
        nmin = -nd1;
        nmax = -nd2;
        vx = v2;
    }
    else {
        nmin = -nd2;
        nmax = -nd1;
        vx = v1;
    }

    /* compute the address of the last result digit + 1 */
    vr = result - nmax; vr[0] = EOS;

    /* initialize the carry */
    carry = 0;

    /* add the digits */
    for (i = -1; i >= nmin; i--) {

        /* check for decimal point */
        if (v1[i] == '.')
            vr[i] = '.';
        else {

            /* add the current pair of digits with the carry */
            vr[i] = v1[i] + v2[i] + carry - '0';

            /* check for carry */
            if (vr[i] > '9') {
                vr[i] -= 10;
                carry = 1;
            }
            else
                carry = 0;
        }
    }

    /* add the remaining digits */
    for (; i >= nmax; i--) {

        /* add the current digit with the carry */
        vr[i] = vx[i] + carry;

        /* check for carry */
        if (vr[i] > '9') {
            vr[i] -= 10;
            carry = 1;
        }
        else
            carry = 0;
    }

    /* check for a carry out and shift digits if necessary */
    if (carry == 1) {

        /* shift the digits */
        for (i = -nmax; i >= 0; i--)
            result[i+1] = result[i];
        result[0] = '1';
    }
}

/* sub - subtract two unsigned numeric strings */
static sub(result,arg1,arg2)
  char *result,*arg1,arg2;
{
    char *vr,*v1,*v2,*vx;
    int borrow,i,nmin,nmax,nd1,nd2;

    /* get number of digits in each argument */
    nd1 = getlen(arg1);
    nd2 = getlen(arg2);

    /* compute the address of the last digit + 1 */
    v1 = arg1 + nd1;
    v2 = arg2 + nd2;

    /* compute minimum and maximum of nd1 and nd2 */
    if (nd1 < nd2) {
        nmin = -nd1;
        nmax = -nd2;
        vx = v2;
    }
    else {
        nmin = -nd2;
        nmax = -nd1;
        vx = v1;
    }

    /* compute the address of the last result digit + 1 */
    vr = result - nmax; vr[0] = EOS;

    /* initialize the borrow */
    borrow = 0;

    /* subtract the digits */
    for (i = -1; i >= nmin; i--) {

        /* check for decimal point */
        if (v1[i] == '.')
            vr[i] = '.';
        else {

            /* subtract the current pair of digits with the borrow */
            vr[i] = v1[i] - v2[i] - borrow + '0';

            /* check for borrow */
            if (vr[i] < '0') {
                vr[i] += 10;
                borrow = 1;
            }
            else
                borrow = 0;
        }
    }

    /* subtract the remaining digits */
    for (; i >= nmax; i--) {

        /* subtract the current digit with the borrow */
        vr[i] = vx[i] - borrow;

        /* check for borrow */
        if (vr[i] < '0') {
            vr[i] += 10;
            borrow = 1;
        }
        else
            borrow = 0;
    }
}

/* getsign - get the sign of a numeric string */
static int getsign(parg)
  char **parg;
{
    int sign;
    char *p;

    /* eliminate leading spaces */
    while (isspace(**parg))
        *parg += 1;

    /* get the sign */
    switch (**parg) {
    case '+':   *parg += 1;
                sign = 1;
                break;
    case '-':   *parg += 1;
                sign = -1;
                break;
    default:    sign = 1;
                break;
    }

    /* eliminate leading spaces and zeros */
    while (isspace(**parg) || **parg == '0')
        *parg += 1;

    /* if the value is zero, make sure that the sign is positive */
    switch (**parg) {
    case EOS:   sign = 1;
                break;
    case '.':   for (p = *parg + 1; *p == '0'; p++)
                    ;
                if (*p == EOS)
                    sign = 1;
                break;
    }

    /* return the sign */
    return (sign);
}

/* getlen - get the length of a numeric string */
static int getlen(arg)
  char *arg;
{
    int len;

    /* get the length of the string */
    len = strlen(arg);

    /* eliminate trailing spaces */
    while (isspace(arg[len-1]))
        len -= 1;

    /* return the length */
    return (len);
}

/* compare - compare two unsigned numeric strings */
static int compare(arg1,arg2)
  char *arg1,*arg2;
{
    int len1,len2;

    /* compare lengths */
    if ((len1 = getlen(arg1)) != (len2 = getlen(arg2)))
        if (len1 < len2)
            return (-1);
        else
            return (1);

    /* compare strings */
    return (strcmp(arg1,arg2));
}

