/*******************************************************************************/
/* Filename      : ose_buf.c                                                   */
/* Description   : 内存管理                                                    */
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
* Purpose   : 内存池管理模块初始化
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
*   OSE_SUCCESS 初始化成�?
*   OSE_FAILURE 初始化失�?
* Note:
*******************************************************************************/
Ose_status ose_init_pools()
{
    Ose_pool_id id;
    Ose_status  ret;

    /*初始化OSE和系统的内存分配关系*/
    //ose_init_mem();

    for(id = 0; id < OSE_MAX_POOLS; id++)
    {
        ose_pool[id].stPoolInfo.cl_nums  = 0;
        ose_pool[id].stPoolInfo.max_size = 0;
        ose_pool[id].stPoolInfo.size_map = (UINT8*)NULL;
        ose_pool[id].stPoolInfo.cl_info  = (Ose_cl_info*)NULL;
    }

    /*初始化内存池*/
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
* Purpose   : 内存池初始化
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
*   OSE_SUCCESS 初始化成�?
*   OSE_FAILURE 初始化失�?
* Note:
*******************************************************************************/
Ose_status ose_init_block_pool(Ose_pool_id pool_id)
{
    Ose_pool_info* pool_ptr = &ose_pool[pool_id].stPoolInfo;
    Ose_status     ret;

    /*按照对齐大小，调整配置大�?*/
    ose_init_pool_config(pool_id);

    /*为每个内存簇申请控制字段*/
    pool_ptr->cl_info = (Ose_cl_info*)malloc(sizeof(Ose_cl_info) * pool_ptr->cl_nums);
    if(pool_ptr->cl_info == NULL)
    {
        OSE_ERROR("ose_init_block_pool cl_info ERROR:");
        return OSE_NO_MEM;
    }
    /*申请内存大小匹配�?*/
    pool_ptr->size_map = (UINT8*)malloc((pool_ptr->max_size / OSE_MEM_POOL_PAGE_SIZE) + 1);
    if(pool_ptr->size_map == NULL)
    {
        OSE_ERROR("ose_init_block_pool size_map ERROR:");
        return OSE_NO_MEM;
    }

    /*初始化内存簇*/
    ret = ose_init_cl(pool_id);
    if(ret != OSE_SUCCESS)
    {
        return ret;
    }

    /*初始化内存block*/
    ose_init_block(pool_id);

    /*初始化内存匹配表*/
    ose_init_block_map(pool_id);

    /*初始化互斥量*/
    ose_init_pool_lock(pool_id);

    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_init_pool_config
* Purpose   : 内存簇配置的初始�?
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
            *   为保证每个用户区的起始和结束地址都能整除uiMemAlignSize
            *
            *   |-uiOseHeadSize--|__________user_____________|-uiOseHeadSize--|__________user_____________|
            *
            *   首先保证ose头和用户区大小都是uiMemAlignSize的整数�?
            *   只要使第一个用户区起始为uiMemAlignSize整数�?
            *   那后续的所有用户区起始地址都可以保�?
        */

    /*前后向对齐的大小*/
    uiMemAlignSize = ose_pool[pool_id].UiAlignSize;

    /*计算内存池的最大block，以及簇数量*/
    while((tbl_ptr->block_size != 0) && (tbl_ptr->block_nums != 0))
    {
        /*计算block扩展后的大小*/
        tbl_ptr->block_size = (OSE_MEM_ALIGN(tbl_ptr->block_size, uiMemAlignSize));

        /*包括ose保留头的大小*/
        tbl_ptr->block_size += ose_pool[pool_id].uiReserveOseHead;

        /*簇的数量�?1*/
        pool_ptr->cl_nums++;

        /*后面簇的大小小于前面，配置表错误*/
        OSE_ASSERT(tbl_ptr->block_size >= pool_ptr->max_size);

        /*记录下本池最大的block大小*/
        pool_ptr->max_size = tbl_ptr->block_size;

        tbl_ptr++;
    }
    /*一个内存池必须至少一个簇*/
    OSE_ASSERT(pool_ptr->cl_nums != 0);
}
/*****************************************************************************
* Function  : ose_init_cl
* Purpose   : 初始化内存簇控制字段
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
*   OSE_SUCCESS 初始化成�?
*   OSE_FAILURE 初始化失�?
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

    /*用户区起始的对齐大小*/
    uiMemAlignSize = ose_pool[pool_id].UiAlignSize;

    /*用户区前面的ose控制头大�?*/
    uiOseHeadSize = ose_pool[pool_id].uiReserveOseHead + sizeof(Ose_block*);
    /*为了保证用户区的前后地址对齐uiMemAlignSize，对uiOseHeadSize进行取整*/
    uiOseHeadSize = OSE_MEM_ALIGN(uiOseHeadSize, uiMemAlignSize);
    /*不能为了简�?,而更新全局变量里的uiReserveOseHead*/
    /*    uiOseHeadSize、uiMemAlignSize和uiReserveOseHead等关�?
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

    /*初始化内存簇控制字段*/
    for(cl_index = 0; cl_index < pool_ptr->cl_nums; cl_index++)
    {
        /*初始化簇控制字段*/
        pool_ptr->cl_info[cl_index].blk_nums = tbl_ptr->block_nums;
        pool_ptr->cl_info[cl_index].blk_size = tbl_ptr->block_size;
        pool_ptr->cl_info[cl_index].free_num = tbl_ptr->block_nums;

        /*一个block对应一个控制头,最后预留一对ose的控制字�?*/
        /*多申请uiMemAlignSize大小的内存，
                *一定能让第一个用户区起始地址被uiMemAlignSize整除*/

        /*为block控制头申请内�?*/
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
        /*为内存簇的block申请内存*/
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

        /*下一个簇的配�?*/
        tbl_ptr++;
    }
    ose_trace(OSE_TRACE_INFO,"%d mem pool total size:%f KB",pool_id,mem_total_size/1024);
    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_init_block
* Purpose   : 初始化block
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

    Ose_block*  pstBlock;  /*ose控制头的地址*/
    Ose_block** ppstBlock; /*用户区前面的控制头，它指向真实的ose控制�?*/

    Ose_pool_info* pool_ptr = &ose_pool[pool_id].stPoolInfo;

    uiMemAlignSize = ose_pool[pool_id].UiAlignSize;

    /*为了保证用户区的前后地址对齐uiMemAlignSize，对uiOseHeadSize进行取整*/
    uiOseHeadSize = ose_pool[pool_id].uiReserveOseHead + sizeof(Ose_block*);
    uiOseHeadSize = OSE_MEM_ALIGN(uiOseHeadSize, uiMemAlignSize);

    for(cl_index = 0; cl_index < pool_ptr->cl_nums; cl_index++)
    {
        /*取得第一个block的地址*/
        ppstBlock = (Ose_block**)pool_ptr->cl_info[cl_index].blk_user_head_ptr;

        /*为第一个block偏移，使得它的用户区起始能整除uiMemAlignSize*/
        ppstBlock = (Ose_block**)OSE_MEM_ALIGN(((UINT32)ppstBlock) + uiOseHeadSize, uiMemAlignSize);
        ppstBlock = (Ose_block**)(((UINT32)ppstBlock) - ose_pool[pool_id].uiReserveOseHead - sizeof(Ose_block*));

        /*取得第一个block控制头的地址*/
        pstBlock = pool_ptr->cl_info[cl_index].blk_cont_head_ptr;

        /*一个pstBlock对应一个ppstBlock*/
        for(blk_index = 0; blk_index < pool_ptr->cl_info[cl_index].blk_nums; blk_index++)
        {
            /*用户区前面一个指针指向真实的block控制�?*/
            *ppstBlock = pstBlock;

            /*对block控制头进行初始化*/
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

            /*两个block控制字段向后�?*/

            /*用户区的控制�?*/
            ppstBlock = (Ose_block**)(pstBlock->usr_data + pool_ptr->cl_info[cl_index].blk_size + uiOseHeadSize);
            ppstBlock = (Ose_block**)(((UINT32)ppstBlock) - ose_pool[pool_id].uiReserveOseHead - sizeof(Ose_block*));

            /*ose真实控制头向后移*/
            pstBlock++;
        }
        /*最后一个有效block控制头的下一节点为空*/
        pstBlock--;
        pstBlock->next_blk_ptr = NULL;

        /*设置最后预留的一对控制头*/
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
* Purpose   : 内存匹配表的初始�?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       N/A                 N/A
*
*
* Return: �?
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

    /*内存匹配表里，一个字节代码一个索�?*/
    /*一个索引至少代表索引乘以OSE_MEM_POOL_PAGE_SIZE大小的内�?*/
    for(map_index = 0; map_index <= map_index_end; map_index++)
    {
        /*计算当前索引需要的内存大小*/
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
* Purpose   : 一个内存池一个互斥量，该函数负责初始化互斥量
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
        /*该内存池不需要互�?*/
        return;
    }

/*该内存池需要互�?*/
#ifdef LINUX_SWITCH
    ose_pool[pool_id].uiPoolLock = ose_create_mutex("POOLLOCK", OSE_TRUE);
#endif
}
/*外部接口*/
/*****************************************************************************
* Function  : ose_buf_get_mem
* Purpose   : 申请内存block
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
*   非NULL 成功
*   其他不返�?
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
* Purpose   : 申请内存block
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
*   非NULL 成功
*   NULL失败
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

    /*取得内存池内存簇信息*/
    pool_ptr = &ose_pool[pool_id].stPoolInfo;
    cl_info  = pool_ptr->cl_info;
    cl_nums  = pool_ptr->cl_nums;

    /*超过内存池的最大block */
    if(size > pool_ptr->max_size)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_buf_get_mem_allow_nullptr]: get max size !!!");
        return NULL;
    }

    blk_ptr = (Ose_block*)NULL;
    /*计算查找的初始索�?*/
    cl_index = pool_ptr->size_map[size / OSE_MEM_POOL_PAGE_SIZE];
    /*进入互斥�?*/
    ose_pool_lock_get(pool_id);

    for(; cl_index < cl_nums; cl_index++)
    {
#if OSE_MEM_POOL_PAGE_SIZE > 1
        if(cl_info[cl_index].blk_size >= size)
#endif
        {
            if(cl_info[cl_index].free_num > 0)
            {
                /*找到合适的内存*/
                blk_ptr = cl_info[cl_index].free_head_blk_ptr;
                cl_info[cl_index].free_num--;

                /*先将该block从簇控制链表里删�?*/
                cl_info[cl_index].free_head_blk_ptr = cl_info[cl_index].free_head_blk_ptr->next_blk_ptr;
                if(cl_info[cl_index].free_head_blk_ptr == NULL)
                {
                    /*只有当前这个block*/
                    cl_info[cl_index].free_tail_blk_ptr = NULL;
                }
                break;
            }
        }
    }

    ose_pool_lock_put(pool_id);

    if(blk_ptr != (Ose_block*)NULL)
    {
        /*申请成功*/
        blk_ptr->used_cnt = 1;

        return (void*)blk_ptr->usr_data;
    }
    else
    {
        /*本内存池内存耗尽*/
        return NULL;
    }
}
/*****************************************************************************
* Function  : ose_buf_free_mem
* Purpose   : 释放内存block
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

    /*是否为空*/
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
    /*是否重复释放*/
    if(blk_ptr->used_cnt == 0)
    {
        is_unused_block = OSE_TRUE;
    }
    else
    {
        is_unused_block = OSE_FALSE;

        blk_ptr->used_cnt--;

        /*保留释放者信�?*/
        if(blk_ptr->used_cnt == 0)
        {
            cl_info_ptr->free_num++;
            /*将结点放在空闲链表头�?*/
            if(cl_info_ptr->free_head_blk_ptr == NULL)
            {
                /*链表无结�?*/
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

    /*重复释放*/
    if(is_unused_block == OSE_TRUE)
    {
        ose_trace(OSE_TRACE_ERROR,"[ose_buf_free_mem]: free unused !!!");
        return OSE_FAILURE;
    }

    return OSE_SUCCESS;
}
/*****************************************************************************
* Function  : ose_buf_link_mem
* Purpose   : 复用内存block
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

    /*是否为空*/
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

    /*没有分配的内�?*/
    if(blk_ptr->used_cnt == 0)
    {
        ose_pool_lock_put(blk_ptr->pool_id);
        ose_trace(OSE_TRACE_ERROR,"[ose_buf_link_mem]: link unused !!!");
        return OSE_FAILURE;
    }

    blk_ptr->used_cnt++;

    /*检查cut是否超限*/
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
* Purpose   : 删除内存管理所有资�?
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
            /*释放block以及控制头空�?*/
            free((UINT8*)(ose_pool[id].stPoolInfo.cl_info[cl_index].blk_cont_head_ptr));
            (*ose_pool[id].pfFreeBlockMem)((UINT8*)(ose_pool[id].stPoolInfo.cl_info[cl_index].blk_user_head_ptr));
        }
        /*释放map空间*/
        free((UINT8*)(ose_pool[id].stPoolInfo.size_map));
        /*释放内存池的簇控制字�?*/
        free((UINT8*)(ose_pool[id].stPoolInfo.cl_info));
        /*初始化内存池字段*/
        ose_pool[id].stPoolInfo.cl_nums  = 0;
        ose_pool[id].stPoolInfo.max_size = 0;
        ose_pool[id].stPoolInfo.size_map = (UINT8*)NULL;
        ose_pool[id].stPoolInfo.cl_info  = (Ose_cl_info*)NULL;
        ose_trace(OSE_TRACE_INFO,"free mem pool id %d.",id);
    }
    return OSE_SUCCESS;
}