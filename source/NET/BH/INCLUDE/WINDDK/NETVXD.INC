VNetSup_Device_ID	EQU	0480H
VRedir_Device_ID	EQU	0481H
VBrowse_Device_ID	EQU	0482H
VSHARE_Device_ID	EQU	0483H
SERVER_Device_ID	EQU	032H
IFSMgr_Device_ID	EQU	0484H
MEMPROBE_Device_ID	EQU	0485H
VFAT_Device_ID	EQU	0486H
NWLINK_Device_ID	EQU	0487H
VNWLINK_Device_ID	EQU	0487H
NWSUP_Device_ID	EQU	0487H
VTDI_Device_ID	EQU	0488H
VIP_Device_ID	EQU	0489H
VTCP_Device_ID	EQU	048AH
VCache_Device_ID	EQU	048BH
VUDP_Device_ID	EQU	048CH
VAsync_Device_ID	EQU	048DH
NOBALL_Device_ID	EQU	048EH
FILESEC_Device_ID	EQU	0491H
MAC_Device_ID	EQU	0000H
NDISMapper_Device_ID	EQU	0000H
ODIMapper_Device_ID	EQU	0000H
NDIS_DEVICE_ID	EQU	00028H
VNB_Device_ID	EQU	00031H
ifdef VMM_Init_Order
MEMPROBE_Init_Order	EQU	<1000H+VMM_Init_Order>
VShare_Init_Order	EQU	<08000H+DOSMGR_Init_Order>
IFSMgr_Init_Order	EQU	<10000H+V86MMGR_Init_Order>
VNetSup_Init_Order	EQU	<18000H+V86MMGR_Init_Order>
VRedir_Init_Order	EQU	<20000H+V86MMGR_Init_Order>
VServer_Init_Order	EQU	<30000H+V86MMGR_Init_Order>
VCache_Init_Order	EQU	<0100H+VMM_Init_Order>
NDIS_Init_Order	EQU	<02000H+VNETBIOS_Init_Order>
MAC_Init_Order	EQU	<02000H+NDIS_Init_Order>
GATEWAY_Init_Order	EQU	<01000H+MAC_Init_Order>
NETBEUI_Init_Order	EQU	<02000H+MAC_Init_Order>
PROTOCOL_Init_Order	EQU	<02000H+MAC_Init_Order>
VTDI_Init_Order	EQU	<02000H+MAC_Init_Order>
VIP_Init_Order	EQU	<02000H+VTDI_Init_Order>
VTCP_Init_Order	EQU	<02000H+VIP_Init_Order>
VUDP_Init_Order	EQU	<02000H+VIP_Init_Order>
FSD_Init_Order	EQU	<00001H+IFSMgr_Init_Order>
FILESEC_Init_Order	EQU	<00100H+VServer_Init_Order>
endif
