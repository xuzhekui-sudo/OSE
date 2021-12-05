/*******************************************************************************/
/* Filename      : ose_all.h                                                   */
/* Description   : include all .h file                                         */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_ALL_H
#define OSE_ALL_H

#define LINUX_SWITCH                    1

/*基础类型和数据结构定�?*/
#include <ose_type.h>
/*OSE的接口头文件*/
#include <ose_pub.h>
/*OSE的配置头文件*/
#include <ose_con.h>
/*OSE涉及到的错误码定�?*/
#include <ose_errno.h>
/*初始化模块的头文�?*/
#include <ose_init.h>
/*信号�?*/
#include <ose_sema.h>
/*互斥�?*/
#include <ose_mutex.h>
/*FSM消息相关定义和接�?*/
#include <ose_fsm.h>
/*OSE使用的所有链表和队列相关定义和接�?*/
#include <ose_list.h>
/*任务间邮箱和任务内邮箱相关定义和接口*/
#include <ose_mb.h>
/*任务管理*/
#include <ose_task.h>
/*OSE提供的辅助功能相关定义和接口*/
#include <ose_utl.h>
/*静态内存管理相关定�?*/
#include <ose_buf.h>
/*定时器相关定义和接口*/
#include <ose_timer.h>
#include <ose_trace.h>
#include <ose_link.h>

#endif /*OSE_ALL_H*/