#include "NvM.h"
#include "Dem.h"
#include <string.h>
uint8_t NvM_DemImage[512];
const NvM_BlockDescriptorType NvM_BlockDescriptor[NVM_BLOCK_COUNT]={{512,2,NvM_DemImage,0,1,1}};
void NvM_ApplicationCapture(void){
 static uint8_t captured[512];Dem_Serialize(captured);
 if(memcmp(captured,NvM_DemImage,sizeof(captured))){
  memcpy(NvM_DemImage,captured,sizeof(captured));(void)NvM_SetRamBlockStatus(NVM_BLOCK_DEM,1);
 }
}
void NvM_ApplicationRestore(void){
 Dem_PreInit();Dem_Init(&Dem_Config);
 if(!Dem_Deserialize(NvM_DemImage))Dem_StartRestoredCycle();
}
