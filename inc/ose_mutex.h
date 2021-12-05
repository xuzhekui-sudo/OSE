/*******************************************************************************/
/* Filename      : ose_mutex.h                                                 */
/* Description   : 互斥�?                                                       */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_MUTEX_H
#define OSE_MUTEX_H

#define OSE_MUTEX_NAME "OseMutex"

/*互斥量控制字�?*/
typedef struct Ose_mutex_st
{
#ifdef LINUX_SWITCH
    Ose_mutex_name     name;
    pthread_mutex_t    mutex;
    Bool               is_used;
#endif
} Ose_mutex;

#ifdef __cplusplus
extern "C" {
#endif
Ose_status ose_init_mutex(void);
Ose_status ose_create_init_mutex(Ose_mutex_name name, Ose_mutex_id* mutex_ptr, Bool flag);
Bool ose_is_mutex_created(Ose_mutex_id id);
Ose_status ose_mutex_delete_all(void);
#ifdef __cplusplus
}
#endif

#endif /*OSE_MUTEX_H*/