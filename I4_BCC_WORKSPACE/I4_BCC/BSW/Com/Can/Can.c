/* F93 controller/confirmation interface, adapted to the G26 MCP2515 owner. */
#include "Can.h"
#include "../CanIf/CanIf.h"
#include "../CanSM/CanSM.h"
#include "g26_board.h"
#include <string.h>
#ifndef G26_HOST_TEST
#include "g26_mcp2515.h"
#include "g26_mcp_stm32.h"
#include "main.h"
static G26_Mcp2515 device;
#endif
static uint8_t initialized, pending, pendingApplication;
static uint16_t pendingId, txAge, recovery;
static Can_ControllerStateType controller;
void Can_Init(void)
{
    initialized=1; pending=0; recovery=0; controller=CAN_CS_STOPPED;
#ifndef G26_HOST_TEST
#if G26_MCP2515_REWIRED_BOARD
    G26_McpInitState(&device,G26_McpStm32Bus());
    if(!G26_McpConfigureBitrate(&device,G26_MCP_OSC_HZ,500000U) && !G26_McpStartApp(&device)) controller=CAN_CS_STARTED;
    else recovery=200;
#endif
#else
    controller=CAN_CS_STARTED;
#endif
}
static Can_StatusType writeFrame(uint16_t id,const uint8_t *p,uint8_t n,uint8_t app)
{
    if(!initialized || controller!=CAN_CS_STARTED || pending || !p || n>8 || id>0x7FF) return CAN_NOT_OK;
#ifdef G26_HOST_TEST
    extern Can_StatusType Can_TestTransmit(uint16_t,const uint8_t *,uint8_t);
    if(Can_TestTransmit(id,p,n)!=CAN_OK) return CAN_NOT_OK;
#else
    G26_CanFrame f={0}; f.id=id; f.dlc=n; memcpy(f.data,p,n);
    if(G26_McpSend(&device,&f)) return CAN_NOT_OK;
#endif
    pending=1; pendingApplication=app; pendingId=id; txAge=0; return CAN_OK;
}
Can_StatusType Can_Write(uint16_t id,const uint8_t *p,uint8_t n){return writeFrame(id,p,n,0);}
Can_StatusType Can_WriteApplication(uint16_t id,const uint8_t *p,uint8_t n){return writeFrame(id,p,n,1);}
void Can_TxConfirmation(uint8_t ok)
{
    if(!pending) return;
    pending=0;
    if(pendingApplication) CanIf_ApplicationTxConfirmation(pendingId,ok); else CanIf_TxConfirmation(ok);
}
void Can_BusOff(void){Can_TxConfirmation(0);controller=CAN_CS_BUSOFF;recovery=200;CanSM_ControllerBusOff();}
void Can_RxIndication(const Can_FrameType *f){if(initialized && f && f->dlc<=8 && controller==CAN_CS_STARTED)CanIf_RxIndication(f);}
Can_ControllerStateType Can_GetControllerState(void){return controller;}
Can_StatusType Can_SetControllerMode(Can_ControllerStateType state)
{
    if(state!=CAN_CS_STARTED && state!=CAN_CS_STOPPED) return CAN_NOT_OK;
    if(controller==CAN_CS_BUSOFF) return CAN_NOT_OK;
    controller=state;return CAN_OK;
}
void Can_MainFunction(void)
{
    if(!initialized)return;
    if(recovery){
        if(--recovery)return;
#ifndef G26_HOST_TEST
#if G26_MCP2515_REWIRED_BOARD
        if(G26_McpStartApp(&device)){recovery=200;return;}
#else
        return;
#endif
#endif
        controller=CAN_CS_STARTED;
    }
    if(controller!=CAN_CS_STARTED)return;
#ifndef G26_HOST_TEST
    if(pending){int r=G26_McpTxStatus(&device);if(r>0)Can_TxConfirmation(1);else if(r<0)Can_TxConfirmation(0);}
    for(unsigned i=0;i<4;++i){
        G26_CanFrame f;int r=G26_McpPoll(&device,&f);
        if(r<0){Can_BusOff();return;}if(!r)break;
        Can_FrameType frame;frame.id=(uint16_t)f.id;frame.dlc=f.dlc;memcpy(frame.data,f.data,8);Can_RxIndication(&frame);
    }
#endif
    if(pending && ++txAge>=200){
#ifndef G26_HOST_TEST
        G26_McpAbortTx(&device);
#endif
        Can_TxConfirmation(0);
    }
}
