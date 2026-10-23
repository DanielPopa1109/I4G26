#include "Dem.h"
#include "NvM.h"
static volatile uint8 dirty;
static uint16 flushTicks;
void Dem_ReceiveFault(uint8 event){(void)Dem_SetEventStatus(event,DEM_EVENT_STATUS_FAILED);}
uint8 Dem_IsLegacyFaultActive(uint8 slot){
 uint8 status=0;
 if(slot>=DEM_EVENT_COUNT)return 0;
 (void)Dem_GetEventUdsStatus(Dem_Config.events[slot].eventId,&status);
 return (status&DEM_UDS_STATUS_TF)!=0;
}
void Dem_ApplicationStatusChanged(Dem_EventIdType event,uint8 before,uint8 after){
 (void)event;(void)before;(void)after;dirty=1;
}
void Dem_PersistenceMainFunction(void){
 if(flushTicks<1000)++flushTicks;
 if(dirty && flushTicks>=1000 && NvM_IsIdle() && Dem_GetClearDTCStatus()!=DEM_CLEAR_PENDING){
  /* Consume the flag before the protected snapshot. Reports arriving during
   * serialization/enqueue must remain dirty for a subsequent write. */
  dirty=0;
  Dem_Serialize(NvM_DemImage);
  if(!NvM_WriteBlock(NVM_BLOCK_DEM,NvM_DemImage))flushTicks=0;
  else dirty=1;
 }
 NvM_RequestResultType r;
 if(!NvM_GetErrorStatus(NVM_BLOCK_DEM,&r) && r==NVM_REQ_NOT_OK)dirty=1;
}
