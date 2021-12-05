/*******************************************************************************/
/* Filename      : ose_buf.h                                                   */
/* Description   : 内存管理                                                     */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_BUF_H
#define OSE_BUF_H

/*�ڴ�������ṹ*/
typedef struct Ose_mem_desc_tbl_st
{
    UINT32 block_size;
    UINT32 block_nums;
} Ose_mem_desc_tbl;

/*block�����ֶ�*/
typedef struct Ose_block_st
{
    UINT8                reserved; /*�����ֶ�*/
    UINT8                pool_id;
    UINT8                cl_id;
    UINT8                used_cnt;
    struct Ose_block_st* next_blk_ptr; /*ָ����һ��block*/

#ifdef FEA_BUFFER_LEAK_2_SWITCH
    UINT32 getfile;                          /*�����ߵ��ļ���*/
    UINT32 linkfreefile;                     /*��used_cntλ0ʱ�����ͷ��ߵ��ļ���;����0λlink�ߵ��ļ���*/
    UINT16 getline;                          /*�����ߵ��к�*/
    UINT16 linkfreeline;                     /*��used_cntλ0ʱ�����ͷ��ߵ��к�;����0λlink�ߵ��к�*/
    UINT32 real_size;                        /*������ڴ����ʵ����*/
    /*Uint16                  instance_id;*/ /*���ò����ԣ��ռ��ϵͳʱ��*/
    UINT32 systime;
#endif

    UINT8* usr_data;
} Ose_block;

/*�ڴ�ؿ����ֶ�*/
typedef struct Ose_cl_info_st
{
    UINT32 blk_nums; /*�ڴ����block������*/
    UINT32 blk_size; /*ÿ��block�Ĵ�С*/

    Ose_block* blk_cont_head_ptr; /*block����ͷ����ʼ��ַ*/
    void*      blk_user_head_ptr; /*block�û��������ʼ��ַ*/

    UINT32     free_num;          /*����block������*/
    Ose_block* free_head_blk_ptr; /*�����ڴ�block��ͷ���*/
    Ose_block* free_tail_blk_ptr; /*�����ڴ��β�����*/
} Ose_cl_info;

/*�ڴ�ؿ����ֶ�*/
typedef struct Ose_pool_info_st
{
    UINT32       cl_nums;  /*�ڴ�صĸ���*/
    UINT32       max_size; /*���ڴ���ṩ�����block*/
    UINT8*       size_map; /*�ڴ�ƥ����ַ*/
    Ose_cl_info* cl_info;  /*��һ���ڴ�ؿ����ֶεĵ�ַ*/
} Ose_pool_info;

/*�����ڴ��ʱ��Ҫ��������/�ͷ��ڴ�*/
/*��ͬ�ڴ�ض�ʹ�õ��ⲿ�ڴ�������*/
/*��˿��Ը���ʵ���ⲿ�ڴ��������������ͷź���*/

/*�ڴ�block��������ͷź���*/
typedef void* (*Ose_malloc_block_mem_pf)(UINT32 size);
typedef void (*Ose_free_block_mem_pf)(UINT8* buf_ptr);

/*�ڴ�ص�����*/
typedef struct Ose_pool_st
{
    Ose_mem_desc_tbl*       pstPoolDesc;
    Ose_pool_info           stPoolInfo;
    Ose_pool_id             ucPoolId; /*�����0��ʼ���*/
    /***********�ڴ������*********/
    Bool                    bPoolLock; /*�Ƿ񻥳�,OSE_FALSE��ʾ������*/
    Ose_mutex_id            uiPoolLock;
    UINT32                  UiAlignSize; /*ÿ��block��ʼ�ͽ�����ַ������ֽ���*/
    UINT32                  uiReserveOseHead; /*ÿ��ose����ͷ���Ᵽ�����ֽ���*/
    Ose_malloc_block_mem_pf pfMallocBlockMem; /*����block���ڴ�ռ�*/
    Ose_free_block_mem_pf   pfFreeBlockMem;
} Ose_pool;

/*block peak*/
typedef struct Ose_block_peak_st
{
    UINT32 exp_peak_num;
    UINT32 exp_used_num;
    UINT32 act_peak_num;
    UINT32 act_used_num;
} Ose_block_peak;

#define HL_INSTANCE_ID   0

/*OSE���ڴ�block������־*/
#define OSE_BLK_RESERVED_VALUE     0xcf
/*link������������*/
#define OSE_MAX_LINK_BUFF_TIMES    250
/*OSE�ڴ�ص���С���Ҿ���*/
#define OSE_MEM_POOL_PAGE_SIZE     1 /*128*/
/*�����ڴ����ʱ��һ��������������*/
#define OSE_MULTI_MEM_MAX_NUM      64
/*�ڲ���*/ /*��������*/
#define OSE_BLOCK_CHECK            0
#define OSE_BLOCK_PRINT            1
#define OSE_BLOCK_GET              0
#define OSE_BLOCK_LINK             1
#define OSE_BLOCK_FREE             2
/*����ȡ��*/
#define OSE_MEM_ALIGN(size, agsize) ((((size) + (agsize - 1)) / agsize) * agsize)

/*�ڴ�ػ���*/
#ifdef LINUX_SWITCH
#define ose_pool_lock_define
#define ose_pool_lock_get(pid)  \
    if(ose_pool[pid].bPoolLock) \
    ose_obtain_mutex(ose_pool[pid].uiPoolLock, OSE_WAIT_FOREVER)
#define ose_pool_lock_put(pid)  \
    if(ose_pool[pid].bPoolLock) \
    ose_release_mutex(ose_pool[pid].uiPoolLock)
#endif

#ifdef __cplusplus
extern "C" {
#endif
Ose_status ose_init_pools(void);
Ose_status ose_init_block_pool(Ose_pool_id);
void       ose_init_pool_config(Ose_pool_id);
Ose_status ose_init_cl(Ose_pool_id);
void       ose_init_block(Ose_pool_id);
void       ose_init_block_map(Ose_pool_id);
void       ose_init_pool_lock(Ose_pool_id);
Ose_status ose_buf_delete_all(void);
Ose_status ose_buf_link_mem(UINT8*);
#ifdef __cplusplus
}
#endif

#endif /*OSE_BUF_H*/