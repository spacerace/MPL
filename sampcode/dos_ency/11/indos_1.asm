        test    ss:[CriticalErrorFlag],0FFH        ;(versions 3.1 and later)
        jne     NearLabel
        push    ss:[NearWord]
        int     28H
