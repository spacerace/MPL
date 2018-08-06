// pmsvmsgs.h

#define PMSV_COPY               (WM_USER + 0)
#define PMSV_PASTE              (WM_USER + 1)
#define PMSV_PASTE_MSG          (WM_USER + 2)

#define PMSV_TERMINATE          (WM_USER + 3)
#define PMSV_REQUEST            (WM_USER + 4)
#define PMSV_POKE               (WM_USER + 5)
#define PMSV_ADVISE             (WM_USER + 6)
#define PMSV_ADVISE_NO_DATA     (WM_USER + 7)
#define PMSV_UNADVISE           (WM_USER + 8)
#define PMSV_EXECUTE            (WM_USER + 9)

#define PMSV_THREAD_TERMINATE   (WM_USER + 10)

#define MAX_APPLICATION_LEN     (32+1)
#define MAX_TOPIC_LEN           (32+1)

#ifdef DEBUG_MESSAGES
#define PMSV_MSG                (WM_USER + 20)
#endif

typedef struct _client
    {
    USHORT  isddeclient;
    HQUEUE  qhandle;
    PID     kclient_pid;
    HWND    hwnd;
    HWND    hwndpmserver;
    PVOID   threadstack;
    CHAR    appname[MAX_APPLICATION_LEN];
    CHAR    topicname[MAX_TOPIC_LEN];
    } CLIENT;
typedef CLIENT *PCLIENT;


