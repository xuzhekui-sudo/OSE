/*******************************************************************************/
/* Filename      : ose_list.c                                                  */
/* Description   : �ṩ�������еȽӿ�                                             */
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
* Purpose   : ����һ���̶����ȵĶ���
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   queue_len           UINT32                  In          ����Ԫ�ظ���
*
*
* Return:
*   ��NULL:�����ɹ�
*   NULL: ����ʧ��
* Note:
*******************************************************************************/
Ose_fixed_queue* ose_create_fixed_queue(UINT32 queue_len)
{
    Ose_fixed_queue* queue_ptr;

    /*���г��Ȳ���Ϊ0*/
    if(1 >= queue_len)
    {
        return (Ose_fixed_queue*)NULL;
    }

    /*���ٶ��пռ�*/
    queue_ptr = (Ose_fixed_queue*)ose_get_mem(OSE_PRIVATE_POOL_ID,sizeof(Ose_fixed_queue) + sizeof(UINT32*) * queue_len);

    /*��ʼ������*/
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
* Purpose   : �����β������һ�����
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   queue_ptr           Ose_fixed_queue         In          ���п����ֶ�
*   item                UINT32                  In          ��Ҫ���������
*
* Return:
*   OSE_SUCCESS :����ɹ�
*   OSE_FAILURE :����ʧ��
* Note:
*******************************************************************************/
Ose_status ose_add_fixed_queue_tail(Ose_fixed_queue* queue_ptr, UINT32 item)
{
    /*���⴦�������������*/
    if((queue_ptr->head != queue_ptr->tail) && (queue_ptr->head == queue_ptr->free_tail))
    {
        return OSE_FAILURE;
    }
    /*���⴦������յ����*/
    else if((queue_ptr->head == queue_ptr->tail) && (queue_ptr->tail == queue_ptr->free_tail))
    {
        /*NULL;*/
    }
    /*һ�����*/
    else
    {
        if((++queue_ptr->tail) == queue_ptr->maxnum)
        {
            queue_ptr->tail = 0;
        }
    }

    /*free_tail����*/
    if((++queue_ptr->free_tail) >= queue_ptr->maxnum)
    {
        queue_ptr->free_tail = 0;
    }

    queue_ptr->node[queue_ptr->tail] = item;
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_add_fixed_queue_head
* Purpose   : �����ͷ������һ�����
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   queue_ptr           Ose_fixed_queue         In          ���п����ֶ�
*   item                UINT32                  In          ��Ҫ���������
*
* Return:
*   OSE_SUCCESS :����ɹ�
*   OSE_FAILURE :����ʧ��
* Note:
*******************************************************************************/
Ose_status ose_add_fixed_queue_head(Ose_fixed_queue* queue_ptr, UINT32 item)
{
    /*���⴦�������������*/
    if((queue_ptr->head != queue_ptr->tail) && (queue_ptr->head == queue_ptr->free_tail))
    {
        return OSE_FAILURE;
    }
    /*���⴦������յ����*/
    else if((queue_ptr->head == queue_ptr->tail) && (queue_ptr->tail == queue_ptr->free_tail))
    {
        /*free_tail����*/
        if((++queue_ptr->free_tail) >= queue_ptr->maxnum)
        {
            queue_ptr->free_tail = 0;
        }
    }
    /*һ�����*/
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
* Purpose   : ��ȡ����ͷ��һ�����
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   queue_ptr           Ose_fixed_queue         In          ���п����ֶ�
*   item                UINT32                  In          ��Ҫ���������
*
* Return:
*   OSE_SUCCESS :��ȡ�ɹ�
*   OSE_FAILURE :��ȡʧ��
* Note:
*******************************************************************************/
Ose_status ose_get_fixed_queue_head(Ose_fixed_queue* queue_ptr, UINT32* item_ptr)
{
    /*���⴦������յ����*/
    if((queue_ptr->head == queue_ptr->tail) && (queue_ptr->tail == queue_ptr->free_tail))
    {
        return OSE_FAILURE;
    }

    /*ȡ�����*/
    *item_ptr = queue_ptr->node[queue_ptr->head];
    /*���⴦������ֻ��һ����Ϣ�����*/
    if((queue_ptr->head == queue_ptr->tail) && (queue_ptr->tail != queue_ptr->free_tail))
    {
        queue_ptr->free_tail = queue_ptr->head;
    }
    /*һ�����*/
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
* Purpose   :��ȡ�̶����е�ʣ������
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   queue_ptr           Ose_fixed_queue         In          ���п����ֶ�
*   max_num_ptr         UINT32*                 In          ������������ŵ�ַ
*   available_num_ptr   UINT32*                 In          ����ʣ��ռ��ŵ�ַ
* Return: ��
* Note:
*******************************************************************************/
void ose_get_fixed_queue_spare(Ose_fixed_queue* queue_ptr, UINT32* max_num_ptr, UINT32* available_num_ptr)
{
    /*�ڲ����������жϿ�ָ����*/

    /*ȡ��������*/
    *max_num_ptr = queue_ptr->maxnum;

    /*ȡ��ʣ��ռ�*/
    if((queue_ptr->head == queue_ptr->free_tail) && (queue_ptr->tail == queue_ptr->free_tail))
    {
        /*�����*/
        *available_num_ptr = queue_ptr->maxnum;
    }
    else if(queue_ptr->head == queue_ptr->free_tail)
    {
        /*�ռ���*/
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
* Purpose   : ɾ������
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*   queue_ptr           Ose_fixed_queue         In          ���п����ֶ�
*
* Return:
*   OSE_SUCCESS :��ȡ�ɹ�
*   OSE_FAILURE :��ȡʧ��
* Note:
*******************************************************************************/
Ose_status ose_delete_fixed_queue(Ose_fixed_queue* queue_ptr)
{
    ose_free_mem((UINT8*)queue_ptr);

    return OSE_SUCCESS;
}