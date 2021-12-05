/*******************************************************************************/
/* Filename      : ose_task.h                                                  */
/* Description   : 系统任务头文件                                                */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_TASK_H
#define OSE_TASK_H


/*任务的控制块*/
typedef struct Ose_task_spec_tbl_st
{
    pthread_t         thread_id;
    char const*       task_name;
    pthread_attr_t    task_attr;
    UINT16            error_code;
    UINT32            is_used;
} Ose_task_spec_tbl;

#ifdef __cplusplus
extern "C" {
#endif
Ose_status ose_init_task(void);
Ose_status ose_task_delete_all(void);
void*      ose_task_entry(void*);
void       ose_task_entry_init(Ose_task_param);
void       ose_task_entry_main(Ose_task_param);
#ifdef __cplusplus
}
#endif

#endif /*OSE_TASK_H*/