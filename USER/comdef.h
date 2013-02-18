/*��������*/
#ifndef _comdef_h_
#define _comdef_h_

#include "stm32f10x.h"
#include <stdio.h>

typedef enum{CMDPACKAGE=0x01, DATAPACKAGE=0x02, ENDPACKAGE=0x08, ASKPACKAGE=0x07} PackageType; //ָ�ư�����

#define ISASCII         	1	//ASCII
#define ISHEX            	0	//HEX
#define DFPAGECOUNT			411	//���ֿ���س���
#define DFPAGEFLAG			412	//���ֿ��ǳ���,��Atmel128 EEPROM��дʧ�ܵ�ʱ���ȡ

//����������Ϣ
#define KeyPressTime		150	
#define WaitKeyPressTime 	250
#define UpKey				1	 //�ϼ�
#define PowerKey			2	 //��Դ��
#define DownKey				3	 //�·�
#define CancleKey			4	 //ȡ��
#define EnterKey			5	 //ȷ��
#define UpLongKey			6	 //�ϼ�����
#define PowerLongKey		7	 //��Դ������
#define DownLongKey			8	 //�·�����
#define CancleLongKey		9	 //ȡ������
#define EnterLongKey		10	 //ȷ������

// ����������
#define C_OK					0x00                // ������ȷ
#define C_CMDERR				0x01                // �����ִ���
#define C_PARAERR				0x02                // �����������
#define C_CRCERR				0x03                // У��ʹ���
#define C_ICREADERR				0x04                // ����ʧ��
#define C_ICWRITEERR			0x05                // д��ʧ��
#define C_ICTIMEOUT				0x06                // ����ʱ����
#define C_ICINITNOTEND     		0x07                // ����ʼ��δ����
#define C_ICNOTINIT        		0x08                // ��δ��ʼ��
#define C_ICUNKNOWN        		0x09                // ��״̬δ֪
#define C_ICINVALID        		0x0A                // ����Ч
#define C_ICOPTERR         		0x0B                // ������ʧ��
#define C_ICVERIFYERR      		0x0C				// ���ض�����
#define C_ICCRCERR         		0x0D                // ��У��ʹ���
#define C_ICDIEFFULL	   		0x0E				// DIEF�ļ�����
#define C_NOIC             		0x0F                // ��δ����
#define C_ICCHANGED        		0x10                // ���Ų�ƥ��
#define C_ICLOGINED	       		0x11				// IC���Ѿ�ǩ��	
#define C_ICLOCKED         		0x12                // IC���Ѿ�������
#define C_ICUNENABLED         	0x13                // IC��δ����
#define C_ICENDED         		0x14                // IC���Ѿ���ҵ��У
#define C_NOTCOACHIC       		0x15                // �ǽ�����
#define C_NOTADMINIC       		0x16                // �Ǽ�У����Ա��
#define C_NOTSUPERIC       		0x17                // �ǳ����û���
#define C_NOTDRIVERIC       	0x18                // ��ѧԱ��
#define C_ICSTATUSERR    		0x19                // ��״̬�Ƿ�
#define C_ICNOTSB				0x1A				// ������TSB��Ϣ
#define C_ICTYPEERROR			0x1B				// ��l���ʹ���
#define C_COMMTIMEOUT        	0x1C                // ͨ�ų�ʱ����
#define C_NOTKEYIC        		0x1D                // ����Կ��
#define C_FIDENROLLERR 	   		0x20                // ָ�Ʋɼ�ʧ��
#define C_FIDGETIMAGEERR 		0x21                // ָ�Ʋɼ�ʧ��(ָ��ͼ��ɼ�����)
#define C_FIDTIMEOUT       		0x22                // FIDģ�鳬ʱ����
#define C_FIDBADFINGER     		0x23                // ָ��������,�޷��ɼ�ָ�ƴ���
#define C_FIDGENERR      		0x24                // �ϳ�ģ�����
#define C_FIDEXTERR      		0x25                // �ϳ�ģ�����
#define C_FIDCMPERR				0x26				// ָ�ƱȽ�ʧ��
#define C_FIDBCTERR		   		0x27				// ָ��ģ�����
#define C_FIDINVALIDMB			0x28				// ģ�����ݷǷ�
#define C_FIDGETIMAGETIMEOUT	0x29                // δ�ڹ涨��ʱ���ڷ�����ָ����

#define C_EEPROMREADEER    		0x30                // ��EEPROM����
#define C_EEPROMWRITEERR   		0x31                // дEEPROM����
#define C_EEPROMCRCERR	   		0x32				// ��дFLASH����
#define C_EEPROMVERIFYERR   	0x33				// �ض�����
#define C_STIQFULL		   		0x34				// FLASH�еĶ����Ѿ���
#define C_STIQEMPTY	   			0x35				// ������ǰ���ж϶��е�����Ϊ�գ�����
#define C_STIQEND            	0x36                // ��FLASHʱ��������������е�ǰ���ݺ󣬶���Ϊ��

#define C_RECOVERYERROR	   		0x40				// �ӵ�ָ�ʱ��������	
#define C_UARTTIMEOUT        	0x41                // ͨѶ��ʱ����
#define C_SEPCIALLOGINERR  		0x42                // �޷�Ӧ��ǩ��
#define C_PWDERR           		0x43                // ����Ա�������
#define C_DCBTFULL        		0x44                // DCBT�Ѿ���
#define C_ERRORKEY         		0x45                // ��������
#define C_CANCELED         		0x46                // ������ȡ��
#define C_DOWNSTATUSERROR  		0x47                // ����ʱ�ֳֻ�״̬����
#define C_DATETIMEERR	   		0x48				// ʱ�����ڴ���	
#define C_MEMALLOCERR	   		0x49				// �洢���ռ�������
#define C_ICEXPIRED     		0x4A                // ������
#define C_DOUBLEERR         	0x4B                // ��д��Ϣ2��������
#define C_FIRSTERR       		0x4C                // ��д��Ϣ��һ������
#define C_SECONDERR      		0x4D                // ��д��Ϣ�ڶ�������.
#define C_FIRSTOK          		0x4E                // 2����Ϣ����ȡ�ɹ�,�ҵ�һ����
#define C_SECONDOK         		0x4F                // 2����Ϣ����ȡ�ɹ�,�ҵڶ�����

#define C_RENEW            		0x50                // ������ʾ
#define C_APPEND           		0x51                // ������ʾ
#define C_NOSUBJECT        		0x52                // û��������ѵ��Ŀ
#define C_UNMATCHEDSUBJECT		0x53				// ѧԱ��ѵ��Ŀ�͵�ǰ��ѵ��Ŀ��һ��

#define C_EEPROMDATERR			0x60				// EEPROM��DAT�����
#define C_EEPROMDPBERR			0x61				// EEPROM��DPB����
#define C_EEPROMSYSPARAERR		0x62				// EEPROM��SYSPARA����
#define C_EEPROMKEYAERR			0x63				// EEPROM��KEY����	
#define C_EEPROMKEYNOTINIT		0x64				// EEPROM��KEYδ��ʼ��
#define C_EEPROMCSIERR			0x65				// EEPORM��CSI����
#define C_EEPROMDIERR			0x66				// EEPROM��DI����
#define C_EEPROMSIERR			0x67				// EEPROM��SI����
#define C_EEPROMSINOTINIT		0x68				// EEPROM��SIδ����
#define C_EEPROMVIERR			0x69				// EEPROM��VI����
#define C_EEPROMSTIQPOINTERR	0x6A				// EEPROM�ж���ָ�����
#define C_EEPROMNOTSBERR		0x6B				// EEPROM��TSB�������ѡ��
#define C_EEPROMCTSBERR			0x6C				// EEPROM��CTSB����

#define C_TRAINTIMEOUT			0x70				// ѵ��ʱ���Ѿ����
#define C_LOGIN_KEY				0x71				// ��LOGIN����
#define C_SPECIALVERIFY			0x71				// �����ָ֤��ʱʹ��Ӧ��ǩ��
#define C_LOGOUT_KEY			0x72				// ��LOGOUT����
#define C_COACHVERIFY			0x72				// �����ָ֤��ʱʹ�ý���Ա����
#define C_NOCOACHVERIFY			0x73				// ���������Ա����
#define C_TSBERRFORTRAINING		0x74				// ѧԱѵ���У��������޸�ѵ����Ŀ
#define C_NEXTDAYTRAIN			0x75				// ����ѵ��
#define C_NOTHEORYTRAIN			0x76				// �������������ѵ��
#define C_NOSUBJECTTRAIN		0x77				// �������������ѵ��
#define C_DRIVERNOTINDCBT		0x78				// DCBT���Ҳ���ѧԱ
#define C_DRIVERALREADYINDCBT	0x79				// DCBT���Ѿ��и�ѧԱ
#define C_SIMULATECAR			0x7A				// ѵ��ģ����
#define C_FC_FALSEFINGER		0x7B				// ����ָ
#define C_FC_ERROR				0x7C				// FingerChip����
#define C_FC_TOO_SLOW			0x7D				// �β�̫��
#define C_FC_TOO_FAST			0x7E				// �β�̫��
#define C_FC_BAD_IMAGE			0x7F				// ͼ��������

#define C_CHANGETIMEERR			0x80				// ΢��������

#define C_BASENOTLOGIN			0x81				// ����Աδ�ڳ���ǩ��
#define C_BASENOTLOGOUT			0x82				// ����Աδ�ڳ���ǩ��
#define C_BASERELOGIN			0x83				// ����Աδ�ڳ���ǩ��

#define C_WAITKEYTIMOUT			0xF0				// ������ʱ
#define C_NOTOK            		0xFF                // ͨ�ô���

#endif
