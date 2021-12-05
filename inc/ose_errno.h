/*******************************************************************************/
/* Filename      : ose_error.h                                                 */
/* Description   : 错误码宏定义                                                  */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_ERROR_H
#define OSE_ERROR_H

/***********************************************
*1000以内的错误码为OSE使用�?
*
*OSE将这些错误码全部做为严重异常使用
*
*对于错误码的报送，为避免多次报�?,OSE只在接口函数里报
*接口函数调用接口函数，调用者不报�?
********************************/
/*断言失败*/
#define OSE_ASSERT_FAIL 0

/*OSE状态异�?*/
#define OSE_STATE_ERROR 1

/*OSE初始化失�?*/
#define OSE_INIT_FAIL 2

/*任务ID非法*/
#define OSE_TASK_ID_ERROR 3

/*任务入口函数为空*/
#define OSE_TASK_ENTRY_NULL 4

/*创建的任务已经创�?*/
#define OSE_TASK_CREATED 5

/*邮箱创建失败*/
#define OSE_CREATE_MB_FAIL 6

/*系统创建任务失败*/
#define OSE_SYS_CREATE_TASK_FAIL 7

/*任务没创�?*/
#define OSE_TASK_NO_CREATED 8

/*任务优先级设置失�?*/
#define OSE_TASK_SET_PRI_FAIL 9

/*获取任务优先级时，出参位�?*/
#define OSE_TASK_GET_PTR_NULL 10

/*获取优先级失�?*/
#define OSE_TASK_GET_PRI_FAIL 11

/*创建任务邮箱(内和�?)失败*/
#define OSE_MB_CREATE_FAIL 12

/*创建邮箱时，邮箱大小�?0*/
#define OSE_MB_SIZE_ZERO 13

/*创建任务间邮箱时,邮箱id错误*/
#define OSE_MBIDOR_QID_ERROR 14

/*创建任务间邮箱时，邮箱已创建*/
#define OSE_MB_CREATED 15

/*获取任务间邮箱未创建*/
#define OSE_GET_MB_NO_CREATE 16

/*发送任务间邮箱未创�?*/
#define OSE_SEND_MB_NOCREATE 16

/*任务间邮箱id非法*/
#define OSE_MB_MID_ERROR 17

/*任务间邮箱获取失�?*/
#define OSE_MB_GET_ERROR 18

/*任务间邮箱发送失�?(队列函数返回失败)*/
#define OSE_MB_SEND_ERROR 19

/*发送消息时，目的任务的任务内邮箱没创建*/
#define OSE_QUEUE_NO_CREATED 20

/*删除任务内邮箱时，qid非法*/
#define OSE_DEL_QUEUE_ERROR 21

/*创建互斥量失�?*/
#define OSE_MUTEX_CREATE_FAIL 22

/*创建互斥量时，falg非法*/
#define OSE_MUTEX_FLAG_ERROR 23

/*互斥量操作时，id非法*/
#define OSE_MUTEX_ID_ERR 24

/*互斥量操作时，互斥量没创�?*/
#define OSE_MUTEX_NO_CREATED 25

/*互斥量获取失�?*/
#define OSE_MUTEX_GET_FAIL 26

/*创建信号量时，最大计数为0*/
#define OSE_SEMA_COUNT_ERROR 27

/*信号量操作时，id非法*/
#define OSE_SEMA_ID_ERROR 28

/*信号量操作时，没创建*/
#define OSE_SEMA_NO_CREATED 29

/*信号量获取失�?*/
#define OSE_SEMA_GET_FAIL 30

/*创建定时器时，id非法*/
#define OSE_CREATE_TIMERID_ERROR 31

/*定时器重复创�?*/
#define OSE_TIMER_CREATED 32

/*创建定时器时，定时器类型错误*/
#define OSE_TIMER_FLAG_ERROR 33

/*定时器创�?(底层)失败*/
#define OSE_TIMER_CREATE_FAIL 34

/*定时器资源申请失�?*/
#define OSE_TIMER_LIST_ERROR 35

/*开启时定时器超时消息为空，而从未开启过*/
#define OSE_TIMER_MSG_ERROR 36

/*定时器开�?(底层)失败*/
#define OSE_TIMER_START_FAIL 37

/*定时器停�?(底层)失败*/
#define OSE_TIMER_STOP_FAIL 38

/*操作定时器时，定时器未创�?*/
#define OSE_NO_CREATE 39

/*定时器删�?(底层)失败*/
#define OSE_TIMER_DELETE_FAIL 40

/*定时器的超时消息队列�?*/
#define OSE_TIMEOUT_QUEUE_OVERFLOW 41

/*内存池创建互斥量失败*/
#define OSE_BUF_INIT_MUTEX_FAIL 50

/*内存池申请内存失�?*/
#define OSE_BUF_INIT_MALLOC_FAIL 51

/*申请内存时，内存耗尽*/
#define OSE_BUF_NO_MEM 52

/*申请内存时，长度太大*/
#define OSE_BUF_GET_MAXSIZE 53

/*block头保留字段错�?*/
#define OSE_BUF_BLOCK_ERR 54

/*重复释放内存(释放的内存没分配)*/
#define OSE_FREE_UNUSED_BUFF 55

/*释放空指�?*/
#define OSE_FREE_NULL 56

/*link内存没分�?*/
#define OSE_LINK_UNUSED_BUFF 57

/*link的次数超�?*/
#define OSE_LINK_EXCEED_MAXCNT 58

/*link内存为空*/
#define OSE_LINK_NULL 59

/*link或者free内存时，该内存地址是OSE的地址范围但不是有效地址*/
#define OSE_BUFF_ADDR_ERR01 60

/*link或者free内存时，该内存地址不是OSE的地址范围*/
#define OSE_BUFF_ADDR_ERR02 61

/*没有动态内�?*/
#define OSE_NO_MEM 62

/*释放动态内存为空指�?*/
#define OSE_MEM_FREE_NULL 63

/*获取系统时间错误*/
#define OSE_GET_SYSTIME_ERROR 64

/*创建事件调度器时，开启或停止函数为空*/
#define OSE_EVENT_CREATE_FUN_NULL 65

/*往事件调度器里设置事件时，回调函数为空*/
#define OSE_EVENT_SET_FUNC_NULL 66

/*操作事件调度器时，入参的调度器指针为�?*/
#define OSE_EVENT_ES_NULL 67

/*操作事件调度器时，入参的调度器指针错�?(一般es不是合法的�?)*/
#define OSE_EVENT_ES_ERROR 68

/*重置或者重启事件时，事件指针为�?*/
#define OSE_EVENT_IS_NULL 69

/*重置或者重启事件时，事件指针不是合法事�?(非法或已超时)*/
#define OSE_EVENT_IS_ERROR 70

/*获取事件调度器里某事件的剩余时间时，出参为空*/
#define OSE_EVENT_GET_TIME_NULL 71

/*调度器被挂起后，上层调用OSE的超时接�?*/
#define OSE_EVENT_CALL_HANDLE_ERROR 72

/*操作hash表时，表指针为空*/
#define OSE_HASH_TBL_NULL 73

/*操作hash表里某个结点时，出参为空*/
#define OSE_HASH_ITEM_PTR_NULL 74

/*内存拷贝时，重叠*/
#define OSE_MEMCPY_WRAP 75

/*任务初始化函数为�?*/
#define OSE_INIT_FUNC_NULL 76

/*任务初始化函数返回失�?*/
#define OSE_INIT_FUNC_FAIL 77

/*创建tcp连接时，系统返回失败*/
#define OSE_SSL_CREATE_FAIL 78

/*重启TCP连接时，连接号错�?*/
#define OSE_SSL_RESET_SLNOERR 79

/*重启TCP连接时，该连接没创建*/
#define OSE_SSL_RESET_NOCREATE 80

/*重启TCP服务器端错误*/
#define OSE_SSL_RESET_ERROR 81

/*接收数据包时，系统返回错�?*/
#define OSE_SSL_RECV_ERROR 82

/*发送数据包时，系统返回错误*/
#define OSE_SSL_SEND_ERROR 83

/*删除tcp连接时，错误*/
#define OSE_SSL_DEL_ERROR 84

/*blist队列操作时，队列控制字段尾空指针*/
#define OSE_BLIST_NULL 85

/*blist队列操作时，出参为空*/
#define OSE_BLIST_OUTPUT_NULL 86

/*blist队列操作时，结点指针为空*/
#define OSE_BLIST_NODE_NULL 87

/*信号量count超过上限*/
#define OSE_SEMA_COUNT_EXCEED 88

/*底层信号量获取失�?*/
#define OSE_SEMA_GET_ERROR 89

/*批量内存操作时，预覆盖OSE头部*/
#define OSE_BLOCK_HEAD_CRITICAL 90

/*批量内存操作时，尾部越界*/
#define OSE_BLOCK_BODY_CRITICAL 91

/*批量内存操作时，长度比申请内存还�?*/
#define OSE_BLOCK_LENGTH_TALL 92

/*批量内存操作�?,内存已经释放*/
#define OSE_BLOCK_UNUSED_BUFF 93

/*内存异常*/
#define OSE_BLOCK_ERROR 94

/*GSMOSE空指针或者其他指针错�?*/
#define OSE_INVALID_POINTER 95

/*定时器资源不�?*/
#define OSE_TIMER_NO_ID 96

/*申请批量内存失败*/
#define OSE_MULTI_MEM_FAIL 97

/*动态内存释放异�?*/
#define OSE_DYNAMIC_MEM_ERR 98


#endif /*OSE_ERROR_H*/