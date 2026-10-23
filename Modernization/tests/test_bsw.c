#include <assert.h>
#ifdef NDEBUG
#error Test assertions must be enabled
#endif
#include <stdio.h>
#include <string.h>
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
#include "Fls.h"
#include "g26_layout.h"
static uint8_t flash[0x80000],sent[8];
static uint16_t sentId;
static unsigned sends,resets,confirms,failProgram,failErase,eraseCount;
static uint8_t autoConfirm=1;
static int writable(uint32_t a,uint32_t n){return n && ((a>=FLS_BANK0 && a<FLS_BANK0+FLS_SIZE0 && n<=FLS_BANK0+FLS_SIZE0-a)||(a>=FLS_BANK1 && a<FLS_BANK1+FLS_SIZE1 && n<=FLS_BANK1+FLS_SIZE1-a));}
int Fls_Read(uint32_t a,void *p,uint32_t n){if(a<0x08000000 || n>sizeof(flash)-(a-0x08000000))return 1;memcpy(p,flash+a-0x08000000,n);return 0;}
int Fls_Program(uint32_t a,const void *p,uint32_t n){
 assert(writable(a,n));if(failProgram)return 1;
 for(uint32_t i=0;i<n;++i){uint8_t v=((const uint8_t *)p)[i],*d=flash+a-0x08000000+i;if((*d&v)!=v)return 1;*d&=v;}return 0;
}
int Fls_EraseBank(uint8_t b){assert(b<2);++eraseCount;if(failErase)return 1;memset(flash+(b?FLS_BANK1:FLS_BANK0)-0x08000000,255,b?FLS_SIZE1:FLS_SIZE0);return 0;}
Can_StatusType Can_TestTransmit(uint16_t id,const uint8_t *p,uint8_t n){assert(n<=8);memcpy(sent,p,n);sentId=id;++sends;return CAN_OK;}
void Com_AppRx(uint16_t id){(void)id;}
void Com_AppTx(void){}
void Dcm_TestReset(uint8_t s){assert(s==2 || s==5 || s==6);++resets;}
static void network(void){Can_Init();CanIf_Init();CanTp_Init();PduR_Init();Com_Init();CanNm_Init();CanSM_Init();ComM_Init();Dcm_Init();MemIf_SetErasePreparationCallback(Dcm_PrepareLongFlashOperation);sends=resets=0;autoConfirm=1;}
static void tick(void){if(autoConfirm){Can_TxConfirmation(1);++confirms;}Can_MainFunction();CanTp_MainFunction();ComM_MainFunction();CanNm_MainFunction();CanSM_MainFunction();Com_MainFunction();Dcm_MainFunction();NvM_MainFunction();Dem_MainFunction();}
static void ticks(unsigned n){while(n--)tick();}
static void receive(uint16_t id,const uint8_t *p,uint8_t n){Can_FrameType f={0};f.id=id;f.dlc=n;memcpy(f.data,p,n);Can_RxIndication(&f);}
static void request(const uint8_t *p,uint16_t n){Dcm_TpRxIndication(p,n);tick();}
static void nrc(uint8_t code){assert(sentId==0x709 && sent[2]==0x7F && sent[4]==code);}
static void storage(void){NvM_Init();NvM_ReadAll();for(unsigned i=0;i<64 && !NvM_IsIdle();++i)NvM_MainFunction();assert(NvM_IsIdle());}
static void drain(void){for(unsigned i=0;i<512 && !NvM_IsIdle();++i)NvM_MainFunction();assert(NvM_IsIdle());}
static void test_network(void){
 network();uint8_t p[8]={0x55,2,0x3E,0};receive(0x711,p,8);ticks(2);assert(!sends);
 receive(0x710,p,8);ticks(2);assert(sends==1 && sentId==0x709 && sent[2]==0x7E);
 ticks(1);p[0]=0x54;receive(0x710,p,8);ticks(2);assert(sends==1);
 p[0]=0x55;p[1]=0x10;p[2]=9;memset(p+3,1,5);receive(0x710,p,8);assert(sent[1]==0x30);tick();
 p[1]=0x22;receive(0x710,p,8);assert(PduR_GetRxErrorCount()==1);
 p[1]=0x10;receive(0x710,p,8);ticks(201);assert(PduR_GetRxErrorCount()==2);
 uint8_t nm=0xFF;receive(0x3FF,&nm,1);ticks(2);assert(CanSM_IsTxAllowed());
 uint8_t body[4]={0xC5,0xA3,0x71,0xA1};receive(0x040,body,4);
 uint32_t v=0;assert(!Com_ReceiveSignal(COM_FOG,&v) && v==5);assert(!Com_ReceiveSignal(COM_HIGH,&v) && v==3);
 assert(!Com_ReceiveSignal(COM_LOCK,&v) && v==5);assert(!Com_ReceiveSignal(COM_LIGHT,&v) && v==0x71);
 Com_IpduGroupControl(0,0);Com_IpduGroupControl(0,1);assert(Com_ReceiveSignal(COM_FOG,&v));
 receive(0x040,body,4);assert(!Com_ReceiveSignal(COM_FOG,&v) && v==5);
 v=257;assert(Com_SendSignal(COM_OUTPUTS,&v));v=0xA5;assert(!Com_SendSignal(COM_OUTPUTS,&v));
 assert(!Com_TriggerIPDUSend(1));assert(sentId==0x241 && sent[1]==0xA5);tick();
 ticks(101);assert(!Com_IsSignalValid(COM_FOG));assert(Com_ReceiveSignal(COM_FOG,&v) && v==0);
 ticks(101);assert(!CanSM_IsTxAllowed());
 /* Diagnostic path remains usable with normal communication restricted. */
 p[0]=0x55;p[1]=2;p[2]=0x3E;p[3]=0;receive(0x710,p,8);ticks(2);assert(sent[2]==0x7E);
 Can_BusOff();assert(Can_GetControllerState()==CAN_CS_BUSOFF);ticks(199);assert(Can_GetControllerState()==CAN_CS_BUSOFF);tick();assert(Can_GetControllerState()==CAN_CS_STARTED);
 /* FF cannot complete until its physical TX confirmation arrives; early FC is retained. */
 network();autoConfirm=0;uint8_t data[30];memset(data,0x5A,sizeof(data));assert(CanTp_Transmit(data,sizeof(data))==CAN_OK);
 ticks(3);assert(sends==1 && CanTp_IsBusy());uint8_t fc[8]={0x41,0x30,2,10};receive(0x710,fc,8);
 Can_TxConfirmation(1);tick();assert(sends==2 && sent[1]==0x21);
 Can_TxConfirmation(1);tick();assert(sends==2);ticks(2);assert(sends==3 && sent[1]==0x22);
 Can_TxConfirmation(1);ticks(10);assert(sends==3);receive(0x710,fc,8);tick();assert(sends==4);
 Can_TxConfirmation(0);assert(!CanTp_IsBusy() && PduR_GetTxErrorCount()==1);
 network();autoConfirm=0;assert(CanTp_Transmit(data,3)==CAN_OK);ticks(201);assert(!CanTp_IsBusy());
 network();uint8_t ff[8]={0x55,0x10,9,0x22,0xF1,0x86,0xF1,0x87};
 receive(0x710,ff,8);assert(sent[1]==0x30);tick();
 uint8_t cf[8]={0x55,0x21,0xF1,0xA0,0xF1,0x86};receive(0x710,cf,8);ticks(2);assert(sent[1]==0x10);
 uint8_t more[8]={0x41,0x30,0,0};receive(0x710,more,8);ticks(10);assert(!CanTp_IsBusy());
 puts("PASS communication: filtering, physical confirmations, bus-off, SF/FF/CF/FC, BS/STmin, sequence/timeouts, COM deadlines, NM3 and diagnostic gating");
}
static void test_memory(void){
 memset(flash,255,sizeof(flash));storage();NvM_RequestResultType result;
 assert(!NvM_GetErrorStatus(1,&result) && result==NVM_REQ_RESTORED_FROM_ROM);
 Dem_ReceiveFault(DTC_LIGHT_SENSOR_MALFUNCTION);assert(Dem_IsLegacyFaultActive(0));NvM_WriteAll();drain();
 storage();assert(Dem_IsLegacyFaultActive(0));uint8 status;assert(!Dem_GetEventUdsStatus(3,&status) && (status&0x0C)==0x0C);
 uint8_t saved[512];Dem_Serialize(saved);uint8_t committedCounter=NvM_DemImage[8];
 /* Interrupt every step in a new record; restore the previously committed record. */
 uint8_t baseline[sizeof(flash)];memcpy(baseline,flash,sizeof(flash));
 for(unsigned cut=0;cut<20;++cut){
  memcpy(flash,baseline,sizeof(flash));storage();uint8_t changed[512];memcpy(changed,saved,512);changed[8]^=1;
  assert(!NvM_WriteBlock(1,changed));for(unsigned i=0;i<cut;++i)NvM_MainFunction();
  storage();assert(Dem_IsLegacyFaultActive(0));
  if(cut<17)assert(NvM_DemImage[8]==committedCounter);
 }
 memcpy(flash,baseline,sizeof(flash));storage();
 /* CRC damage rejects the only committed record and restores defaults. */
 flash[FLS_BANK0-0x08000000+100]^=1;storage();assert(!Dem_IsLegacyFaultActive(0));
 memcpy(flash,baseline,sizeof(flash));storage();
 /* Coalesce equal values and rotate the journal; erase only inactive bank. */
 unsigned oldErase=eraseCount;NvM_WriteAll();drain();assert(eraseCount==oldErase);
 for(unsigned i=0;i<70;++i){NvM_DemImage[8]=(uint8_t)i;assert(!NvM_WriteBlock(1,NvM_DemImage));drain();}
 assert(eraseCount>oldErase);storage();assert(Dem_IsLegacyFaultActive(0));
 for(uint32_t i=0;i<FLS_BANK0-0x08000000;++i)assert(flash[i]==255);
 for(uint32_t i=0x20000;i<0x60000;++i)assert(flash[i]==255);
 failProgram=1;NvM_DemImage[8]^=1;assert(!NvM_WriteBlock(1,NvM_DemImage));drain();
 assert(!NvM_GetErrorStatus(1,&result) && result==NVM_REQ_NOT_OK);failProgram=0;
 storage();assert(Dem_IsLegacyFaultActive(0));
 puts("PASS persistence: defaults, ReadAll/WriteAll, DEM restoration, CRC rejection, interrupted writes, journal rotation, deduplication, protected partitions and flash errors");
}
static void test_dem(void){
 Dem_PreInit();Dem_Init(&Dem_Config);uint8_t status;uint16 count,size;uint8_t data[16];
 assert(Dem_SetEventStatus(0xFFFF,DEM_EVENT_STATUS_FAILED)==E_NOT_OK);
 assert(!Dem_SetEventStatus(3,DEM_EVENT_STATUS_PREFAILED));assert(!Dem_GetEventUdsStatus(3,&status) && !(status&1));
 assert(!Dem_SetEventStatus(3,DEM_EVENT_STATUS_PREFAILED));assert(!Dem_GetEventUdsStatus(3,&status) && (status&0x0D)==0x0D);
 size=sizeof(data);assert(!Dem_GetSnapshotDataByDTC(3,DEM_DTC_FORMAT_UDS,DEM_DTC_ORIGIN_PRIMARY_MEMORY,1,data,&size) && size==6);
 size=sizeof(data);assert(!Dem_GetExtendedDataRecordByDTC(3,DEM_DTC_FORMAT_UDS,DEM_DTC_ORIGIN_PRIMARY_MEMORY,1,data,&size) && size==3 && data[0]==1);
 assert(!Dem_SetDTCFilter(0,1,DEM_DTC_FORMAT_UDS,DEM_DTC_ORIGIN_PRIMARY_MEMORY,FALSE,0,FALSE));assert(!Dem_GetNumberOfFilteredDTC(0,&count) && count==1);
 assert(!Dem_DisableDTCSetting(0));assert(!Dem_SetEventStatus(3,DEM_EVENT_STATUS_PASSED));assert(Dem_IsLegacyFaultActive(0));assert(!Dem_EnableDTCSetting(0));
 assert(!Dem_SetOperationCycleState(0,DEM_CYCLE_STATE_END));
 for(unsigned i=0;i<40;++i){assert(!Dem_SetOperationCycleState(0,DEM_CYCLE_STATE_START));assert(!Dem_SetEventStatus(3,DEM_EVENT_STATUS_PASSED));assert(!Dem_SetOperationCycleState(0,DEM_CYCLE_STATE_END));}
 assert(!Dem_GetEventUdsStatus(3,&status) && !(status&0x0D));
 size=sizeof(data);assert(Dem_GetSnapshotDataByDTC(3,DEM_DTC_FORMAT_UDS,DEM_DTC_ORIGIN_PRIMARY_MEMORY,1,data,&size)==E_NOT_OK);
 size=sizeof(data);assert(Dem_GetExtendedDataRecordByDTC(3,DEM_DTC_FORMAT_UDS,DEM_DTC_ORIGIN_PRIMARY_MEMORY,1,data,&size)==E_NOT_OK);
 storage();assert(Dem_IsLegacyFaultActive(0));
 /* A failed persistent clear restores the previous event memory. */
 assert(!Dem_SelectDTC(0,0xFFFFFF,DEM_DTC_FORMAT_UDS,DEM_DTC_ORIGIN_PRIMARY_MEMORY));failProgram=1;
 assert(Dem_ClearDTC(0)==DEM_PENDING);drain();Dem_MainFunction();assert(Dem_GetClearDTCStatus()==DEM_CLEAR_RESULT_FAILED && Dem_IsLegacyFaultActive(0));failProgram=0;
 puts("PASS DEM: debouncing, confirmation, operation cycles/aging, snapshots, extended data, filtering, DTC setting and persistent-clear failure rollback");
}
static void test_diagnostics(void){
 network();storage();uint8_t r[8]={0x10,3};request(r,2);assert(sent[2]==0x50 && Dcm_DiagnosticSession==EXTENDED);ticks(2);
 r[0]=0x3E;r[1]=0x80;unsigned before=sends;request(r,2);ticks(2);assert(sends==before);
 r[0]=0x22;r[1]=0xF1;r[2]=0xA0;request(r,3);assert(sent[1]==0x10);tick();uint8_t fc[8]={0x41,0x30,0,0};receive(0x710,fc,8);ticks(5);assert(!CanTp_IsBusy());
 r[0]=0x22;r[1]=0;r[2]=0;request(r,3);nrc(0x31);ticks(2);
 r[0]=0x19;r[1]=1;r[2]=0xFF;request(r,3);assert(sent[2]==0x59 && sent[3]==1);ticks(2);
 r[0]=0x28;r[1]=1;r[2]=3;request(r,3);assert(sent[2]==0x68);ticks(2);
 r[0]=0x19;r[1]=4;r[2]=0;r[3]=0;r[4]=3;r[5]=1;request(r,6);assert(sent[1]==0x10);tick();receive(0x710,fc,8);ticks(6);
 r[0]=0x14;r[1]=r[2]=r[3]=0xFF;request(r,4);ticks(100);assert(sent[2]==0x54 && !Dem_IsLegacyFaultActive(0));ticks(2);storage();assert(!Dem_IsLegacyFaultActive(0));
 r[0]=0x10;r[1]=3;request(r,2);ticks(2);
 /* Hold NvM processing to prove P2 NRC78 and P2* scheduling. */
 Dem_ReceiveFault(DTC_VIBRATION_SENSOR_MALFUNCTION);
 r[0]=0x31;r[1]=1;r[2]=0xF0;r[3]=1;request(r,4);
 for(unsigned i=0;i<11;++i){Can_TxConfirmation(1);Dcm_MainFunction();}
 assert(sent[2]==0x7F && sent[3]==0x31 && sent[4]==0x78);
 before=sends;for(unsigned i=0;i<1002;++i){Can_TxConfirmation(1);Dcm_MainFunction();}
 assert(sends>before && sent[2]==0x7F && sent[4]==0x78);
 ticks(100);assert(sent[2]==0x71);ticks(2);
 r[0]=0x10;r[1]=2;request(r,2);autoConfirm=0;ticks(100);assert(!resets);Can_TxConfirmation(1);assert(resets==1);
 network();r[0]=0x10;r[1]=3;request(r,2);ticks(1002);assert(Dcm_DiagnosticSession==DEFAULT);
 puts("PASS diagnostics: sessions/S3, TesterPresent, multi-frame DID/snapshot, NRCs, DTC/ClearDTC persistence, communication control, asynchronous RID/P2/P2* and reset after TX");
}
static void test_long_erase(void){
 network();storage();
 /* Consume every unused slot without damaging committed records. */
 for(unsigned bank=0;bank<2;++bank){uint32_t base=(bank?FLS_BANK1:FLS_BANK0)-0x08000000,size=bank?FLS_SIZE1:FLS_SIZE0;
  for(uint32_t at=0;at<size;at+=MEMIF_IMAGE_SIZE){unsigned empty=1;
   for(unsigned i=0;i<MEMIF_IMAGE_SIZE;++i)if(flash[base+at+i]!=255){empty=0;break;}
   if(empty)memset(flash+base+at,0xA5,MEMIF_IMAGE_SIZE);
  }
 }
 uint8_t r[4]={0x10,3};request(r,2);ticks(2);Dem_ReceiveFault(DTC_PERIPHERAL_ERROR);
 unsigned before=eraseCount;r[0]=0x31;r[1]=1;r[2]=0xF0;r[3]=1;request(r,4);assert(eraseCount==before);
 autoConfirm=0;tick();assert(sent[2]==0x7F && sent[4]==0x78 && eraseCount==before);
 tick();assert(eraseCount==before);Can_TxConfirmation(1);tick();assert(eraseCount==before+1);
 autoConfirm=1;ticks(100);assert(sent[2]==0x71);
 puts("PASS long erase: active diagnostic journal rotation waits for physical NRC78 confirmation");
}
int main(void){test_memory();test_dem();test_network();test_diagnostics();test_long_erase();return 0;}
