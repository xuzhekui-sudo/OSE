/*******************************************************************************/
/* Filename      : ose_task.c                                                  */
/* Description   : 系统任务                                                     */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#include <ose_all.h>
#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 8)

extern Ose_task_desc_tbl g_ose_task_desc_tbl[OSE_MAX_TASKS];
extern Ose_task_desc_tbl g_ose_task_config[];
extern Ose_ext_mb        ose_mailbox[OSE_MAX_MAILBOXS];
extern Ose_mutex_id      g_ose_init_mutex;
extern Ose_sema_id       g_ose_init_sema;
extern UINT8             g_ose_task_init_flag[OSE_MAX_TASKS];
extern Ose_task_id       g_ose_start_sequence[];
extern Ose_mutex         ose_mutex[OSE_MAX_MUTEXES];
/*所有任务的控制块*/
Ose_task_spec_tbl        g_ose_task_spec_tbl[OSE_MAX_TASKS];
Ose_thread_cond          g_ose_create_cond;
Ose_mutex_id             g_ose_create_task_mutex = OSE_UNAVAILABLE_ID;
/*****************************************************************************
* Function  : ose_init_task
* Purpose   : 任务管理初始化
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       N/A                 N/A
*
* Return:
*   OSE_SUCCESS 初始化成功
*   OSE_FAILURE 初始化失败
* Note:
*******************************************************************************/
Ose_status ose_init_task(void)
{
    UINT8 index;
    /*初始化任务控制字段*/
    for(index = 0; index < OSE_MAX_TASKS; index++)
    {
        g_ose_task_spec_tbl[index].is_used = OSE_FALSE;
    }

    /*配置要创建的标准任务*/
    for(index = 0; index < OSE_MAX_TASKS; index++)
    {
        if(g_ose_task_config[index].task_id >= OSE_MAX_TASKS)
        {
            break;
        }

        memcpy(&g_ose_task_desc_tbl[g_ose_task_config[index].task_id],
                   &g_ose_task_config[index],
                   sizeof(Ose_task_desc_tbl));
    }
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_is_task_created
* Purpose   : 查询某任务是否创建
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       task_id          Ose_task_id               In          任务编号
*
* Return:
*   OSE_TRUE 任务创建
*   OSE_FALSE 任务没创建
* Note:
*******************************************************************************/
Bool ose_is_task_created(Ose_task_id task_id)
{
    /*检查任务id*/
    if(task_id >= OSE_MAX_TASKS)
    {
        return OSE_FALSE;
    }

    return g_ose_task_spec_tbl[task_id].is_used;
}
/*****************************************************************************
* Function  : ose_create_task
* Purpose   : 创建任务
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       tid             UINT8                   In          任务编号
*       entry           Ose_task_entry          In          任务入口函数
*       param           UINT32                  In          入口函数的入参
*       pri             UINT8                   In          任务优先级
*       stack_size      UINT16                  In          任务栈大小
*       mb_size         UINT32                  In          任务间邮箱的大小
*
* Return:
*   OSE_SUCCESS 任务创建
*   OSE_FAILURE 任务没创建
* Note:     如果本任务的任务间邮箱没创建，创建任务间邮箱。
*******************************************************************************/
Ose_status ose_create_task(Ose_task_id     tid,
                           Ose_task_entry  entry,
                           Ose_task_pri    pri,
                           Ose_stack_size  stack_size,
                           Ose_task_param  param,
                           UINT32          mb_size,
                           const char*     pstTaskName)
{
    Ose_status ret_status;

    /*检查任务id*/
    if(tid >= OSE_MAX_TASKS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_create_task]: max task id %d !!!",tid);
        return OSE_FAILURE;
    }

    /*检查入口函数*/
    if(entry == NULL)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_create_task]: entry error !!!");
        return OSE_FAILURE;
    }

    /*检查任务是否已经创建*/
    if(g_ose_task_spec_tbl[tid].is_used == OSE_TRUE)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_create_task]: is_used error !!!");
        return OSE_FAILURE;
    }

    /*同一个tid不考虑两个任务调用的情况*/
    /*该任务控制字段置为创建*/
    g_ose_task_spec_tbl[tid].is_used = OSE_TRUE;

    /*判断当前任务是否创建了任务间邮箱*/
    if(ose_mailbox[tid].blist_ptr == NULL)
    {
        if(mb_size == (UINT32)-1)
        {
            /*OSE计算邮箱大小*/
            ret_status = ose_create_mb(tid, ((1 + pri / 10) * 20));
        }
        else if(mb_size == 0)
        {
            /*不创建任务间邮箱*/
            ret_status = OSE_SUCCESS;
        }
        else
        {
            /*由入参值创建邮箱大小*/
            ret_status = ose_create_mb(tid, mb_size);
        }

        /*对邮箱创建结果进行检查*/
        if(ret_status != OSE_SUCCESS)
        {
            ose_trace(OSE_TRACE_ERROR,"[ose_create_task]: ose_mailbox error !!!");
            return OSE_FAILURE;
        }

        /*保存任务间邮箱到任务描述表*/
        g_ose_task_desc_tbl[tid].task_ext_queue = tid;
    }

    /*保存任务名字*/
    OSE_ASSERT(pstTaskName != NULL);
    g_ose_task_spec_tbl[tid].task_name = pstTaskName;

/*创建任务*/
#ifdef LINUX_SWITCH
    memset(&(g_ose_task_spec_tbl[tid].task_attr),0,sizeof(pthread_attr_t));
    pthread_attr_init(&(g_ose_task_spec_tbl[tid].task_attr));
    ret_status = pthread_attr_setstacksize(&(g_ose_task_spec_tbl[tid].task_attr), stack_size);
    /*配置线程栈大小*/
    if(ret_status == -1)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_create_task]: set task stack size !!!");
        return OSE_FAILURE;
    }
    ret_status = pthread_create(&(g_ose_task_spec_tbl[tid].thread_id),
                         &(g_ose_task_spec_tbl[tid].task_attr),
                         entry,
                         (void*)&param);
    ose_task_timedwait(&g_ose_create_cond,g_ose_create_task_mutex);
    if(ret_status != 0)
    {
        /*系统创建任务失败*/
        OSE_ERROR("ose_create_task ERROR: ");
        return OSE_FAILURE;
    }
#endif
    ose_trace(OSE_TRACE_INFO,"create tsak is %s",g_ose_task_spec_tbl[tid].task_name);
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_task_join
* Purpose   : 删除任务
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return:
*   OSE_SUCCESS 任务删除成功
*   OSE_FAILURE 任务没删除失败
* Note:
*******************************************************************************/
Ose_status ose_task_join(void)
{
    UINT8 index;
    for(index = 0; index < OSE_MAX_TASKS; index++)
    {
        if(g_ose_start_sequence[index] >= OSE_MAX_TASKS)
        {
            break;
        }
        if(g_ose_task_spec_tbl[g_ose_start_sequence[index]].thread_id != 0)
        {
            pthread_join(g_ose_task_spec_tbl[g_ose_start_sequence[index]].thread_id,NULL);
            ose_trace(OSE_TRACE_INFO,"%s join",g_ose_task_spec_tbl[g_ose_start_sequence[index]].task_name);
        }
    }
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_create_task_signal
* Purpose   : 创建信号给任务
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return:
*   OSE_SUCCESS 任务发信号成功
*   OSE_FAILURE 任务发信号失败
* Note:
*******************************************************************************/
Ose_status ose_create_task_signal()
{
#ifdef LINUX_SWITCH
    if(0 != pthread_cond_init(&g_ose_create_cond, NULL))
    {
        OSE_ERROR("ERROR:g_ose_create_cond:");
    }
    /*创建一个互斥量，保护g_ose_create_task_mutex*/
    g_ose_create_task_mutex = ose_create_mutex((Ose_mutex_name) "OseCreateTaskMutex", OSE_TRUE);
    if(g_ose_create_task_mutex == OSE_UNAVAILABLE_ID)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_create_task_signal]: unavailable id !!!");
        return OSE_FAILURE;
    }
#endif
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_post_task_signal
* Purpose   : 发信号给任务
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return:
*   OSE_SUCCESS 任务发信号成功
*   OSE_FAILURE 任务发信号失败
* Note:
*******************************************************************************/
Ose_status ose_post_task_signal()
{
    return ose_task_signal(&g_ose_create_cond,g_ose_create_task_mutex);
}
/*****************************************************************************
* Function  : ose_task_signal
* Purpose   : 发信号给任务
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return:
*   OSE_SUCCESS 任务发信号成功
*   OSE_FAILURE 任务发信号失败
* Note:
*******************************************************************************/
Ose_status ose_task_signal(Ose_thread_cond* cond,Ose_mutex_id mutex_id)
{
    ose_obtain_mutex(mutex_id, OSE_WAIT_FOREVER);
    pthread_cond_signal(cond);
    ose_release_mutex(mutex_id);
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_task_timedwait
* Purpose   : 等待任务
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return:
*   OSE_SUCCESS 任务等待成功
*   OSE_FAILURE 任务等待失败
* Note:
*******************************************************************************/
Ose_status ose_task_timedwait(Ose_thread_cond* cond,Ose_mutex_id mutex_id)
{
    struct timeval  now;
    struct timespec outtime;

    ose_obtain_mutex(mutex_id, OSE_WAIT_FOREVER);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 1;
    outtime.tv_nsec = now.tv_usec * 1000;
    if(0 != pthread_cond_timedwait(cond, &ose_mutex[mutex_id].mutex, &outtime))
    {
        /*系统创建任务失败*/
        OSE_ERROR("ose_task_timedwait ERROR:");
    }
    ose_release_mutex(mutex_id);
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_delete_task
* Purpose   : 删除任务
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       tid                 Ose_task_id         In          任务编号
*
* Return:
*   OSE_SUCCESS 任务删除成功
*   OSE_FAILURE 任务没删除失败
* Note:
*******************************************************************************/
Ose_status ose_delete_task(Ose_task_id tid)
{
    SINT32 ret;
    /*检查任务id*/
    if(tid >= OSE_MAX_TASKS)
    {
        return OSE_FAILURE;
    }

    /*检查任务是否创建*/
    if(OSE_FALSE == ose_is_task_created(tid))
    {
        return OSE_FAILURE;
    }

    /*置任务控制字段的标志位*/
    g_ose_task_spec_tbl[tid].is_used = OSE_FALSE;

/*删除任务.*/
#ifdef LINUX_SWITCH
    /*不再使用线程属性，将其销毁*/
    ret = pthread_attr_destroy(&(g_ose_task_spec_tbl[tid].task_attr)); 
    if(ret != 0)
    {
        OSE_ERROR("ose_delete_task destroy ERROR: ");
        return OSE_FAILURE;
    }
    
    ret = pthread_cancel(g_ose_task_spec_tbl[tid].thread_id);
    if(ret != 0)
    {
        OSE_ERROR("ose_delete_task cancel ERROR: ");
        return OSE_FAILURE;
    }
    usleep(1000);
    /*如果创建了任务间邮箱，就删除*/
    if(ose_mailbox[tid].blist_ptr != NULL)
    {
        ose_delete_mb(tid);
    }
    ose_trace(OSE_TRACE_INFO,"delete task id %d",tid);
#endif
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_task_delete_all
* Purpose   : 删除所有任务
* Relation  :
*
* Input Parameters:
* Return:
* Note:
*******************************************************************************/
Ose_status ose_task_delete_all()
{
#ifdef LINUX_SWITCH
    UINT8 index;

    for(index = 0; index < OSE_MAX_TASKS + 1; index++)
    {
        if(g_ose_start_sequence[index] >= OSE_MAX_TASKS)
        {
            break;
        }
        if(ose_is_task_created(g_ose_start_sequence[index]) && g_ose_task_desc_tbl[g_ose_start_sequence[index]].task_func.task_delete != NULL)
        {
            (*g_ose_task_desc_tbl[g_ose_start_sequence[index]].task_func.task_delete)();
            ose_delete_task(g_ose_start_sequence[index]);
        }
    }
    pthread_cond_destroy(&g_ose_create_cond);
#endif
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_task_entry
* Purpose   : 标准任务入口
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       param               UINT32               In          任务编号
* Return: 无
* Note:
*******************************************************************************/
void* ose_task_entry(void* param_ptr)
{
    Ose_task_param    param  = 0;

    param = *(Ose_task_param*)param_ptr;
    /*执行任务初始化*/
    ose_task_entry_init(param);
#ifdef LINUX_SWITCH
    ose_post_task_signal();
#endif
    /*进入消息循环处理函数*/
    ose_task_entry_main(param);
    return NULL;
}
/*****************************************************************************
* Function  : ose_task_entry_init
* Purpose   : 标准任务初始化
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   param             Ose_task_param                In          任务编号
* Return: 无
* Note:
*******************************************************************************/
void ose_task_entry_init(Ose_task_param param)
{
    UINT8       tid;
    Ose_status  init_ret;
    UINT32      loop;
    Bool        init_was_finished;

    /*入参即为任务编号*/
    tid = (Ose_task_id)param;
    
    /*入参是对应标准任务的编号，小于OSE_MAX_TASKS*/
    OSE_ASSERT(tid < OSE_MAX_TASKS);

    /*执行标准任务的初始化函数*/
    if(g_ose_task_desc_tbl[tid].task_func.task_init == NULL)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_task_entry_init]: task_init is null error !!!");
        return;
    }
    init_ret = (*g_ose_task_desc_tbl[tid].task_func.task_init)();
    if(init_ret != OSE_SUCCESS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_task_entry_init]: task_init ret error !!!");
        return;
    }

    ose_obtain_mutex(g_ose_init_mutex, OSE_WAIT_FOREVER);

    /*更新当前任务的初始化完成标志*/
    g_ose_task_init_flag[tid] = 1;

    /*检查其他任务是否完成初始化*/
    init_was_finished = OSE_TRUE;
    for(loop = 0; loop < OSE_MAX_TASKS; loop++)
    {
        if(g_ose_task_init_flag[loop] == 0)
        {
            init_was_finished = OSE_FALSE;
            break;
        }
    }

    ose_release_mutex(g_ose_init_mutex);

    /*如果所有的标准任务都执行完毕初始化，唤醒初始化任务*/
    if(init_was_finished == OSE_TRUE)
    {
        ose_release_sema(g_ose_init_sema);
    }
}

/*****************************************************************************
* Function  : ose_task_entry_main
* Purpose   : 标准任务处理fsm消息的函数
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       param              UINT32                In          任务编号
* Return: 无
* Note:
*******************************************************************************/
void ose_task_entry_main(Ose_task_param param)
{
    Ose_fsm_message* fsm_msg_ptr = NULL;
    Ose_task_id      tid;

    /*入参即为任务编号*/
    tid = (Ose_task_id)param;

    OSE_ASSERT(g_ose_task_desc_tbl[tid].task_func.task_main != NULL);
    /*任务处于一个循环接收消息里*/
    while(1)
    {
        /*取出任务间邮箱里的消息*/
        fsm_msg_ptr = (Ose_fsm_message*)ose_get_from_mb(tid, OSE_WAIT_FOREVER);
        /*调用对应任务的主函数处理该消息*/
        (*g_ose_task_desc_tbl[tid].task_func.task_main)(fsm_msg_ptr);
    }
}
