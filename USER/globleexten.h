/*全局变量声明*/
#ifndef _globleexten_h_
#define _globleexten_h_

#include "stm32f10x.h"
#include "time.h"

extern u8 reboot;

extern u8 DOWNLOADKEY;		//下载密钥操作标志
extern u16 KeyCuration;		//按键被持续按下的时间
extern u8 KeyPressflag;		//按键是否有被按下
extern u8 WaitKeyPressflag;	//是否在等待按键按下
extern u16 g_FPTimeOut;		//指纹超时控制
extern u8 USARTInterruptFlag;//串口1中断标志位
extern u8 gcRetCode;		//串口2接收数据成功与失败标志位
extern u8 RefreshTime;		//是否需要刷新屏幕显示时间
extern u8 UpdateTimeFlag;	//上位机是否有下发时间来应答设备需要更新时间的请求
extern struct tm time;		//时间结构体
extern u8 SysVersionStr[14];//系统版本号


extern u8 Mapped[32]; 		//指纹模块返回信息映射表
extern u8 PackageBuffer[80]; //数据包接收缓冲区

extern u32 gwSpeedPluseMile;//脉冲计数器
#endif
