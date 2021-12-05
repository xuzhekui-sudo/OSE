/*******************************************************************************/
/* Filename      : ose_list.c                                                  */
/* Description   : 提供常见队列等接�?                                             */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#include <ose_all.h>
#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 4)

/*****************************************************************************
* Function  : ose_create_fixed_queue
* Purpose   : 创建一个固定长度的队列
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   queue_len           UINT32                  In          队列元素个数
*
*
* Return:
*   非NULL:创建成功
*   NULL: 创建失败
* Note:
*******************************************************************************/
Ose_fixed_queue* ose_create_fixed_queue(UINT32 queue_len)
{
    Ose_fixed_queue* queue_ptr;

    /*队列长度不能�?0*/
    if(1 >= queue_len)
    {
        return (Ose_fixed_queue*)NULL;
    }

    /*开辟队列空�?*/
    queue_ptr = (Ose_fixed_queue*)ose_get_mem(OSE_PRIVATE_POOL_ID,sizeof(Ose_fixed_queue) + sizeof(UINT32*) * queue_len);

    /*初始化队�?*/
    queue_ptr->head      = 0;
    queue_ptr->tail      = 0;
    queue_ptr->free_tail = 0;
    queue_ptr->maxnum    = queue_len;
    queue_ptr->node      = (UINT32*)(1 + queue_ptr);

    memset((void*)queue_ptr->node, 0, sizeof(UINT32*) * queue_len);

    return queue_ptr;
}
/*****************************************************************************
* Function  : ose_add_fixed_queue_tail
* Purpose   : 向队列尾部插入一个结�?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   queue_ptr           Ose_fixed_queue         In          队列控制字段
*   item                UINT32                  In          需要插入的数据
*
* Return:
*   OSE_SUCCESS :插入成功
*   OSE_FAILURE :插入失败
* Note:
*******************************************************************************/
Ose_status ose_add_fixed_queue_tail(Ose_fixed_queue* queue_ptr, UINT32 item)
{
    /*特殊处理邮箱满的情况*/
    if((queue_ptr->head != queue_ptr->tail) && (queue_ptr->head == queue_ptr->free_tail))
    {
        return OSE_FAILURE;
    }
    /*特殊处理邮箱空的情况*/
    else if((queue_ptr->head == queue_ptr->tail) && (queue_ptr->tail == queue_ptr->free_tail))
    {
        /*NULL;*/
    }
    /*一般情�?*/
    else
    {
        if((++queue_ptr->tail) == queue_ptr->maxnum)
        {
            queue_ptr->tail = 0;
        }
    }

    /*free_tail后移*/
    if((++queue_ptr->free_tail) >= queue_ptr->maxnum)
    {
        queue_ptr->free_tail = 0;
    }

    queue_ptr->node[queue_ptr->tail] = item;
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_add_fixed_queue_head
* Purpose   : 向队列头部插入一个结�?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   queue_ptr           Ose_fixed_queue         In          队列控制字段
*   item                UINT32                  In          需要插入的数据
*
* Return:
*   OSE_SUCCESS :插入成功
*   OSE_FAILURE :插入失败
* Note:
*******************************************************************************/
Ose_status ose_add_fixed_queue_head(Ose_fixed_queue* queue_ptr, UINT32 item)
{
    /*特殊处理邮箱满的情况*/
    if((queue_ptr->head != queue_ptr->tail) && (queue_ptr->head == queue_ptr->free_tail))
    {
        return OSE_FAILURE;
    }
    /*特殊处理邮箱空的情况*/
    else if((queue_ptr->head == queue_ptr->tail) && (queue_ptr->tail == queue_ptr->free_tail))
    {
        /*free_tail后移*/
        if((++queue_ptr->free_tail) >= queue_ptr->maxnum)
        {
            queue_ptr->free_tail = 0;
        }
    }
    /*一般情�?*/
    else
    {
        if(queue_ptr->head == 0)
        {
            queue_ptr->head = queue_ptr->maxnum - 1;
        }
        else
        {
            queue_ptr->head--;
        }
    }

    queue_ptr->node[queue_ptr->head] = item;

    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_get_fixed_queue_head
* Purpose   : 获取队列头部一个结�?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   queue_ptr           Ose_fixed_queue         In          队列控制字段
*   item                UINT32                  In          需要插入的数据
*
* Return:
*   OSE_SUCCESS :获取成功
*   OSE_FAILURE :获取失败
* Note:
*******************************************************************************/
Ose_status ose_get_fixed_queue_head(Ose_fixed_queue* queue_ptr, UINT32* item_ptr)
{
    /*特殊处理邮箱空的情况*/
    if((queue_ptr->head == queue_ptr->tail) && (queue_ptr->tail == queue_ptr->free_tail))
    {
        return OSE_FAILURE;
    }

    /*取出结点*/
    *item_ptr = queue_ptr->node[queue_ptr->head];
    /*特殊处理邮箱只有一个消息的情况*/
    if((queue_ptr->head == queue_ptr->tail) && (queue_ptr->tail != queue_ptr->free_tail))
    {
        queue_ptr->free_tail = queue_ptr->head;
    }
    /*一般情�?*/
    else
    {
        if((++queue_ptr->head) == queue_ptr->maxnum)
        {
            queue_ptr->head = 0;
        }
    }

    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_get_fixed_queue_spare
* Purpose   :获取固定队列的剩余容�?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   queue_ptr           Ose_fixed_queue         In          队列控制字段
*   max_num_ptr         UINT32*                 In          队列总容量存放地址
*   available_num_ptr   UINT32*                 In          队列剩余空间存放地址
* Return: �?
* Note:
*******************************************************************************/
void ose_get_fixed_queue_spare(Ose_fixed_queue* queue_ptr, UINT32* max_num_ptr, UINT32* available_num_ptr)
{
    /*内部函数，不判断空指针了*/

    /*取出总容�?*/
    *max_num_ptr = queue_ptr->maxnum;

    /*取出剩余空间*/
    if((queue_ptr->head == queue_ptr->free_tail) && (queue_ptr->tail == queue_ptr->free_tail))
    {
        /*邮箱�?*/
        *available_num_ptr = queue_ptr->maxnum;
    }
    else if(queue_ptr->head == queue_ptr->free_tail)
    {
        /*空间�?*/
        *available_num_ptr = 0;
    }
    else if(queue_ptr->head < queue_ptr->free_tail)
    {
        *available_num_ptr = (queue_ptr->maxnum - queue_ptr->free_tail) + queue_ptr->head;
    }
    else
    {
        *available_num_ptr = queue_ptr->head - queue_ptr->free_tail;
    }
}
/*****************************************************************************
* Function  : ose_delete_fixed_queue
* Purpose   : 删除队列
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   queue_ptr           Ose_fixed_queue         In          队列控制字段
*
* Return:
*   OSE_SUCCESS :获取成功
*   OSE_FAILURE :获取失败
* Note:
*******************************************************************************/
Ose_status ose_delete_fixed_queue(Ose_fixed_queue* queue_ptr)
{
    ose_free_mem((UINT8*)queue_ptr);

    return OSE_SUCCESS;
}