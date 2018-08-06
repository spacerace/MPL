#define   BIFFDLG   2

#define   IDROW     101
#define   IDCOLUMN  102
#define   IDTEXT    103

#define   IDBLANK   201
#define   IDLABEL   202
#define   IDBOOL    203
#define   IDINT     204
#define   IDNUMBER  205
#define   IDFORMULA 206

struct  BIFFBOF {
        WORD    wRecType;
        WORD    wRecLength;
        WORD    wVersion;
      } ;

struct  BIFFEOF {
        WORD    wRecType;
        WORD    wRecLength;
      } ;

struct  BIFFINTEGER {
        WORD    wRecType;
        WORD    wRecLength;
        WORD    wRow;
        WORD    wColumn;
        char    cAttrib[3];
        WORD    wInteger;
      } ;

struct  BIFFNUMBER {
        WORD    wRecType;
        WORD    wRecLength;
        WORD    wRow;
        WORD    wColumn;
        char    cAttrib[3];
        double  dNumber;
      } ;

struct  BIFFSTRING {
        WORD    wRecType;
        WORD    wRecLength;
        WORD    wRow;
        WORD    wColumn;
        char    cAttrib[3];
        BYTE    bTextLength;
        char    cLabel[1];
      } ;

struct  BIFFFORMULA {
        WORD    wRecType;
        WORD    wRecLength;
        WORD    wRow;
        WORD    wColumn;
        char    cAttrib[3];
        char    cCurValue[8];
        BYTE    bRecalc;
        BYTE    bTextLength;
        char    cLabel[1];
      } ;

