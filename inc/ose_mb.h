/*******************************************************************************/
/* Filename      : ose_mb.h                                                   */
/* Description   : ����                                                        */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_MB_H
#define OSE_MB_H

/*������ź����ͻ�������*/
typedef struct Ose_ext_mb_name_st
{
    char           sema_lock_wr_name[30];
    char           sema_lock_rd_name[30];
    char           mutex_name[30];
} Ose_ext_mb_name;
/*���������Ŀ����ֶ�*/
typedef struct Ose_ext_mb_st
{
    Ose_fixed_queue* blist_ptr;
    Ose_sema_id      sema_lock_wr;
    Ose_sema_id      sema_lock_rd;
    Ose_mutex_id     mutex;
    Ose_ext_mb_name  name;
} Ose_ext_mb;

#ifdef __cplusplus
extern "C" {
#endif
Ose_status ose_init_mb(void);
Ose_status ose_create_mb(Ose_mb_id, UINT32);
Ose_status ose_send_to_mb_head(Ose_mb_id, Ose_fsm_message*, Ose_timeout);
Ose_status ose_send_to_mb_tail(Ose_mb_id, Ose_fsm_message*, Ose_timeout);
Bool       ose_is_mb_created(Ose_mb_id);
UINT32     ose_get_spare_mb(Ose_mb_id); //���������ж�
Ose_status ose_delete_mb(Ose_mb_id);
Ose_status ose_mb_delete_all(void);

#ifdef __cplusplus
}
#endif

#endif /*OSE_MB_H*/