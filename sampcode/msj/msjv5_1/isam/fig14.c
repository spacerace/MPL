

/* An abbreviated list of opcode constants. */
#define OPADDINDEX       /* add an index */
#define OPBGNTRAN        /* begin a transaction */
#define OPBUILD          /* create an ISAM file */
#define OPCLOSE          /* close an ISAM file */
#define OPCOMMIT         /* commit a transaction */
#define OPDELETE         /* delete a record */
#define OPOPEN           /* open an ISAM file */
#define OPREAD           /* read a record */
#define OPREWRITE        /* update a record */
#define OPWRITE          /* write a record */

struct isrequest {
    int          iOpcode;
    int          iIsfd;
    int          iMode;
    int          iKeynum;
    int          iReclen;
    long         lRecnum;
    long         lUniqid;
    GLOBALHANDLE hBuffer;
    GLOBALHANDLE hFileName1;
    GLOBALHANDLE hFileName2;
    GLOBALHANDLE hRecord;
    char         isstat1;
    char         isstat2;
    int          iserrno;
    int          iserrio;
    int          isretval;
};
