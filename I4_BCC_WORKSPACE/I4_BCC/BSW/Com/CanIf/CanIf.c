#include "CanIf.h"
#include "../CanTp/CanTp.h"
#include "../PduR/PduR.h"
#include "../CanNm/CanNm.h"
#include "../Com/Com.h"
#include "g26_board.h"
typedef struct { uint16_t id; uint8_t minimum,maximum,target; } RxConfig;
static const RxConfig rx[]={{0x710,2,8,0},{0x3FF,1,1,1},{0x040,4,4,2}};
static uint8_t initialized,flowControl;
void CanIf_Init(void){initialized=1;flowControl=0;}
Can_StatusType CanIf_Transmit(uint8_t pdu,const uint8_t *p,uint8_t n)
{
    if(!initialized || pdu!=CANIF_TX_DIAG || !p || n!=8)return CAN_NOT_OK;
    Can_StatusType r=Can_Write(0x709,p,n);
    if(r==CAN_OK)flowControl=(p[1]&0xF0)==0x30;
    return r;
}
void CanIf_RxIndication(const Can_FrameType *f)
{
    if(!initialized || !f)return;
    for(unsigned i=0;i<sizeof(rx)/sizeof(rx[0]);++i)
        if(f->id==rx[i].id && f->dlc>=rx[i].minimum && f->dlc<=rx[i].maximum){
            if(rx[i].target==0)CanTp_RxIndication(f->data,f->dlc);
            else if(rx[i].target==1)CanNm_RxIndication(f->id,f->data,f->dlc);
            else PduR_CanIfRxIndication(f->id,f->data,f->dlc);
            return;
        }
}
void CanIf_TxConfirmation(uint8_t ok){if(flowControl)CanTp_FcConfirmation(ok);else CanTp_TxConfirmation(ok);}
Can_StatusType CanIf_TransmitApplication(uint16_t id,const uint8_t *p,uint8_t n)
{
    if(!initialized || !G26_ENABLE_PROPOSED_STATUS || id!=0x241 || n!=4 || !p)return CAN_NOT_OK;
    return Can_WriteApplication(id,p,n);
}
void CanIf_ApplicationTxConfirmation(uint16_t id,uint8_t ok){Com_TxConfirmation(id,ok);}
