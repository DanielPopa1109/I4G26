#include "Dcm_Cfg.h"
/* No security algorithm, coding sessions or private OEM services are configured. */
const Dcm_ServiceConfig Dcm_Services[DCM_SERVICE_COUNT]={
 {0x10,2,2,7},{0x11,2,2,7},{0x14,4,4,7},{0x19,2,6,7},
 {0x22,3,255,7},{0x28,3,3,2},{0x85,2,2,2},{0x3E,2,2,7},{0x31,4,256,2}
};
