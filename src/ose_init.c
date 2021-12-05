/*******************************************************************************/
/* Filename      : ose_init.c                                                  */
/* Description   : 系统初始化                                                  */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#include <ose_all.h>
#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 3)

extern Ose_task_id       g_ose_start_sequence[];
extern Ose_semaphore     ose_semaphore[OSE_MAX_SEMAPHORES];
/*任务描述表，在OSE初始化时，根据任务配置表初始化*/
Ose_task_desc_tbl g_ose_task_desc_tbl[OSE_MAX_TASKS] = {
    { 0, 0, 0, 0, OSE_TRUE, NULL, { NULL, NULL, NULL, NULL }, NULL, 0, 0 }
};
/*用于初始化时，标准任务和初始化任务的通信*/
Ose_sema_id         g_ose_init_sema  = OSE_UNAVAILABLE_ID;
Ose_mutex_id        g_ose_init_mutex = OSE_UNAVAILABLE_ID;
/*0为未完成初始化，1为完成*/
UINT8               g_ose_task_init_flag[OSE_MAX_TASKS];
Ose_sema_id         ose_exit_sema = OSE_UNAVAILABLE_ID;

/*****************************************************************************
* Function  : ose_kernel_init
* Purpose   : OSE初始化
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return:
*   OSE_SUCCESS 初始化成功
*   OSE_FAILURE 初始化失败
* Note:
*******************************************************************************/
Ose_status ose_kernel_init(void)
{
    Ose_status ret;
    /*OSE系统资源初始化*/
    ret = ose_init_system_res();
    if(ret != OSE_SUCCESS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_kernel_init]: system res failure !!!");
        return OSE_FAILURE;
    }

    /*OSE初始化上层任务的执行环境*/
    ret = ose_init_user_res();
    if(ret != OSE_SUCCESS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_kernel_init]: user res failure !!!");
        return OSE_FAILURE;
    }
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_kernel_exit
* Purpose   : OSE退出
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return:
*   OSE_SUCCESS OSE退出成功
*   OSE_FAILURE OSE退出失败
* Note:
*******************************************************************************/
Ose_status ose_kernel_exit()
{
    /*清除本模块使用的互斥量和信号量资源*/
    g_ose_init_sema  = OSE_UNAVAILABLE_ID;
    g_ose_init_mutex = OSE_UNAVAILABLE_ID;

    /*删除所有任务*/
    if(ose_task_delete_all() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }

    /*删除所有任务间邮箱*/
    if(ose_mb_delete_all() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }

    /*删除所有已用信号量*/
    if(ose_sema_delete_all() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }

    /*删除所有已使用互斥量*/
    if(ose_mutex_delete_all() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*删除内存池，释放内存给OS*/
    if(ose_buf_delete_all() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*删除已使用的定时器*/
    if(ose_timer_delete_all() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_init_system_res
* Purpose   : 初始化OSE的系统资源
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return: OSE的状态
* Note:
*******************************************************************************/
Ose_status ose_init_system_res()
{
    /*初始化任务管理*/
    if(ose_init_task() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*初始化互斥量管理模块*/
    if(ose_init_mutex() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*初始化OSE的内存管理模块*/
    if(ose_init_pools() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*初始化信号量管理模块*/
    if(ose_init_sema() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*初始化任务间邮箱*/
    if(ose_init_mb() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*初始化定时器*/
    if(ose_init_timer() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_init_user_res
* Purpose   : OSE初始化上层任务的执行环境
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return: OSE的状态
* Note:
*******************************************************************************/
Ose_status ose_init_user_res()
{
    UINT32          loop;
    UINT32          mb_size;
    Ose_task_id     task_id;
    Ose_status      ret_status;
    Bool            wait_flag = OSE_FALSE;

    /*创建一个初始count为0的信号量*/
    g_ose_init_sema = ose_create_sema((Ose_sema_name) "OseInitSema", 1, OSE_FALSE);
    if(g_ose_init_sema == OSE_UNAVAILABLE_ID)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_init_user_res]: sema unavailable id !!!");
        return OSE_FAILURE;
    }

    /*创建一个互斥量，保护g_ose_task_init_flag*/
    g_ose_init_mutex = ose_create_mutex((Ose_mutex_name) "OseInitMutex", OSE_TRUE);
    if(g_ose_init_mutex == OSE_UNAVAILABLE_ID)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_init_user_res]: mutex unavailable id !!!");
        return OSE_FAILURE;
    }
    ose_create_task_signal();
    /*所有任务初始化未已完成，需要创建的标准任务默认未完成*/
    memset((void*)g_ose_task_init_flag, 1, sizeof(g_ose_task_init_flag));

    /*创建标准任务的任务间邮箱*/
    for(loop = 0; loop <= OSE_MAX_TASKS; loop++)
    {
        if(g_ose_start_sequence[loop] >= OSE_MAX_TASKS)
        {
            break;
        }

        /*得到需要启动的任务id*/
        task_id                       = g_ose_start_sequence[loop];
        g_ose_task_init_flag[task_id] = 0;
        wait_flag                     = OSE_TRUE;

        /*创建任务间邮箱*/
        mb_size = g_ose_task_desc_tbl[task_id].task_mb_size;
        if(mb_size == (UINT32)-1)
        {
            /*OSE计算邮箱大小*/
            ret_status = ose_create_mb(task_id, ((1 + g_ose_task_desc_tbl[task_id].task_pri / 10) * 20));
        }
        else if(mb_size == 0)
        {
            /*不创建任务间邮箱*/
            ret_status = OSE_SUCCESS;
        }
        else
        {
            /*由入参值创建邮箱大小*/
            ret_status = ose_create_mb(task_id, mb_size);
        }

        if(OSE_SUCCESS != ret_status)
        {
            ose_trace(OSE_TRACE_ERROR,"[ose_init_user_res]: create mb fail !!!");
            return OSE_FAILURE;
        }
    }

    /*创建标准任务并逐一等待初始化完毕*/
    for(loop = 0; loop <= OSE_MAX_TASKS; loop++)
    {
        if(g_ose_start_sequence[loop] >= OSE_MAX_TASKS)
        {
            break;
        }

        /*得到需要启动的任务id*/
        task_id = g_ose_start_sequence[loop];

        //转换接收邮箱
        /*创建任务但不创建任务间邮箱*/
        if(ose_create_task(task_id,
                           (Ose_task_entry)ose_task_entry,
                           g_ose_task_desc_tbl[task_id].task_pri,
                           g_ose_task_desc_tbl[task_id].stack_size,
                           (Ose_task_param)task_id,
                           0,
                           g_ose_task_desc_tbl[task_id].task_name) != OSE_SUCCESS)
        {
            ose_trace(OSE_TRACE_ERROR,"[ose_init_user_res]: create task fail !!!");
            return OSE_FAILURE;
        }
    }
    if(wait_flag == OSE_TRUE)
    {
        /*等待上面创建的任务都初始化完成了再继续向后执行*/
        ose_obtain_sema(g_ose_init_sema, OSE_WAIT_FOREVER);
    }
    /*删除该互斥量和信号量资源*/
    ose_delete_sema(g_ose_init_sema);
    ose_delete_mutex(g_ose_init_mutex);
    /*执行完毕，返回成功*/
    return OSE_SUCCESS;
}