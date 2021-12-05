/*******************************************************************************/
/* Filename      : ose_trace.c                                                 */
/* Description   : 日志管理                                                    */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-04-28             Original                        */
/*******************************************************************************/

#include <ose_all.h>
#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 11)


UINT32   gTraceLevel = OSE_TRACE_ALL; // 日志等级

/*****************************************************************************
* Function  : ose_trace
* Purpose   : 写日�?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       log_level           UINT32                 In         日志等级
*
*
* Return:
*******************************************************************************/
void ose_trace(UINT32 log_level, char const* arg_ptr, ...)
{
    char    trace_buff[1024] = { 0 };
    char    time_buff[128] = { 0 };
    va_list sa;
    char    str_buff[255];

    // 过滤日志等级
    if (log_level > gTraceLevel)
    {
        return;
    }
    /* 获取打印信息 */
    va_start(sa, arg_ptr);
    vsnprintf(&str_buff[0], (size_t)(sizeof(str_buff) - 1), (char*)arg_ptr, sa);
    va_end(sa);

    str_buff[254] = '\0';

    // 写入日志时间
    ose_get_trace_time(time_buff);
    // 在原内容中添加日志等级标�?
    snprintf(trace_buff, sizeof(trace_buff) - 1, "[%s][%s]%s", time_buff,ose_get_trace_level(log_level),&str_buff[0]);
    printf("%s\n",trace_buff); 
    return;
}
/*****************************************************************************
* Function  : ose_get_trace_level
* Purpose   : 获取对应的日志等�?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       log_level           UINT32                 In         日志等级
*
*
* Return:
*   日志等级信息字符�?
*******************************************************************************/
char* ose_get_trace_level(UINT32 log_level)
{
    char* level = NULL;
    switch(log_level)
    {
        case OSE_TRACE_FATAL:
            level = OSE_TRACE_LEVEL_STR_FATAL;
            break;
        case OSE_TRACE_ERROR:
            level = OSE_TRACE_LEVEL_STR_ERROR;
            break;
        case OSE_TRACE_WARN:
            level = OSE_TRACE_LEVEL_STR_WARN;
            break;
        case OSE_TRACE_DEBUG:
            level = OSE_TRACE_LEVEL_STR_DEBUG;
            break;
        case OSE_TRACE_INFO:
            level = OSE_TRACE_LEVEL_STR_INFO;
            break;
        case OSE_TRACE_ALL:
            level = OSE_TRACE_LEVEL_STR_ALL;
            break;
        default:
            level = OSE_TRACE_LEVEL_STR_OTHER;
            break;
    }
    return level;
}
/*****************************************************************************
* Function  : ose_get_trace_time
* Purpose   : 获取时间�?
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       time_str           UINT8*                 In         时间�?
*
*
* Return:
*******************************************************************************/
void ose_get_trace_time(char* time_str)
{
    struct timeval   tv; 
    struct timezone  tz; 
    struct tm        *p; 

    gettimeofday(&tv, &tz); 
    p = localtime(&tv.tv_sec); 
    sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d:%03ld", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec/1000);
}
/*****************************************************************************
* Function  : ose_set_trace_level
* Purpose   : 设置日志保存等级
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*       level           UINT32                   In         日志等级
*
*
* Return:
*   日志等级信息字符�?
*******************************************************************************/
void ose_set_trace_level(UINT32 level)
{
    if (level <= OSE_TRACE_ALL)
    {
        gTraceLevel = level;
    }
    else
    {
        gTraceLevel = OSE_TRACE_INFO;
    }
}