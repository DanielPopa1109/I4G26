#ifndef G26_DEM_CFG_H
#define G26_DEM_CFG_H
#include "Dem_Types.h"

#define DEM_EVENT_COUNT 8U
#define DEM_PRIMARY_MEMORY_SIZE 8U
#define DEM_DEFAULT_OPERATION_CYCLE 0U

typedef struct {
    Dem_EventIdType eventId;
    Dem_DTCType dtc;
    uint8 priority;
    sint16 failedThreshold;
    sint16 passedThreshold;
    uint8 confirmationThreshold;
    uint8 agingThreshold;
} Dem_EventConfigType;

typedef struct {
    const Dem_EventConfigType *events;
    uint8 eventCount;
    uint8 primaryMemorySize;
    Dem_OperationCycleIdType operationCycle;
    Dem_UdsStatusByteType statusAvailabilityMask;
    void (*statusChanged)(Dem_EventIdType event,uint8 oldStatus,uint8 newStatus);
} Dem_ConfigType;

extern const Dem_ConfigType Dem_Config;
#endif
