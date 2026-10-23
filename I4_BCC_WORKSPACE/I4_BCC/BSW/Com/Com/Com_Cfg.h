#ifndef COM_CFG_H
#define COM_CFG_H
#include <stdint.h>
#define COM_PDU_COUNT 2U
#define COM_SIGNAL_COUNT 12U
enum { COM_FOG,COM_GEAR,COM_HIGH,COM_IGNITION,COM_TURN,COM_LIGHT,COM_RLS,COM_LOCK,COM_READY,COM_OUTPUTS,COM_DTCS,COM_COUNTER };
typedef struct {uint16_t canId;uint8_t dlc,tx,group;uint16_t period,timeout;} Com_PduConfig;
typedef struct {uint8_t pdu,bit,width;uint32_t initial,maximum;} Com_SignalConfig;
extern const Com_PduConfig Com_Pdus[COM_PDU_COUNT];
extern const Com_SignalConfig Com_Signals[COM_SIGNAL_COUNT];
void Com_AppRx(uint16_t id);
void Com_AppTx(void);
#endif
