/*
 *    Program name:  strmath.c
 *    
 *    Program author:  Robert E. Sawyer
 *                     Torrance, California
 *
 *    Released for personal, non-commercial use only, 1 August 1984.
 *
 *    Specifications to which the main functions were written:
 *        They must do decimal arithmetic on numbers stored in strings.
 *        The numbers must be in the form:
 *           
 *        Snnnnnnn.nnnn
 *         
 *        where "S" is an optional sign (+ or -), "n" is any
 *        decimal digit (0 through 9) and "." is an optional
 *        decimal point.  The four routines should be called
 *        "decadd", "decsub", "decmul", and "decdiv", and they
 *        each must be called as follows:
 *        
 *                char n1[21],n2[21],result[21];
 *                strcpy (n1,"123.45");
 *                strcpy (n2,"-3.0)";
 *        
 *                if (!decadd(result,n1,n2))
 *                    printf ("Error adding %s to %s\n", n1, n2);
 *                else
 *                    printf ("Result is %s\n", result);
 *        
 *        In this example, the program should print
 *        
 *                Result is 120.45
 *        
 *        "decsub", "decmul", and "decdiv" should work the same
 *        way.
 *        
 *        These routines should return a 1 (true) if the arguments
 *        are valid and the result can be calculated, and a 0
 *        (false) if anything is wrong.  In the case of an error,
 *        the "result" argument should contain a null value.
 *        
 *        The strings may have from 1 to 18 significant digits.
 *        Since there can be a leading sign and a decimal point,
 *        the strings must be able to hold at least 20 characters.
 *        They are dimensioned with 21 above to handle the 20
 *        character limit plus the trailing zero needed in C
 *        strings.
 *        
 *        If the result will be too long (i.e. more than 18
 *        significant digits) treat that case as an error
 *        (i.e., return a 0 and set "result" to be a null).
 *        The same holds true for an attempted division by zero.  
 *        
 *        Math must be EXACT (!!) -- that means no rounding!
 *        These routines must run on an IBM PC without an
 *        8087 chip, and they must be written using only
 *        C functions (i.e., no "C Tools", "C Food", or
 *        other enhancement packages).
 *
 *    Sources:  The lowest level string-math functions are modelled
 *        on corresponding algorithms in D.E.Knuth's, 'The Art of
 *        Computer Programming', Vol.2, and on C.E.Burton's algorithms
 *        in DDJ, #89, 'RSA: A Public Key Cryptosystem, Part 1'.
 */

#define N    18     /*chosen maximum number of significant digits */
#define MIN(x, y)  ((x) > (y) ? (y) : (x))
#define MAX(x, y)  ((x) > (y) ? (x) : (y))

/*-----------------driver to test the functions-------------------*/
main()                
    {
    char n1[N+3], n2[N+3], nn1[N+3], nn2[N+3], res1[N+3], res2[N+3];
    int l1, l2, lr1, lr2, ok;
    char msg[N+3];

    printf("Arithmetic to %.0d significant digits\n\n", N);
    while (1)
        {
        printf("Enter 1st number:  ");
        scanf("%s", nn1);

         printf("Enter 2nd number:  ");
        scanf("%s", nn2);

        l1 = strlen(nn1);
        l2 = strlen(nn2);

        strcpy(n1, nn1);
        strcpy(n2, nn2);
        
        ok = decadd(res1, n1, n2);
        if (!ok)
            strcpy(msg, "*ERROR*");
        else
            msg[0] = '\0';
        printf("\n ADD -> %s  %s\n", res1, msg);

        ok = decsub(res1, n1, n2);
        if (!ok)
            strcpy(msg, "*ERROR*");
        else
            msg[0] = '\0';
        printf("\n SUB -> %s  %s\n", res1, msg);

        ok = decmul(res1, n1, n2);
        if (!ok)
            strcpy(msg, "*ERROR*");
        else
            msg[0] = '\0';
        printf("\n MUL -> %s  %s\n", res1, msg);

        ok = decdiv(res1, n1, n2);
        if (!ok)
            strcpy(msg, "*ERROR*");
        else
            msg[0] = '\0';
        printf("\n DIV -> %s  %s\n", res1, msg);

        printf("-------------\n");
        }
    }

/*---------------------main functions--------------------------*/

decadd(sum, n1, n2)
    char sum[], n1[], n2[];
    {
    char num[N+3], num1[N+3], num2[N+3];
    int sign1, digl1, digr1, sign2, digl2, digr2;
    int sign, digr;
    int len, len1, len2;
    int del;

    if (parse(n1, num1, &sign1, &digl1, &digr1) &&
        parse(n2, num2, &sign2, &digl2, &digr2))
        {
        len1 = strlen(num1);
        len2 = strlen(num2);
        len  = MAX(digl1, digl2) + MAX(digr1, digr2);
        if (len > N)    /* allow for sign, decimal point, and '\0' */
            {
            sum[0] = '\0';
            return (0);
            }
        if (digr1 < digr2)
            {
            del = digr2 - digr1;
            pad(num1, del);
            len1 += del;
            digr = digr2;
            } 
        else
            {
            del = digr1 - digr2;
            pad(num2, del);
            len2 += del;
            digr = digr1;
            }
        if (!add(num1, len1, sign1, num2, len2, sign2, num, &sign))
            {
            sum[0] = '\0';
                return (0);
            }
        if ((len = strlen(num)) > N)
            {
            sum[0] = '\0';
            return (0);
            }
        fix(num, len, sign, digr);
        strcpy(sum, num);
        return (1);
        }
    else
        {
        sum[0] = '\0';
        return (0);
        }
    }

decsub(dif, n1, n2)
    char dif[], n1[], n2[];
    {
    char num[N+3], num1[N+3], num2[N+3];
    int sign1, digl1, digr1, sign2, digl2, digr2;
    int sign, digr;
    int len, len1, len2;
    int del;

    if (parse(n1, num1, &sign1, &digl1, &digr1) &&
        parse(n2, num2, &sign2, &digl2, &digr2))
        {
        len1 = strlen(num1);
        len2 = strlen(num2);
        len  = MAX(digl1, digl2) + MAX(digr1, digr2);
        if (len > N)    /* allow for sign, decimal point, and '\0' */
            {
            dif[0] = '\0';
            return (0);
            }
        if (digr1 < digr2)
            {
            del = digr2 - digr1;
            pad(num1, del);
            len1 += del;
            digr = digr2;
            } 
        else
            {
            del = digr1 - digr2;
            pad(num2, del);
            len2 += del;
            digr = digr1;
            }
        if (!sub(num1, len1, sign1, num2, len2, sign2, num, &sign))
            {
            dif[0] = '\0';
            return (0);
            }
        if ((len = strlen(num)) > N)
            {
            dif[0] = '\0';
            return (0);
            }
        fix(num, len, sign, digr);
        strcpy(dif, num);
        return (1);
        }
    else
        {
        dif[0] = '\0';
        return (0);
        }
    }

decmul(prod, n1, n2)
    char prod[], n1[], n2[];
    {
    char num[N+3], num1[N+3], num2[N+3];
    int sign1, digl1, digr1, sign2, digl2, digr2;
    int sign, digr;
    int len, len1, len2;

    if (parse(n1, num1, &sign1, &digl1, &digr1) &&
        parse(n2, num2, &sign2, &digl2, &digr2))
        {
        len1 = strlen(num1);
        len2 = strlen(num2);
        if (len1+len2-1 > N)    /* allow for sign, decimal point, and '\0' */
            {
            prod[0] = '\0';
            return (0);
            }
        digr = digr1 + digr2;
        if (!mul(num1, len1, sign1, num2, len2, sign2, num, &sign))
            {
            prod[0] = '\0';
            return (0);
            }
        if ((len = strlen(num)) > N)
            {
            prod[0] = '\0';
            return (0);
            }
        fix(num, len, sign, digr);
        strcpy(prod, num);
        return (1);
        }
    else
        {
        prod[0] = '\0';
        return (0);
        }
    }

decdiv(quot, n1, n2)          
    char quot[], n1[], n2[];
    {
    char num[N+N+1], num1[N+N+1], num2[N+3];
    int sign1, digl1, digr1, sign2, digl2, digr2;
    int sign, digr, digl, inum;
    int len, len1, len2;

    if (parse(n1, num1, &sign1, &digl1, &digr1) &&
        parse(n2, num2, &sign2, &digl2, &digr2))
        {
        len1 = strlen(num1);
        len2 = strlen(num2);
        pad(num1, N+N-len1);
        if (!div(num1, N+N, sign1, num2, len2, sign2, num, &sign))
            {
            quot[0] = '\0';
            return (0);
            }
        if ((len=strlen(num))==N+N-len2)
            digl = len1 - len2;
        else
            digl = len1 - len2 + 1;
        for (inum=len-1; (num[inum]=='0') && (inum>0); --inum)
            ;
        num[++inum] = '\0';
        len = inum;
        digr = len - digl + digr1 - digr2;
        if ((len > N) || (digr > N) || (digl > N))
            {
            quot[0] = '\0';
            return (0);
            }
        fix(num, len, sign, digr);
        strcpy(quot, num);
        return (1);
        }
    else
        {
        quot[0] = '\0';
        return (0);
        }
    }

/*-----------intermediate level string-math functions----------*/

/*    add()
 *    performs the appropriate string math
 */
add(num1, len1, sign1, num2, len2, sign2, num, sign)
    char num1[], num2[], num[];
    int sign1, sign2, *sign;
    int len1, len2;
    {
    if ((sign1>0) && (sign2>0))
        {
        *sign = 1;
        if (!sadd(num1, len1, num2, len2, num))
            return (0);
        }
    else if ((sign1<0) && (sign2<0))
        {
        *sign = -1;
        if (!sadd(num1, len1, num2, len2, num))
            return (0);
        }
    else if ((sign1<0) && (sign2>0))
        {
        if ((len2>len1) || ((len2==len1) && (strcmp(num2,num1) > 0)))
            {
            *sign = 1;
            if (!ssub(num2, len2, num1, len1, num))
                return (0);
            }
        else
            {
            *sign = -1;
            if (!ssub(num1, len1, num2, len2, num))
                return (0);
            }
        }
    else 
        {
        if ((len2>len1) || ((len2==len1) && (strcmp(num2,num1) > 0)))
            {
            *sign = -1;
            if (!ssub(num2, len2, num1, len1, num))
                return (0);
            }
        else
            {
            *sign = 1;
            if (!ssub(num1, len1, num2, len2, num))
                return (0);
            }
        }
    return (1);
    }

/*    sub()
 *    performs the appropriate string math
 */
sub(num1, len1, sign1, num2, len2, sign2, num, sign)
    char num1[], num2[], num[];
    int sign1, sign2, *sign;
    int len1, len2;
    {
    if ((sign1>0) && (sign2>0))
        {
        if ((len2>len1) || ((len2==len1) && (strcmp(num2,num1) > 0)))
            {
            *sign = -1;
            if (!ssub(num2, len2, num1, len1, num))
                return (0);
            }
        else
            {
            *sign = 1;
            if (!ssub(num1, len1, num2, len2, num))
                return (0);
            }
        }
    else if ((sign1<0) && (sign2<0))
        {
        if ((len2>len1) || ((len2==len1) && (strcmp(num2,num1) > 0)))
            {
            *sign = 1;
             if (!ssub(num2, len2, num1, len1, num))
                return (0);
            }
        else
            {
            *sign = -1;
            if (!ssub(num1, len1, num2, len2, num))
                return (0);
            }
        }
    else if ((sign1<0) && (sign2>0))
        {
        *sign = -1;
        if (!sadd(num2, len2, num1, len1, num))
            return (0);
        }
    else 
        {
        *sign = 1;
        if (!sadd(num1, len1, num2, len2, num))
            return (0);
        }
    return (1);
    }

/*    mul()
 *    performs the appropriate string math
 */
mul(num1, len1, sign1, num2, len2, sign2, num, sign)
    char num1[], num2[], num[];
    int sign1, sign2, *sign;
    int len1, len2;
    {
    if (sign1*sign2 > 0)
        {
        *sign = 1;
        if (!smul(num1, len1, num2, len2, num))
            return (0);
        }
    else 
        {
        *sign = -1;
        if (!smul(num1, len1, num2, len2, num))
            return (0);
        }
    return (1);
    }

/*    div()
 *    performs the appropriate string math
 */
div(num1, len1, sign1, num2, len2, sign2, num, sign)
    char num1[], num2[], num[];
    int sign1, sign2, *sign;
    int len1, len2;
    {
    char rem[N+3];

    if (sign1*sign2 > 0)
        {
        *sign = 1;
        if (!sdiv(num1, len1, num2, len2, num, rem))
            return (0);
        if (rem[0] > '0')
            return (0);
        }
    else 
        {
        *sign = -1;
        if (!sdiv(num1, len1, num2, len2, num, rem))
            return (0);
        if (rem[0] > '0')
            return (0);
        }
    return (1);
    }

/*-----------lowest level string-math functions------------*/

/*  sadd(), ssub(), smul(), sdiv() perform the lowest level
 *  manipulations on strings interpreted as nonnegative integers.
 *    leading '0's are removed from num1, num2, and from the result
 *  substrings, to which a terminal null is appended;
 *    each function returns 0 on error.
 */

/* sadd() yields sum[], 
 * lens <= max(len1, len2) + 1  
 */
sadd(num1, len1, num2, len2, sum)
    char num1[], num2[], sum[];
    int len1, len2;
    {
    int lens, add, carry, index1, index2, indexs, temp1, temp2;

    lens = MAX(len1, len2) + 1;
    indexs = lens - 1;
    index1 = len1 - 1;
    index2 = len2 - 1;
    carry = 0;
    while ( MIN(index1, index2) >= 0)
        {
        temp1 = num1[index1] - '0';
        temp2 = num2[index2] - '0';
        add = carry + temp1 + temp2;
        sum[indexs] = add % 10 + '0';
        carry = add / 10;
        --index1;
        --index2;
        --indexs;
        }
    if (index2 < 0)
        {
        while (index1 >= 0)
            {
            temp1 = num1[index1] - '0';
            add = carry + temp1;
            sum[indexs] = add % 10 + '0';
            carry = add / 10;
            --index1;
            --indexs;
            }
        }
    else
        {
        while (index2 >= 0)
            {
            temp1 = num2[index2] - '0';
            add = carry + temp1;
            sum[indexs] = add % 10 + '0';
            carry = add / 10;
            --index2;
            --indexs;
            }
        }
    sum[indexs] = carry + '0';
    --indexs;
    while (indexs >= 0)
        {
        sum[indexs] = '0';
        --indexs;
        }
    while ((sum[0]=='0') && (lens>1))
        {
        for (indexs=1; indexs<lens; ++indexs)
            sum[indexs-1] = sum[indexs];
        --lens;
        }
    sum[lens] = '\0';
    return lens;
    }

/* ssub() yields dif[], 
 * lend <= len1
 */        
ssub(num1, len1, num2, len2, dif)
    char num1[], num2[], dif[];
    int len1, len2;
    {
    int lend, sub, borrow, index1, index2, indexd, temp1, temp2;

    lend = len1;
    if (len1>=len2)
        {
        indexd = lend - 1;
        index1 = len1 - 1;
        index2 = len2 - 1;
        borrow = 0;

        while (index2 >= 0)
            {
            temp1 = num1[index1] - '0';
            temp2 = num2[index2] - '0';
            sub = borrow + temp1 - temp2;
            if (sub < 0)
                {
                sub += 10;
                borrow = -1;
                }
            else
                borrow = 0;
            dif[indexd] = sub + '0';
            --index1;
            --index2;
            --indexd;
            }
        while (index1 >= 0)
            {
            temp1 = num1[index1] - '0';
            sub = borrow + temp1;
            if (sub < 0)
                {
                sub += 10;        
                borrow = -1;
                }
            else
                borrow = 0;
            dif[indexd] = sub + '0';
            --index1;
            --indexd;
            }
        if (borrow==0)
            while (indexd >= 0)
                {
                dif[indexd] = '0';
                --indexd;
                }
        else
            {
            dif[0] = '\0';
            return 0;
            }
        while ((dif[0]=='0') && (lend>1))
            {
            for (indexd=1; indexd<lend; ++indexd)
                dif[indexd-1] = dif[indexd];
            --lend;
            }
        dif[lend] = '\0';
        }
    else
        {
        dif[0] = '\0';
        return 0;
        }
    return lend;
    }

/* smul() yields prod[], 
 * lenp <= len1 + len2 
 */
smul(num1, len1, num2, len2, prod)
    char num1[], num2[], prod[];
    int len1, len2;
    {
    int lenp, mult, carry, index1, index2, indexp, temp1, temp2, tempp;

    lenp = len1 + len2;
    index2 = len2 - 1;
    for (indexp=0; indexp<lenp; ++indexp)
        prod[indexp] = '0';
    while (index2 >= 0)
        {
        indexp = lenp + index2 - len2;
        if (num2[index2] == '0')
            {
            carry = 0;
            indexp -= len1;
            }
        else
            {
            index1 = len1 - 1;
            carry = 0;
            while (index1 >= 0)
                {
                temp1 = num1[index1] - '0';
                temp2 = num2[index2] - '0';
                tempp = prod[indexp] - '0';
                mult = carry + temp1*temp2 + tempp;
                prod[indexp] = mult % 10 + '0';
                carry = mult / 10;
                --index1;
                --indexp;
                }
            }
        prod[indexp] = carry + '0';
        --index2;
        }
    while ((prod[0]=='0') && (lenp>1))
        {
        for (indexp=1; indexp<lenp; ++indexp)
            prod[indexp-1] = prod[indexp];
        --lenp;
        }
    prod[lenp] = '\0';
    return lenp;        
    }
    
/* sdiv() yields quot[] and rem[],     
 * lenq <= max(len1-len2, 1) 
 * lenr <= len2
 */
sdiv(num1, len1, num2, len2, quot, rem)    
    char num1[], num2[], quot[], rem[];
    int len1, len2;
    {
    int lenq, lenr, div, flag, scale,
        index, index1, index2, indexq, indexr, 
        temp1, temp2, temp3, temp4, temp5, qtest;

    if (num1[0] != '0')
        {
        for (index1=len1; index1>0; --index1)
            num1[index1] = num1[index1-1];
        num1[0] = '0';
        ++len1;
        }
    while ((num2[0]=='0') && (len2>1))
        {
        for (index2=1; index2<len2; ++index2)
            num2[index2-1] = num2[index2];
        --len2;
        }
    lenq = MAX(len1-len2, 1);
    lenr = len2;
    if ((len1<len2) || ((len1==len2) && (num1[0]<num2[0])))
        {
        quot[0] = '0';
        lenq = 1;
        strcpy(rem, num1);
        lenr = len1;
        }
    if(num2[0] != '0')
        {
        for (indexq=0; indexq<lenq; ++indexq)
            quot[indexq] = '0';
        for (indexr=0; indexr<lenr; ++indexr)
            rem[indexr] = '0';
        flag = 0;
        for (index1=1; index1<len1; ++index1)
            if (num1[index1] != '0')
                {
                flag = 1;
                break;
                }
        if (flag != 0)
            {
            temp1 = num2[0] - '0';
            scale = 10 / (temp1 + 1);
            if (scale > 1)
                {
                flag = 0;
                index1 = len1 -1;
                while (index1 >= 0)
                    {
                    temp1 = num1[index1] - '0';
                    div = flag + scale*temp1;
                    num1[index1] = div % 10 + '0';
                    flag = div /10;
                    --index1;
                    }
                flag = 0;
                index2 = len2 -1;
                while (index2 >= 0)
                    {
                    temp1 = num2[index2] - '0';
                    div = flag + scale*temp1;
                    num2[index2] = div % 10 + '0';
                    flag = div / 10;
                    --index2;
                    }
                }
            index1 = 0;
            while (index1 < (len1-len2))
                {
                if (num2[0] == num1[index1])
                    qtest = 9;
                else
                    {
                    temp1 = num1[index1] - '0';
                    temp3 = num2[0] - '0';
                    if ((index1+1) < len1)
                        temp2 = num1[index1+1] - '0';
                    else
                        temp2 = 0;
                    qtest = (10*temp1 + temp2) / temp3;
                    }
                temp2 = num1[index1] - '0';
                temp4 = num2[0] - '0';
                if (len2 >= 2)
                    temp1 = num2[1] - '0';
                else
                    temp1 = 0;
                if ((index1+1) < len1)
                    {
                    temp3 = num1[index1+1] - '0';
                    if ((index1+2) < len1)
                        temp5 = num1[index1+2] - '0';
                    else
                        temp5 = 0;
                    }
                else
                    {
                    temp3 = 0;
                    temp5 = 0;
                    }                
                while (qtest*temp1 > (10*(10*temp2 + temp3 
                                    - qtest*temp4) + temp5))
                    --qtest;
                index2 = len2 - 1;
                index = index1 + len2;
                flag = 0;
                while (index >= index1)
                    {
                    if (index2 >= 0)
                        {
                        temp1 = num2[index2] - '0';
                        flag -= qtest*temp1;
                        }
                    temp1 = num1[index] - '0';
                    div = flag + temp1;
                    if (div < 0)
                        {
                        flag = div / 10;
                        div %= 10;
                        if (div < 0)
                            {
                            div += 10;
                            --flag;
                            }
                        }
                    else
                        flag = 0;
                    num1[index] = div + '0';
                    --index2;
                    --index;
                    }
                indexq = lenq - (len1 - len2) + index1;   
                if (flag != 0)
                    {
                    quot[indexq] = qtest - 1 + '0';
                    index2 = len2 - 1;
                    index = index1 + len2;
                    flag = 0;
                    while (index >= index1)
                        {
                        if (index2 >= 0)
                            {
                            temp1 = num2[index2] - '0';
                            flag += temp1;
                            }
                        temp1 = num1[index] - '0';
                        div = flag + temp1;
                        if (div > 9)
                            {
                            div -= 10;
                            flag = 1;
                            }
                        else
                            flag = 0;
                        num1[index] = div + '0';
                        --index2;
                        --index;
                        }
                    }
                else
                    quot[indexq] = qtest + '0';
                ++index1;
                }
            index1 = len1 - len2;
            indexr = lenr - len2;
            flag = 0;
            while (index1 < len1)
                {
                temp1 = num1[index1] - '0';
                div = temp1 + 10*flag;
                rem[indexr] = div / scale + '0';
                flag = div % scale;
                ++index1;
                ++indexr;
                }
            index2 = 0;
            flag = 0;
            while (index2 < len2)
                {
                temp1 = num2[index2] - '0';
                div = temp1 + 10*flag;
                num2[index2] = div / scale + '0';
                flag = div % scale;
                ++index2;
                }
            }
        while ((quot[0]=='0') && (lenq>1))
            {
            for (indexq=1; indexq<lenq; ++indexq)
                quot[indexq-1] = quot[indexq];
            --lenq;
            }
        while ((rem[0]=='0') && (lenr>1))
            {
            for (indexr=1; indexr<lenr; ++indexr)
                rem[indexr-1] = rem[indexr];
            --lenr;
            }
        quot[lenq] = '\0';
        rem[lenr] = '\0';
        }
    else
        {
        quot[0] = '\0';
        rem[0] = '\0';
        return 0;
        }
    return (lenq+lenr) * (lenq>0) * (lenr>0);
    }

/*    parse() returns 0 on error.
 *    input:    str[] a string of digits with optional decimal point
 *            and leading sign.
 *    output:    num[] is a string containing the significant digits
 *            of str[], and digl/digr is the number of digits to the
 *          left/right of the decimal point.
 */    
parse(str, num, sign, digl, digr)
    char str[], num[];
    int *sign, *digl, *digr;
    {
    char c;
    int dflag, istr, inum, len;

    dflag = 0;
    inum = 0;
    *digl = 0;
    *digr = 0;
    c = str[0];
    if (c=='+')
        *sign = 1;
    else if (c=='-')
        *sign = -1;
    else if (c=='.')
        {
        *sign = 1;
        dflag = 1;
        }
    else if ((c>='0') && (c<='9'))
        {
        *sign = 1;
        if (c != '0')
            {
            *digl = 1;
            num[0] = c;
            ++inum;
            }
        }
    else
        return (0);
    istr = 1;
    while ((c=str[istr++]) != '\0')
        {
        if ((c=='.') && (dflag==0))
            dflag = 1;
        else if ((c>='0') && (c<='9'))
            {
            if ((c!='0') || (*digl>0) || (dflag==1))
                {
                if (dflag==0)
                    ++(*digl);
                else
                    ++(*digr);
                num[inum] = c;
                ++inum;
                }
            }
        else
            return (0);
        }
    --inum;
      while ((num[inum]=='0') && (inum>= *digl))
        {
        --(*digr);
        --inum;
        }
    num[++inum] = '\0';
    len = inum;
    while ((num[0]=='0') && (len>1))
        {
        for (inum=1; inum<len; ++inum)
            num[inum-1] = num[inum];
        --len;
        if (*digl>0)
            --(*digl);
        }
    num[len] = '\0';
    return (1);
    }

/*    fix()
 *    inserts sign and decimal point in digit string num[]
 */
fix(num, len, sign, digr)
    char num[];
    int len, sign, digr;
    {
    int inum;
    char nnum[N+3];

    if (digr >= 0)
        {
        if (len >= digr)
            {
            for (inum=len; inum>len-digr; --inum)
                num[inum] = num[inum-1];
            num[inum] = '.';
            ++len;
            num[len] = '\0';
            }
        else
            {
            nnum[0] = '.';
            for (inum=1; inum<=digr; ++inum)
                {
                if (inum<=digr-len)
                    nnum[inum] = '0';
                else
                    nnum[inum] = num[inum-digr+len-1];
                }
            nnum[inum] = '\0';
            strcpy(num, nnum);
            len = digr+1;
            }
        }
    else
        {
        len -= digr - 1;
        pad(num, -digr);
        num[len-1] = '.';
        num[len] = '\0';

        }
    if (sign==-1)
        {
        for (inum=len; inum>0; --inum)
            num[inum] = num[inum-1];
        num[0] = '-';
        ++len;
        num[len] = '\0';
        }

    }

/*     pad() 
 *  input:    num[] a string .
 *            dig a number of '0's to be appended to num[].
 *    output: num[] with dig '0's appended.
 */
 pad(num, dig)
     char num[];
     int dig;
     {
     int inum, len;

    len = strlen(num);
    for (inum=len; inum < len+dig; ++inum)
        num[inum] = '0';
    num[inum] = '\0';
    }

/*------------------end of file----------------------*/

