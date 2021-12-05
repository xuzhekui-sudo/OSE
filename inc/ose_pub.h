/*******************************************************************************/
/* Filename      : ose_pub.h                                                   */
/* Description   : OSE的接口头文件                                               */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_PUB_H
#define OSE_PUB_H

#include <ose_con.h>
#include <ose_errno.h>
#include <ose_type.h>
#include <ose_utl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h> 
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <stddef.h>
#include <execinfo.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ether.h> 
#include <arpa/inet.h>
#include <math.h>
#include <limits.h>
#include <stdarg.h>
#include <termios.h>
#include <ctype.h>

#define __HM_SLOT_TEST__                    0       /*1 open,  0 close*/

/*have to open __HM_SLOT_TEST__ and close __HM_SLOT_TEST_SPESIFIC_FRAME__*/
#define __HM_SLOT_TEST_WITH_SPLIT__         0       /*1 open,  0 close*/

/*have to open __HM_SLOT_TEST__  and close __HM_SLOT_TEST_WITH_SPLIT__*/
#define __HM_SLOT_TEST_SPESIFIC_FRAME__     0       /*1 open,  0 close*/

/*close all relevant macro if no __HM_SLOT_TEST__*/
#if !__HM_SLOT_TEST__
#define __HM_SLOT_TEST_WITH_SPLIT__         0       /*alwasy 0 to close, do not change it*/
#define __HM_SLOT_TEST_SPESIFIC_FRAME__     0       /*alwasy 0 to close, do not change it*/
#endif

typedef SINT32 Ose_status;
typedef SINT32 Ose_timeout;
typedef UINT32 Ose_timestamp;
typedef UINT8  Ose_task_id;
typedef UINT8  Ose_process_id;
typedef UINT32 Ose_prim_id;
typedef UINT16 Ose_instance_id;
typedef pthread_cond_t Ose_thread_cond;
//init
#ifdef __cplusplus
extern "C" {
#endif
Ose_status ose_kernel_init(void);
Ose_status ose_kernel_exit(void);
#ifdef __cplusplus
}
#endif
//sema
/*信号量id*/
typedef UINT32 Ose_sema_id;
/*信号量名�?*/
typedef char const* Ose_sema_name;
/*信号量最大计�?*/
typedef UINT32 Ose_sema_count;

#ifdef __cplusplus
extern "C" {
#endif
Ose_sema_id ose_create_sema(Ose_sema_name, Ose_sema_count, Bool);
Ose_status  ose_obtain_sema(Ose_sema_id, Ose_timeout);
Ose_status  ose_release_sema(Ose_sema_id);
Ose_status  ose_delete_sema(Ose_sema_id);
#ifdef __cplusplus
}
#endif
//mutex
/*互斥量id*/
typedef UINT32 Ose_mutex_id;
/*互斥量名�?*/
typedef char const* Ose_mutex_name;

#ifdef __cplusplus
extern "C" {
#endif
Ose_mutex_id ose_create_mutex(Ose_mutex_name, Bool);
Ose_status   ose_obtain_mutex(Ose_mutex_id, Ose_timeout);
Ose_status   ose_release_mutex(Ose_mutex_id);
Ose_status   ose_delete_mutex(Ose_mutex_id);
#ifdef __cplusplus
}
#endif
//FSM消息
typedef struct Ose_parameters_st
{
    UINT16   parameter_len;
    void*    parameter_ptr;
} Ose_parameters;
/*fsm消息定义*/
typedef struct Ose_fsm_message_st
{
    Ose_task_id    src_task_id;
    Ose_process_id src_process_id;
    Ose_task_id    dest_task_id;
    Ose_process_id dest_process_id;
    Ose_prim_id    prim_id;
    Ose_parameters parameters;
    Ose_timestamp  timestamp;
} Ose_fsm_message;
/*获取fsm消息中源任务id*/
#define FSM_SRC_TASK_ID(fsm_msg_ptr) ((fsm_msg_ptr)->src_task_id)
/*获取fsm消息中源进程id*/
#define FSM_SRC_PROCESS_ID(fsm_msg_ptr) ((fsm_msg_ptr)->src_process_id)
/*获取fsm消息中目的任务id*/
#define FSM_DEST_TASK_ID(fsm_msg_ptr) ((fsm_msg_ptr)->dest_task_id)
/*获取fsm消息中目的进程id*/
#define FSM_DEST_PROCESS_ID(fsm_msg_ptr) ((fsm_msg_ptr)->dest_process_id)
/*parameters结构相关�?*/
#define FSM_PRIMITIVE_ID(fsm_msg_ptr) ((fsm_msg_ptr)->prim_id)
#define FSM_PARAM_SIZE(fsm_msg_ptr) ((fsm_msg_ptr)->parameters.parameter_len)
#define FSM_PARAM_PTR(fsm_msg_ptr) ((fsm_msg_ptr)->parameters.parameter_ptr)
#define FSM_TIMESTAMP(fsm_msg_ptr) ((fsm_msg_ptr)->timestamp)

#ifdef __cplusplus
extern "C" {
#endif
Ose_fsm_message* ose_fsm_get_msg(UINT16);
Ose_status       ose_fsm_free_msg(Ose_fsm_message*);
Ose_status       ose_fsm_link_msg(Ose_fsm_message*);
#ifdef __cplusplus
}
#endif

//list
/*固定长度队列控制字段*/
typedef struct Ose_fixed_queue_st
{
    UINT32  head;      /*头部结点编号*/
    UINT32  tail;      /*尾部结点*/
    UINT32  free_tail; /*空闲尾部�?
                                *当队列为空时，上述三者值相�?
                                *当队列为满时，头尾不等但free_tail等于head*/
    UINT32  maxnum;    /*结点数量*/
    UINT32* node;      /*队列空间*/
} Ose_fixed_queue;

//mb
/*任务间邮箱id*/
typedef UINT8 Ose_mb_id;
/*任务内邮箱id*/
typedef Ose_fixed_queue* Ose_queue_id;
/*邮箱和任务映射关系的回调函数*/
typedef Ose_mb_id (*Ose_mb_transform_func)(Ose_fsm_message*);

#ifdef __cplusplus
extern "C" {
#endif
Ose_fsm_message* ose_get_from_mb(Ose_mb_id, Ose_timeout);
Ose_status       ose_send_message(Ose_fsm_message*);
Ose_status       ose_send_message_ex(Ose_fsm_message*, Bool);
Ose_status       ose_send_message_ex_mb(Ose_fsm_message* fsm_msg_ptr, Bool is_preferred, Bool use_mb);

#ifdef __cplusplus
}
#endif

//task
/*优先�?*/
typedef UINT8 Ose_task_pri;
/*栈大�?*/
typedef UINT16 Ose_stack_size;
/*任务入口函数的入�?*/
typedef UINT32 Ose_task_param;
/*任务入口函数的指�?*/
typedef void* (*Ose_task_entry)(void*);
/*各标准任务的初始化函数的指针*/
typedef Ose_status (*Ose_func_init)(void);
/*标准任务的主处理函数的指�?*/
typedef Ose_status (*Ose_func_main)(Ose_fsm_message*);
/*删除函数的函数指�?*/
typedef Ose_status (*Ose_func_delete)(void);
/*任务的重启函数。目前保�?*/
typedef Ose_status (*Ose_func_reset)(void);

#define OSE_PROCESS_NUMS 15
typedef struct Ose_process_desc_tbl_st
{
    Ose_task_id task_id;
    UINT8       module_nums;
    struct task_func
    {
        Ose_func_init   func_init;
        Ose_func_main   func_main;
        Ose_func_reset  func_reset;
        Ose_func_delete func_delete;
    } process_info[OSE_PROCESS_NUMS];
} Ose_process_desc_tbl;

/*任务描述�?*/
typedef struct Ose_task_desc_tbl_st
{
    Ose_task_id       task_id;
    Ose_task_pri      task_pri;
    Ose_stack_size    stack_size;
    UINT32            task_mb_size;
    SINT32            is_free_fsm_msg;
    char const*       task_name;
    struct
    {
        Ose_func_init   task_init;
        Ose_func_main   task_main;
        Ose_func_reset  task_reset; /*暂不使用*/
        Ose_func_delete task_delete;
    } task_func;
    Ose_process_desc_tbl* process_tbl_ptr; /*暂不使用*/
    Ose_mb_id             task_ext_queue;  /*任务间邮�?*/
    Ose_queue_id          task_int_queue;  /*任务内邮�?*/
} Ose_task_desc_tbl;

#ifdef __cplusplus
extern "C" {
#endif
Ose_status
           ose_create_task(Ose_task_id, Ose_task_entry, Ose_task_pri, Ose_stack_size, Ose_task_param, UINT32, const char*);
Ose_status ose_delete_task(Ose_task_id);
Bool       ose_is_task_created(Ose_task_id);
Ose_status ose_task_join();
Ose_status ose_create_task_signal();
Ose_status ose_task_signal(Ose_thread_cond*,Ose_mutex_id);
Ose_status ose_task_timedwait(Ose_thread_cond*,Ose_mutex_id);
Ose_status ose_post_task_signal();
#ifdef __cplusplus
}
#endif

//buf
/*OSE的最大内存池个数*/
#define OSE_MAX_POOLS               3
/*内存池编�?*/
/*该编号必须从0开始逐一递增*/
#define OSE_PRIVATE_POOL_ID         0
#define OSE_FSM_MSG_HEADER_POOL_ID  1
#define OSE_COMMON_POOL_ID          2

typedef UINT8 Ose_pool_id;

/*外部申请函数*/
#define ose_get_mem(pool_id, size) ose_buf_get_mem(pool_id, size, HL_INSTANCE_ID, FILEID, __LINE__)
/*外部释放函数*/
#define ose_free_mem(ptr) ose_buf_free_mem((UINT8*)ptr, FILEID, __LINE__)

#ifdef __cplusplus
extern "C" {
#endif
void*      ose_buf_get_mem(Ose_pool_id, UINT32, Ose_instance_id, UINT32, UINT32);
Ose_status ose_buf_free_mem(UINT8*, UINT32,UINT32);
void*      ose_buf_get_mem_allow_nullptr(Ose_pool_id, UINT32, Ose_instance_id, UINT32, UINT32);
#ifdef __cplusplus
}
#endif

//timer
/*定时器id*/
typedef UINT32  Ose_timer_id;
/*定时器开启时，传入的定时时间*/
typedef UINT32  Ose_timer_value;
typedef timer_t Ose_plt_timer;
typedef union sigval Ose_timer_param;
/* ose的定时器资源结构*/
typedef struct Ose_timer_st
{
    Ose_timer_id        timer_id;
    const char*         timer_name;
    Ose_plt_timer       plt_timer_id;
    Ose_timer_value     timer_value;
    void                (*callback)(Ose_timer_param);
} Ose_timer;

#ifdef __cplusplus
extern "C" {
#endif
Ose_timer_id ose_create_timer(Ose_timer*);
Ose_status   ose_start_timer(Ose_timer_id);
Ose_status   ose_stop_timer(Ose_timer_id);
Ose_status   ose_delete_timer(Ose_timer_id);
Ose_status   ose_change_timer_value(Ose_timer_id,Ose_timer_value);
#ifdef __cplusplus
}
#endif

//trace
#define OSE_TRACE_FATAL       0     /* 严重错误 */
#define OSE_TRACE_ERROR       1     /* 一般错�? */
#define OSE_TRACE_WARN        2     /* 警告 */
#define OSE_TRACE_DEBUG       3     /* 调试信息 */
#define OSE_TRACE_INFO        4     /* 一般信�? */
#define OSE_TRACE_ALL         5     /* 所有信�? */

#ifdef __cplusplus
extern "C" {
#endif
void ose_trace(UINT32 log_level, char const* arg_ptr, ...);
void ose_set_trace_level(UINT32 level);
#ifdef __cplusplus
}
#endif

/*
* @struct   LINK_LIST_NODE
* 链表节点
*/
typedef struct link_list_node
{
    void*                   data;
    struct link_list_node*  next;
}LINK_LIST_NODE;
/*
* @struct   LINK_LIST
* 链表
*/
typedef struct link_list
{
    LINK_LIST_NODE*     first;
    LINK_LIST_NODE*     last;
    SINT32              count;
    SINT32              (*equal)(void * a, void * b);
}LINK_LIST;

#ifdef __cplusplus
extern "C" {
#endif
LINK_LIST* ose_link_list_create();
void ose_link_list_free(LINK_LIST * list);
void ose_link_list_insert_tail(LINK_LIST * const list, void* const data);
void ose_link_list_insert_head(LINK_LIST * const list, void* const data);
SINT32 ose_link_list_get_size(const LINK_LIST * const list);
void* ose_link_list_remove_tail(LINK_LIST* const list);
void* ose_link_list_remove_head(LINK_LIST * const list);
void ose_link_list_insert_at(LINK_LIST * const list, void* const data, SINT32 index);
void* ose_link_list_remove_at(LINK_LIST* const list, SINT32 index);
void* ose_link_list_get_data_at(const LINK_LIST * const list, SINT32 index);
void* ose_link_list_get_data_head(const LINK_LIST * const list);
void* ose_link_list_get_data_tail(const LINK_LIST * const list);
SINT32 ose_link_list_find_data_index(const LINK_LIST * const list, void * data);
#ifdef __cplusplus
}
#endif

/*Ose_status返回�?*/
#define OSE_SUCCESS                  0  /*成功*/
#define OSE_FAILURE                  -1 /*失败*/
/*其他�?: 失败*/
#define OSE_NO_WAIT                  0
#define OSE_WAIT_FOREVER             -1
#define OSE_WAIT_TIMEOUT             107
/*无效的信号量和互斥量id*/
#define OSE_UNAVAILABLE_ID           0xFFFFFFFF
#define OSE_TRUE                     1
#define OSE_FALSE                    0
#define OSE_ASSERT(expt)             assert(expt)
#define OSE_ERROR(STR)               perror(STR)


/*OSE平台代码打印*/
#if OSE_PRINT_SW
#define OSE_PRINT(format,...)  \
    do \
    { \
        char   time_buf[25];\
        struct timeval  tv; \
        struct tm*      t; \
        gettimeofday(&tv, NULL); \
        t = localtime(&tv.tv_sec); \
        sprintf(time_buf,"%04d-%02d-%02d %02d:%02d:%02d:%03ld", \
                1900+t->tm_year, 1+t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec,tv.tv_usec/1000); \
        printf("[%s][%s] [%05d ][%s] : "format"\n", time_buf,\
                 __FILE__, __LINE__, __FUNCTION__,##__VA_ARGS__); \
    } while(0)
#else
#define OSE_PRINT(format,...)
#endif

#endif /*OSE_PUB_H*/