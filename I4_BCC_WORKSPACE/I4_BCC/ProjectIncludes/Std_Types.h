/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Standard Types	     		 *
******************************************/
#ifndef Std_Types_H
#define Std_Types_H
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "stdint.h"
#include "string.h"
/*****************************************
		END OF INCLUDE PATHS		     *
******************************************/
/*****************************************
*		DEFINES					 		 *
******************************************/
/* Standard define */
#define E_NOT_OK 		1
/* Standard define */
#define E_OK 			0
/* Standard define */
#define NULL_PTR 		0
/* Standard define */
#define STD_HIGH		0x01
/* Standard define */
#define STD_LOW			0x00
/* Standard define */
#define EXTERN 			extern
/* Standard define */
#define STATIC			static
/* Standard define */
#define FLOAT			float
/* Standard define */
#define VOID			void
/*****************************************
* 		END OF DEFINES					 *
******************************************/
/*****************************************
*		VARIABLES					 	 *
******************************************/
/* Standard data type */
typedef uint8_t uint8;
/* Standard data type */
typedef uint16_t uint16;
/* Standard data type */
typedef uint32_t uint32;
/* Standard data type */
typedef uint8 StdReturnType;
typedef uint8 Std_ReturnType;
typedef int8_t sint8;
typedef int16_t sint16;
typedef uint8 boolean;
#define TRUE 1U
#define FALSE 0U
/*****************************************
*		END OF VARIABLES				 *
******************************************/
#endif /* Std_Types_H */
