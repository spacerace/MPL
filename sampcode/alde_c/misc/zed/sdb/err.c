/* SDB - error messages */

#include "sdbio.h"

char *db_ertxt(msg)
  int msg;
{
    char *txt;

    /* select the appropriate message text */
    switch (msg) {
    case INSMEM:
        txt = "insufficient memory";
        break;
    case RELFNF:
        txt = "relation file not found";
        break;
    case BADHDR:
        txt = "bad relation header";
        break;
    case TUPINP:
        txt = "tuple input error";
        break;
    case TUPOUT:
        txt = "tuple output error";
        break;
    case RELFUL:
        txt = "relation file full";
        break;
    case RELCRE:
        txt = "error creating relation file";
        break;
    case DUPATT:
        txt = "duplicate attribute";
        break;
    case MAXATT:
        txt = "too many attributes";
        break;
    case INSBLK:
        txt = "insufficient disk space";
        break;
    case SYNTAX:
        txt = "syntax error";
        break;
    case ATUNDF:
        txt = "undefined attribute";
        break;
    case ATAMBG:
        txt = "ambiguous attribute";
        break;
    case RLUNDF:
        txt = "undefined relation";
        break;
    case CDSIZE:
        txt = "boolean expression too complex";
        break;
    case INPFNF:
        txt = "input file not found";
        break;
    case OUTCRE:
        txt = "error creating output file";
        break;
    case INDFNF:
        txt = "indirect command file not found";
        break;
    case BADSET:
        txt = "bad set parameter";
        break;
    default:
        txt = "undefined error";
        break;
    }

    /* return the message text */
    return (txt);
}

