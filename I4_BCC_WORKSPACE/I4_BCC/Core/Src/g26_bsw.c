#include "g26_bsw.h"
#include "Can.h"
#include "CanIf.h"
#include "CanTp.h"
#include "PduR.h"
#include "Com.h"
#include "CanNm.h"
#include "CanSM.h"
#include "ComM.h"
#include "Dcm.h"
#include "Dem.h"
#include "NvM.h"
#include "MemIf.h"
static unsigned initialized;
void G26_BswInit(void){
 Dem_PreInit();Dem_Init(&Dem_Config);NvM_Init();NvM_ReadAll();
 /* Restore completes before application monitors run. Reads only, no erase. */
 for(unsigned i=0;i<16 && !NvM_IsIdle();++i)NvM_MainFunction();
 CanIf_Init();CanTp_Init();PduR_Init();Com_Init();CanNm_Init();CanSM_Init();ComM_Init();Dcm_Init();Can_Init();
 MemIf_SetErasePreparationCallback(Dcm_PrepareLongFlashOperation);
 initialized=1;
}
void G26_BswMainFunction(void){
 if(!initialized)return;
 Can_MainFunction();CanTp_MainFunction();ComM_MainFunction();CanNm_MainFunction();CanSM_MainFunction();
 Com_MainFunction();Dcm_MainFunction();NvM_MainFunction();Dem_MainFunction();
 extern void Dem_PersistenceMainFunction(void);Dem_PersistenceMainFunction();
}
