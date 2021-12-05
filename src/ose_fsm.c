/*******************************************************************************/
/* Filename      : ose_fsm.c                                                   */
/* Description   : FSM消息                                                      */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#include <ose_all.h>

#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 2)

/*****************************************************************************
* Function  : ose_get_fsm_msg
* Purpose   : fsm消息的申请函数
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       N/A                 N/A
*
*
* Return: 返回即成功
* Note:
*******************************************************************************/
Ose_fsm_message* ose_fsm_get_msg(UINT16 parameter_len)
{
    Ose_fsm_message* fsm_msg_ptr;

    /*查找block*/
    fsm_msg_ptr = (Ose_fsm_message*)ose_get_mem(OSE_FSM_MSG_HEADER_POOL_ID, sizeof(Ose_fsm_message));

    /*如果parameter_len不为0 ，申请内存*/
    if(parameter_len != 0)
    {
        /*为该parameter申请内存*/
        FSM_PARAM_PTR(fsm_msg_ptr)  = ose_get_mem(OSE_COMMON_POOL_ID,parameter_len);
        FSM_PARAM_SIZE(fsm_msg_ptr) = parameter_len;
    }
    else
    {
        /*对parameter结构对应字段赋值,parameter没有申请内存*/
        FSM_PARAM_PTR(fsm_msg_ptr)  = NULL;
        FSM_PARAM_SIZE(fsm_msg_ptr) = 0;
    }
    return fsm_msg_ptr;
}
/*****************************************************************************
* Function  : ose_fsm_free_msg
* Purpose   : fsm消息的释放函数
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       N/A                 N/A
*
*
* Return: 返回即成功
* Note:
*******************************************************************************/
Ose_status ose_fsm_free_msg(Ose_fsm_message* fsm_msg_ptr)
{
    OSE_ASSERT(fsm_msg_ptr != NULL);

    /*判断是否为一段式*/
    if(((UINT32)FSM_PARAM_PTR(fsm_msg_ptr)) == ((UINT32)(fsm_msg_ptr + 1)))
    {
        ose_free_mem((UINT8*)fsm_msg_ptr);
        fsm_msg_ptr = NULL;
        return OSE_SUCCESS;
    }

    /*如果parameter存在申请的内存，就释放掉*/
    if(FSM_PARAM_PTR(fsm_msg_ptr) != NULL)
    {
        ose_free_mem((UINT8*)FSM_PARAM_PTR(fsm_msg_ptr));
        FSM_PARAM_PTR(fsm_msg_ptr) = NULL;
    }
    /*释放消息头*/
    ose_free_mem((UINT8*)fsm_msg_ptr);
    fsm_msg_ptr = NULL;
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_fsm_link_msg
* Purpose   : 复用整过fsm消息
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       N/A                 N/A
*
*
* Return: 返回即成功
* Note:
*******************************************************************************/
Ose_status ose_fsm_link_msg(Ose_fsm_message* fsm_msg_ptr)
{
    OSE_ASSERT(fsm_msg_ptr != NULL);

    /*判断是否为一段式*/
    if(((UINT32)FSM_PARAM_PTR(fsm_msg_ptr)) == ((UINT32)(fsm_msg_ptr + 1)))
    {
        ose_buf_link_mem((UINT8*)fsm_msg_ptr);
        return OSE_SUCCESS;
    }

    /*如果parameter存在申请的内存，就link*/
    if(FSM_PARAM_PTR(fsm_msg_ptr) != NULL)
    {
        ose_buf_link_mem((UINT8*)FSM_PARAM_PTR(fsm_msg_ptr));
    }

    /*link消息头*/
    ose_buf_link_mem((UINT8*)fsm_msg_ptr);

    return OSE_SUCCESS;
}