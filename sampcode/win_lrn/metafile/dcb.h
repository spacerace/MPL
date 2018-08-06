typedef struct tagDCB {
    BYTE Id;              /* Internal Device ID              */
    WORD BaudRate;        /* Baudrate at which runing        */
    BYTE ByteSize;        /* Number of bits/byte, 4-8        */
    BYTE Parity;          /* 0-4=None,Odd,Even,Mark,Space    */
    BYTE StopBits;        /* 0,1,2 = 1, 1.5, 2               */
    WORD RlsTimeout;      /* Timeout for RLSD to be set      */
    WORD CtsTimeout;      /* Timeout for CTS to be set       */
    WORD DsrTimeout;      /* Timeout for DSR to be set       */

    BYTE fBinary: 1;      /* Binary Mode (skip EOF check     */
    BYTE fRtsDisable:1;   /* Don't assert RTS at init time   */
    BYTE fParity: 1;      /* Enable parity checking          */
    BYTE fOutxCtsFlow:1;  /* CTS handshaking on output       */
    BYTE fOutxDsrFlow:1;  /* DSR handshaking on output       */
    BYTE fDummy: 2;       /* Reserved                        */
    BYTE fDtrDisable:1;   /* Don't assert DTR at init time   */

    BYTE fOutX: 1;        /* Enable output X-ON/X-OFF        */
    BYTE fInX: 1;         /* Enable input X-ON/X-OFF         */
    BYTE fPeChar: 1;      /* Enable Parity Err Replacement   */
    BYTE fNull: 1;        /* Enable Null stripping           */
    BYTE fChEvt: 1;       /* Enable Rx character event.      */
    BYTE fDtrflow: 1;     /* DTR handshake on input          */
    BYTE fRtsflow: 1;     /* RTS handshake on input          */
    BYTE fDummy2: 1;

    char XonChar;         /* Tx and Rx X-ON character        */
    char XoffChar;        /* Tx and Rx X-OFF character       */
    WORD XonLim;          /* Transmit X-ON threshold         */
    WORD XoffLim;         /* Transmit X-OFF threshold        */
    char PeChar;          /* Parity error replacement char   */
    char EofChar;         /* End of Input character          */
    char EvtChar;         /* Recieved Event character        */
    WORD TxDelay;         /* Amount of time between chars    */
} DCB;
