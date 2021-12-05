/*******************************************************************************/
/* Filename      : ose_list.h                                                  */
/* Description   : 提供可变和固定队列功能                                      */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_LIST_H
#define OSE_LIST_H

#ifdef __cplusplus
extern "C" {
#endif
Ose_fixed_queue* ose_create_fixed_queue(UINT32);
Ose_status       ose_add_fixed_queue_tail(Ose_fixed_queue*, UINT32);
Ose_status       ose_add_fixed_queue_head(Ose_fixed_queue*, UINT32);
Ose_status       ose_get_fixed_queue_head(Ose_fixed_queue*, UINT32*);
void             ose_get_fixed_queue_spare(Ose_fixed_queue*, UINT32*, UINT32*);
Ose_status       ose_delete_fixed_queue(Ose_fixed_queue*);

#ifdef __cplusplus
}
#endif

#endif /*OSE_LIST_H*/