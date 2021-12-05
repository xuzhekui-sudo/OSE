/*******************************************************************************/
/*******************************************************************************/
/* Filename      : ose_utl.c                                                   */
/* Description   : ��������                                                     */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#include <ose_all.h>
#undef FILEID
#define FILEID (OSE_FILE_ID_BASE + 9)

/*****************************************************************************
* Function  : ose_get_system_time
* Purpose   : ��ȡϵͳʱ���
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return:
* Note:
*******************************************************************************/
UINT32 ose_get_system_time(void)
{
    UINT32 sys_time = 0;
#ifdef LINUX_SWITCH
    struct timeval tv;

    gettimeofday(&tv,NULL);
    //����
    sys_time = tv.tv_sec*1000 + tv.tv_usec/1000;
#endif
    return sys_time;
}
/*****************************************************************************
* Function  : ose_btol_u16
* Purpose   :��16λ�������ֽ��Ⱥ󵹻�
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return:
* Note:
*******************************************************************************/
UINT16 ose_btol_u16(UINT16 value)
{
    UINT16 tmp;
    tmp = value & 0x00ff;
    tmp <<= 8;
    tmp = tmp | ((value >> 8) & 0x00ff);
    return tmp;
}
/*****************************************************************************
* Function  : ose_btol_u32
* Purpose   :��32λ�������ֽ��Ⱥ󵹻�
* Relation  :
*
* Input Parameters:
*
*       Name                Type                In/Out      Description
*   -----------         --------------          ------      -----------
*
* Return:
* Note:
*******************************************************************************/
UINT32 ose_btol_u32(UINT32 value)
{
    UINT32 tmp;
    tmp = (value << 24) & 0xff000000;
    tmp |= (value << 8) & 0x00ff0000;
    tmp |= (value >> 8) & 0x0000ff00;
    tmp |= (value >> 24) & 0x000000ff;
    return tmp;
}