/*******************************************************************************/
/* Filename      : ose_timer.h                                                 */
/* Description   : 定时器管�?                                                  */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_TIMER_H
#define OSE_TIMER_H


#ifdef __cplusplus
extern "C" {
#endif
Ose_status ose_init_timer(void);
Ose_status ose_timer_delete_all(void);
#ifdef __cplusplus
}
#endif

#endif /*OSE_TIMER_H*/