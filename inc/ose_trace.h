/*******************************************************************************/
/* Filename      : ose_trace.h                                                 */
/* Description   : 日志管理                                                    */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-04-28             Original                        */
/*******************************************************************************/

#ifndef OSE_TRACE_H
#define OSE_TRACE_H

#define OSE_TRACE_LEVEL_STR_FATAL    "FATAL"
#define OSE_TRACE_LEVEL_STR_ERROR    "ERROR"
#define OSE_TRACE_LEVEL_STR_WARN     "WARN"
#define OSE_TRACE_LEVEL_STR_DEBUG    "DEBUG"
#define OSE_TRACE_LEVEL_STR_INFO     "INFO"
#define OSE_TRACE_LEVEL_STR_ALL      "ALL"
#define OSE_TRACE_LEVEL_STR_OTHER    "OTHER"

#ifdef __cplusplus
extern "C" {
#endif

char* ose_get_trace_level(UINT32 log_level);
void ose_get_trace_time(char *pszTimeStr);

#ifdef __cplusplus
}
#endif

#endif /*OSE_TRACE_H*/