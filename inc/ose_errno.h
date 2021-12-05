/*******************************************************************************/
/* Filename      : ose_error.h                                                 */
/* Description   : ������궨��                                                  */
/*                                                                             */
/* Notes         :                                                             */
/*                                                                             */
/* Change History: 0.01 2018-01-29             Original                        */
/*******************************************************************************/

#ifndef OSE_ERROR_H
#define OSE_ERROR_H

/***********************************************
*1000���ڵĴ�����ΪOSEʹ�á�
*
*OSE����Щ������ȫ����Ϊ�����쳣ʹ��
*
*���ڴ�����ı��ͣ�Ϊ�����α���,OSEֻ�ڽӿں����ﱨ
*�ӿں������ýӿں����������߲�����
********************************/
/*����ʧ��*/
#define OSE_ASSERT_FAIL 0

/*OSE״̬�쳣*/
#define OSE_STATE_ERROR 1

/*OSE��ʼ��ʧ��*/
#define OSE_INIT_FAIL 2

/*����ID�Ƿ�*/
#define OSE_TASK_ID_ERROR 3

/*������ں���Ϊ��*/
#define OSE_TASK_ENTRY_NULL 4

/*�����������Ѿ�����*/
#define OSE_TASK_CREATED 5

/*���䴴��ʧ��*/
#define OSE_CREATE_MB_FAIL 6

/*ϵͳ��������ʧ��*/
#define OSE_SYS_CREATE_TASK_FAIL 7

/*����û����*/
#define OSE_TASK_NO_CREATED 8

/*�������ȼ�����ʧ��*/
#define OSE_TASK_SET_PRI_FAIL 9

/*��ȡ�������ȼ�ʱ������λ��*/
#define OSE_TASK_GET_PTR_NULL 10

/*��ȡ���ȼ�ʧ��*/
#define OSE_TASK_GET_PRI_FAIL 11

/*������������(�ںͼ�)ʧ��*/
#define OSE_MB_CREATE_FAIL 12

/*��������ʱ�������СΪ0*/
#define OSE_MB_SIZE_ZERO 13

/*�������������ʱ,����id����*/
#define OSE_MBIDOR_QID_ERROR 14

/*�������������ʱ�������Ѵ���*/
#define OSE_MB_CREATED 15

/*��ȡ���������δ����*/
#define OSE_GET_MB_NO_CREATE 16

/*�������������δ����*/
#define OSE_SEND_MB_NOCREATE 16

/*���������id�Ƿ�*/
#define OSE_MB_MID_ERROR 17

/*����������ȡʧ��*/
#define OSE_MB_GET_ERROR 18

/*��������䷢��ʧ��(���к�������ʧ��)*/
#define OSE_MB_SEND_ERROR 19

/*������Ϣʱ��Ŀ�����������������û����*/
#define OSE_QUEUE_NO_CREATED 20

/*ɾ������������ʱ��qid�Ƿ�*/
#define OSE_DEL_QUEUE_ERROR 21

/*����������ʧ��*/
#define OSE_MUTEX_CREATE_FAIL 22

/*����������ʱ��falg�Ƿ�*/
#define OSE_MUTEX_FLAG_ERROR 23

/*����������ʱ��id�Ƿ�*/
#define OSE_MUTEX_ID_ERR 24

/*����������ʱ��������û����*/
#define OSE_MUTEX_NO_CREATED 25

/*��������ȡʧ��*/
#define OSE_MUTEX_GET_FAIL 26

/*�����ź���ʱ��������Ϊ0*/
#define OSE_SEMA_COUNT_ERROR 27

/*�ź�������ʱ��id�Ƿ�*/
#define OSE_SEMA_ID_ERROR 28

/*�ź�������ʱ��û����*/
#define OSE_SEMA_NO_CREATED 29

/*�ź�����ȡʧ��*/
#define OSE_SEMA_GET_FAIL 30

/*������ʱ��ʱ��id�Ƿ�*/
#define OSE_CREATE_TIMERID_ERROR 31

/*��ʱ���ظ�����*/
#define OSE_TIMER_CREATED 32

/*������ʱ��ʱ����ʱ�����ʹ���*/
#define OSE_TIMER_FLAG_ERROR 33

/*��ʱ������(�ײ�)ʧ��*/
#define OSE_TIMER_CREATE_FAIL 34

/*��ʱ����Դ����ʧ��*/
#define OSE_TIMER_LIST_ERROR 35

/*����ʱ��ʱ����ʱ��ϢΪ�գ�����δ������*/
#define OSE_TIMER_MSG_ERROR 36

/*��ʱ������(�ײ�)ʧ��*/
#define OSE_TIMER_START_FAIL 37

/*��ʱ��ֹͣ(�ײ�)ʧ��*/
#define OSE_TIMER_STOP_FAIL 38

/*������ʱ��ʱ����ʱ��δ����*/
#define OSE_NO_CREATE 39

/*��ʱ��ɾ��(�ײ�)ʧ��*/
#define OSE_TIMER_DELETE_FAIL 40

/*��ʱ���ĳ�ʱ��Ϣ������*/
#define OSE_TIMEOUT_QUEUE_OVERFLOW 41

/*�ڴ�ش���������ʧ��*/
#define OSE_BUF_INIT_MUTEX_FAIL 50

/*�ڴ�������ڴ�ʧ��*/
#define OSE_BUF_INIT_MALLOC_FAIL 51

/*�����ڴ�ʱ���ڴ�ľ�*/
#define OSE_BUF_NO_MEM 52

/*�����ڴ�ʱ������̫��*/
#define OSE_BUF_GET_MAXSIZE 53

/*blockͷ�����ֶδ���*/
#define OSE_BUF_BLOCK_ERR 54

/*�ظ��ͷ��ڴ�(�ͷŵ��ڴ�û����)*/
#define OSE_FREE_UNUSED_BUFF 55

/*�ͷſ�ָ��*/
#define OSE_FREE_NULL 56

/*link�ڴ�û����*/
#define OSE_LINK_UNUSED_BUFF 57

/*link�Ĵ�������*/
#define OSE_LINK_EXCEED_MAXCNT 58

/*link�ڴ�Ϊ��*/
#define OSE_LINK_NULL 59

/*link����free�ڴ�ʱ�����ڴ��ַ��OSE�ĵ�ַ��Χ��������Ч��ַ*/
#define OSE_BUFF_ADDR_ERR01 60

/*link����free�ڴ�ʱ�����ڴ��ַ����OSE�ĵ�ַ��Χ*/
#define OSE_BUFF_ADDR_ERR02 61

/*û�ж�̬�ڴ�*/
#define OSE_NO_MEM 62

/*�ͷŶ�̬�ڴ�Ϊ��ָ��*/
#define OSE_MEM_FREE_NULL 63

/*��ȡϵͳʱ�����*/
#define OSE_GET_SYSTIME_ERROR 64

/*�����¼�������ʱ��������ֹͣ����Ϊ��*/
#define OSE_EVENT_CREATE_FUN_NULL 65

/*���¼��������������¼�ʱ���ص�����Ϊ��*/
#define OSE_EVENT_SET_FUNC_NULL 66

/*�����¼�������ʱ����εĵ�����ָ��Ϊ��*/
#define OSE_EVENT_ES_NULL 67

/*�����¼�������ʱ����εĵ�����ָ�����(һ��es���ǺϷ���ֵ)*/
#define OSE_EVENT_ES_ERROR 68

/*���û��������¼�ʱ���¼�ָ��Ϊ��*/
#define OSE_EVENT_IS_NULL 69

/*���û��������¼�ʱ���¼�ָ�벻�ǺϷ��¼�(�Ƿ����ѳ�ʱ)*/
#define OSE_EVENT_IS_ERROR 70

/*��ȡ�¼���������ĳ�¼���ʣ��ʱ��ʱ������Ϊ��*/
#define OSE_EVENT_GET_TIME_NULL 71

/*��������������ϲ����OSE�ĳ�ʱ�ӿ�*/
#define OSE_EVENT_CALL_HANDLE_ERROR 72

/*����hash��ʱ����ָ��Ϊ��*/
#define OSE_HASH_TBL_NULL 73

/*����hash����ĳ�����ʱ������Ϊ��*/
#define OSE_HASH_ITEM_PTR_NULL 74

/*�ڴ濽��ʱ���ص�*/
#define OSE_MEMCPY_WRAP 75

/*�����ʼ������Ϊ��*/
#define OSE_INIT_FUNC_NULL 76

/*�����ʼ����������ʧ��*/
#define OSE_INIT_FUNC_FAIL 77

/*����tcp����ʱ��ϵͳ����ʧ��*/
#define OSE_SSL_CREATE_FAIL 78

/*����TCP����ʱ�����ӺŴ���*/
#define OSE_SSL_RESET_SLNOERR 79

/*����TCP����ʱ��������û����*/
#define OSE_SSL_RESET_NOCREATE 80

/*����TCP�������˴���*/
#define OSE_SSL_RESET_ERROR 81

/*�������ݰ�ʱ��ϵͳ���ش���*/
#define OSE_SSL_RECV_ERROR 82

/*�������ݰ�ʱ��ϵͳ���ش���*/
#define OSE_SSL_SEND_ERROR 83

/*ɾ��tcp����ʱ������*/
#define OSE_SSL_DEL_ERROR 84

/*blist���в���ʱ�����п����ֶ�β��ָ��*/
#define OSE_BLIST_NULL 85

/*blist���в���ʱ������Ϊ��*/
#define OSE_BLIST_OUTPUT_NULL 86

/*blist���в���ʱ�����ָ��Ϊ��*/
#define OSE_BLIST_NODE_NULL 87

/*�ź���count��������*/
#define OSE_SEMA_COUNT_EXCEED 88

/*�ײ��ź�����ȡʧ��*/
#define OSE_SEMA_GET_ERROR 89

/*�����ڴ����ʱ��Ԥ����OSEͷ��*/
#define OSE_BLOCK_HEAD_CRITICAL 90

/*�����ڴ����ʱ��β��Խ��*/
#define OSE_BLOCK_BODY_CRITICAL 91

/*�����ڴ����ʱ�����ȱ������ڴ滹��*/
#define OSE_BLOCK_LENGTH_TALL 92

/*�����ڴ����ʱ,�ڴ��Ѿ��ͷ�*/
#define OSE_BLOCK_UNUSED_BUFF 93

/*�ڴ��쳣*/
#define OSE_BLOCK_ERROR 94

/*GSMOSE��ָ���������ָ�����*/
#define OSE_INVALID_POINTER 95

/*��ʱ����Դ����*/
#define OSE_TIMER_NO_ID 96

/*���������ڴ�ʧ��*/
#define OSE_MULTI_MEM_FAIL 97

/*��̬�ڴ��ͷ��쳣*/
#define OSE_DYNAMIC_MEM_ERR 98


#endif /*OSE_ERROR_H*/