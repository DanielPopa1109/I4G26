#ifndef DCM_APP_H
#define DCM_APP_H
#include "Dcm.h"
Dcm_JobResultType Dcm_AppPersist(Dcm_OpStatusType status);
uint16 Dcm_AppReadDid(uint16 did,uint8 *out);

void Dcm_AppCommunication(uint8 control);
void Dcm_AppReset(uint8 session);
#endif
