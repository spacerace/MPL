/* pmserver.h RHS 9/1/89
 *
 * common header file for PMSERVER and client programs
 */
#define PMSERVERQUE "\\QUEUES\\PMSERVER.QUE"

                            // messages sent by client
#define PMS_INIT            100                 // client initializing
#define PMS_COPY            101                 // copy data to clipboard
#define PMS_PASTE           103                 // get data from clipboard
#define PMS_TERMINATE       104                 // client is terminating
#define PMS_CLPBRD_QUERY    105                 // is anything in clipboard?

                            // messages sent by server
#define PMS_CLPBRD          120                 // clipboard data available
#define PMS_NO_INIT         121                 // can't initialize client
#define PMS_INIT_ACK        122                 // server acknowledges init
#define PMS_MSG_UNKNOWN     123                 // server can't identify msg
#define PMS_CLPBRD_EMPTY    124                 // clipboard data not avail
#define PMS_CLPBRD_DATA     125                 // here's your clipboard data


typedef struct _clientdata
    {
    PID        pid;
    BYTE    qname[21];
    } CLIENTDATA;

#define CLIENTDATAMSG(ptr)    ((CLIENTDATA *)ptr)

#define MAXAPPNAME      50
#define MAXTOPICNAME    50
