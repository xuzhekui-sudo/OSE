/*******************************************************************************/
/* Filename      : ose_buf.c                                                   */
/* Description   : �ڴ����                                                    */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#include <ose_all.h>

#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 1)


extern Ose_pool ose_pool[];

/*****************************************************************************
* Function  : ose_init_pools
* Purpose   : �ڴ�ع���ģ���ʼ��
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
*   OSE_SUCCESS ��ʼ���ɹ�
*   OSE_FAILURE ��ʼ��ʧ��
* Note:
*******************************************************************************/
Ose_status ose_init_pools()
{
    Ose_pool_id id;
    Ose_status  ret;

    /*��ʼ��OSE��ϵͳ���ڴ�����ϵ*/
    //ose_init_mem();

    for(id = 0; id < OSE_MAX_POOLS; id++)
    {
        ose_pool[id].stPoolInfo.cl_nums  = 0;
        ose_pool[id].stPoolInfo.max_size = 0;
        ose_pool[id].stPoolInfo.size_map = (UINT8*)NULL;
        ose_pool[id].stPoolInfo.cl_info  = (Ose_cl_info*)NULL;
    }

    /*��ʼ���ڴ��*/
    for(id = 0; id < OSE_MAX_POOLS; id++)
    {
        OSE_ASSERT(ose_pool[id].ucPoolId == id);
        ret = ose_init_block_pool(id);
        if(ret != OSE_SUCCESS)
        {
            return ret;
        }
    }
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_init_block_pool
* Purpose   : �ڴ�س�ʼ��
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
*   OSE_SUCCESS ��ʼ���ɹ�
*   OSE_FAILURE ��ʼ��ʧ��
* Note:
*******************************************************************************/
Ose_status ose_init_block_pool(Ose_pool_id pool_id)
{
    Ose_pool_info* pool_ptr = &ose_pool[pool_id].stPoolInfo;
    Ose_status     ret;

    /*���ն����С���������ô�С*/
    ose_init_pool_config(pool_id);

    /*Ϊÿ���ڴ����������ֶ�*/
    pool_ptr->cl_info = (Ose_cl_info*)malloc(sizeof(Ose_cl_info) * pool_ptr->cl_nums);
    if(pool_ptr->cl_info == NULL)
    {
        OSE_ERROR("ose_init_block_pool cl_info ERROR:");
        return OSE_NO_MEM;
    }
    /*�����ڴ��Сƥ���*/
    pool_ptr->size_map = (UINT8*)malloc((pool_ptr->max_size / OSE_MEM_POOL_PAGE_SIZE) + 1);
    if(pool_ptr->size_map == NULL)
    {
        OSE_ERROR("ose_init_block_pool size_map ERROR:");
        return OSE_NO_MEM;
    }

    /*��ʼ���ڴ��*/
    ret = ose_init_cl(pool_id);
    if(ret != OSE_SUCCESS)
    {
        return ret;
    }

    /*��ʼ���ڴ�block*/
    ose_init_block(pool_id);

    /*��ʼ���ڴ�ƥ���*/
    ose_init_block_map(pool_id);

    /*��ʼ��������*/
    ose_init_pool_lock(pool_id);

    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_init_pool_config
* Purpose   : �ڴ�����õĳ�ʼ��
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
* Note:
*******************************************************************************/
void ose_init_pool_config(Ose_pool_id pool_id)
{
    UINT32            uiMemAlignSize;
    Ose_mem_desc_tbl* tbl_ptr  = ose_pool[pool_id].pstPoolDesc;
    Ose_pool_info*    pool_ptr = &ose_pool[pool_id].stPoolInfo;

    /*
            *   Ϊ��֤ÿ���û�������ʼ�ͽ�����ַ��������uiMemAlignSize
            *
            *   |-uiOseHeadSize--|__________user_____________|-uiOseHeadSize--|__________user_____________|
            *
            *   ���ȱ�֤oseͷ���û�����С����uiMemAlignSize��������
            *   ֻҪʹ��һ���û�����ʼΪuiMemAlignSize������
            *   �Ǻ����������û�����ʼ��ַ�����Ա�֤
        */

    /*ǰ�������Ĵ�С*/
    uiMemAlignSize = ose_pool[pool_id].UiAlignSize;

    /*�����ڴ�ص����block���Լ�������*/
    while((tbl_ptr->block_size != 0) && (tbl_ptr->block_nums != 0))
    {
        /*����block��չ��Ĵ�С*/
        tbl_ptr->block_size = (OSE_MEM_ALIGN(tbl_ptr->block_size, uiMemAlignSize));

        /*����ose����ͷ�Ĵ�С*/
        tbl_ptr->block_size += ose_pool[pool_id].uiReserveOseHead;

        /*�ص�������1*/
        pool_ptr->cl_nums++;

        /*����صĴ�СС��ǰ�棬���ñ����*/
        OSE_ASSERT(tbl_ptr->block_size >= pool_ptr->max_size);

        /*��¼�±�������block��С*/
        pool_ptr->max_size = tbl_ptr->block_size;

        tbl_ptr++;
    }
    /*һ���ڴ�ر�������һ����*/
    OSE_ASSERT(pool_ptr->cl_nums != 0);
}
/*****************************************************************************
* Function  : ose_init_cl
* Purpose   : ��ʼ���ڴ�ؿ����ֶ�
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
*   OSE_SUCCESS ��ʼ���ɹ�
*   OSE_FAILURE ��ʼ��ʧ��
* Note:
*******************************************************************************/
Ose_status ose_init_cl(Ose_pool_id pool_id)
{
    UINT32 cl_index;
    UINT32 uiMemAlignSize, uiMallocSize, uiOseHeadSize;
    FLOAT32 mem_total_size = 0;

    void* temp_ptr;

    Ose_pool_info*    pool_ptr = &ose_pool[pool_id].stPoolInfo;
    Ose_mem_desc_tbl* tbl_ptr  = ose_pool[pool_id].pstPoolDesc;

    /*�û�����ʼ�Ķ����С*/
    uiMemAlignSize = ose_pool[pool_id].UiAlignSize;

    /*�û���ǰ���ose����ͷ��С*/
    uiOseHeadSize = ose_pool[pool_id].uiReserveOseHead + sizeof(Ose_block*);
    /*Ϊ�˱�֤�û�����ǰ���ַ����uiMemAlignSize����uiOseHeadSize����ȡ��*/
    uiOseHeadSize = OSE_MEM_ALIGN(uiOseHeadSize, uiMemAlignSize);
    /*����Ϊ�˼�,������ȫ�ֱ������uiReserveOseHead*/
    /*    uiOseHeadSize��uiMemAlignSize��uiReserveOseHead�ȹ�ϵ
            *
            *   |__________|--sizeof(Ose_block*)--|~~~uiReserveOseHead~~~  |
            *                                                            /\
            *                                                             |
            *                                                             |
            *                                                       user_data
            *   |                                                                                                       |
            *   |<-----------------uiOseHeadSize------------------------>|
            *   |                                                                                                       |
            *   |<---------------uiMemAlignSize * n---------------------->|
        */

    /*��ʼ���ڴ�ؿ����ֶ�*/
    for(cl_index = 0; cl_index < pool_ptr->cl_nums; cl_index++)
    {
        /*��ʼ���ؿ����ֶ�*/
        pool_ptr->cl_info[cl_index].blk_nums = tbl_ptr->block_nums;
        pool_ptr->cl_info[cl_index].blk_size = tbl_ptr->block_size;
        pool_ptr->cl_info[cl_index].free_num = tbl_ptr->block_nums;

        /*һ��block��Ӧһ������ͷ,���Ԥ��һ��ose�Ŀ����ֶ�*/
        /*������uiMemAlignSize��С���ڴ棬
                *һ�����õ�һ���û�����ʼ��ַ��uiMemAlignSize����*/

        /*Ϊblock����ͷ�����ڴ�*/
        uiMallocSize = sizeof(Ose_block) * (tbl_ptr->block_nums + 1);
        temp_ptr     = malloc(uiMallocSize);
        if(temp_ptr == NULL)
        {
            return OSE_NO_MEM;
        }
        mem_total_size += uiMallocSize;
        pool_ptr->cl_info[cl_index].blk_cont_head_ptr = (Ose_block*)temp_ptr;
        pool_ptr->cl_info[cl_index].free_head_blk_ptr = (Ose_block*)temp_ptr;
        pool_ptr->cl_info[cl_index].free_tail_blk_ptr =
            pool_ptr->cl_info[cl_index].free_head_blk_ptr + (tbl_ptr->block_nums - 1);
        /*Ϊ�ڴ�ص�block�����ڴ�*/
        uiMallocSize =
            (tbl_ptr->block_size + uiOseHeadSize - ose_pool[pool_id].uiReserveOseHead) * tbl_ptr->block_nums +
            uiMemAlignSize + uiOseHeadSize;

        temp_ptr = (*ose_pool[pool_id].pfMallocBlockMem)(uiMallocSize);
        if(temp_ptr == NULL)
        {
            return OSE_NO_MEM;
        }
        mem_total_size += uiMallocSize;
        pool_ptr->cl_info[cl_index].blk_user_head_ptr = temp_ptr;

        /*��һ���ص�����*/
        tbl_ptr++;
    }
    ose_trace(OSE_TRACE_INFO,"%d mem pool total size:%f KB",pool_id,mem_total_size/1024);
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_init_block
* Purpose   : ��ʼ��block
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
* Note:
*******************************************************************************/
void ose_init_block(Ose_pool_id pool_id)
{
    UINT32 cl_index, blk_index, uiMemAlignSize, uiOseHeadSize;

    Ose_block*  pstBlock;  /*ose����ͷ�ĵ�ַ*/
    Ose_block** ppstBlock; /*�û���ǰ��Ŀ���ͷ����ָ����ʵ��ose����ͷ*/

    Ose_pool_info* pool_ptr = &ose_pool[pool_id].stPoolInfo;

    uiMemAlignSize = ose_pool[pool_id].UiAlignSize;

    /*Ϊ�˱�֤�û�����ǰ���ַ����uiMemAlignSize����uiOseHeadSize����ȡ��*/
    uiOseHeadSize = ose_pool[pool_id].uiReserveOseHead + sizeof(Ose_block*);
    uiOseHeadSize = OSE_MEM_ALIGN(uiOseHeadSize, uiMemAlignSize);

    for(cl_index = 0; cl_index < pool_ptr->cl_nums; cl_index++)
    {
        /*ȡ�õ�һ��block�ĵ�ַ*/
        ppstBlock = (Ose_block**)pool_ptr->cl_info[cl_index].blk_user_head_ptr;

        /*Ϊ��һ��blockƫ�ƣ�ʹ�������û�����ʼ������uiMemAlignSize*/
        ppstBlock = (Ose_block**)OSE_MEM_ALIGN(((UINT32)ppstBlock) + uiOseHeadSize, uiMemAlignSize);
        ppstBlock = (Ose_block**)(((UINT32)ppstBlock) - ose_pool[pool_id].uiReserveOseHead - sizeof(Ose_block*));

        /*ȡ�õ�һ��block����ͷ�ĵ�ַ*/
        pstBlock = pool_ptr->cl_info[cl_index].blk_cont_head_ptr;

        /*һ��pstBlock��Ӧһ��ppstBlock*/
        for(blk_index = 0; blk_index < pool_ptr->cl_info[cl_index].blk_nums; blk_index++)
        {
            /*�û���ǰ��һ��ָ��ָ����ʵ��block����ͷ*/
            *ppstBlock = pstBlock;

            /*��block����ͷ���г�ʼ��*/
            pstBlock->reserved     = OSE_BLK_RESERVED_VALUE;
            pstBlock->pool_id      = pool_id;
            pstBlock->cl_id        = (UINT8)cl_index;
            pstBlock->used_cnt     = 0;
            pstBlock->usr_data     = (UINT8*)&ppstBlock[1];
            pstBlock->next_blk_ptr = pstBlock + 1;
#ifdef FEA_BUFFER_LEAK_2_SWITCH
            pstBlock->getfile      = 0;
            pstBlock->linkfreefile = 0;
            pstBlock->getline      = 0;
            pstBlock->linkfreeline = 0;
            pstBlock->real_size    = pool_ptr->cl_info[cl_index].blk_size;
            pstBlock->systime      = 0;
#endif

            /*����block�����ֶ������*/

            /*�û����Ŀ���ͷ*/
            ppstBlock = (Ose_block**)(pstBlock->usr_data + pool_ptr->cl_info[cl_index].blk_size + uiOseHeadSize);
            ppstBlock = (Ose_block**)(((UINT32)ppstBlock) - ose_pool[pool_id].uiReserveOseHead - sizeof(Ose_block*));

            /*ose��ʵ����ͷ�����*/
            pstBlock++;
        }
        /*���һ����Чblock����ͷ����һ�ڵ�Ϊ��*/
        pstBlock--;
        pstBlock->next_blk_ptr = NULL;

        /*�������Ԥ����һ�Կ���ͷ*/
        pstBlock++;
        *ppstBlock             = pstBlock;
        pstBlock->reserved     = OSE_BLK_RESERVED_VALUE;
        pstBlock->pool_id      = pool_id;
        pstBlock->cl_id        = (UINT8)cl_index;
        pstBlock->used_cnt     = 0;
        pstBlock->usr_data     = (UINT8*)&ppstBlock[1];
        pstBlock->next_blk_ptr = (Ose_block*)NULL;
#ifdef FEA_BUFFER_LEAK_2_SWITCH
        pstBlock->getfile      = 0;
        pstBlock->linkfreefile = 0;
        pstBlock->getline      = 0;
        pstBlock->linkfreeline = 0;
        pstBlock->real_size    = 0;
        pstBlock->systime      = 0;
#endif
    }
}
/*****************************************************************************
* Function  : ose_init_mem_map
* Purpose   : �ڴ�ƥ���ĳ�ʼ��
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       N/A                 N/A
*
*
* Return: ��
* Note:
*******************************************************************************/
void ose_init_block_map(Ose_pool_id pool_id)
{
    UINT32            map_index, map_index_end;
    UINT8             current_cl_index;
    UINT32            need_size;
    Ose_pool_info*    pool_ptr = &ose_pool[pool_id].stPoolInfo;
    Ose_mem_desc_tbl* tbl_ptr  = ose_pool[pool_id].pstPoolDesc;

    current_cl_index = 0;
    map_index_end    = pool_ptr->max_size / OSE_MEM_POOL_PAGE_SIZE;

    /*�ڴ�ƥ����һ���ֽڴ���һ������*/
    /*һ���������ٴ�����������OSE_MEM_POOL_PAGE_SIZE��С���ڴ�*/
    for(map_index = 0; map_index <= map_index_end; map_index++)
    {
        /*���㵱ǰ������Ҫ���ڴ��С*/
        need_size = map_index * OSE_MEM_POOL_PAGE_SIZE;

        while(current_cl_index < pool_ptr->cl_nums)
        {
            if(tbl_ptr[current_cl_index].block_size >= need_size)
            {
                pool_ptr->size_map[map_index] = current_cl_index;
                break;
            }
            else
            {
                current_cl_index++;
            }
        }
    }
}
/*****************************************************************************
* Function  : ose_init_pool_lock
* Purpose   : һ���ڴ��һ�����������ú��������ʼ��������
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
* Note:
*******************************************************************************/
void ose_init_pool_lock(Ose_pool_id pool_id)
{
    if(ose_pool[pool_id].bPoolLock == OSE_FALSE)
    {
        /*���ڴ�ز���Ҫ����*/
        return;
    }

/*���ڴ����Ҫ����*/
#ifdef LINUX_SWITCH
    ose_pool[pool_id].uiPoolLock = ose_create_mutex("POOLLOCK", OSE_TRUE);
#endif
}
/*�ⲿ�ӿ�*/
/*****************************************************************************
* Function  : ose_buf_get_mem
* Purpose   : �����ڴ�block
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
*   ��NULL �ɹ�
*   ����������
* Note:
*******************************************************************************/
void* ose_buf_get_mem(Ose_pool_id pool_id, UINT32 size, Ose_instance_id inst_id, UINT32 file, UINT32 line)
{
    void* pTemp;

    if(size == 0)
    {
        return NULL;
    }

    pTemp = ose_buf_get_mem_allow_nullptr(pool_id, size, inst_id, file, line);
    if(pTemp == NULL)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_buf_get_mem]: no mem!!!");
    }

    return pTemp;
}
/*****************************************************************************
* Function  : ose_buf_get_mem_allow_nullptr
* Purpose   : �����ڴ�block
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
*   ��NULL �ɹ�
*   NULLʧ��
* Note:
*******************************************************************************/
void* ose_buf_get_mem_allow_nullptr(Ose_pool_id pool_id, UINT32 size, Ose_instance_id inst_id, UINT32 file, UINT32 line)
{
    Ose_pool_info* pool_ptr;
    Ose_cl_info*   cl_info;
    Ose_block*     blk_ptr;
    UINT32         cl_nums;
    UINT8          cl_index;
    ose_pool_lock_define

    OSE_ASSERT(pool_id < OSE_MAX_POOLS);

    if(size == 0)
    {
        return NULL;
    }

    /*ȡ���ڴ���ڴ����Ϣ*/
    pool_ptr = &ose_pool[pool_id].stPoolInfo;
    cl_info  = pool_ptr->cl_info;
    cl_nums  = pool_ptr->cl_nums;

    /*�����ڴ�ص����block */
    if(size > pool_ptr->max_size)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_buf_get_mem_allow_nullptr]: get max size !!!");
        return NULL;
    }

    blk_ptr = (Ose_block*)NULL;
    /*������ҵĳ�ʼ����*/
    cl_index = pool_ptr->size_map[size / OSE_MEM_POOL_PAGE_SIZE];
    /*���뻥����*/
    ose_pool_lock_get(pool_id);

    for(; cl_index < cl_nums; cl_index++)
    {
#if OSE_MEM_POOL_PAGE_SIZE > 1
        if(cl_info[cl_index].blk_size >= size)
#endif
        {
            if(cl_info[cl_index].free_num > 0)
            {
                /*�ҵ����ʵ��ڴ�*/
                blk_ptr = cl_info[cl_index].free_head_blk_ptr;
                cl_info[cl_index].free_num--;

                /*�Ƚ���block�Ӵؿ���������ɾ��*/
                cl_info[cl_index].free_head_blk_ptr = cl_info[cl_index].free_head_blk_ptr->next_blk_ptr;
                if(cl_info[cl_index].free_head_blk_ptr == NULL)
                {
                    /*ֻ�е�ǰ���block*/
                    cl_info[cl_index].free_tail_blk_ptr = NULL;
                }
                break;
            }
        }
    }

    ose_pool_lock_put(pool_id);

    if(blk_ptr != (Ose_block*)NULL)
    {
        /*����ɹ�*/
        blk_ptr->used_cnt = 1;

        return (void*)blk_ptr->usr_data;
    }
    else
    {
        /*���ڴ���ڴ�ľ�*/
        return NULL;
    }
}
/*****************************************************************************
* Function  : ose_buf_free_mem
* Purpose   : �ͷ��ڴ�block
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
* Note:
*******************************************************************************/
Ose_status ose_buf_free_mem(UINT8* ptr, UINT32 file, UINT32 line)
{
    Ose_block*   blk_ptr;
    Ose_cl_info* cl_info_ptr;
    Bool         is_unused_block;
    ose_pool_lock_define

    /*�Ƿ�Ϊ��*/
    if(ptr == NULL)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_buf_free_mem]: free null !!!");
        return OSE_FAILURE;
    }
    else
    {
        blk_ptr = *(Ose_block**)(ptr - sizeof(Ose_block*));
    }

    cl_info_ptr = &ose_pool[blk_ptr->pool_id].stPoolInfo.cl_info[blk_ptr->cl_id];

    ose_pool_lock_get(blk_ptr->pool_id);
    /*�Ƿ��ظ��ͷ�*/
    if(blk_ptr->used_cnt == 0)
    {
        is_unused_block = OSE_TRUE;
    }
    else
    {
        is_unused_block = OSE_FALSE;

        blk_ptr->used_cnt--;

        /*�����ͷ�����Ϣ*/
        if(blk_ptr->used_cnt == 0)
        {
            cl_info_ptr->free_num++;
            /*�������ڿ�������ͷ��*/
            if(cl_info_ptr->free_head_blk_ptr == NULL)
            {
                /*�����޽��*/
                cl_info_ptr->free_tail_blk_ptr = blk_ptr;
                cl_info_ptr->free_head_blk_ptr = blk_ptr;
                blk_ptr->next_blk_ptr          = NULL;
            }
            else
            {
                blk_ptr->next_blk_ptr          = cl_info_ptr->free_head_blk_ptr;
                cl_info_ptr->free_head_blk_ptr = blk_ptr;
            }
        }
    }

    ose_pool_lock_put(blk_ptr->pool_id);

    /*�ظ��ͷ�*/
    if(is_unused_block == OSE_TRUE)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_buf_free_mem]: free unused !!!");
        return OSE_FAILURE;
    }

    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_buf_link_mem
* Purpose   : �����ڴ�block
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
* Note:
*******************************************************************************/
Ose_status ose_buf_link_mem(UINT8* ptr)
{
    Ose_block* blk_ptr;
    ose_pool_lock_define

    /*�Ƿ�Ϊ��*/
    if(ptr == NULL)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_buf_link_mem]: link null !!!");
        return OSE_FAILURE;
    }
    else
    {
        blk_ptr = *(Ose_block**)(ptr - sizeof(Ose_block*));
    }

    ose_pool_lock_get(blk_ptr->pool_id);

    /*û�з�����ڴ�*/
    if(blk_ptr->used_cnt == 0)
    {
        ose_pool_lock_put(blk_ptr->pool_id);
        ose_trace(OSE_TRACE_ERROR,"[ose_buf_link_mem]: link unused !!!");
        return OSE_FAILURE;
    }

    blk_ptr->used_cnt++;

    /*���cut�Ƿ���*/
    if(OSE_MAX_LINK_BUFF_TIMES < blk_ptr->used_cnt)
    {
        ose_pool_lock_put(blk_ptr->pool_id);
        ose_trace(OSE_TRACE_ERROR,"[ose_buf_link_mem]: link exceed max cut !!!");
        return OSE_FAILURE;
    }

    ose_pool_lock_put(blk_ptr->pool_id);

    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_buf_delete_all
* Purpose   : ɾ���ڴ����������Դ
* Relation  :
*
* Input Parameters:
* Return:
* Note:
*******************************************************************************/
Ose_status ose_buf_delete_all()
{
    Ose_pool_id id;
    UINT32      cl_index;

    for(id = 0; id < OSE_MAX_POOLS; id++)
    {
        for(cl_index = 0; cl_index < ose_pool[id].stPoolInfo.cl_nums; cl_index++)
        {
            /*�ͷ�block�Լ�����ͷ�ռ�*/
            free((UINT8*)(ose_pool[id].stPoolInfo.cl_info[cl_index].blk_cont_head_ptr));
            (*ose_pool[id].pfFreeBlockMem)((UINT8*)(ose_pool[id].stPoolInfo.cl_info[cl_index].blk_user_head_ptr));
        }
        /*�ͷ�map�ռ�*/
        free((UINT8*)(ose_pool[id].stPoolInfo.size_map));
        /*�ͷ��ڴ�صĴؿ����ֶ�*/
        free((UINT8*)(ose_pool[id].stPoolInfo.cl_info));
        /*��ʼ���ڴ���ֶ�*/
        ose_pool[id].stPoolInfo.cl_nums  = 0;
        ose_pool[id].stPoolInfo.max_size = 0;
        ose_pool[id].stPoolInfo.size_map = (UINT8*)NULL;
        ose_pool[id].stPoolInfo.cl_info  = (Ose_cl_info*)NULL;
        ose_trace(OSE_TRACE_INFO,"free mem pool id %d.",id);
    }
    return OSE_SUCCESS;
}