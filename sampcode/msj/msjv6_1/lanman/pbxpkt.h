/*-------------------------------------------------------------------
  PBXPKT.H -- PBX Protocol definitions

  Description:

  This file contains all of the constants and structures necessary
  for a client to use the PBX service.  To use PBX, first:

    a) Broadcast a message on the second-class mailslot ANNOUNCELINE
       with the priority of ANNOUNCEPRIORITY.  If a PBX exists on
       the domain, it will respond with a message containing the
       computer name of the computer on which it is executing
       (prepended with two backslashes).

    b) Open the PBX named-pipe.  Construct the full UNC name by
       prepending the target computer name (e.g., \\BRENDAN) to
       the constant PIPELINE (usually, this will be taken from
       the PBX response the broadcast in step a).

    c) Build and send as the first packet on the pipe a REGISTER
       request.  This packet should contain the client name and type
       (type is client dependent but cannot be zero).  If another
       client of the same name and type exists, the request will be
       rejected and a new name should be tried.

  After registering with PBX, you may send either LISTQUERY or
  CONNECT packets to PBX.  You may also asynchronously receive a
  CONNECT packet from PBX if another client requests a connection
  with you.  PBX responds to all packets directed to it with an
  acknowledgement by turning on the high-order bit of the usPktID
  field, setting the usRetCode field, and returning the packet.

  Once two clients are connected, PBX no longer examines the data
  being sent.  Instead, the data is forwarded directly to the target
  client.  A connection can only be broken by one of the clients
  closing their end of the named-pipe (PBX will then close the pipe
  for the other client), after which both clients must re-open the
  pipe and re-register with the PBX if they want to continue using
  the PBX connect to other clients.

  Building PBX Packets:
    ->  = Input to PBX (you must place in the packet)
    <-  = Output from PBX (available in the acknowledgement packet)

  REGISTER:
    PBXPKT.usPktID                  -> REGISTER
    PBXPKT.usPktSize                -> sizeof(PBXPKT)
    PBXPKT.usNameCnt                -> 1
    PBXPKT.aPBXName[0].pszName      -> Client name
    PBXPKT.aPBXName[0].usClientType -> Client program ID (should
                                       identify the program using
                                       PBX; it cannot be zero)

    PBXPKT.usPktID                  <- ACK | REGISTER
    PBXPKT.usPktSize                <- sizeof(PBXPKT)
    PBXPKT.usRetCode                <- Return code

  CONNECT :
    PBXPKT.usPktID                  -> CONNECT
    PBXPKT.usPktSize                -> sizeof(PBXPKT)
    PBXPKT.usNameCnt                -> 1
    PBXPKT.aPBXName[0].pszName      -> Target client name
    PBXPKT.aPBXName[0].usClientType -> Target client program ID

    PBXPKT.usPktID                  <- ACK | CONNECT
    PBXPKT.usPktSize                <- sizeof(PBXPKT)
    PBXPKT.usRetCode                <- Return code

  LISTQUERY:
    PBXPKT.usPktID                  -> LISTQUERY
    PBXPKT.usPktSize                -> sizeof(PBXPKT)
    PBXPKT.aLQNames.usFirstName     -> First name to return

    PBXPKT.usPktID                  <- ACK | LISTQUERY
    PBXPKT.usPktSize                <- sizeof(PBXPKT) +
                   (sizeof(PBXNAME) * PBXPKT.aLQNames.usNameCnt-1);
    PBXPKT.usRetCode                <- Return code
    PBXPKT.aLQNames.usNameCnt       <- Number of names returned
    PBXPKT.aLQNames.fMoreNames      <- Zero or non-zero
    PBXPKT.aPBXName[x].pszName      <- Client name
    PBXPKT.aPBXName[x].usClientType <- Client program ID

    In a LISTQUERY acknowledgement, PBX will set
    PBXPKT.aLQNames.usNameCnt to the number of names returned.
    If more names exist then PBX could return,
    PBXPKT.aLQNames.fMoreNames will be non-zero.  To retreive
    the remaining names, send additional LISTQUERY requests setting
    the PBXPKT.aLQNames.usNameCnt field to the total number of
    retrieved names plus one (on the first call it should be zero).

    Because the number of names PBX may return is unknown at the
    time of the call, the packet size cannot be predicted (though
    it will never exceeded the pipe buffer size).  Reading a full
    PBXPKT will read the first returned name, leaving
    PBXPKT.aLQNames.usNameCnt-1 names in the pipe buffer.

  Author:  Brendan Dixon
           Microsoft, inc.
           LAN Manager Developer Support

  This code example is provided for demonstration purposes only.
  Microsoft makes no warranty, either express or implied,
  as to its usability in any given situation.
-------------------------------------------------------------------*/

#ifndef _PBXPKTH
#define _PBXPKTH

// Defines ----------------------------------------------------------
#define NAMESIZE            (15+1)      // Maximum client name size
#define PBXMSGSIZE          (17+1)      // Maximum PBX message size

#define PIPELINE            "\\PIPE\\MSJ\\PBX"     // PBX Named-Pipe
#define ANNOUNCELINE        "\\MAILSLOT\\MSJ\\PBX" // PBX Mailslot
#define ANNOUNCEPRIORITY    (9)         // PBX Message priority

// Acknowledgement return codes (returned by PBX)
#define PBXERROR            1           // Unexpected PBX error

// REGISTER request return codes
#define R_TOOMANYNAMES      2           // Too many names supplied
#define R_NONAME            3           // No name supplied
#define R_INVALIDCLIENTTYPE 4           // Client type was zero
#define R_DUPLICATENAME     5           // Duplicate name in table

// CONNECT request return codes
#define C_TOOMANYNAMES      2           // Too many names supplied
#define C_NONAME            3           // No name supplied
#define C_INVALIDNAME       4           // Invalid connection name
#define C_TARGETBUSY        5           // Target client is not free
#define C_TARGETERR         6           // Unable to inform target

// PBX packet identifiers
#define ACK                 0x8000      // Acknowledgement bit mask

#define REGISTER            1           // Register a client
#define CONNECT             2           // Connect two clients
#define LISTQUERY           3           // Return list of clients

// Structures -------------------------------------------------------
// PBX names structure
typedef struct _PBXNAME {
  char            pszName[NAMESIZE];      // Client name
  unsigned short  usClientType;           // Client type
                                          //   This value is client
                                          //   dependent; however,
                                          //   PBX reserves zero
} PBXNAME;

// PBX Packet structure
typedef struct _PBXPKT {
  unsigned short  usPktID;                // Packet Identifer
  unsigned short  usPktSize;              // Packet size
  unsigned short  usRetCode;              // Return code
  union {
    unsigned short  usNameCnt;            // Supplied names count
    struct {
      unsigned short  usFirstName;        // First name to retrieve
      unsigned short  usNameCnt;          // Returned names count
      int             fMoreNames;         // More names exist flag
    }               aLQNames;             // LISTQUERY names struct
  };
  PBXNAME         aPBXName[1];            // Array of PBX names
} PBXPKT;

#endif
