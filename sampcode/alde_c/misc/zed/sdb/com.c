/* SDB - expression compiler
    syntax:
        <expr>          ::= <lor> <EOS>
        <lor>           ::= <land> { '|' <land> }
        <land>          ::= <relat> { '&' <relat> }
        <relat>         ::= <primary> { <relop> <primary> }
        <primary>       ::= <factor> | <unop> <unary>
        <factor>        ::= <number> | <string> | '(' <query> ')'
        <number>        ::= <digit> | <number> <digit>
        <string>        ::= '"' <chars> '"'
        <chars>         ::= nil | <chars> <character>
        <relop>         ::= '=' | '<>' | '<' | '>' | '<=' | '>='
        <unop>          ::= '+' | '-' | '~'
*/

#include "sdbio.h"

extern int dbv_token;
extern char dbv_tstring[];
extern int dbv_tvalue;

extern int db_xand();
extern int db_xor();
extern int db_xnot();
extern int db_xlss();
extern int db_xleq();
extern int db_xeql();
extern int db_xgeq();
extern int db_xgtr();
extern int db_xneq();
extern int db_xpush();
extern int db_xstop();

static union codecell code[CODEMAX+1];
static int cndx;
static struct sel *selptr;

/* compile - compile a boolean expression */
int db_compile(slptr)
  struct sel *slptr;
{
    int result,i;
    union codecell *cptr;
#ifdef Lattice
    int (*dns)();  /*dns*/
#endif

    /* save the selection structure pointer */
    selptr = slptr;

    /* initialize the code array index */
    cndx = 0;

    /* parse the boolean expression */
    if (!expr(&result)) {
        code[cndx++].c_operator = db_xstop;
        freelit(code);
        return (FALSE);
    }

    /* terminate the code */
    code[cndx++].c_operator = db_xstop;

    /* allocate space for the code array */
    if ((cptr = malloc(sizeof(union codecell) * cndx)) == NULL) {
        freelit(code);
        return (FALSE);
    }

    /* store the code into the code array */
    slptr->sl_where = cptr;
    for (i = 0; i < cndx; i++) {
        (*cptr++).c_operator = code[i].c_operator;
#ifndef Lattice
        if (code[i].c_operator == db_xpush)
#else
        if ( code[i].c_operator == (dns=db_xpush) )  /*dns*/
#endif
            (*cptr++).c_operand = code[++i].c_operand;
    }

    /* return successfully */
    return (TRUE);
}

/* db_fcode - free a code array */
db_fcode(slptr)
  struct sel *slptr;
{
    /* make sure there is a where clause */
    if (slptr->sl_where == NULL)
        return;

    /* free the literals */
    freelit(slptr->sl_where);

    /* free the code array */
    free(slptr->sl_where);
}

/* operator - insert an operator into the code array */
static int operator(opr)
  int (*opr)();
{
    /* insert the operator */
    if (cndx < CODEMAX)
        code[cndx++].c_operator = opr;
    else
        return (db_ferror(CDSIZE));

    /* return successfully */
    return (TRUE);
}

/* operand - insert an operand into the code array */
static int operand(opr)
  struct operand *opr;
{
    /* insert the push operator */
    if (!operator(db_xpush))
        return (FALSE);

    /* insert the operand */
    if (cndx < CODEMAX)
        code[cndx++].c_operand = opr;
    else
        return (db_ferror(CDSIZE));

    /* return successfully */
    return (TRUE);
}

/* expr - compile an expression */
static int expr(result)
  int *result;
{
    int lval,rval;

    if (!land(&lval))
        return (FALSE);
    while (db_token() == '|') {
        db_ntoken();
        if (!land(&rval))
            return (FALSE);
        if (!operator(db_xor))
            return (FALSE);
    }
    *result = lval;
    return (TRUE);
}

static int land(result)
  int *result;
{
    int lval,rval;

    if (!relat(&lval))
        return (FALSE);
    while (db_token() == '&') {
        db_ntoken();
        if (!relat(&rval))
            return (FALSE);
        if (!operator(db_xand))
            return (FALSE);
    }
    *result = lval;
    return (TRUE);
}

static int relat(result)
  int *result;
{
    int lval,rval;
    int tkn;

    if (!primary(&lval))
        return (FALSE);
    while (db_token() <= LSS && dbv_token >= GTR) {
        tkn = db_ntoken();
        if (!primary(&rval))
            return (FALSE);
        switch (tkn) {
        case LSS:
                if (!operator(db_xlss))
                    return (FALSE);
                break;
        case LEQ:
                if (!operator(db_xleq))
                    return (FALSE);
                break;
        case EQL:
                if (!operator(db_xeql))
                    return (FALSE);
                break;
        case NEQ:
                if (!operator(db_xneq))
                    return (FALSE);
                break;
        case GEQ:
                if (!operator(db_xgeq))
                   return (FALSE);
                break;
        case GTR:
                if (!operator(db_xgtr))
                    return (FALSE);
                break;
        }
    }
    *result = lval;
    return (TRUE);
}

static int primary(result)
  int *result;
{
    int val;
    int tkn;

    if (db_token() == '~') {
        tkn = db_ntoken();
        if (!primary(&val))
            return (FALSE);
        switch (tkn) {
        case '~':
                if (!operator(db_xnot))
                    return (FALSE);
                break;
        }
    }
    else
        if (!factor(&val))
            return (FALSE);
    *result = val;
    return (TRUE);
}

static int factor(result)
  int *result;
{
    int val;

    if (db_token() == '(') {
        db_ntoken();
        if (!expr(&val))
            return (FALSE);
        if (db_token() != ')')
            return (db_ferror(SYNTAX));
        db_ntoken();
    }
    else
        if (!get_operand(&val))
            return (FALSE);
    *result = val;
    return (TRUE);
}

/* get_operand - get an operand (number, string, or attribute) */
static int get_operand(result)
  int *result;
{
    /* determine operand type */
    if (db_ntoken() == NUMBER)
        return (get_number(result));
    else if (dbv_token == ID)
        return (get_attr(result));
    else if (dbv_token == STRING)
        return (get_string(result));
    else
        return (db_ferror(SYNTAX));
}

/* get_attr - get an attribute argument */
static int get_attr(result)
  int *result;
{
    struct operand *opr;
    char rname[RNSIZE+1],aname[ANSIZE+1];
    char *aptr; int atype,alen;

    /* save the attribute name */
    stccpy(aname,dbv_tstring,ANSIZE); aname[ANSIZE] = EOS;

    /* check for a "." indicating a qualified attribute name */
    if (db_token() == '.') {
        db_ntoken();

        /* the previous ID was really a relation name */
        strcpy(rname,aname);

        /* check for the real attribute name */
        if (db_ntoken() != ID)
            return (db_ferror(SYNTAX));

        /* save the attribute name */
        stccpy(aname,dbv_tstring,ANSIZE); aname[ANSIZE] = EOS;

        /* lookup the attribute name */
        if (!db_sattr(selptr,rname,aname,&atype,&aptr,&alen))
            return (FALSE);
    }
    else
        if (!db_sattr(selptr,NULL,aname,&atype,&aptr,&alen))
            return (FALSE);

    /* get a new operand structure */
    if ((opr = malloc(sizeof(struct operand))) == NULL)
        return (db_ferror(INSMEM));

    /* initialize the new operand structure */
    opr->o_type = ATTR;
    opr->o_value.ov_char.ovc_type = atype;
    opr->o_value.ov_char.ovc_string = aptr;
    opr->o_value.ov_char.ovc_length = alen;

    /* insert the operand into the code array */
    if (!operand(opr)) {
        free(opr);
        return (FALSE);
    }

    /* store operand type */
    *result = atype;

    /* return successfully */
    return (TRUE);
}

/* get_number - get a numeric operand */
static int get_number(result)
  int *result;
{
    struct operand *opr;

    /* get a new operand structure */
    if ((opr = malloc(sizeof(struct operand))) == NULL)
        return (db_ferror(INSMEM));

    /* initialize the new operand structure */
    opr->o_type = LITERAL;
    if ((opr->o_value.ov_char.ovc_string =
                malloc(strlen(dbv_tstring)+1)) == NULL) {
        free(opr);
        return (db_ferror(INSMEM));
    }
    opr->o_value.ov_char.ovc_type = TNUM;
    strcpy(opr->o_value.ov_char.ovc_string,dbv_tstring);
    opr->o_value.ov_char.ovc_length = strlen(dbv_tstring);

    /* insert the operand into the code array */
    if (!operand(opr)) {
        free(opr->o_value.ov_char.ovc_string); free(opr);
        return (FALSE);
    }

    /* operand type is number */
    *result = TNUM;

    /* return successfully */
    return (TRUE);
}

/* get_string - get a string operand */
static int get_string(result)
  int *result;
{
    struct operand *opr;

    /* get a new operand structure */
    if ((opr = malloc(sizeof(struct operand))) == NULL)
        return (db_ferror(INSMEM));

    /* initialize the new operand structure */
    opr->o_type = LITERAL;
    if ((opr->o_value.ov_char.ovc_string =
                malloc(strlen(dbv_tstring)+1)) == NULL) {
        free(opr);
        return (db_ferror(INSMEM));
    }
    opr->o_value.ov_char.ovc_type = TCHAR;
    strcpy(opr->o_value.ov_char.ovc_string,dbv_tstring);
    opr->o_value.ov_char.ovc_length = strlen(dbv_tstring);

    /* insert the operand into the code array */
    if (!operand(opr)) {
        free(opr->o_value.ov_char.ovc_string); free(opr);
        return (FALSE);
    }

    /* operand type is character */
    *result = TCHAR;

    /* return successfully */
    return (TRUE);
}

/* freelit - free the literals in a code array */
static freelit(cptr)
  union codecell *cptr;
{
#ifdef Lattice
    int (*dns)();  /*dns*/
    for (; (*cptr).c_operator != (dns=db_xstop); cptr++)  /*dns*/
        if ((*cptr).c_operator == (dns=db_xpush) )        /*dns*/
#else
    for (; (*cptr).c_operator != db_xstop; cptr++)
        if ((*cptr).c_operator == db_xpush )
#endif
            if ((*++cptr).c_operand->o_type == LITERAL)
                free((*cptr).c_operand->o_value.ov_char.ovc_string);
}

