/*******************************************************************************/
/* Filename      : ose_utl.h                                                   */
/* Description   : 辅助函数                                                     */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_UTL_H
#define OSE_UTL_H

#ifdef __cplusplus
extern "C" {
#endif

UINT32 ose_get_system_time(void);
UINT16 ose_btol_u16(UINT16);
UINT32 ose_btol_u32(UINT32);
#ifdef __cplusplus
}
#endif

#endif /*OSE_UTL_H*/