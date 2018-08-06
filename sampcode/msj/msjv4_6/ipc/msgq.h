/* msgq.h RHS 7/15/89
 *
 * Message Queue prototypes
 */

void MsgQCreate(HQUEUE *qhandle, char *qname);
void MsgQOpen(HQUEUE *qhandle, char *qname);
void MsgQSend(HQUEUE qhandle, USHORT event);
void MsgQClose(HQUEUE qhandle);
void MsgQGet(HQUEUE qhandle, USHORT *event);


