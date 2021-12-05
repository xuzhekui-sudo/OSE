/*******************************************************************************/
/* Filename      : ose_fsm.c                                                   */
/* Description   : FSM��Ϣ                                                      */
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
* Purpose   : fsm��Ϣ�����뺯��
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       N/A                 N/A
*
*
* Return: ���ؼ��ɹ�
* Note:
*******************************************************************************/
Ose_fsm_message* ose_fsm_get_msg(UINT16 parameter_len)
{
    Ose_fsm_message* fsm_msg_ptr;

    /*����block*/
    fsm_msg_ptr = (Ose_fsm_message*)ose_get_mem(OSE_FSM_MSG_HEADER_POOL_ID, sizeof(Ose_fsm_message));

    /*���parameter_len��Ϊ0 �������ڴ�*/
    if(parameter_len != 0)
    {
        /*Ϊ��parameter�����ڴ�*/
        FSM_PARAM_PTR(fsm_msg_ptr)  = ose_get_mem(OSE_COMMON_POOL_ID,parameter_len);
        FSM_PARAM_SIZE(fsm_msg_ptr) = parameter_len;
    }
    else
    {
        /*��parameter�ṹ��Ӧ�ֶθ�ֵ,parameterû�������ڴ�*/
        FSM_PARAM_PTR(fsm_msg_ptr)  = NULL;
        FSM_PARAM_SIZE(fsm_msg_ptr) = 0;
    }
    return fsm_msg_ptr;
}
/*****************************************************************************
* Function  : ose_fsm_free_msg
* Purpose   : fsm��Ϣ���ͷź���
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       N/A                 N/A
*
*
* Return: ���ؼ��ɹ�
* Note:
*******************************************************************************/
Ose_status ose_fsm_free_msg(Ose_fsm_message* fsm_msg_ptr)
{
    OSE_ASSERT(fsm_msg_ptr != NULL);

    /*�ж��Ƿ�Ϊһ��ʽ*/
    if(((UINT32)FSM_PARAM_PTR(fsm_msg_ptr)) == ((UINT32)(fsm_msg_ptr + 1)))
    {
        ose_free_mem((UINT8*)fsm_msg_ptr);
        fsm_msg_ptr = NULL;
        return OSE_SUCCESS;
    }

    /*���parameter����������ڴ棬���ͷŵ�*/
    if(FSM_PARAM_PTR(fsm_msg_ptr) != NULL)
    {
        ose_free_mem((UINT8*)FSM_PARAM_PTR(fsm_msg_ptr));
        FSM_PARAM_PTR(fsm_msg_ptr) = NULL;
    }
    /*�ͷ���Ϣͷ*/
    ose_free_mem((UINT8*)fsm_msg_ptr);
    fsm_msg_ptr = NULL;
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_fsm_link_msg
* Purpose   : ��������fsm��Ϣ
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       N/A                 N/A
*
*
* Return: ���ؼ��ɹ�
* Note:
*******************************************************************************/
Ose_status ose_fsm_link_msg(Ose_fsm_message* fsm_msg_ptr)
{
    OSE_ASSERT(fsm_msg_ptr != NULL);

    /*�ж��Ƿ�Ϊһ��ʽ*/
    if(((UINT32)FSM_PARAM_PTR(fsm_msg_ptr)) == ((UINT32)(fsm_msg_ptr + 1)))
    {
        ose_buf_link_mem((UINT8*)fsm_msg_ptr);
        return OSE_SUCCESS;
    }

    /*���parameter����������ڴ棬��link*/
    if(FSM_PARAM_PTR(fsm_msg_ptr) != NULL)
    {
        ose_buf_link_mem((UINT8*)FSM_PARAM_PTR(fsm_msg_ptr));
    }

    /*link��Ϣͷ*/
    ose_buf_link_mem((UINT8*)fsm_msg_ptr);

    return OSE_SUCCESS;
}