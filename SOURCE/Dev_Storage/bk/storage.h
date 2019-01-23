#ifndef STORAGE_H
#define STORAGE_H

#define FLASHADDR_BASEINFO      		0x0000	//基本信息
	#define BASEOFFSET_DEVICEVER		0x00	//设备版本（2B）
	#define BASEOFFSET_DEVICEADDR		0x02	//设备地址（2B）
	#define BASEOFFSET_BAUD				0x04	//波特率（2B）
	#define BASEOFFSET_INTERVAL         0x06    //输出间隔时间
	#define BASEOFFSET_OUTPUTFORMAT		0x08	//输出格式，十进制或十六进制（2B）
	#define BASEOFFSET_OUTPUTMODE		0x0A	//输出模式，主动输出和被动输出（2B）
	#define BASEOFFSET_BUZZER			0x0C	//蜂鸣器控制（2B）
	#define BASEOFFSET_RELAY			0x0E	//继电器调整值（2B）
	
#endif



