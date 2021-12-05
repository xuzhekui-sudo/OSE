/*******************************************************************************/
/* Filename      : ose_link.c                                                  */
/* Description   : 链表                                                        */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#include <ose_all.h>
#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 12)


/******************************************************************************/
/* Function  : ose_link_list_create                                           */
/* Purpose   : 创建链表                                                       */
/* Parameters:                                                                */
/* Return    :                                                                */
/* @ LINK_LIST* link_list                                                     */
/******************************************************************************/
LINK_LIST* ose_link_list_create()
{  
    LINK_LIST*  link_list = (LINK_LIST *)ose_get_mem(OSE_COMMON_POOL_ID, sizeof(LINK_LIST));

    link_list->count = 0;
    link_list->first = NULL;
    link_list->last  = NULL;
    link_list->equal = NULL;
    return link_list;  
} 
/******************************************************************************/
/* Function  : ose_link_list_free                                             */
/* Purpose   : 释放链表                                                       */
/* Parameters:                                                                */
/* @ LINK_LIST* link_list                                                     */
/* Return    :                                                                */
/******************************************************************************/
void ose_link_list_free(LINK_LIST * list)
{  
    LINK_LIST_NODE * p = NULL;
    
    while (list->first)
    {
        p = list->first->next;
        ose_free_mem(list->first);
        list->first = p;
    }
    ose_free_mem(list);
} 
/******************************************************************************/
/* Function  : ose_link_list_insert_tail                                      */
/* Purpose   : 插入在尾�?                                                     */
/* Parameters:                                                                */
/* @ LINK_LIST * const list                                                   */
/* @ void* const data                                                         */
/* Return    :                                                                */
/******************************************************************************/
void ose_link_list_insert_tail(LINK_LIST * const list, void* const data)
{ 
    LINK_LIST_NODE * node = (LINK_LIST_NODE *)ose_get_mem(OSE_COMMON_POOL_ID, sizeof(LINK_LIST_NODE));

    node->data = data;
    node->next = NULL;
    if (list->count)
    {
        list->last->next = node;
        list->last = node;
    }
    else
    {
        list->first = node;
        list->last = node;
    }
    (list->count)++;  
}
/******************************************************************************/
/* Function  : ose_link_list_insert_head                                      */
/* Purpose   : 插入在首�?                                                     */
/* Parameters:                                                                */
/* @ LINK_LIST * const list                                                   */
/* @ void* const data                                                         */
/* Return    :                                                                */
/******************************************************************************/
void ose_link_list_insert_head(LINK_LIST * const list, void* const data)
{
    LINK_LIST_NODE * node = (LINK_LIST_NODE *)ose_get_mem(OSE_COMMON_POOL_ID,sizeof(LINK_LIST_NODE));

    node->data = data;
    node->next = NULL;
  
    if (list->count)
    {
        node->next = list->first;
        list->first = node;
    }
    else
    {  
        list->first = node;
        list->last = node;
    }
    (list->count)++;
}
/******************************************************************************/
/* Function  : ose_link_list_get_size                                         */
/* Purpose   : 取得长度                                                       */
/* Parameters:                                                                */
/* @ LINK_LIST * const list                                                   */
/* Return    :                                                                */
/******************************************************************************/
SINT32 ose_link_list_get_size(const LINK_LIST * const list)
{  
    return list->count;  
}
/******************************************************************************/
/* Function  : ose_link_list_remove_tail                                      */
/* Purpose   : 删除在尾�?                                                     */
/* Parameters:                                                                */
/* @ LINK_LIST * const list                                                   */
/* Return    :                                                                */
/******************************************************************************/
void* ose_link_list_remove_tail(LINK_LIST* const list)
{
    LINK_LIST_NODE* p   = NULL;
    void*           re  = NULL;
    if (list->count == 1)
    {
        return ose_link_list_remove_head(list);
    }
    p = list->first;
    while (p->next != list->last)
    {
        p = p->next;
    }
    re = list->last->data;
    ose_free_mem(list->last);
    p->next = NULL;
    list->last = p;
    (list->count)--;
    return re;
}
/******************************************************************************/
/* Function  : ose_link_list_remove_head                                      */
/* Purpose   : 删除在首�?                                                     */
/* Parameters:                                                                */
/* @ LINK_LIST * const list                                                   */
/* Return    :                                                                */
/******************************************************************************/
void* ose_link_list_remove_head(LINK_LIST * const list)
{
    LINK_LIST_NODE* p  = NULL;
    void*           re = NULL;

    p = list->first;  
    list->first = p->next;  
    
    re = p->data;
    ose_free_mem(p);
    (list->count)--;
    if (list->count == 0)
    {  
        list->last = NULL;
    }
    return re;
}
/******************************************************************************/
/* Function  : ose_link_list_insert_at                                        */
/* Purpose   : 插入在莫个位                                                   */
/* Parameters:                                                                */
/* @ LINK_LIST * const list                                                   */
/* @ void* const data                                                         */
/* @ SINT32 index                                                             */
/* Return    :                                                                */
/******************************************************************************/
void ose_link_list_insert_at(LINK_LIST * const list, void* const data, SINT32 index)
{
    LINK_LIST_NODE * node = NULL;
    LINK_LIST_NODE * p = NULL;
    SINT32           i = 0;

    if(index == 0)
    {
        ose_link_list_insert_head(list, data);
        return;
    }
    if(index == list->count)
    {
        ose_link_list_insert_tail(list, data);
        return;
    }
    node = (LINK_LIST_NODE *)ose_get_mem(OSE_COMMON_POOL_ID,sizeof(LINK_LIST_NODE));
    node->data = data;
    node->next = NULL;
  
    p = list->first;
    for(i = 0; i < index - 1; i++)
    {
        p = p->next;
    }
    node->next = p->next;
    p->next = node;
  
    (list->count)++;
}
/******************************************************************************/
/* Function  : ose_link_list_remove_at                                        */
/* Purpose   : 删除在莫个位                                                   */
/* Parameters:                                                                */
/* @ LINK_LIST * const list                                                   */
/* @ SINT32 index                                                             */
/* Return    :                                                                */
/******************************************************************************/
void* ose_link_list_remove_at(LINK_LIST* const list, SINT32 index)
{
    LINK_LIST_NODE*    tp = NULL;
    LINK_LIST_NODE*    p  = NULL;
    void *             re = NULL;
    SINT32             i  = 0;

    if(index > list->count - 1)
    {
        return NULL;
    }
    if(index == 0)
    {
        return ose_link_list_remove_head(list);
    }
    if(index == list->count - 1)
    {
        return ose_link_list_remove_tail(list);
    }

    p = list->first;
    for(i = 0; i < index - 1; i++)
    {
        p = p->next;
    }

    tp = p->next;
    p->next = p->next->next;
    
    re = tp->data;
    ose_free_mem(tp);
    (list->count)--;
    return re;
}
/******************************************************************************/
/* Function  : ose_link_list_get_data_at                                      */
/* Purpose   : 取得数据                                                       */
/* Parameters:                                                                */
/* @ LINK_LIST * const list                                                   */
/* @ SINT32 index                                                             */
/* Return    :                                                                */
/******************************************************************************/
void* ose_link_list_get_data_at(const LINK_LIST * const list, SINT32 index)
{
    LINK_LIST_NODE*    p  = NULL;
    SINT32             i  = 0;
    
    if(index > list->count - 1)
    {
        return NULL;
    }
    if(index == list->count - 1)
    {
        return ose_link_list_get_data_tail(list);
    }

    p = list->first;
    for(i = 0; i < index; i++)
    {
        p = p->next;
    }
    return p->data;
}
/******************************************************************************/
/* Function  : ose_link_list_get_data_head                                    */
/* Purpose   : 取得第一个数�?                                                 */
/* Parameters:                                                                */
/* @ LINK_LIST * const list                                                   */
/* Return    :                                                                */
/******************************************************************************/
void* ose_link_list_get_data_head(const LINK_LIST * const list)
{
    return list->first->data;
}
/******************************************************************************/
/* Function  : ose_link_list_get_data_tail                                    */
/* Purpose   : 取得最后一个数�?                                               */
/* Parameters:                                                                */
/* @ LINK_LIST * const list                                                   */
/* Return    :                                                                */
/******************************************************************************/
void* ose_link_list_get_data_tail(const LINK_LIST * const list)
{
    return list->last->data;
}
/******************************************************************************/
/* Function  : ose_link_list_find_data_index                                  */
/* Purpose   : 查找某个数据的位�?                                             */
/* @  查找某个数据的位�?,如果equal方法为空，比较地址，否则调用equal方法       */
/* Parameters:                                                                */
/* @ LINK_LIST * const list                                                   */
/* @ void * data                                                              */
/* Return    :                                                                */
/* @ -1: 不存�?                                                               */
/* @ index: 存在                                                              */
/******************************************************************************/
SINT32 ose_link_list_find_data_index(const LINK_LIST * const list, void * data)
{
    LINK_LIST_NODE*  p  = NULL;
    SINT32           re = 0;
    
    p = list->first;

    if(list->equal)
    {
        while(p)
        {
            if(p->data == data || (*(list->equal))(p->data, data))
            {
                return re;
            }
            re++;
            p = p->next;
        }
    }
    else  
    {
        while(p)  
        {
            if(p->data == data)
            {
                return re;
            }
            re++;
            p = p->next;
        }
    }
    return -1;
}  