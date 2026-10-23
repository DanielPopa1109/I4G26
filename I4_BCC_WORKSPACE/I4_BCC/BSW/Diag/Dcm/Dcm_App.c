#include "Dcm_App.h"
#include "Dem.h"
#include "Com.h"
#include "NvM.h"
#include "g26_layout.h"
#include <string.h>
#ifndef G26_HOST_TEST
#include "main.h"
#endif
Dcm_JobResultType Dcm_AppPersist(Dcm_OpStatusType status){
 if(status==DCM_CANCEL)return DCM_JOB_FAILED; /* Let an already issued Flash job reach a safe commit boundary. */
 if(status==DCM_INITIAL){
  if(!NvM_Ready || !NvM_IsIdle())return DCM_JOB_FAILED;
  NvM_WriteAll();return DCM_JOB_PENDING;
 }
 if(!NvM_IsIdle())return DCM_JOB_PENDING;
 return NvM_LastWriteResult?DCM_JOB_FAILED:DCM_JOB_OK;
}
uint16 Dcm_AppReadDid(uint16 did,uint8 *out){
 const char *p=0;
 if(did==0xF1A0)p="G26-BODY";
 else if(did==0xF187)p="G26A2";
 if(!p)return 0;
 uint16 n=(uint16)strlen(p);memcpy(out,p,n);return n;
}
void Dcm_AppCommunication(uint8 control){Com_IpduGroupControl(0,control==0);Com_IpduGroupControl(1,control==0);}
void Dcm_AppReset(uint8 session){
#ifndef G26_HOST_TEST
 if(session==2){
  volatile uint32_t *box=(volatile uint32_t *)FBL_BOOT_MAILBOX;
  box[1]=2;box[2]=~2U;box[0]=G26_BOOT_MAGIC;__DSB();
 }
 NVIC_SystemReset();
#else
 extern void Dcm_TestReset(uint8);
 Dcm_TestReset(session);
#endif
}
