/*******************************************************************************/
/* Filename      : ose_con.h                                                   */
/* Description   : 共通宏定义                                                  */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_CON_H
#define OSE_CON_H

/********************************************/
/* 文件编号                                 */
/********************************************/
#define OSE_FILE_ID_BASE                    50000
#define ADHOC_FILE_ID_BASE                  60000
#define ADHOC_MAIN_FILE_ID_BASE             ADHOC_FILE_ID_BASE
#define ADHOC_HM_FILE_ID_BASE               ADHOC_FILE_ID_BASE + 100
#define ADHOC_UNICAST_ROUTE_FILE_ID_BASE            ADHOC_FILE_ID_BASE + 200
#define ADHOC_NETLAYER_FILE_ID_BASE         ADHOC_FILE_ID_BASE + 300
#define ADHOC_IP_ADAPTER_FILE_ID_BASE       ADHOC_FILE_ID_BASE + 400
#define ADHOC_LM_ADAPTER_FILE_ID_BASE       ADHOC_FILE_ID_BASE + 500
#define ADHOC_MULTICAST_ROUTE_FILE_ID_BASE  ADHOC_FILE_ID_BASE + 600
#define ADHOC_AT_FILE_ID_BASE               ADHOC_FILE_ID_BASE + 700

#define OSE_MAX_TASKS                       128                           /* 上层的最大任务个数 */
#define OSE_MAX_MAILBOXS                    OSE_MAX_TASKS                 /* 最大邮箱个数 */
#define OSE_MAX_SEMAPHORES                  100                           /* OSE可分配的最大信号量个数 */
#define OSE_MAX_MUTEXES                     200                           /* OSE可分配的最大互斥量个数 */
#define OSE_UNAVAILABLE_ID                  0xFFFFFFFF                    /* 无效值 */
#define OSE_MAX_TIMERS                      5000                          /* 定时器个数 */
#define SEM_VALUE_MAX                       32767                         /* 信号量最大count值 */

/********************************************/
/* TASK                                     */
/********************************************/
/*鑷粍缃戞ā鍧楁爣鍑嗕换鍔?*/
/*tk8*/
#define OSE_AT_TASK_ID                      8
#define OSE_AT_PROC_ID                      0
#define OSE_AT_TASK_PRI                     8
#define OSE_AT_TASK_STACK                   4096
#define OSE_AT_TASK_MBSIZE                  350
#define OSE_AT_TASK_NAME                    "tk8_AT"
/*tk9*/
#define OSE_MAIN_TASK_ID                    9
#define OSE_MAIN_PROC_ID                    0
#define OSE_MAIN_TASK_PRI                   9
#define OSE_MAIN_TASK_STACK                 4096
#define OSE_MAIN_TASK_MBSIZE                350
#define OSE_MAIN_TASK_NAME                  "tk9_MAIN"
/*tk10*/
#define OSE_HM_TASK_ID                      10
#define OSE_HM_PROC_ID                      0
#define OSE_HM_TASK_PRI                     10
#define OSE_HM_TASK_STACK                   4096
#define OSE_HM_TASK_MBSIZE                  350
#define OSE_HM_TASK_NAME                    "tk10_HM"
/*tk11*/
#define OSE_NETLAYER_TASK_ID                11
#define OSE_NETLAYER_PROC_ID                0
#define OSE_NETLAYER_TASK_PRI               11
#define OSE_NETLAYER_TASK_STACK             4096
#define OSE_NETLAYER_TASK_MBSIZE            350
#define OSE_NETLAYER_TASK_NAME              "tk11_NETLAYER"
/*tk12*/
#define OSE_UNICAST_ROUTE_TASK_ID           12
#define OSE_UNICAST_ROUTE_PROC_ID           0
#define OSE_UNICAST_ROUTE_TASK_PRI          12
#define OSE_UNICAST_ROUTE_TASK_STACK        4096
#define OSE_UNICAST_ROUTE_TASK_MBSIZE       350
#define OSE_UNICAST_ROUTE_TASK_NAME         "tk12_UNICAST_ROUTE"
/*tk13*/
#define OSE_IP_ADAPTER_TASK_ID              13
#define OSE_IP_ADAPTER_PROC_ID              0
#define OSE_IP_ADAPTER_TASK_PRI             13
#define OSE_IP_ADAPTER_TASK_STACK           4096
#define OSE_IP_ADAPTER_TASK_MBSIZE          350
#define OSE_IP_ADAPTER_TASK_NAME            "tk13_IP_ADAPTER"
/*tk14*/
#define OSE_IP_ADAPTER_READ_TASK_ID         14
#define OSE_IP_ADAPTER_READ_PROC_ID         0
#define OSE_IP_ADAPTER_READ_TASK_PRI        14
#define OSE_IP_ADAPTER_READ_TASK_STACK      4096
#define OSE_IP_ADAPTER_READ_TASK_MBSIZE     350
#define OSE_IP_ADAPTER_READ_TASK_NAME       "tk14_IP_ADAPTER_READ"
/*tk15*/
#define OSE_HM_QUEUE_TASK_ID                15
#define OSE_HM_QUEUE_PROC_ID                0
#define OSE_HM_QUEUE_TASK_PRI               15
#define OSE_HM_QUEUE_TASK_STACK             4096
#define OSE_HM_QUEUE_TASK_MBSIZE            350
#define OSE_HM_QUEUE_TASK_NAME              "tk15_HM_QUEUE"
/*tk16*/
#define OSE_LM_ADAPTER_TASK_ID              16
#define OSE_LM_ADAPTER_PROC_ID              0
#define OSE_LM_ADAPTER_TASK_PRI             16
#define OSE_LM_ADAPTER_TASK_STACK           4096
#define OSE_LM_ADAPTER_TASK_MBSIZE          350
#define OSE_LM_ADAPTER_TASK_NAME            "tk16_LM_ADAPTER"
/*tk18*/
#define OSE_LM_ADAPTER_DATA_TASK_ID         17
#define OSE_LM_ADAPTER_DATA_PROC_ID         0
#define OSE_LM_ADAPTER_DATA_TASK_PRI        17
#define OSE_LM_ADAPTER_DATA_TASK_STACK      4096
#define OSE_LM_ADAPTER_DATA_TASK_MBSIZE     350
#define OSE_LM_ADAPTER_DATA_TASK_NAME       "tk17_LM_ADAPTER_DATA"
/*tk18*/
#define OSE_LM_ADAPTER_CTRL_TASK_ID         18
#define OSE_LM_ADAPTER_CTRL_PROC_ID         0
#define OSE_LM_ADAPTER_CTRL_TASK_PRI        18
#define OSE_LM_ADAPTER_CTRL_TASK_STACK      4096
#define OSE_LM_ADAPTER_CTRL_TASK_MBSIZE     350
#define OSE_LM_ADAPTER_CTRL_TASK_NAME       "tk18_LM_ADAPTER_CTRL"
/*tk19*/
#define OSE_AT_READ_TASK_ID                 19
#define OSE_AT_READ_PROC_ID                 0
#define OSE_AT_READ_TASK_PRI                19
#define OSE_AT_READ_TASK_STACK              4096
#define OSE_AT_READ_TASK_MBSIZE             350
#define OSE_AT_READ_TASK_NAME               "tk19_AT_READ"
/*tk20*/
#define OSE_AT_WRITE_TASK_ID                20
#define OSE_AT_WRITE_PROC_ID                0
#define OSE_AT_WRITE_TASK_PRI               20
#define OSE_AT_WRITE_TASK_STACK             4096
#define OSE_AT_WRITE_TASK_MBSIZE            350
#define OSE_AT_WRITE_TASK_NAME              "tk20_AT_WRITE"
/*tk21*/
#define OSE_MULTICAST_ROUTE_TASK_ID         21
#define OSE_MULTICAST_ROUTE_PROC_ID         0
#define OSE_MULTICAST_ROUTE_TASK_PRI        21
#define OSE_MULTICAST_ROUTE_TASK_STACK      4096
#define OSE_MULTICAST_ROUTE_TASK_MBSIZE     350
#define OSE_MULTICAST_ROUTE_TASK_NAME       "tk21_MULTICAST"

#endif /*OSE_CON_H*/