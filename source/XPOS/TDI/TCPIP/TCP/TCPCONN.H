/********************************************************************/
/**                     Microsoft LAN Manager                      **/
/**               Copyright(c) Microsoft Corp., 1990-1993          **/
/********************************************************************/
/* :ts=4 */

//** TCPCONN.H - TCP connection related definitions.
//
// This file contains the definitions for connection related structures,
// such as the TCPConnReq structure.
//

#define	INVALID_CONN_INDEX	0xffffff

//* Structure used for tracking Connect/Listen/Accept/Disconnect requests.

#define	tcr_signature	0x20524354		// 'TCR '

struct TCPConnReq {
	struct TCPReq	tcr_req;			// General request structure.
#ifdef DEBUG
	ulong			tcr_sig;
#endif
	struct _TDI_CONNECTION_INFORMATION	*tcr_conninfo;	// Where to return info.
	ushort			tcr_flags;			// Flags for this request.
	ushort			tcr_timeout;		// Timeout value for this request.
};

typedef struct TCPConnReq TCPConnReq;

typedef void (*ConnDoneRtn)(struct TCPConn *, CTELockHandle); 

//* Structure of a TCB Connection. A TCP Connection points to a TCP and an 
//	address object.

#define	tc_signature	0x20204354		// 'TC '

struct TCPConn {
#ifdef DEBUG
	ulong			tc_sig;
#endif
	struct	TCB 	*tc_tcb;			// Pointer to TCB for connection.
	struct	AddrObj	*tc_ao;				// Back pointer to AddrObj.
	struct	TCPConn	*tc_next;			// Linkage on AO.
	uchar			tc_inst;			// Instance number.
	uchar			tc_flags;			// Flags for connection.
	ushort			tc_refcnt;			// Count of TCBs which reference this
										// connection.
	void			*tc_context;		// User's context.
	CTEReqCmpltRtn	tc_rtn;				// Completion routine.
	PVOID			tc_rtncontext;		// User context for completion routine.
	ConnDoneRtn		tc_donertn; 		// Routine to call when refcnt goes to 0.
	uint			tc_tcbflags;		// Flags for TCB when it comes in.
	uint			tc_window;			// Default window for TCB.

}; /* TCPConn */

typedef struct TCPConn TCPConn;

#define	CONN_CLOSING	1				// Connection is closing.
#define	CONN_DISACC		2				// Conn. is disassociating.
#define	CONN_WINSET		4				// Window explictly set.

#define	CONN_INVALID	(CONN_CLOSING | CONN_DISACC)

//* Structure of a ConnTable.
typedef struct TCPConn	*TCPConnTable[];

extern TCPConnTable	*ConnTable;

#define	FREE_CONN_INDEX(i) (*ConnTable)[(i)] = NULL
EXTERNAL_LOCK(ConnTableLock)


//* External definitions for TDI entry points.
extern TDI_STATUS TdiOpenConnection(PTDI_REQUEST Request, PVOID Context);
extern TDI_STATUS TdiCloseConnection(PTDI_REQUEST Request);
extern TDI_STATUS TdiAssociateAddress(PTDI_REQUEST Request, HANDLE AddrHandle);
extern TDI_STATUS TdiDisAssociateAddress(PTDI_REQUEST Request);
extern TDI_STATUS TdiConnect(PTDI_REQUEST Request, void *Timeout, 
		PTDI_CONNECTION_INFORMATION RequestAddr, 
		PTDI_CONNECTION_INFORMATION ReturnAddr);
extern TDI_STATUS TdiListen(PTDI_REQUEST Request, ushort Flags, 
		PTDI_CONNECTION_INFORMATION AcceptableAddr, 
		PTDI_CONNECTION_INFORMATION ConnectedAddr);
extern TDI_STATUS TdiAccept(PTDI_REQUEST Request, 
		PTDI_CONNECTION_INFORMATION AcceptInfo, 
		PTDI_CONNECTION_INFORMATION ConnectedInfo);
extern TDI_STATUS TdiDisconnect(PTDI_REQUEST Request, void *TO, ushort Flags,
		PTDI_CONNECTION_INFORMATION DiscConnInfo, 
		PTDI_CONNECTION_INFORMATION ReturnInfo);

extern struct TCPConnReq *GetConnReq(void);
extern void	FreeConnReq(struct TCPConnReq *FreedReq);
extern void DerefTCB(struct TCB *DoneTCB, CTELockHandle Handle);
extern void	InitRCE(struct TCB *NewTCB);
extern void AcceptConn(struct TCB *AcceptTCB, CTELockHandle Handle);
extern void FreeConnID(uint ConnID);
extern void NotifyOfDisc(struct TCB *DiscTCB, struct IPOptInfo *DiscInfo, 
	TDI_STATUS Status);
extern TCPConn *GetConnFromConnID(uint ConnID);

extern void TryToCloseTCB(struct TCB *ClosedTCB, uchar Reason, 
	CTELockHandle Handle);
extern TDI_STATUS InitTCBFromConn(struct TCPConn *Conn, struct TCB *NewTCB, 
	PTDI_CONNECTION_INFORMATION Addr);

extern void PushData(struct TCB *PushTCB);
extern TDI_STATUS MapIPError(IP_STATUS IPError, TDI_STATUS Default);
extern void GracefulClose(struct TCB *CloseTCB, uint ToTimeWait, uint Notify,
	CTELockHandle Handle);
extern void RemoveTCBFromConn(struct TCB *RemovedTCB);
