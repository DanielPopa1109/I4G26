#include "Com_Cfg.h"
/* 0x040 layout from ZGW_CAN_3.dbc. Timeout and status are G26 local policy. */
const Com_PduConfig Com_Pdus[COM_PDU_COUNT]={{0x040,4,0,0,0,100},{0x241,4,1,1,20,0}};
const Com_SignalConfig Com_Signals[COM_SIGNAL_COUNT]={
 {0,0,3,0,7},{0,3,3,0,7},{0,6,2,0,3},{0,8,4,0,15},
 {0,12,4,0,15},{0,16,8,0,255},{0,24,5,0,31},{0,29,3,0,7},
 {1,0,8,0,255},{1,8,8,0,255},{1,16,8,0,255},{1,24,8,0,255}
};
