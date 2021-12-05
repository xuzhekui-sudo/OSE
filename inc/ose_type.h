/*******************************************************************************/
/* Filename      : ose_type.h                                                  */
/* Description   : this file is to define all basic types and some common types*/
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_TYPE_H
#define OSE_TYPE_H

typedef char                 CHAR;
typedef signed char          SINT8;
typedef signed short int     SINT16;
typedef signed int           SINT32;
typedef unsigned char        UINT8;
typedef unsigned short int   UINT16;
typedef unsigned int         UINT32;
typedef unsigned long long   UINT64;
typedef void                 VOID;
typedef float                FLOAT32;
typedef double               FLOAT64;
typedef int                  Bool;

#define TRUE                 1
#define FALSE                0
/*enum type redefinition*/
#define ENUM                 enum
/*union type redefinition*/
#define UNION                union
/*const qualify keyword*/
#define CONST                const

#endif /*OSE_TYPE_H*/
