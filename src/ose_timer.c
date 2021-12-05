/*******************************************************************************/
/* Filename      : ose_timer.c                                                 */
/* Description   : 定时器管�?                                                  */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#include <ose_all.h>
#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 10)

/*定时器资源的定义*/
Ose_timer       ose_timer[OSE_MAX_TIMERS];
/*定时器模块的互斥�?*/
Ose_mutex_id    ose_tm_lock = OSE_UNAVAILABLE_ID;

/*****************************************************************************
* Function  : ose_init_timer
* Purpose   : 定时器初始化函数
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
*
* Return:
*   OSE_SUCCESS 初始化成�?
*   OSE_FAILURE 初始化失�?
* Note:
*******************************************************************************/
Ose_status ose_init_timer()
{
    UINT32      index;

    /*初始化定时器控制字段*/
    for(index = 0; index < OSE_MAX_TIMERS; index++)
    {
        ose_timer[index].timer_id = OSE_MAX_TIMERS;
        ose_timer[index].timer_name = NULL;
        ose_timer[index].plt_timer_id = 0;
        ose_timer[index].timer_value = 0;
        ose_timer[index].callback = NULL;
    }

    /*创建定时器模块的互斥�?*/
    ose_tm_lock = ose_create_mutex((Ose_mutex_name) "TMMutex", OSE_TRUE);
    if(ose_tm_lock == OSE_UNAVAILABLE_ID)
    {
        return OSE_FAILURE;
    }

    return OSE_SUCCESS;
}
/*定时器相关接口函�?*/
/*****************************************************************************
* Function  : ose_create_timer
* Purpose   : 创建定时�?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       flag            Ose_timer_flag          In          定时器类�?
* Return: 定时器id
* Note:
*******************************************************************************/
Ose_timer_id ose_create_timer(Ose_timer* timer_ptr)
{
    SINT32              index   = 0;
    Ose_status          result  = OSE_FAILURE;
    struct sigevent     sig_event;

    /*获取互斥�?*/
    ose_obtain_mutex(ose_tm_lock, OSE_WAIT_FOREVER);

    for(index = 0; index < OSE_MAX_TIMERS; index++)
    {
        if(0 == ose_timer[index].plt_timer_id && ose_timer[index].callback == NULL)
        {
            ose_timer[index].timer_id    = index;
            ose_timer[index].timer_name  = timer_ptr->timer_name;
            ose_timer[index].timer_value = timer_ptr->timer_value;
            ose_timer[index].callback    = timer_ptr->callback;

            memset(&sig_event, 0, sizeof(struct sigevent));
            sig_event.sigev_value.sival_int = ose_timer[index].timer_id;        //回调函数参数
            sig_event.sigev_notify = SIGEV_THREAD;                              //线程通知的方式，派驻新线�?
            sig_event.sigev_notify_function = timer_ptr->callback;              //线程函数地址
            if(timer_create(CLOCK_REALTIME, &sig_event, &(ose_timer[index].plt_timer_id)) != 0)  
            {  
                ose_release_mutex(ose_tm_lock);
                OSE_ERROR("ose_create_timer ERROR: ");
                return OSE_MAX_TIMERS; 
            }
            result = OSE_SUCCESS;
            break;
        }
    }
    /*退出临界区*/
    ose_release_mutex(ose_tm_lock);

    if(OSE_SUCCESS == result)
    {
        ose_trace(OSE_TRACE_INFO,"create timer %s success id:%d.",timer_ptr->timer_name,index);
        return index;
    }
    else
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_create_timer]: create timer %s failure !!!",timer_ptr->timer_name);
        return OSE_UNAVAILABLE_ID;
    }
}
/*****************************************************************************
* Function  : ose_start_timer
* Purpose   : 定时器开�?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       id            Ose_timer_id               In          定时器ID
*       value         Ose_timer_value            In          微秒
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
* Note:     如果一个已经计时的定时器，那么重新计时
*******************************************************************************/
Ose_status ose_start_timer(Ose_timer_id id)
{
    Ose_timer*          pstTimer = NULL; /*指向当前的定时器控制字段*/
    Ose_status          ret = OSE_SUCCESS;
    struct itimerspec   it;
    UINT32              tv_sec_value = 0;
    UINT32              tv_nsec_value = 0;

    /*检查定时器id*/
    if(id >= OSE_MAX_TIMERS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_start_timer]: max timer id !!!");
        return OSE_FAILURE;
    }
    /*获取互斥�?*/
    ose_obtain_mutex(ose_tm_lock, OSE_WAIT_FOREVER);
    /*获取定时器结点的指针*/
    pstTimer = &ose_timer[id];
    if(NULL == pstTimer || 0 == pstTimer->plt_timer_id)
    {
        ose_release_mutex(ose_tm_lock);
        ose_trace(OSE_TRACE_ERROR,"[ose_start_timer]: no create !!!");
        return OSE_FAILURE;
    }
    /*开启对应定时器*/
    tv_sec_value = pstTimer->timer_value / 1000;
    tv_nsec_value = ((pstTimer->timer_value % 1000) * 1000000);
    //间隔时间
    it.it_interval.tv_sec = tv_sec_value;
    it.it_interval.tv_nsec = tv_nsec_value;
    //启动时间
    it.it_value.tv_sec = tv_sec_value;
    it.it_value.tv_nsec = tv_nsec_value;
    if(timer_settime(pstTimer->plt_timer_id, 0, &it, NULL) != 0)
    {  
        OSE_ERROR("ERROR:OSE_TIMER_START_FAIL");
        ret = OSE_FAILURE;
    }
    ose_release_mutex(ose_tm_lock);
    return ret;
}
/*****************************************************************************
* Function  : ose_stop_timer
* Purpose   : 定时器停止计�?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       N/A                 N/A
*
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
*******************************************************************************/
Ose_status ose_stop_timer(Ose_timer_id id)
{
    /*指向当前的定时器控制字段*/
    Ose_timer*          pstTimer;
    Ose_status          ret;
    struct itimerspec   it;

    /*检查定时器id*/
    if(id >= OSE_MAX_TIMERS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_stop_timer]: max timer id !!!");
        return OSE_FAILURE;
    }

    /*获取互斥�?*/
    ose_obtain_mutex(ose_tm_lock, OSE_WAIT_FOREVER);

    /*获取定时器结点的指针*/
    pstTimer = &ose_timer[id];

    /*检查定时器是否创建*/
    if(NULL == pstTimer || 0 == pstTimer->plt_timer_id)
    {
        ose_release_mutex(ose_tm_lock);
        ose_trace(OSE_TRACE_ERROR,"[ose_stop_timer]: no create !!!");
        return OSE_FAILURE;
    }

    /*停止对应定时�?*/
    //间隔时间
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_nsec = 0;
    //启动时间
    it.it_value.tv_sec = 0;
    it.it_value.tv_nsec = 0;
    if(timer_settime(pstTimer->plt_timer_id, 0, &it, NULL) != 0)
    {  
        OSE_ERROR("ERROR:OSE_TIMER_STOP_FAIL");
        ret = OSE_FAILURE;
    }
    else
    {
        ret = OSE_SUCCESS;
    }
    ose_release_mutex(ose_tm_lock);
    return ret;
}
/*****************************************************************************
* Function  : ose_delete_timer
* Purpose   : 删除定时�?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       N/A                 N/A
*
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
*******************************************************************************/
Ose_status ose_delete_timer(Ose_timer_id id)
{
    /*指向当前的定时器控制字段*/
    Ose_timer*  pstTimer;
    Ose_status  ret;

    /*检查定时器id*/
    if(id >= OSE_MAX_TIMERS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_delete_timer]: max timer id !!!");
        return OSE_FAILURE;
    }

    /*获取互斥�?*/
    ose_obtain_mutex(ose_tm_lock, OSE_WAIT_FOREVER);

    /*获取定时器结点的指针*/
    pstTimer = &ose_timer[id];

    /*检查定时器是否创建*/
    if(NULL == pstTimer || 0 == pstTimer->plt_timer_id)
    {
        ose_release_mutex(ose_tm_lock);
        ose_trace(OSE_TRACE_ERROR,"[ose_delete_timer]: no create !!!");
        return OSE_FAILURE;
    }
    if(timer_delete(pstTimer->plt_timer_id) != 0)
    {
        OSE_ERROR("ERROR:OSE_TIMER_DELETE_FAIL");
        ret = OSE_FAILURE;
    }
    else
    {
        ret = OSE_SUCCESS;
    }
    pstTimer->timer_id = OSE_MAX_TIMERS;
    pstTimer->timer_name = NULL;
    pstTimer->plt_timer_id = 0;
    pstTimer->timer_value = 0;
    pstTimer->callback = NULL;
    ose_release_mutex(ose_tm_lock);
    ose_trace(OSE_TRACE_INFO,"delete timer %d ",id);
    return ret;
}
/*****************************************************************************
* Function  : ose_change_timer_value
* Purpose   : 修改定时器时�?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       id            Ose_timer_id               In          定时器ID
*       value         Ose_timer_value            In          微秒
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
* Note:     如果一个已经计时的定时器，那么重新计时
*******************************************************************************/
Ose_status ose_change_timer_value(Ose_timer_id id , Ose_timer_value timer_value)
{
    Ose_timer*          pstTimer = NULL; /*指向当前的定时器控制字段*/
    /*检查定时器id*/
    if(id >= OSE_MAX_TIMERS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_change_timer_value]: max timer id !!!");
        return OSE_FAILURE;
    }
    /*获取互斥�?*/
    ose_obtain_mutex(ose_tm_lock, OSE_WAIT_FOREVER);
    /*获取定时器结点的指针*/
    pstTimer = &ose_timer[id];
    if(NULL == pstTimer || 0 == pstTimer->plt_timer_id)
    {
        ose_release_mutex(ose_tm_lock);
        ose_trace(OSE_TRACE_ERROR,"[ose_change_timer_value]: no create !!!");
        return OSE_FAILURE;
    }
    /*开启对应定时器*/
    pstTimer->timer_value = timer_value;
    ose_release_mutex(ose_tm_lock);
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_timer_delete_all
* Purpose   : 删除所有已使用定时�?
* Relation  :
*
* Input Parameters:
* Return:
* Note:
*******************************************************************************/
Ose_status ose_timer_delete_all()
{
    Ose_timer_id index;

    ose_tm_lock = OSE_UNAVAILABLE_ID;

    for(index = 0; index < OSE_MAX_TIMERS; index++)
    {
        if(ose_timer[index].plt_timer_id != 0)
        {
            /*删除定时�?*/
            ose_delete_timer(index);
        }
        ose_timer[index].timer_id = OSE_MAX_TIMERS;
        ose_timer[index].timer_name = NULL;
        ose_timer[index].plt_timer_id = 0;
        ose_timer[index].timer_value = 0;
        ose_timer[index].callback = NULL;
    }
    return OSE_SUCCESS;
}