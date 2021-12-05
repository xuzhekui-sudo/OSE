/*******************************************************************************/
/* Filename      : ose_timer.c                                                 */
/* Description   : å®æ¶å¨ç®¡ç?                                                  */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#include <ose_all.h>
#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 10)

/*å®æ¶å¨èµæºçå®ä¹*/
Ose_timer       ose_timer[OSE_MAX_TIMERS];
/*å®æ¶å¨æ¨¡åçäºæ¥é?*/
Ose_mutex_id    ose_tm_lock = OSE_UNAVAILABLE_ID;

/*****************************************************************************
* Function  : ose_init_timer
* Purpose   : å®æ¶å¨åå§åå½æ°
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
*
* Return:
*   OSE_SUCCESS åå§åæå?
*   OSE_FAILURE åå§åå¤±è´?
* Note:
*******************************************************************************/
Ose_status ose_init_timer()
{
    UINT32      index;

    /*åå§åå®æ¶å¨æ§å¶å­æ®µ*/
    for(index = 0; index < OSE_MAX_TIMERS; index++)
    {
        ose_timer[index].timer_id = OSE_MAX_TIMERS;
        ose_timer[index].timer_name = NULL;
        ose_timer[index].plt_timer_id = 0;
        ose_timer[index].timer_value = 0;
        ose_timer[index].callback = NULL;
    }

    /*åå»ºå®æ¶å¨æ¨¡åçäºæ¥é?*/
    ose_tm_lock = ose_create_mutex((Ose_mutex_name) "TMMutex", OSE_TRUE);
    if(ose_tm_lock == OSE_UNAVAILABLE_ID)
    {
        return OSE_FAILURE;
    }

    return OSE_SUCCESS;
}
/*å®æ¶å¨ç¸å³æ¥å£å½æ?*/
/*****************************************************************************
* Function  : ose_create_timer
* Purpose   : åå»ºå®æ¶å?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       flag            Ose_timer_flag          In          å®æ¶å¨ç±»å?
* Return: å®æ¶å¨id
* Note:
*******************************************************************************/
Ose_timer_id ose_create_timer(Ose_timer* timer_ptr)
{
    SINT32              index   = 0;
    Ose_status          result  = OSE_FAILURE;
    struct sigevent     sig_event;

    /*è·åäºæ¥é?*/
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
            sig_event.sigev_value.sival_int = ose_timer[index].timer_id;        //åè°å½æ°åæ°
            sig_event.sigev_notify = SIGEV_THREAD;                              //çº¿ç¨éç¥çæ¹å¼ï¼æ´¾é©»æ°çº¿ç¨?
            sig_event.sigev_notify_function = timer_ptr->callback;              //çº¿ç¨å½æ°å°å
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
    /*éåºä¸´çåº*/
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
* Purpose   : å®æ¶å¨å¼å?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       id            Ose_timer_id               In          å®æ¶å¨ID
*       value         Ose_timer_value            In          å¾®ç§
*
* Return:
*   OSE_SUCCESS :æå
*   OSE_FAILURE :å¤±è´¥
* Note:     å¦æä¸ä¸ªå·²ç»è®¡æ¶çå®æ¶å¨ï¼é£ä¹éæ°è®¡æ¶
*******************************************************************************/
Ose_status ose_start_timer(Ose_timer_id id)
{
    Ose_timer*          pstTimer = NULL; /*æåå½åçå®æ¶å¨æ§å¶å­æ®µ*/
    Ose_status          ret = OSE_SUCCESS;
    struct itimerspec   it;
    UINT32              tv_sec_value = 0;
    UINT32              tv_nsec_value = 0;

    /*æ£æ¥å®æ¶å¨id*/
    if(id >= OSE_MAX_TIMERS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_start_timer]: max timer id !!!");
        return OSE_FAILURE;
    }
    /*è·åäºæ¥é?*/
    ose_obtain_mutex(ose_tm_lock, OSE_WAIT_FOREVER);
    /*è·åå®æ¶å¨ç»ç¹çæé*/
    pstTimer = &ose_timer[id];
    if(NULL == pstTimer || 0 == pstTimer->plt_timer_id)
    {
        ose_release_mutex(ose_tm_lock);
        ose_trace(OSE_TRACE_ERROR,"[ose_start_timer]: no create !!!");
        return OSE_FAILURE;
    }
    /*å¼å¯å¯¹åºå®æ¶å¨*/
    tv_sec_value = pstTimer->timer_value / 1000;
    tv_nsec_value = ((pstTimer->timer_value % 1000) * 1000000);
    //é´éæ¶é´
    it.it_interval.tv_sec = tv_sec_value;
    it.it_interval.tv_nsec = tv_nsec_value;
    //å¯å¨æ¶é´
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
* Purpose   : å®æ¶å¨åæ­¢è®¡æ?
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
*   OSE_SUCCESS :æå
*   OSE_FAILURE :å¤±è´¥
*******************************************************************************/
Ose_status ose_stop_timer(Ose_timer_id id)
{
    /*æåå½åçå®æ¶å¨æ§å¶å­æ®µ*/
    Ose_timer*          pstTimer;
    Ose_status          ret;
    struct itimerspec   it;

    /*æ£æ¥å®æ¶å¨id*/
    if(id >= OSE_MAX_TIMERS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_stop_timer]: max timer id !!!");
        return OSE_FAILURE;
    }

    /*è·åäºæ¥é?*/
    ose_obtain_mutex(ose_tm_lock, OSE_WAIT_FOREVER);

    /*è·åå®æ¶å¨ç»ç¹çæé*/
    pstTimer = &ose_timer[id];

    /*æ£æ¥å®æ¶å¨æ¯å¦åå»º*/
    if(NULL == pstTimer || 0 == pstTimer->plt_timer_id)
    {
        ose_release_mutex(ose_tm_lock);
        ose_trace(OSE_TRACE_ERROR,"[ose_stop_timer]: no create !!!");
        return OSE_FAILURE;
    }

    /*åæ­¢å¯¹åºå®æ¶å?*/
    //é´éæ¶é´
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_nsec = 0;
    //å¯å¨æ¶é´
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
* Purpose   : å é¤å®æ¶å?
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
*   OSE_SUCCESS :æå
*   OSE_FAILURE :å¤±è´¥
*******************************************************************************/
Ose_status ose_delete_timer(Ose_timer_id id)
{
    /*æåå½åçå®æ¶å¨æ§å¶å­æ®µ*/
    Ose_timer*  pstTimer;
    Ose_status  ret;

    /*æ£æ¥å®æ¶å¨id*/
    if(id >= OSE_MAX_TIMERS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_delete_timer]: max timer id !!!");
        return OSE_FAILURE;
    }

    /*è·åäºæ¥é?*/
    ose_obtain_mutex(ose_tm_lock, OSE_WAIT_FOREVER);

    /*è·åå®æ¶å¨ç»ç¹çæé*/
    pstTimer = &ose_timer[id];

    /*æ£æ¥å®æ¶å¨æ¯å¦åå»º*/
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
* Purpose   : ä¿®æ¹å®æ¶å¨æ¶é?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       id            Ose_timer_id               In          å®æ¶å¨ID
*       value         Ose_timer_value            In          å¾®ç§
*
* Return:
*   OSE_SUCCESS :æå
*   OSE_FAILURE :å¤±è´¥
* Note:     å¦æä¸ä¸ªå·²ç»è®¡æ¶çå®æ¶å¨ï¼é£ä¹éæ°è®¡æ¶
*******************************************************************************/
Ose_status ose_change_timer_value(Ose_timer_id id , Ose_timer_value timer_value)
{
    Ose_timer*          pstTimer = NULL; /*æåå½åçå®æ¶å¨æ§å¶å­æ®µ*/
    /*æ£æ¥å®æ¶å¨id*/
    if(id >= OSE_MAX_TIMERS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_change_timer_value]: max timer id !!!");
        return OSE_FAILURE;
    }
    /*è·åäºæ¥é?*/
    ose_obtain_mutex(ose_tm_lock, OSE_WAIT_FOREVER);
    /*è·åå®æ¶å¨ç»ç¹çæé*/
    pstTimer = &ose_timer[id];
    if(NULL == pstTimer || 0 == pstTimer->plt_timer_id)
    {
        ose_release_mutex(ose_tm_lock);
        ose_trace(OSE_TRACE_ERROR,"[ose_change_timer_value]: no create !!!");
        return OSE_FAILURE;
    }
    /*å¼å¯å¯¹åºå®æ¶å¨*/
    pstTimer->timer_value = timer_value;
    ose_release_mutex(ose_tm_lock);
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_timer_delete_all
* Purpose   : å é¤ææå·²ä½¿ç¨å®æ¶å?
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
            /*å é¤å®æ¶å?*/
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