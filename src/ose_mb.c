/*******************************************************************************/
/* Filename      : ose_mb.c                                                    */
/* Description   : 邮箱                                                         */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#include <ose_all.h>
#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 5)

extern Ose_task_desc_tbl g_ose_task_desc_tbl[OSE_MAX_TASKS];
/*任务间邮箱*/
Ose_ext_mb ose_mailbox[OSE_MAX_MAILBOXS];
/*每个元素对应当前索引的taskid，如果不为空即有回调*/
Ose_mb_transform_func ose_tid_mid_transform_tbl[OSE_MAX_MAILBOXS];

/*****************************************************************************
* Function  : ose_init_mb
* Purpose   : 任务间邮箱初始化
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
*   OSE_SUCCESS 初始化成功
*   OSE_FAILURE 初始化失败
* Note:     任务内邮箱不需要初始化，创建时动态建立
*******************************************************************************/
Ose_status ose_init_mb(void)
{
    UINT32 index;

/*初始化邮箱的控制字段*/
#ifdef LINUX_SWITCH
    for(index = 0; index < OSE_MAX_MAILBOXS; index++)
    {
        ose_mailbox[index].blist_ptr = (Ose_fixed_queue*)NULL;
        ose_mailbox[index].sema_lock_wr = 0;
        ose_mailbox[index].sema_lock_rd = 0;
        ose_mailbox[index].mutex = 0;
        memset(&(ose_mailbox[index].name),0x00,sizeof(Ose_ext_mb_name));
    }
#endif

    /*默认任务id即为邮箱id*/
    for(index = 0; index < OSE_MAX_MAILBOXS; index++)
    {
        ose_tid_mid_transform_tbl[index] = NULL;
    }

    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_create_mb
* Purpose   : 创建任务间邮箱
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       mid             Ose_mb_id                In          邮箱id
*       max_nums        UINT32                   In          邮箱大小
*
* Return:
*   OSE_SUCCESS :   成功
*   其他        :   失败
* Note:
*******************************************************************************/
Ose_status ose_create_mb(Ose_mb_id mid, UINT32 max_nums)
{
    /*检查邮箱id*/
    if(mid >= OSE_MAX_MAILBOXS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_create_mb]: max mailbox id !!!");
        return OSE_MBIDOR_QID_ERROR;
    }

    /*检查邮箱是否创建*/
    if(OSE_TRUE == ose_is_mb_created(mid))
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_create_mb]: mailbox created !!!");
        return OSE_MB_CREATED;
    }
    
#ifdef LINUX_SWITCH
    sprintf(ose_mailbox[mid].name.sema_lock_rd_name,"sema_lock_rd_task_%d",mid);
    sprintf(ose_mailbox[mid].name.sema_lock_wr_name,"sema_lock_wr_task_%d",mid);
    sprintf(ose_mailbox[mid].name.mutex_name,"queue_mutex_task_%d",mid);
    /*创建读写信号量和互斥量,不需要检查返回值*/
    ose_mailbox[mid].sema_lock_rd = ose_create_sema(ose_mailbox[mid].name.sema_lock_rd_name, max_nums, OSE_FALSE);
    ose_mailbox[mid].sema_lock_wr = ose_create_sema(ose_mailbox[mid].name.sema_lock_wr_name, max_nums, OSE_TRUE);

    /*创建任务间邮箱的操作互斥量*/
    ose_mailbox[mid].mutex = ose_create_mutex((Ose_mutex_name)ose_mailbox[mid].name.mutex_name, OSE_TRUE);

    /*创建邮箱队列*/
    ose_mailbox[mid].blist_ptr = (Ose_queue_id)ose_create_fixed_queue(max_nums);
    if(ose_mailbox[mid].blist_ptr == (Ose_queue_id)NULL)
    {
        ose_delete_sema(ose_mailbox[mid].sema_lock_rd);
        ose_delete_sema(ose_mailbox[mid].sema_lock_wr);
        ose_delete_mutex(ose_mailbox[mid].mutex);
        ose_trace(OSE_TRACE_ERROR,"[ose_create_mb]: mailbox create fail!!!");
        return OSE_MB_CREATE_FAIL;
    }

    return OSE_SUCCESS;
#endif
}
/*****************************************************************************
* Function  : ose_get_from_mb
* Purpose   : 获取任务间邮箱的消息
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       mid             Ose_mb_id               In          邮箱id
*
* Return:
*   NULL     :失败
*   非NULL    :成功
* Note:
*******************************************************************************/
Ose_fsm_message* ose_get_from_mb(Ose_mb_id mid, Ose_timeout timeout)
{
    Ose_status ret_lx;
    Ose_fsm_message* fsm_msg_ptr;

    /*检查邮箱id*/
    if(mid >= OSE_MAX_MAILBOXS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_get_from_mb]: max mailbox id !!!");
        return (Ose_fsm_message*)NULL;
    }

    /*检查邮箱是否创建*/
    if(OSE_TRUE != ose_is_mb_created(mid))
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_get_from_mb]: mailbox created !!!");
        return (Ose_fsm_message*)NULL;
    }

#ifdef LINUX_SWITCH
    /*等待资源，如果有资源则直接返回*/
    /*读资源减少一个*/
    ret_lx = ose_obtain_sema(ose_mailbox[mid].sema_lock_rd, timeout);
    if(ret_lx == OSE_WAIT_TIMEOUT)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_get_from_mb]: mb sema lock rd !!!");
        return (Ose_fsm_message*)NULL;
    }

    /*获取互斥量，进入互斥区*/
    ret_lx = ose_obtain_mutex(ose_mailbox[mid].mutex, OSE_WAIT_FOREVER);
    OSE_ASSERT(ret_lx == OSE_SUCCESS);

    /*从队列里取出结点*/
    ret_lx = ose_get_fixed_queue_head((Ose_fixed_queue*)ose_mailbox[mid].blist_ptr, (UINT32*)&fsm_msg_ptr);
    ose_release_mutex(ose_mailbox[mid].mutex);
    if(OSE_SUCCESS != ret_lx)
    {
        /*队列为空*/
        ose_trace(OSE_TRACE_ERROR,"[ose_get_from_mb]: mb queue null !!!");
        return (Ose_fsm_message*)NULL;
    }
    /*增加一个可写资源*/
    ose_release_sema(ose_mailbox[mid].sema_lock_wr);
#endif
    return fsm_msg_ptr;
}
/*****************************************************************************
* Function  : ose_send_to_mb_ex
* Purpose   : 向任务间邮箱的头部发送消息
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   qid                 Ose_queue_id            In          邮箱
*   msg_ptr             Ose_fsm_message*        In          发送的消息
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
*   OSE_WAIT_TIMEOUT    超时还没发送成功
* Note:
*******************************************************************************/
Ose_status ose_send_to_mb_ex(Ose_mb_id mid, UINT8* msg_ptr, Ose_timeout timeout, Bool is_preferred)
{
    Ose_status ret;

    /*检查邮箱id*/
    if(mid >= OSE_MAX_MAILBOXS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_send_to_mb_ex]: max mailbox id !!!");
        return OSE_FAILURE;
    }

    /*检查邮箱是否创建*/
    if(OSE_TRUE != ose_is_mb_created(mid))
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_send_to_mb_ex]: mailbox created !!!");
        return OSE_FAILURE;
    }

    if(is_preferred == OSE_TRUE)
    {
        ret = ose_send_to_mb_head(mid, (Ose_fsm_message*)msg_ptr, timeout);
    }
    else
    {
        ret = ose_send_to_mb_tail(mid, (Ose_fsm_message*)msg_ptr, timeout);
    }

    if(ret == OSE_WAIT_TIMEOUT)
    {
        ose_fsm_free_msg((Ose_fsm_message*)msg_ptr);
    }

    return ret;
}
/*****************************************************************************
* Function  : ose_send_to_mb_head
* Purpose   : 向任务间邮箱的头部发送消息
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   qid                 Ose_queue_id            In          邮箱
*   msg_ptr             Ose_fsm_message*        In          发送的消息
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
*   OSE_WAIT_TIMEOUT    超时还没发送成功
* Note:
*******************************************************************************/
Ose_status ose_send_to_mb_head(Ose_mb_id mid, Ose_fsm_message* msg_ptr, Ose_timeout timeout)
{
#ifdef LINUX_SWITCH
    Ose_status ret_lx;

    /*获取一个写资源*/
    if(ose_obtain_sema(ose_mailbox[mid].sema_lock_wr, timeout) == OSE_WAIT_TIMEOUT)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_send_to_mb_head]: mb sema lock wr timeout !!!");
        return OSE_WAIT_TIMEOUT;
    }

    /*获取互斥量，进入互斥区*/
    ose_obtain_mutex(ose_mailbox[mid].mutex, OSE_WAIT_FOREVER);

    /*插入队列头部*/
    ret_lx = ose_add_fixed_queue_head(ose_mailbox[mid].blist_ptr, (UINT32)msg_ptr);
    ose_release_mutex(ose_mailbox[mid].mutex);
    if(OSE_SUCCESS != ret_lx)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_send_to_mb_head]: mb add queue head fail !!!");
        return OSE_FAILURE;
    }

    /*增加一个可读资源*/
    ose_release_sema(ose_mailbox[mid].sema_lock_rd);
#endif
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_send_to_mb_tail
* Purpose   : 向任务间邮箱的尾部发送消息
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   qid                 Ose_queue_id            In          邮箱
*   msg_ptr             Ose_fsm_message*        In          发送的消息
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
*   OSE_WAIT_TIMEOUT    超时还没发送成功
* Note:
*******************************************************************************/
Ose_status ose_send_to_mb_tail(Ose_mb_id mid, Ose_fsm_message* msg_ptr, Ose_timeout timeout)
{
#ifdef LINUX_SWITCH
    Ose_status ret_lx;
    /*获取一个写资源*/
    if(ose_obtain_sema(ose_mailbox[mid].sema_lock_wr, timeout) == OSE_WAIT_TIMEOUT)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_send_to_mb_tail]: mb sema lock wr timeout !!!");
        return OSE_WAIT_TIMEOUT;
    }

    /*获取互斥量，进入互斥区*/
    ose_obtain_mutex(ose_mailbox[mid].mutex, OSE_WAIT_FOREVER);
    /*插入队列头部*/
    ret_lx = ose_add_fixed_queue_tail(ose_mailbox[mid].blist_ptr, (UINT32)msg_ptr);
    ose_release_mutex(ose_mailbox[mid].mutex);
    if(OSE_SUCCESS != ret_lx)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_send_to_mb_tail]: mb add queue tail fail !!!");
        return OSE_FAILURE;
    }

    /*增加一个可读资源*/
    ose_release_sema(ose_mailbox[mid].sema_lock_rd);
#endif

    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_is_mb_created
* Purpose   : 查询某任务间邮箱是否创建
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       mid                 Ose_mb_id           In          任务间邮箱id
*
* Return:
*   OSE_TRUE    :创建
*   OSE_FALSE   :没创建
* Note:
*******************************************************************************/
Bool ose_is_mb_created(Ose_mb_id mid)
{
#ifdef LINUX_SWITCH
    if(ose_mailbox[mid].blist_ptr != NULL)
    {
        return OSE_TRUE;
    }
#endif
    return OSE_FALSE;
}
/*****************************************************************************
* Function  : ose_delete_mb
* Purpose   : 删除任务间邮箱
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   mid                 Ose_mb_id               In          邮箱id
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
* Note:
*******************************************************************************/
Ose_status ose_delete_mb(Ose_mb_id mid)
{
#ifdef LINUX_SWITCH
    /*删除控制字段里的信号量和互斥量*/
    ose_delete_sema(ose_mailbox[mid].sema_lock_rd);
    ose_delete_sema(ose_mailbox[mid].sema_lock_wr);
    ose_delete_mutex(ose_mailbox[mid].mutex);

    /*删除队列资源*/
    ose_delete_fixed_queue(ose_mailbox[mid].blist_ptr);

    /*清除控制字段*/
    ose_mailbox[mid].blist_ptr = (Ose_fixed_queue*)NULL;
    memset(&(ose_mailbox[mid].name),0x00,sizeof(Ose_ext_mb_name));
    ose_trace(OSE_TRACE_INFO,"delete task mb id %d",mid);
    return OSE_SUCCESS;
#endif
}
/*****************************************************************************
* Function  : ose_get_spare_mb
* Purpose   : 获取任务间邮箱的剩余容量百分比
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   mid                 Ose_mb_id               In          邮箱id
*
* Return:
*   0       :邮箱状态异常或者无空闲容量
*   1-100   :此刻邮箱的空闲容量百分比
* Note:
*******************************************************************************/
UINT32 ose_get_spare_mb(Ose_mb_id mid)
{
#ifdef LINUX_SWITCH
    UINT32 max_num;
    UINT32 available_num;

    /*获取邮箱总容量和空闲容量*/
    ose_get_fixed_queue_spare(ose_mailbox[mid].blist_ptr, &max_num, &available_num);

    return ((available_num * 100) / max_num);
#endif
}
/*****************************************************************************
* Function  : ose_mb_delete_all
* Purpose   : 删除所有创建的任务间和任务内邮箱
* Relation  :
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败
* Note:
*******************************************************************************/
Ose_status ose_mb_delete_all()
{
    UINT32 index;

/*初始化邮箱的控制字段*/
#ifdef LINUX_SWITCH
    for(index = 0; index < OSE_MAX_MAILBOXS; index++)
    {
        if(ose_mailbox[index].blist_ptr != (Ose_fixed_queue*)NULL)
        {
            ose_mailbox[index].blist_ptr = (Ose_fixed_queue*)NULL;

            /*删除控制字段里的信号量和互斥量*/
            ose_mailbox[index].sema_lock_rd = (Ose_sema_id)OSE_UNAVAILABLE_ID;
            ose_mailbox[index].sema_lock_wr = (Ose_sema_id)OSE_UNAVAILABLE_ID;
            ose_mailbox[index].mutex        = (Ose_mutex_id)OSE_UNAVAILABLE_ID;
            ose_trace(OSE_TRACE_INFO,"delete other mb id %d",index);
        }
    }
#endif
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_send_message
* Purpose   : 发送FSM消息到邮箱尾部
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   fsm_msg_ptr         Ose_fsm_message*        In          需要发送的FSM消息
*
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败，不返回
* Note:
*******************************************************************************/
Ose_status ose_send_message(Ose_fsm_message* fsm_msg_ptr)
{
    return ose_send_message_ex(fsm_msg_ptr, OSE_FALSE);
}
Ose_status ose_send_message_ex(Ose_fsm_message* fsm_msg_ptr, Bool is_preferred)
{
    return ose_send_message_ex_mb(fsm_msg_ptr, is_preferred, OSE_FALSE);
}
/*****************************************************************************
* Function  : ose_send_message_ex_mb
* Purpose   : 发送FSM消息到邮箱头部或尾部
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   fsm_msg_ptr         Ose_fsm_message*        In      需要发送的FSM消息
*   is_preferred        Bool                    In      指示发送到头部还是尾部
* Return:
*   OSE_SUCCESS :成功
*   OSE_FAILURE :失败，不返回
* Note:
*******************************************************************************/
Ose_status ose_send_message_ex_mb(Ose_fsm_message* fsm_msg_ptr, Bool is_preferred, Bool use_mb)
{
    Ose_task_id dest_tid, src_tid;
    Ose_status  ret;

    OSE_ASSERT(fsm_msg_ptr != NULL);

    FSM_TIMESTAMP(fsm_msg_ptr) = (Ose_timestamp)ose_get_system_time();

    /*取出发送者和接收者*/
    dest_tid = FSM_DEST_TASK_ID(fsm_msg_ptr);
    src_tid  = FSM_SRC_TASK_ID(fsm_msg_ptr);

    /*任务(邮箱)id是否非法*/
    OSE_ASSERT(dest_tid < OSE_MAX_TASKS);

    /*判断发送到任务内邮箱还是任务间邮箱*/
    if(!use_mb && dest_tid == src_tid)
    {
#if 0   //TBD
        /*发送到任务内邮箱的尾部*/
        /*检查任务内邮箱是否创建*/
        if(OSE_FALSE == ose_is_queue_created(dest_tid))
        {
            OSE_ERROR(OSE_QUEUE_NO_CREATED, dest_tid);
            return OSE_FAILURE;
        }

        /*发送*/
        /*判断消息发送到头部还是尾部*/
        if(OSE_TRUE == is_preferred)
        {
            /*消息发送到头部*/
            ret = ose_send_to_queue_head(g_ose_task_desc_tbl[dest_tid].task_int_queue, (Uint32*)fsm_msg_ptr);
        }
        else
        {
            /*消息发送到尾部*/
            ret = ose_send_to_queue_tail(g_ose_task_desc_tbl[dest_tid].task_int_queue, (Uint32*)fsm_msg_ptr);
        }

        if(ret != OSE_SUCCESS)
        {
            ose_free_fsm_msg(fsm_msg_ptr);
        }
#endif
    }
    else
    {
        /*判断是否要转换目的邮箱id*/
        if(ose_tid_mid_transform_tbl[dest_tid] != NULL)
        {
            dest_tid = (*ose_tid_mid_transform_tbl[dest_tid])(fsm_msg_ptr);
        }

        /*检查邮箱是否创建*/
        if(OSE_TRUE != ose_is_mb_created(dest_tid))
        {
            ose_trace(OSE_TRACE_ERROR,"[ose_send_message_ex_mb]: %d send mb %d is not create !!!",src_tid,dest_tid);
            return OSE_FAILURE;
        }

        /*发送*/
        if(OSE_TRUE == is_preferred)
        {
            ret = ose_send_to_mb_head((Ose_mb_id)dest_tid, fsm_msg_ptr, OSE_WAIT_FOREVER);
        }
        else
        {
            ret = ose_send_to_mb_tail((Ose_mb_id)dest_tid, fsm_msg_ptr, OSE_WAIT_FOREVER);
        }

        if(OSE_WAIT_TIMEOUT == ret)
        {
            ose_fsm_free_msg(fsm_msg_ptr);
        }
    }

    return OSE_SUCCESS;
}
