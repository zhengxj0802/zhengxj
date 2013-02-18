/*常量定义*/
#ifndef _comdef_h_
#define _comdef_h_

#include "stm32f10x.h"
#include <stdio.h>

typedef enum{CMDPACKAGE=0x01, DATAPACKAGE=0x02, ENDPACKAGE=0x08, ASKPACKAGE=0x07} PackageType; //指纹包类型

#define ISASCII         	1	//ASCII
#define ISHEX            	0	//HEX
#define DFPAGECOUNT			411	//汉字库相关常量
#define DFPAGEFLAG			412	//汉字库标记常量,当Atmel128 EEPROM读写失败的时候读取

//按键操作信息
#define KeyPressTime		150	
#define WaitKeyPressTime 	250
#define UpKey				1	 //上键
#define PowerKey			2	 //电源键
#define DownKey				3	 //下翻
#define CancleKey			4	 //取消
#define EnterKey			5	 //确定
#define UpLongKey			6	 //上键长按
#define PowerLongKey		7	 //电源键长按
#define DownLongKey			8	 //下翻长按
#define CancleLongKey		9	 //取消长按
#define EnterLongKey		10	 //确定长按

// 函数处理结果
#define C_OK					0x00                // 操作正确
#define C_CMDERR				0x01                // 命令字错误
#define C_PARAERR				0x02                // 命令参数错误
#define C_CRCERR				0x03                // 校验和错误
#define C_ICREADERR				0x04                // 读卡失败
#define C_ICWRITEERR			0x05                // 写卡失败
#define C_ICTIMEOUT				0x06                // 卡超时错误
#define C_ICINITNOTEND     		0x07                // 卡初始化未结束
#define C_ICNOTINIT        		0x08                // 卡未初始化
#define C_ICUNKNOWN        		0x09                // 卡状态未知
#define C_ICINVALID        		0x0A                // 卡无效
#define C_ICOPTERR         		0x0B                // 卡操作失败
#define C_ICVERIFYERR      		0x0C				// 卡回读错误
#define C_ICCRCERR         		0x0D                // 卡校验和错误
#define C_ICDIEFFULL	   		0x0E				// DIEF文件已满
#define C_NOIC             		0x0F                // 卡未插入
#define C_ICCHANGED        		0x10                // 卡号不匹配
#define C_ICLOGINED	       		0x11				// IC卡已经签到	
#define C_ICLOCKED         		0x12                // IC卡已经被锁定
#define C_ICUNENABLED         	0x13                // IC卡未启用
#define C_ICENDED         		0x14                // IC卡已经结业离校
#define C_NOTCOACHIC       		0x15                // 非教练卡
#define C_NOTADMINIC       		0x16                // 非驾校管理员卡
#define C_NOTSUPERIC       		0x17                // 非超级用户卡
#define C_NOTDRIVERIC       	0x18                // 非学员卡
#define C_ICSTATUSERR    		0x19                // 卡状态非法
#define C_ICNOTSB				0x1A				// 卡中无TSB信息
#define C_ICTYPEERROR			0x1B				// 卡l类型错误
#define C_COMMTIMEOUT        	0x1C                // 通信超时错误
#define C_NOTKEYIC        		0x1D                // 非密钥卡
#define C_FIDENROLLERR 	   		0x20                // 指纹采集失败
#define C_FIDGETIMAGEERR 		0x21                // 指纹采集失败(指纹图像采集错误)
#define C_FIDTIMEOUT       		0x22                // FID模块超时错误
#define C_FIDBADFINGER     		0x23                // 指纹质量低,无法采集指纹错误
#define C_FIDGENERR      		0x24                // 合成模板错误
#define C_FIDEXTERR      		0x25                // 合成模板错误
#define C_FIDCMPERR				0x26				// 指纹比较失败
#define C_FIDBCTERR		   		0x27				// 指纹模块错误
#define C_FIDINVALIDMB			0x28				// 模板数据非法
#define C_FIDGETIMAGETIMEOUT	0x29                // 未在规定的时间内放置手指错误

#define C_EEPROMREADEER    		0x30                // 读EEPROM错误
#define C_EEPROMWRITEERR   		0x31                // 写EEPROM错误
#define C_EEPROMCRCERR	   		0x32				// 读写FLASH错误
#define C_EEPROMVERIFYERR   	0x33				// 回读错误
#define C_STIQFULL		   		0x34				// FLASH中的队列已经满
#define C_STIQEMPTY	   			0x35				// 读队列前，判断队列的内容为空，不读
#define C_STIQEND            	0x36                // 读FLASH时结束，读完队列中当前内容后，队列为空

#define C_RECOVERYERROR	   		0x40				// 加电恢复时遇到错误	
#define C_UARTTIMEOUT        	0x41                // 通讯超时错误
#define C_SEPCIALLOGINERR  		0x42                // 无法应急签到
#define C_PWDERR           		0x43                // 操作员口令错误
#define C_DCBTFULL        		0x44                // DCBT已经满
#define C_ERRORKEY         		0x45                // 按键错误
#define C_CANCELED         		0x46                // 操作被取消
#define C_DOWNSTATUSERROR  		0x47                // 下载时手持机状态错误
#define C_DATETIMEERR	   		0x48				// 时间日期错误	
#define C_MEMALLOCERR	   		0x49				// 存储器空间分配错误
#define C_ICEXPIRED     		0x4A                // 卡过期
#define C_DOUBLEERR         	0x4B                // 读写信息2条都错误
#define C_FIRSTERR       		0x4C                // 读写信息第一条错误
#define C_SECONDERR      		0x4D                // 读写信息第二条错误.
#define C_FIRSTOK          		0x4E                // 2份信息都读取成功,且第一份新
#define C_SECONDOK         		0x4F                // 2份信息都读取成功,且第二份新

#define C_RENEW            		0x50                // 补差提示
#define C_APPEND           		0x51                // 新增提示
#define C_NOSUBJECT        		0x52                // 没有下载培训科目
#define C_UNMATCHEDSUBJECT		0x53				// 学员培训科目和当前培训科目不一致

#define C_EEPROMDATERR			0x60				// EEPROM中DAT表错误
#define C_EEPROMDPBERR			0x61				// EEPROM中DPB错误
#define C_EEPROMSYSPARAERR		0x62				// EEPROM中SYSPARA错误
#define C_EEPROMKEYAERR			0x63				// EEPROM中KEY错误	
#define C_EEPROMKEYNOTINIT		0x64				// EEPROM中KEY未初始化
#define C_EEPROMCSIERR			0x65				// EEPORM中CSI错误
#define C_EEPROMDIERR			0x66				// EEPROM中DI错误
#define C_EEPROMSIERR			0x67				// EEPROM中SI错误
#define C_EEPROMSINOTINIT		0x68				// EEPROM中SI未启用
#define C_EEPROMVIERR			0x69				// EEPROM中VI错误
#define C_EEPROMSTIQPOINTERR	0x6A				// EEPROM中对列指针错误
#define C_EEPROMNOTSBERR		0x6B				// EEPROM中TSB错误或无选项
#define C_EEPROMCTSBERR			0x6C				// EEPROM中CTSB错误

#define C_TRAINTIMEOUT			0x70				// 训练时间已经达标
#define C_LOGIN_KEY				0x71				// 按LOGIN按键
#define C_SPECIALVERIFY			0x71				// 随机验证指纹时使用应急签到
#define C_LOGOUT_KEY			0x72				// 按LOGOUT按键
#define C_COACHVERIFY			0x72				// 随机验证指纹时使用教练员带验
#define C_NOCOACHVERIFY			0x73				// 不允许教练员带验
#define C_TSBERRFORTRAINING		0x74				// 学员训练中，不允许修改训练科目
#define C_NEXTDAYTRAIN			0x75				// 隔日训练
#define C_NOTHEORYTRAIN			0x76				// 不允许进行理论训练
#define C_NOSUBJECTTRAIN		0x77				// 不允许进行术科训练
#define C_DRIVERNOTINDCBT		0x78				// DCBT中找不到学员
#define C_DRIVERALREADYINDCBT	0x79				// DCBT中已经有该学员
#define C_SIMULATECAR			0x7A				// 训练模拟器
#define C_FC_FALSEFINGER		0x7B				// 假手指
#define C_FC_ERROR				0x7C				// FingerChip出错
#define C_FC_TOO_SLOW			0x7D				// 刮擦太慢
#define C_FC_TOO_FAST			0x7E				// 刮擦太快
#define C_FC_BAD_IMAGE			0x7F				// 图像质量差

#define C_CHANGETIMEERR			0x80				// 微调处理标记

#define C_BASENOTLOGIN			0x81				// 教练员未在场地签到
#define C_BASENOTLOGOUT			0x82				// 教练员未在场地签退
#define C_BASERELOGIN			0x83				// 教练员未在场地签到

#define C_WAITKEYTIMOUT			0xF0				// 按键超时
#define C_NOTOK            		0xFF                // 通用错误

#endif
