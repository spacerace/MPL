/* msgq.h RHS
 *
 * Message Queue prototypes
 */

void MsgQCreate(HQUEUE *qhandle, char *qname);
USHORT MsgQOpen(HQUEUE *qhandle, char *qname);
void MsgQSend(HQUEUE qhandle, PVOID event, USHORT size, USHORT msg);
void MsgQClose(HQUEUE qhandle);
void MsgQGet(HQUEUE qhandle,PVOID *event,USHORT *size,USHORT *msg);

