/* pmserver.h RHS
 *
 * common header file for PMSERVER and client programs
 */
#define PMSERVERQUE "\\QUEUES\\PMSERVER.QUE"

                            // messages sent by client
#define PMS_INIT            100         // client initializing
#define PMS_TERMINATE       101         // client is terminating

#define PMS_CLPBRD_COPY     102         // copy data to clipboard
#define PMS_CLPBRD_PASTE    103         // get data from clipboard
#define PMS_CLPBRD_QUERY    105         // is anything in clipboard?
#define PMS_DDE_INIT        106         // initiate DDE
#define PMS_DDE_REQUEST     107         // request DDE data
#define PMS_DDE_ADVISE      108         // be advised of DDE data
#define PMS_DDE_TERMINATE   109         // terminate DDE conversation



                            // messages sent by server
#define PMS_NO_INIT         120         // can't initialize client
#define PMS_INIT_ACK        121         // server acknowledges init
#define PMS_MSG_UNKNOWN     122         // server can't identify msg
#define PMS_SERVER_TERM     123         // server is terminating

#define PMS_CLPBRD          124         // clipboard data available
#define PMS_CLPBRD_EMPTY    125         // clipboard data not avail
#define PMS_CLPBRD_DATA     126         // here's your clipboard data
#define PMS_DDE_INITACK     127         // a data server responded
#define PMS_DDE_INITNAK     128         // no conversation available
#define PMS_DDE_DATA        129         // here's your dde data
#define PMS_DDE_NODATA      130         // dde data not available
#define PMS_DDE_DATACHANGE  131         // the data has changed
#define PMS_DDE_ACK         132         // positive acknowledgement
#define PMS_DDE_NAK         133         // negative acknowledgement

typedef struct _clientdata
    {
    PID        pid;
    BYTE    qname[21];
    } CLIENTDATA;

#define CLIENTDATAMSG(ptr)    ((CLIENTDATA *)ptr)

#define MAXAPPNAME      50
#define MAXTOPICNAME    50
#define MAXITEMLEN      200

#define MAX_ITEM_LEN	(80+1)
#define MAX_VALUE_LEN	(180+1)
typedef struct itemreq
    {
	CHAR	item[MAX_ITEM_LEN];
	CHAR	value[1];
    } ITEMREQ;
typedef ITEMREQ	*PITEMREQ;

