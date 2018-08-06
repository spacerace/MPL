

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
|               fAckReq
|               fRelease
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


/*----------------------------------------------------------------------------
|       PL structure
|
|       The PL (pronounced "plex") structure is used to efficiently
|       manipulate variable sized arrays.
|
|       Fields:
|               iMax            number of allocated items
|               cbItem          sizeof item
|               dAlloc          number of items to allocate at a time
|               rg              the array of items
----------------------------------------------------------------------------*/
typedef struct
                {
                int iMax;
                BYTE cbItem;
                BYTE dAlloc;
                BYTE rg[1];
                }
        PL;

/*----------------------------------------------------------------------------
|       DEFPL macro
|
|       Used to define a specific plex.
|
|       Arguments:
|               PLTYP           name of the plex type
|               TYP             type of item stored in the plex
|               iMax            name to use for the iMax field
|               rg              name to use for the rg field
----------------------------------------------------------------------------*/
#define DEFPL(PLTYP,TYP,iMax,rg) \
        typedef struct \
                { \
                int iMax; \
                char cbItem; \
                char dAlloc; \
                TYP rg[1]; \
                } \
            PLTYP;


/*----------------------------------------------------------------------------
|       CL structure
|
|       Defines the channel communication structure.
|
|       Fields:
|               fUsed           required for use in a PL
|               hwndExt         external side of the channel
|               hwndInt         internal side of the channel
|               fClient         if ddeapp is client side of the channel
----------------------------------------------------------------------------*/
typedef struct {
        BOOL fUsed;
        HWND hwndExt;
        HWND hwndInt;
        BOOL fClient;
} CL;

/*----------------------------------------------------------------------------
|       PLCL
|
|       Plex array of channels
----------------------------------------------------------------------------*/
DEFPL(PLCL, CL, iclMax, rgcl)


/*----------------------------------------------------------------------------
|       DL structure
|
|       Dependent link structure
|
|       Arguments:
|               fUsed           required for use in a PL
|               icl             channel for this link
|               atomRef         string used in the link
----------------------------------------------------------------------------*/
typedef struct {
        BOOL fUsed;
        int icl;
        ATOM atomRef;
} DL;

/*----------------------------------------------------------------------------
|       PLDL
|
|       Plex array of dependent links
----------------------------------------------------------------------------*/
DEFPL(PLDL, DL, idlMax, rgdl)



/* Menu and Dialog box definitions */

#define mbDde 1
#define idmInitiate 256
#define idmTerminate 257
#define idmRequest 258
#define idmPoke 259
#define idmAdvise 260
#define idmUnadvise 261
#define idmExecute 262
#define idmBench 263
#define idmCBench 264
#define idmPasteLink 265
#define idmBug1 512
#define idmBug2 513
#define idmBug3 514
#define idmBug4 515
#define idmBug5 516

#define iddInitiate 1024
#define idtxtApp 256
#define idtxtDoc 257

#define iddRequest 1025
#define idlboxChnl 256
#define idlboxFmt 257
#define idtxtRef 258

#define iddAdvise 1026
#define idlboxChnl 256
#define idlboxFmt 257
#define idtxtRef 258

#define iddUnadvise 1027
#define idlboxChnl 256
#define idlboxRef 257

#define iddExecute 1028
#define idlboxChnl 256
#define idtxtRef 258

#define iddTerminate 1029
#define idlboxChnl 256

#define iddPoke 1030
#define idlboxChnl 256
#define idtxtRef 258
#define idtxtData 259

#define idradioText 260
#define idradioCsv  261
#define idradioFile 262




