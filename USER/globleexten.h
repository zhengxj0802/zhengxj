/*ȫ�ֱ�������*/
#ifndef _globleexten_h_
#define _globleexten_h_

#include "stm32f10x.h"
#include "time.h"

extern u8 reboot;

extern u8 DOWNLOADKEY;		//������Կ������־
extern u16 KeyCuration;		//�������������µ�ʱ��
extern u8 KeyPressflag;		//�����Ƿ��б�����
extern u8 WaitKeyPressflag;	//�Ƿ��ڵȴ���������
extern u16 g_FPTimeOut;		//ָ�Ƴ�ʱ����
extern u8 USARTInterruptFlag;//����1�жϱ�־λ
extern u8 gcRetCode;		//����2�������ݳɹ���ʧ�ܱ�־λ
extern u8 RefreshTime;		//�Ƿ���Ҫˢ����Ļ��ʾʱ��
extern u8 UpdateTimeFlag;	//��λ���Ƿ����·�ʱ����Ӧ���豸��Ҫ����ʱ�������
extern struct tm time;		//ʱ��ṹ��
extern u8 SysVersionStr[14];//ϵͳ�汾��


extern u8 Mapped[32]; 		//ָ��ģ�鷵����Ϣӳ���
extern u8 PackageBuffer[80]; //���ݰ����ջ�����

extern u32 gwSpeedPluseMile;//���������
#endif
