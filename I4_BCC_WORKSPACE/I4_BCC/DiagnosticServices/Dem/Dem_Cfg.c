#include "Dem_Cfg.h"
extern void Dem_ApplicationStatusChanged(Dem_EventIdType,uint8,uint8);
/* Retained G26 monitor numbers, zero-extended to local lab UDS DTCs.
 * These are not OEM assigned DTCs. Two PREFAILED samples qualify a fault. */
#define EVENT(id) {(id),(id),1U,2,-2,1U,40U}
static const Dem_EventConfigType events[DEM_EVENT_COUNT]={
 EVENT(0x03),EVENT(0x13),EVENT(0x02),EVENT(0x12),EVENT(0x22),EVENT(0x35),EVENT(0x56),EVENT(0x99)
};
const Dem_ConfigType Dem_Config={events,DEM_EVENT_COUNT,DEM_PRIMARY_MEMORY_SIZE,0,0x7F,Dem_ApplicationStatusChanged};
