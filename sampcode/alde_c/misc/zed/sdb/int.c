/* SDB - boolean expression evaluator */

#include "sdbio.h"

static struct operand *stack[STACKMAX],**sptr;
static union codecell *cptr;

/* db_interpret - interpret a boolean expression */
int db_interpret(slptr)
  struct sel *slptr;
{
    struct operand *result;
    int r;

    /* check for empty where clause */
    if ((cptr = slptr->sl_where) == NULL)
        return (TRUE);

    /* setup stack */
    sptr = stack;

    /* execute the code */
    while ((*(*cptr++).c_operator)())
        ;

    /* get the result from the top of stack */
    result = *--sptr;
    r = result->o_value.ov_boolean;
    if (result->o_type == TEMP)
        free(result);

    /* make sure the stack is empty */
    while (sptr != stack) {
        if ((*sptr)->o_type == TEMP)
            free(*sptr);
        sptr -= 1;
    }

    /* return result */
    return (r);
}

int db_xstop()
{
    return (FALSE);
}

int db_xpush()
{
    *sptr++ = (*cptr++).c_operand;
}

int db_xand()
{
    return (boolean('&'));
}

int db_xor()
{
    return (boolean('|'));
}

static int boolean(opr)
{
    struct operand *lval,*rval,*result;
    int lv,rv,r;

    rval = *--sptr; lval = *--sptr;
    lv = lval->o_value.ov_boolean;
    rv = rval->o_value.ov_boolean;

    if ((result = malloc(sizeof(struct operand))) == NULL)
        return (db_ferror(INSMEM));
    result->o_type = TEMP;
    switch (opr) {
    case '&':   r = (lv && rv);
                break;
    case '|':   r = (lv || rv);
                break;
    }
    result->o_value.ov_boolean = r;
    *sptr++ = result;
    if (lval->o_type == TEMP)
        free(lval);
    if (rval->o_type == TEMP)
        free(rval);
    return (TRUE);
}

int db_xnot()
{
    struct operand *val,*result;

    val = *--sptr;
    if ((result = malloc(sizeof(struct operand))) == NULL)
        return (db_ferror(INSMEM));
    result->o_type = TEMP;
    result->o_value.ov_boolean = !val->o_value.ov_boolean;
    *sptr++ = result;
    if (val->o_type == TEMP)
        free(val);
    return (TRUE);
}

int db_xlss()
{
    return (compare(LSS));
}

int db_xleq()
{
    return (compare(LEQ));
}

int db_xeql()
{
    return (compare(EQL));
}

int db_xgeq()
{
    return (compare(GEQ));
}

int db_xgtr()
{
    return (compare(GTR));
}

int db_xneq()
{
    return (compare(NEQ));
}

static int compare(cmp)
{
    struct operand *lval,*rval,*result;
    int i;

    rval = *--sptr; lval = *--sptr;
    if ((result = malloc(sizeof(struct operand))) == NULL)
        return (db_ferror(INSMEM));
    result->o_type = TEMP;

    if (lval->o_value.ov_char.ovc_type == TCHAR)
        i = comp(lval,rval);
    else
        i = ncomp(lval,rval);

    switch (cmp) {
    case LSS:   i = (i < 0);
                break;
    case LEQ:   i = (i <= 0);
                break;
    case EQL:   i = (i == 0);
                break;
    case GEQ:   i = (i >= 0);
                break;
    case GTR:   i = (i > 0);
                break;
    case NEQ:   i = (i != 0);
                break;
    }
    result->o_value.ov_boolean = i;
    *sptr++ = result;
    if (lval->o_type == TEMP)
        free(lval);
    if (rval->o_type == TEMP)
        free(rval);
    return (TRUE);
}

static int comp(lval,rval)
  struct operand *lval,*rval;
{
    char *lptr,*rptr; int lctr,rctr;
    int len;

    lptr = lval->o_value.ov_char.ovc_string;
    lctr = lval->o_value.ov_char.ovc_length;
    rptr = rval->o_value.ov_char.ovc_string;
    rctr = rval->o_value.ov_char.ovc_length;

    while (lctr > 0 && (lptr[lctr-1] == 0 || lptr[lctr-1] == ' '))
        lctr--;
    while (rctr > 0 && (rptr[rctr-1] == 0 || rptr[rctr-1] == ' '))
        rctr--;

    if (lctr < rctr)
        len = lctr;
    else
        len = rctr;

    while ((len--) > 0) {
        if (*lptr++ != *rptr++)
            if (*--lptr < *--rptr)
                return (-1);
            else
                return (1);
    }

    if (lctr == rctr)
        return (0);
    else if (lctr < rctr)
        return (-1);
    else
        return (1);
}

static int ncomp(lval,rval)
  struct operand *lval,*rval;
{
    char lstr[NUMBERMAX+1],rstr[NUMBERMAX+1];
    int len;

    stccpy(lstr,lval->o_value.ov_char.ovc_string,
          (len = lval->o_value.ov_char.ovc_length)); lstr[len] = EOS;
    stccpy(rstr,rval->o_value.ov_char.ovc_string,
          (len = rval->o_value.ov_char.ovc_length)); rstr[len] = EOS;

    return (db_cmp(lstr,rstr));
}

