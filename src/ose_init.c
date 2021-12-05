/*******************************************************************************/
/* Filename      : ose_init.c                                                  */
/* Description   : ϵͳ��ʼ��                                                  */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#include <ose_all.h>
#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 3)

extern Ose_task_id       g_ose_start_sequence[];
extern Ose_semaphore     ose_semaphore[OSE_MAX_SEMAPHORES];
/*������������OSE��ʼ��ʱ�������������ñ��ʼ��*/
Ose_task_desc_tbl g_ose_task_desc_tbl[OSE_MAX_TASKS] = {
    { 0, 0, 0, 0, OSE_TRUE, NULL, { NULL, NULL, NULL, NULL }, NULL, 0, 0 }
};
/*���ڳ�ʼ��ʱ����׼����ͳ�ʼ�������ͨ��*/
Ose_sema_id         g_ose_init_sema  = OSE_UNAVAILABLE_ID;
Ose_mutex_id        g_ose_init_mutex = OSE_UNAVAILABLE_ID;
/*0Ϊδ��ɳ�ʼ����1Ϊ���*/
UINT8               g_ose_task_init_flag[OSE_MAX_TASKS];
Ose_sema_id         ose_exit_sema = OSE_UNAVAILABLE_ID;

/*****************************************************************************
* Function  : ose_kernel_init
* Purpose   : OSE��ʼ��
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return:
*   OSE_SUCCESS ��ʼ���ɹ�
*   OSE_FAILURE ��ʼ��ʧ��
* Note:
*******************************************************************************/
Ose_status ose_kernel_init(void)
{
    Ose_status ret;
    /*OSEϵͳ��Դ��ʼ��*/
    ret = ose_init_system_res();
    if(ret != OSE_SUCCESS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_kernel_init]: system res failure !!!");
        return OSE_FAILURE;
    }

    /*OSE��ʼ���ϲ������ִ�л���*/
    ret = ose_init_user_res();
    if(ret != OSE_SUCCESS)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_kernel_init]: user res failure !!!");
        return OSE_FAILURE;
    }
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_kernel_exit
* Purpose   : OSE�˳�
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return:
*   OSE_SUCCESS OSE�˳��ɹ�
*   OSE_FAILURE OSE�˳�ʧ��
* Note:
*******************************************************************************/
Ose_status ose_kernel_exit()
{
    /*�����ģ��ʹ�õĻ��������ź�����Դ*/
    g_ose_init_sema  = OSE_UNAVAILABLE_ID;
    g_ose_init_mutex = OSE_UNAVAILABLE_ID;

    /*ɾ����������*/
    if(ose_task_delete_all() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }

    /*ɾ���������������*/
    if(ose_mb_delete_all() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }

    /*ɾ�����������ź���*/
    if(ose_sema_delete_all() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }

    /*ɾ��������ʹ�û�����*/
    if(ose_mutex_delete_all() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*ɾ���ڴ�أ��ͷ��ڴ��OS*/
    if(ose_buf_delete_all() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*ɾ����ʹ�õĶ�ʱ��*/
    if(ose_timer_delete_all() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_init_system_res
* Purpose   : ��ʼ��OSE��ϵͳ��Դ
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return: OSE��״̬
* Note:
*******************************************************************************/
Ose_status ose_init_system_res()
{
    /*��ʼ���������*/
    if(ose_init_task() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*��ʼ������������ģ��*/
    if(ose_init_mutex() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*��ʼ��OSE���ڴ����ģ��*/
    if(ose_init_pools() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*��ʼ���ź�������ģ��*/
    if(ose_init_sema() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*��ʼ�����������*/
    if(ose_init_mb() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    /*��ʼ����ʱ��*/
    if(ose_init_timer() != OSE_SUCCESS)
    {
        return OSE_FAILURE;
    }
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_init_user_res
* Purpose   : OSE��ʼ���ϲ������ִ�л���
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return: OSE��״̬
* Note:
*******************************************************************************/
Ose_status ose_init_user_res()
{
    UINT32          loop;
    UINT32          mb_size;
    Ose_task_id     task_id;
    Ose_status      ret_status;
    Bool            wait_flag = OSE_FALSE;

    /*����һ����ʼcountΪ0���ź���*/
    g_ose_init_sema = ose_create_sema((Ose_sema_name) "OseInitSema", 1, OSE_FALSE);
    if(g_ose_init_sema == OSE_UNAVAILABLE_ID)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_init_user_res]: sema unavailable id !!!");
        return OSE_FAILURE;
    }

    /*����һ��������������g_ose_task_init_flag*/
    g_ose_init_mutex = ose_create_mutex((Ose_mutex_name) "OseInitMutex", OSE_TRUE);
    if(g_ose_init_mutex == OSE_UNAVAILABLE_ID)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_init_user_res]: mutex unavailable id !!!");
        return OSE_FAILURE;
    }
    ose_create_task_signal();
    /*���������ʼ��δ����ɣ���Ҫ�����ı�׼����Ĭ��δ���*/
    memset((void*)g_ose_task_init_flag, 1, sizeof(g_ose_task_init_flag));

    /*������׼��������������*/
    for(loop = 0; loop <= OSE_MAX_TASKS; loop++)
    {
        if(g_ose_start_sequence[loop] >= OSE_MAX_TASKS)
        {
            break;
        }

        /*�õ���Ҫ����������id*/
        task_id                       = g_ose_start_sequence[loop];
        g_ose_task_init_flag[task_id] = 0;
        wait_flag                     = OSE_TRUE;

        /*�������������*/
        mb_size = g_ose_task_desc_tbl[task_id].task_mb_size;
        if(mb_size == (UINT32)-1)
        {
            /*OSE���������С*/
            ret_status = ose_create_mb(task_id, ((1 + g_ose_task_desc_tbl[task_id].task_pri / 10) * 20));
        }
        else if(mb_size == 0)
        {
            /*���������������*/
            ret_status = OSE_SUCCESS;
        }
        else
        {
            /*�����ֵ���������С*/
            ret_status = ose_create_mb(task_id, mb_size);
        }

        if(OSE_SUCCESS != ret_status)
        {
            ose_trace(OSE_TRACE_ERROR,"[ose_init_user_res]: create mb fail !!!");
            return OSE_FAILURE;
        }
    }

    /*������׼������һ�ȴ���ʼ�����*/
    for(loop = 0; loop <= OSE_MAX_TASKS; loop++)
    {
        if(g_ose_start_sequence[loop] >= OSE_MAX_TASKS)
        {
            break;
        }

        /*�õ���Ҫ����������id*/
        task_id = g_ose_start_sequence[loop];

        //ת����������
        /*�������񵫲��������������*/
        if(ose_create_task(task_id,
                           (Ose_task_entry)ose_task_entry,
                           g_ose_task_desc_tbl[task_id].task_pri,
                           g_ose_task_desc_tbl[task_id].stack_size,
                           (Ose_task_param)task_id,
                           0,
                           g_ose_task_desc_tbl[task_id].task_name) != OSE_SUCCESS)
        {
            ose_trace(OSE_TRACE_ERROR,"[ose_init_user_res]: create task fail !!!");
            return OSE_FAILURE;
        }
    }
    if(wait_flag == OSE_TRUE)
    {
        /*�ȴ����洴�������񶼳�ʼ��������ټ������ִ��*/
        ose_obtain_sema(g_ose_init_sema, OSE_WAIT_FOREVER);
    }
    /*ɾ���û��������ź�����Դ*/
    ose_delete_sema(g_ose_init_sema);
    ose_delete_mutex(g_ose_init_mutex);
    /*ִ����ϣ����سɹ�*/
    return OSE_SUCCESS;
}