        cmp     ss:[CriticalErrorFlag],00          ;(versions earlier than 3.1)
        jne     NearLabel
        int     28H
