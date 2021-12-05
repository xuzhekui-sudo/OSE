/*******************************************************************************/
/* Filename      : ose_mutex.c                                                  */
/* Description   : 互斥量                                                       */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#include <ose_all.h>
#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 6)

/*OSE的互斥量资源*/
Ose_mutex ose_mutex[OSE_MAX_MUTEXES];

/*由于下面接口存在并发调用，涉及对ose_mutex的访问使用该互斥*/
Ose_mutex_id     g_ose_mutex_lock = OSE_UNAVAILABLE_ID;

/*****************************************************************************
* Function  : ose_init_mutex
* Purpose   : OSE互斥量管理模块的初始化
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
Ose_status ose_init_mutex()
{
    UINT32 index;

/*初始化控制字段*/
#ifdef LINUX_SWITCH
    for(index = 0; index < OSE_MAX_MUTEXES; index++)
    {
        ose_mutex[index].name      = NULL;
        ose_mutex[index].is_used   = OSE_FALSE;
        memset(&(ose_mutex[index].mutex),0x00,sizeof(pthread_mutex_t));
    }
#endif
    /*创建一个公共互斥量，OSE用于对ose_mutex的保护*/
    return ose_create_init_mutex((Ose_mutex_name)"OseMutexProt", &g_ose_mutex_lock, OSE_TRUE);
}
/*****************************************************************************
* Function  : ose_create_init_mutex
* Purpose   : OSE初始化完成之前，创建互斥量
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   name                Ose_mutex_name          In          互斥量名称
*   mutex_ptr           Ose_mutex_id*           Out         互斥量id存放地址
*   flag                Bool                    In          互斥量的初时状态
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
* Note:
*******************************************************************************/
Ose_status ose_create_init_mutex(Ose_mutex_name name, Ose_mutex_id* mutex_ptr, Bool flag)
{
    Ose_mutex_name   name_ptr;
    UINT32           index;

#ifdef LINUX_SWITCH
    for(index = 0; index < OSE_MAX_MUTEXES; index++)
    {
        if(ose_mutex[index].is_used == OSE_FALSE)
        {
            if(name == NULL)
            {
                name_ptr = OSE_MUTEX_NAME;
            }
            else
            {
                name_ptr = name;
            }

            if(pthread_mutex_init(&(ose_mutex[index].mutex), NULL) == -1)
            {
                OSE_ERROR("ose_create_init_mutex ERROR: ");
                return OSE_FAILURE;
            }
            /*该index对应的互斥资源分配出去*/
            ose_mutex[index].is_used = OSE_TRUE;
            ose_mutex[index].name    = name_ptr;
            /*申请的互斥量id*/
            *mutex_ptr = index;
            ose_trace(OSE_TRACE_INFO,"create mutex %s success id:%d.",name_ptr,index);
            return OSE_SUCCESS;
        }
    }
    return OSE_FAILURE;
#endif
}
/*****************************************************************************
* Function  : ose_create_mutex
* Purpose   : 创建互斥量
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   name                Ose_mutex_name          In          互斥量名称
*   flag                Bool                    In          互斥量的初时状态
*
* Return:
*   OSE_UNAVAILABLE_ID  :失败
*   其他                :成功
* Note:
*******************************************************************************/
Ose_mutex_id ose_create_mutex(Ose_mutex_name name, Bool flag)
{
    Ose_status   ret = OSE_FAILURE;
    Ose_mutex_id mutex_id;

    /*因该接口可能被多个任务同时调用，首先进行创建互斥*/
    ose_obtain_mutex(g_ose_mutex_lock, OSE_WAIT_FOREVER);

    /*根据flag创建互斥量*/
    switch(flag)
    {
        case OSE_TRUE:
        {
            ret = ose_create_init_mutex(name, &mutex_id, OSE_TRUE);
            break;
        }
        case OSE_FALSE:
        {
            ret = ose_create_init_mutex(name, &mutex_id, OSE_FALSE);
            break;
        }
        default:
        {
            ose_trace(OSE_TRACE_ERROR,"[ose_create_mutex]: create mutex default !!!");
        }
    }

    /*退出互斥区*/
    ose_release_mutex(g_ose_mutex_lock);

    if(OSE_SUCCESS == ret)
    {
        return mutex_id;
    }
    else
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_create_mutex]: create mutex fail !!!");
        return OSE_UNAVAILABLE_ID;
    }
}
/*****************************************************************************
* Function  : ose_obtain_mutex
* Purpose   : 获取互斥量(get)
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       id              Ose_mutex_id            In          要获取的互斥量id
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
*   OSE_WAIT_TIMEOUT    超时后，还没获取到互斥量
* Note:
*******************************************************************************/
Ose_status ose_obtain_mutex(Ose_mutex_id id, Ose_timeout timeout)
{
    SINT32 ret_lx;
    /*检查互斥量id*/
    if(id >= OSE_MAX_MUTEXES)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_obtain_mutex]: mutex id %d !!!",id);
        return OSE_FAILURE;
    }
    /*判断互斥量是否创建*/
    if(OSE_FALSE == ose_is_mutex_created(id))
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_obtain_mutex]: %d mutex is created !!!",id);
        return OSE_FAILURE;
    }
/*linux系统获取互斥量*/
#ifdef LINUX_SWITCH
    ret_lx = pthread_mutex_lock(&(ose_mutex[id].mutex));
    if(ret_lx == 0)
    {
        return OSE_SUCCESS;
    }
    else
    {
        OSE_ERROR("ose_obtain_mutex ERROR");
        return ret_lx;
    }
#endif
}
/*****************************************************************************
* Function  : ose_release_mutex
* Purpose   : 释放互斥量(put)
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       id              Ose_mutex_id            In          要释放的互斥量id
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
* Note:     只要id不非法，互斥量创建；那么该函数不会返回OSE_FAILURE
    其他异常，OSE阻塞任务，不返回。
*******************************************************************************/
Ose_status ose_release_mutex(Ose_mutex_id id)
{
    SINT32 ret_lx;
    /*检查互斥量id*/
    if(id >= OSE_MAX_MUTEXES)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_release_mutex]: release mutex id %d !!!",id);
        return OSE_FAILURE;
    }

    /*判断互斥量是否创建*/
    if(OSE_FALSE == ose_is_mutex_created(id))
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_release_mutex]: %d mutex is created !!!",id);
        return OSE_FAILURE;
    }
#ifdef LINUX_SWITCH
    ret_lx = pthread_mutex_unlock(&(ose_mutex[id].mutex));
    if(0 != ret_lx)
    {
        OSE_ERROR("ose_release_mutex ERROR: ");
    }
    return ret_lx;
#endif
}
/*****************************************************************************
* Function  : ose_delete_mutex
* Purpose   : 删除互斥量
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       id              Ose_mutex_id            In          要删除的信号量id
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
* Note:     只要id不非法，互斥量创建；那么该函数不会返回OSE_FAILURE
    其他异常，OSE阻塞任务，不返回。
    删除互斥量带来的临界问题，需要上层考虑。
*******************************************************************************/
Ose_status ose_delete_mutex(Ose_mutex_id id)
{
    SINT32 ret_lx;
    /*检查互斥量id*/
    if(id >= OSE_MAX_MUTEXES)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_delete_mutex]: delete mutex id %d !!!",id);
        return OSE_FAILURE;
    }

    /*判断互斥量是否创建*/
    if(OSE_FALSE == ose_is_mutex_created(id))
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_delete_mutex]: %d mutex is created !!!",id);
        return OSE_FAILURE;
    }

#ifdef LINUX_SWITCH
    ose_mutex[id].is_used = FALSE;
    ose_mutex[id].name    = NULL;
    ret_lx = pthread_mutex_destroy(&(ose_mutex[id].mutex));
    if(0 != ret_lx)
    {
        OSE_ERROR("ose_delete_mutex ERROR:");
    }
    ose_trace(OSE_TRACE_INFO,"delete mutex id %d.",id);
#endif
    return ret_lx;
}
/*****************************************************************************
* Function  : ose_is_mutex_created
* Purpose   : 查询某互斥量是否创建
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       id                 Ose_mutex_id         In          互斥量id
*
* Return:
*   OSE_TRUE    :创建
*   OSE_FALSE   :没创建
* Note:
*******************************************************************************/
Bool ose_is_mutex_created(Ose_mutex_id id)
{
#ifdef LINUX_SWITCH
    if(ose_mutex[id].is_used == OSE_TRUE)
    {
        return OSE_TRUE;
    }
#endif

    return OSE_FALSE;
}
/*****************************************************************************
* Function  : ose_mutex_delete_all
* Purpose   : 删除所有已使用互斥量
* Relation  :
*
* Input Parameters:
* Return:
* Note:
*******************************************************************************/
Ose_status ose_mutex_delete_all()
{
    Ose_mutex_id index;
    Ose_status   ret;

    g_ose_mutex_lock = OSE_UNAVAILABLE_ID;

#ifdef LINUX_SWITCH
    for(index = 0; index < OSE_MAX_MUTEXES; index++)
    {
        if(ose_mutex[index].is_used == OSE_TRUE)
        {
            ret = ose_delete_mutex(index);
            if(ret != OSE_SUCCESS)
            {
                return OSE_FAILURE;
            }
        }
    }
#endif
    return OSE_SUCCESS;
}