/* DDE window messages */

#define WM_DDE_FIRST    0x03e0
#define WM_DDE_INITIATE (WM_DDE_FIRST+0)
#define WM_DDE_TERMINATE (WM_DDE_FIRST+1)
#define WM_DDE_ADVISE   (WM_DDE_FIRST+2)
#define WM_DDE_UNADVISE (WM_DDE_FIRST+3)
#define WM_DDE_ACK      (WM_DDE_FIRST+4)
#define WM_DDE_DATA     (WM_DDE_FIRST+5)
#define WM_DDE_REQUEST  (WM_DDE_FIRST+6)
#define WM_DDE_POKE     (WM_DDE_FIRST+7)
#define WM_DDE_EXECUTE  (WM_DDE_FIRST+8)
#define WM_DDE_LAST     (WM_DDE_FIRST+8)
#define WM_DDE_TEST     (WM_DDE_LAST+1)

/*----------------------------------------------------------------------------
|       DDELN structure
|
|       WM_DDE_ADVISE (which is really a 'link') parameter structure
|
|       Fields:
|               fAckReq
|               fDeferUpd
|               cf
----------------------------------------------------------------------------*/
typedef struct {
        unsigned unused:13,
                 fRelease:1,
                 fDeferUpd:1,
                 fAckReq:1;
        int cf;
} DDELN;

/*----------------------------------------------------------------------------
|       DDEUP structure
|
|       WM_DDE_DATA parameter structure.  The actual size of this
|       structure depends on the size of the rgb array.
|
|       Fields:
|               fAckReq         Server wants an Ack once data is processed
|               fRelease        We should release the data
|               fAck            This data serves as an ack to a request
|               cf
|               rgb
----------------------------------------------------------------------------*/
typedef struct {
        unsigned unused2:12,
                 fAck:1,
                 fRelease:1,
                 fReserved:1,
                 fAckReq:1;
        int cf;
        BYTE rgb[1];
} DDEUP;
