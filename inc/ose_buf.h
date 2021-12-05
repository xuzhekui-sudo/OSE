/*******************************************************************************/
/* Filename      : ose_buf.h                                                   */
/* Description   : 鍐呭瓨绠＄悊                                                     */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_BUF_H
#define OSE_BUF_H

/*内存簇描述结构*/
typedef struct Ose_mem_desc_tbl_st
{
    UINT32 block_size;
    UINT32 block_nums;
} Ose_mem_desc_tbl;

/*block控制字段*/
typedef struct Ose_block_st
{
    UINT8                reserved; /*保留字段*/
    UINT8                pool_id;
    UINT8                cl_id;
    UINT8                used_cnt;
    struct Ose_block_st* next_blk_ptr; /*指向下一个block*/

#ifdef FEA_BUFFER_LEAK_2_SWITCH
    UINT32 getfile;                          /*申请者的文件号*/
    UINT32 linkfreefile;                     /*当used_cnt位0时，表释放者的文件号;大于0位link者的文件号*/
    UINT16 getline;                          /*申请者的行号*/
    UINT16 linkfreeline;                     /*当used_cnt位0时，表释放者的行号;大于0位link者的行号*/
    UINT32 real_size;                        /*申请该内存的真实长度*/
    /*Uint16                  instance_id;*/ /*作用不明显，空间给系统时间*/
    UINT32 systime;
#endif

    UINT8* usr_data;
} Ose_block;

/*内存簇控制字段*/
typedef struct Ose_cl_info_st
{
    UINT32 blk_nums; /*内存簇里block的数量*/
    UINT32 blk_size; /*每个block的大小*/

    Ose_block* blk_cont_head_ptr; /*block控制头的起始地址*/
    void*      blk_user_head_ptr; /*block用户区域的起始地址*/

    UINT32     free_num;          /*空闲block的数量*/
    Ose_block* free_head_blk_ptr; /*空闲内存block的头结点*/
    Ose_block* free_tail_blk_ptr; /*空闲内存的尾部结点*/
} Ose_cl_info;

/*内存池控制字段*/
typedef struct Ose_pool_info_st
{
    UINT32       cl_nums;  /*内存簇的个数*/
    UINT32       max_size; /*该内存池提供的最大block*/
    UINT8*       size_map; /*内存匹配表地址*/
    Ose_cl_info* cl_info;  /*第一个内存簇控制字段的地址*/
} Ose_pool_info;

/*构造内存池时需要对外申请/释放内存*/
/*因不同内存池对使用的外部内存有需求*/
/*因此可以根据实际外部内存的情况定制申请释放函数*/

/*内存block的申请和释放函数*/
typedef void* (*Ose_malloc_block_mem_pf)(UINT32 size);
typedef void (*Ose_free_block_mem_pf)(UINT8* buf_ptr);

/*内存池的描述*/
typedef struct Ose_pool_st
{
    Ose_mem_desc_tbl*       pstPoolDesc;
    Ose_pool_info           stPoolInfo;
    Ose_pool_id             ucPoolId; /*必须从0开始编号*/
    /***********内存池属性*********/
    Bool                    bPoolLock; /*是否互斥,OSE_FALSE表示不互斥*/
    Ose_mutex_id            uiPoolLock;
    UINT32                  UiAlignSize; /*每个block起始和结束地址对齐的字节数*/
    UINT32                  uiReserveOseHead; /*每个ose控制头额外保留的字节数*/
    Ose_malloc_block_mem_pf pfMallocBlockMem; /*申请block的内存空间*/
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

/*OSE的内存block保留标志*/
#define OSE_BLK_RESERVED_VALUE     0xcf
/*link的最大次数限制*/
#define OSE_MAX_LINK_BUFF_TIMES    250
/*OSE内存池的最小查找精度*/
#define OSE_MEM_POOL_PAGE_SIZE     1 /*128*/
/*批量内存操作时，一次最多操作的数量*/
#define OSE_MULTI_MEM_MAX_NUM      64
/*内部宏*/ /*操作类型*/
#define OSE_BLOCK_CHECK            0
#define OSE_BLOCK_PRINT            1
#define OSE_BLOCK_GET              0
#define OSE_BLOCK_LINK             1
#define OSE_BLOCK_FREE             2
/*向上取整*/
#define OSE_MEM_ALIGN(size, agsize) ((((size) + (agsize - 1)) / agsize) * agsize)

/*内存池互斥*/
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