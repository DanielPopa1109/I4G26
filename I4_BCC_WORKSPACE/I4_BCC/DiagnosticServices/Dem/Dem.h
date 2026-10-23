#ifndef G26_DEM_H
#define G26_DEM_H
#include "Dem_Cfg.h"



void Dem_PreInit(void);
void Dem_Init(const Dem_ConfigType *ConfigPtr);
void Dem_Shutdown(void);
void Dem_MainFunction(void);
Dem_InitStateType Dem_GetInitState(void);
Std_ReturnType Dem_SetEventStatus(Dem_EventIdType EventId, Dem_EventStatusType EventStatus);
Std_ReturnType Dem_ReportErrorStatus(Dem_EventIdType EventId, Dem_EventStatusType EventStatus);
Std_ReturnType Dem_GetEventUdsStatus(Dem_EventIdType EventId, Dem_UdsStatusByteType *Status);
Std_ReturnType Dem_GetFaultDetectionCounter(Dem_EventIdType EventId, sint8 *Counter);
Std_ReturnType Dem_ResetEventDebounceStatus(Dem_EventIdType EventId,
    Dem_DebounceResetStatusType DebounceResetStatus);
Std_ReturnType Dem_GetDTCOfEvent(Dem_EventIdType EventId, Dem_DTCFormatType Format, Dem_DTCType *DTC);
Std_ReturnType Dem_SelectDTC(uint8 ClientId, Dem_DTCType DTC,
    Dem_DTCFormatType Format, Dem_DTCOriginType Origin);
Std_ReturnType Dem_GetDTCSelectionResult(uint8 ClientId);
Std_ReturnType Dem_GetDTCSelectionResultForClearDTC(uint8 ClientId);
Std_ReturnType Dem_GetStatusOfDTC(uint8 ClientId, Dem_UdsStatusByteType *Status);
Std_ReturnType Dem_SetOperationCycleState(Dem_OperationCycleIdType Cycle,
                                           Dem_OperationCycleStateType State);
Std_ReturnType Dem_ClearDTC(uint8 ClientId);
Dem_ClearDTCStatusType Dem_GetClearDTCStatus(void);
Std_ReturnType Dem_SetDTCFilter(uint8 ClientId, uint8 StatusMask, Dem_DTCFormatType Format,
                                Dem_DTCOriginType Origin, boolean FilterWithSeverity,
                                Dem_DTCSeverityType SeverityMask,
                                boolean FilterForFaultDetectionCounter);
Std_ReturnType Dem_GetNumberOfFilteredDTC(uint8 ClientId, uint16 *Count);
Std_ReturnType Dem_GetNumberOfEventMemoryEntries(uint8 ClientId,
    Dem_DTCOriginType Origin, uint8 *Count);
Std_ReturnType Dem_GetNextFilteredDTC(uint8 ClientId, Dem_DTCType *DTC, Dem_UdsStatusByteType *Status);
Std_ReturnType Dem_GetDTCStatusAvailabilityMask(uint8 ClientId,
    Dem_UdsStatusByteType *StatusMask, Dem_DTCOriginType Origin);
Std_ReturnType Dem_SetDtcSetting(boolean Enabled);
Std_ReturnType Dem_DisableDTCSetting(uint8 ClientId);
Std_ReturnType Dem_EnableDTCSetting(uint8 ClientId);
Std_ReturnType Dem_GetSnapshotDataByDTC(Dem_DTCType DTC, Dem_DTCFormatType Format,
    Dem_DTCOriginType Origin, uint8 RecordNumber, uint8 *DestBuffer, uint16 *BufSize);
Std_ReturnType Dem_GetExtendedDataRecordByDTC(Dem_DTCType DTC, Dem_DTCFormatType Format,
    Dem_DTCOriginType Origin, uint8 RecordNumber, uint8 *DestBuffer, uint16 *BufSize);

void Dem_Serialize(uint8 *image);
uint8 Dem_Deserialize(const uint8 *image);
void Dem_StartRestoredCycle(void);

/* G26 monitor compatibility; event state remains private. */

#define DTC_LIGHT_SENSOR_MALFUNCTION 							0x03
#define DTC_VIBRATION_SENSOR_MALFUNCTION 						0x13
#define DTC_REAR_PARKING_DISTANCE_SENSOR_MALFUNCTION 			0x02
#define DTC_FRONT_PARKING_DISTANCE_SENSOR_MALFUNCTION 			0x12
#define DTC_BLUETOOTH_MODULE_MALFUNCTION 						0x22
#define DTC_SOFTWARE_RESET 										0x35
#define DTC_HARDWARE_RESET 										0x56
#define DTC_PERIPHERAL_ERROR 									0x99
#define POSITION_DTC_LIGHT_SENSOR_MALFUNCTION 					0
#define POSITION_DTC_VIBRATION_SENSOR_MALFUNCTION 				1
#define POSITION_DTC_REAR_PARKING_DISTANCE_SENSOR_MALFUNCTION 	2
#define POSITION_DTC_FRONT_PARKING_DISTANCE_SENSOR_MALFUNCTION 	3
#define POSITION_DTC_BLUETOOTH_MODULE_MALFUNCTION 				4
#define POSITION_DTC_SOFTWARE_RESET 							5
#define POSITION_DTC_HARDWARE_RESET 							6
#define POSITION_DTC_PERIPHERAL_ERROR 							7
void Dem_ReceiveFault(uint8 fault);
uint8 Dem_IsLegacyFaultActive(uint8 slot);
#endif
