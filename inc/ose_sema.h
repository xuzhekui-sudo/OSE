/*******************************************************************************/
/* Filename      : ose_sema.h                                                  */
/* Description   : 信号量                                                       */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-30             Original                        */
/*******************************************************************************/

#ifndef OSE_SEMA_H
#define OSE_SEMA_H

#define OSE_SEMA_NAME "Semaphore"

typedef struct Ose_semaphore_st
{
#ifdef LINUX_SWITCH
    sem_t*          sem;
    Ose_sema_name   name;
    SINT32          value;
#endif
} Ose_semaphore;

#ifdef __cplusplus
extern "C" {
#endif
Ose_status ose_init_sema(void);
Bool       ose_is_sema_created(Ose_sema_id);
Ose_status ose_sema_delete_all(void);
#ifdef __cplusplus
}
#endif

#endif /*OSE_SEMA_H*/