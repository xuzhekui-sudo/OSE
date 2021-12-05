/*******************************************************************************/
/* Filename      : ose_sema.c                                                  */
/* Description   : 信号量                                                       */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#include <ose_all.h>
#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 7)

Ose_semaphore ose_semaphore[OSE_MAX_SEMAPHORES];

/*对全局信号量资源ose_semaphore的访问，使用该互斥量*/
Ose_mutex_id g_ose_sema_lock = OSE_UNAVAILABLE_ID;

/*****************************************************************************
* Function  : ose_init_sema
* Purpose   : 信号量模块的初始化
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       N/A                 N/A
*
* Return:
*   OSE_SUCCESS :初始化成功
*   OSE_FAILURE :初始化失败
* Note:
*******************************************************************************/
Ose_status ose_init_sema()
{
    UINT32 index;

/*初始化为空闲资源*/
#ifdef LINUX_SWITCH
    for(index = 0; index < OSE_MAX_SEMAPHORES; index++)
    {
        ose_semaphore[index].sem      = NULL;
        ose_semaphore[index].name     = NULL;
        ose_semaphore[index].value    = 0;
    }
#endif
    /*创建一个公共互斥量，OSE用于对ose_semaphore的保护*/
    /*互斥量的初始化和信号量初始化，是一个函数顺序调用*/
    return ose_create_init_mutex((Ose_sema_name)"OseSemaProt", &g_ose_sema_lock, OSE_TRUE);
}
/*****************************************************************************
* Function  : ose_create_sema
* Purpose   : 创建信号量
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   name                Ose_sema_name           In          信号量名称
*   flag                Bool                    In          信号量的初时状态
*   count               Ose_sema_count          In          信号量的最大计数
* Return:
*   OSE_UNAVAILABLE_ID  :失败
*   其他                :成功
* Note:
*******************************************************************************/
Ose_sema_id ose_create_sema(Ose_sema_name name, Ose_sema_count count, Bool flag)
{
    UINT32            index;
    Ose_status        result = OSE_FAILURE;
    Ose_sema_name     name_ptr;

    /*检查最大计数*/
    if(((Ose_sema_count)0) == count || count > SEM_VALUE_MAX)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_create_sema]: create sema count error !!!");
        return OSE_UNAVAILABLE_ID;
    }

    /*进入互斥区，为保护ose_semaphore的访问*/
    ose_obtain_mutex(g_ose_sema_lock, OSE_WAIT_FOREVER);

#ifdef LINUX_SWITCH
    /*查找空闲信号量，并创建*/
    for(index = 0; index < OSE_MAX_SEMAPHORES; index++)
    {
        /*找到空闲资源*/
        if(ose_semaphore[index].sem == NULL)
        {
            /*生成信号量名字*/
            if(name == NULL)
            {
                name_ptr = OSE_SEMA_NAME;
            }
            else
            {
                name_ptr = name;
            }
            if(flag)
            {
                /*创建初始为最大值的信号量*/
                ose_semaphore[index].sem = sem_open(name_ptr, O_CREAT, 0644, count);
            }
            else
            {
                /*创建初始为最小值的信号量*/
                ose_semaphore[index].sem = sem_open(name_ptr, O_CREAT, 0644, 0);
            }
            /*判断是否创建成功*/
            if(ose_semaphore[index].sem != SEM_FAILED)
            {
                result                     = OSE_SUCCESS;
                ose_semaphore[index].name  = name_ptr;
                if(flag)
                {
                    ose_semaphore[index].value = count;
                }
                else
                {
                    ose_semaphore[index].value = 0;
                }
            }
            else
            {
                OSE_ERROR(name_ptr);
                sem_unlink(name_ptr);
            }
            /*退出循环*/
            break;
        }
    }
#endif
    /*退出临界区*/
    ose_release_mutex(g_ose_sema_lock);

    if(OSE_SUCCESS == result)
    {
        ose_trace(OSE_TRACE_INFO,"create sema %s success id:%d.",name,index);
        return index;
    }
    else
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_create_sema]: create sema %s failure !!!",name);
        return OSE_UNAVAILABLE_ID;
    }
}
/*****************************************************************************
* Function  : ose_obtain_sema
* Purpose   : 获取信号量(get)
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       sid             Ose_sema_id             In          要获取的信号量id
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
*   OSE_WAIT_TIMEOUT    超时后，还没获取到信号量
* Note:
*******************************************************************************/
Ose_status ose_obtain_sema(Ose_sema_id sid, Ose_timeout timeout)
{
    SINT32 ret_status;
#ifdef LINUX_SWITCH
    struct timespec ts;
#endif
    /*检查信号量id*/
    if(sid >= OSE_MAX_SEMAPHORES)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_obtain_sema]: obtain sid：%d !!!",sid);
        return OSE_FAILURE;
    }

    /*进入互斥区，为保护ose_semaphore的访问*/
    /*ose_obtain_mutex(g_ose_sema_lock, OSE_WAIT_FOREVER);*/

    /*判断信号量是否创建*/
    if(OSE_FALSE == ose_is_sema_created(sid))
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_obtain_sema]: obtain sid is sema created !!!");
        return OSE_FAILURE;
    }
/*获取信号量*/
#ifdef LINUX_SWITCH
    if(timeout == OSE_NO_WAIT)
    {
        ret_status = sem_trywait(ose_semaphore[sid].sem);
    }
    else if(timeout == OSE_WAIT_FOREVER)
    {
        ret_status = sem_wait(ose_semaphore[sid].sem);
    }
    else
    {
        if(clock_gettime(CLOCK_REALTIME, &ts) == -1)
        {
            ose_trace(OSE_TRACE_ERROR,"[ose_obtain_sema]: clock_gettime error !!!");
        }
        ts.tv_sec += timeout;
        ret_status = sem_timedwait(ose_semaphore[sid].sem,&ts);
    }
    
    if(ret_status == 0)
    {
        return OSE_SUCCESS;
    }
    else
    {
        OSE_ERROR("ose_release_sema");
        return OSE_FAILURE;
    }
#endif
}
/*****************************************************************************
* Function  : ose_release_sema
* Purpose   : 释放信号量(put)
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       sid             Ose_sema_id             In          要释放的信号量id
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
* Note:
*******************************************************************************/
Ose_status ose_release_sema(Ose_sema_id sid)
{
    SINT32 ret_status;
    /*检查互斥量id*/
    if(sid >= OSE_MAX_SEMAPHORES)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_release_sema]: release sid：%d !!!",sid);
        return OSE_FAILURE;
    }
    /*判断信号量是否创建*/
    if(OSE_FALSE == ose_is_sema_created(sid))
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_release_sema]: release sid is sema created !!!");
        return OSE_FAILURE;
    }

#ifdef LINUX_SWITCH
    /*释放信号量*/
   ret_status = sem_post(ose_semaphore[sid].sem);
   if(-1 == ret_status)
   {
       OSE_ERROR("ose_release_sema");
   }
#endif
    return ret_status;
}
/*****************************************************************************
* Function  : ose_delete_sema
* Purpose   : 删除信号量
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       sid             Ose_sema_id             In          要删除的信号量id
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
* Note:     只要id不非法，信号量创建；那么该函数不会返回OSE_FAILURE
    其他异常，OSE阻塞任务，不返回。
    删除信号量带来的临界问题，需要上层考虑。
*******************************************************************************/
Ose_status ose_delete_sema(Ose_sema_id sid)
{
    SINT32 ret_status;
    /*检查互斥量id*/
    if(sid >= OSE_MAX_SEMAPHORES)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_delete_sema]: sid：%d !!!",sid);
        return OSE_FAILURE;
    }
    /*判断信号量是否创建*/
    if(OSE_FALSE == ose_is_sema_created(sid))
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_delete_sema]: sid is sema created !!!");
        return OSE_FAILURE;
    }

#ifdef LINUX_SWITCH
    ret_status = sem_close(ose_semaphore[sid].sem);
    if(ret_status == -1)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_delete_sema]: sem close %d !!!",sid);
        return OSE_FAILURE;
    }
    ret_status = sem_unlink(ose_semaphore[sid].name);
    if(ret_status == -1)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_delete_sema]: sem unlink %d !!!",sid);
        return OSE_FAILURE;
    }
    ose_semaphore[sid].sem    = NULL;
    ose_semaphore[sid].name   = NULL;
    ose_trace(OSE_TRACE_INFO,"delete sem id %d is %d.",sid,ret_status);
#endif
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_is_sema_created
* Purpose   : 查询某信号量是否创建
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       sid                 Ose_sema_id         In          信号量id
*
* Return:
*   OSE_TRUE    :创建
*   OSE_FALSE   :没创建
* Note:
*******************************************************************************/
Bool ose_is_sema_created(Ose_sema_id sid)
{
#ifdef LINUX_SWITCH
    if(ose_semaphore[sid].sem != NULL)
    {
        return OSE_TRUE;
    }
#endif
    return OSE_FALSE;
}
/*****************************************************************************
* Function  : ose_sema_delete_all
* Purpose   : 删除所有已使用信号量
* Relation  :
*
* Input Parameters:
* Return:
* Note:
*******************************************************************************/
Ose_status ose_sema_delete_all()
{
    Ose_sema_id index;
    Ose_status  ret;

#ifdef LINUX_SWITCH
    for(index = 0; index < OSE_MAX_SEMAPHORES; index++)
    {
        if(ose_semaphore[index].sem != NULL)
        {
            ret = ose_delete_sema(index);
            if(ret != OSE_SUCCESS)
            {
                return OSE_FAILURE;
            }
        }
    }
#endif
    return OSE_SUCCESS;
}