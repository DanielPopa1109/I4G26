/* Application adapter only. BSW owns all bus parsing, transport and timing. */
#include "g26_app_net.h"
#include "Com.h"
#include "CanSM.h"
#include "Dem.h"
#include "g26_board.h"
#include "App_Dependencies.h"
uint8_t G26_AppNetFullCommunication(void){return CanSM_IsTxAllowed();}
void Com_AppRx(uint16_t id){(void)id;}
void G26_AppNetApplyCommand(void){
#if G26_ENABLE_PROPOSED_BODY_COMMANDS
 uint32_t fog,turn,high,lock;
 uint32_t irq=__get_PRIMASK();__disable_irq();
 uint8_t valid=CanSM_IsTxAllowed() && !Com_ReceiveSignal(COM_FOG,&fog) && !Com_ReceiveSignal(COM_TURN,&turn) &&
    !Com_ReceiveSignal(COM_HIGH,&high) && !Com_ReceiveSignal(COM_LOCK,&lock);
 if(!irq)__enable_irq();
 if(!valid)return;
 if(lock<=1)CenLoc_CurrentState=(uint8_t)lock;
 if(high<=1)ExtLights_HighBeam_CurrentState=(uint8_t)high;
 if(fog<=3){ExtLights_FrontFogLight_CurrentState=(fog&1)!=0;ExtLights_RearFogLight_CurrentState=(fog&2)!=0;}
 if(turn<=7){ExtLights_TurnSignalLeft_CurrentState=(turn&1)!=0;ExtLights_TurnSignalRight_CurrentState=(turn&2)!=0;ExtLights_HazardLight_CurrentState=(turn&4)!=0;}
#endif
}
void Com_AppTx(void){
#if G26_ENABLE_PROPOSED_STATUS
 static uint8_t counter;
 uint32_t ready=1U|(CanSM_IsTxAllowed()?2U:0U)|(Com_IsSignalValid(COM_FOG)?4U:0U);
 uint32_t outputs=(CenLoc_CurrentState?1U:0U)|(ExtLights_HighBeam_CurrentState?2U:0U)|
 (ExtLights_TurnSignalLeft_CurrentState?4U:0U)|(ExtLights_TurnSignalRight_CurrentState?8U:0U)|
 (ExtLights_HazardLight_CurrentState?16U:0U)|(ExtLights_FrontFogLight_CurrentState?32U:0U)|(ExtLights_RearFogLight_CurrentState?64U:0U);
 uint32_t count=0,value=counter++;
 for(uint8_t i=0;i<8;++i)count+=Dem_IsLegacyFaultActive(i);
 (void)Com_SendSignal(COM_READY,&ready);(void)Com_SendSignal(COM_OUTPUTS,&outputs);
 (void)Com_SendSignal(COM_DTCS,&count);(void)Com_SendSignal(COM_COUNTER,&value);
#endif
}
