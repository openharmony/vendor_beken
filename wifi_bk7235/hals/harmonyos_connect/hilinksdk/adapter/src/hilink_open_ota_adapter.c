/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: OTA适配实现 (需设备厂商实现)
 */
#include "hilink_open_ota_adapter.h"
#include <stdio.h>
#include <unistd.h>
#include "securec.h"


#define DOWNLOAD_PATITION_ADDR 0x253000   //68+2048+300KB
#define DOWNLOAD_PATITION_LEN 0x187000	  //1536KB
static unsigned int write_length = 0;
#define HILINK_OK 0
#define HILINK_ERROR -1

/*
 * Flash初始化
 * 返回值是true时，表示初始化正常
 * 返回值是false时，表示初始化异常
 */
bool HILINK_OtaAdapterFlashInit(void)
{
	printf("HILINK_OtaAdapterFlashInit start\r\n");
	IoTFlashInit();
	write_length = 0;
	return true;
}


/*
 * 判断需要升级的分区
 * 返回值是UPGRADE_FW_BIN1时，表示升级固件到分区1
 * 返回值是UPGRADE_FW_BIN2时，表示升级固件到分区2
 */
unsigned int HILINK_OtaAdapterGetUpdateIndex(void)
{
	printf("HILINK_OtaAdapterGetUpdateIndex start\r\n");
    return UPGRADE_FW_BIN1;
}

/*
 * 擦除需要升级的分区
 * size表示需要擦除的分区大小
 * 返回值是ILINK_OK时，表示擦除成功
 * 返回值是HILINK_ERROR时，表示擦除失败
 */
int HILINK_OtaAdapterFlashErase(unsigned int size)
{
	printf("HILINK_OtaAdapterFlashErase start size=%d\r\n",size);
	return IoTFlashErase(DOWNLOAD_PATITION_ADDR,DOWNLOAD_PATITION_LEN);
}

/*
 * 升级数据写入升级的分区
 * buf表示待写入数据
 * bufLen表示待写入数据的长度
 * 返回值是HILINK_OK时，表示写入成功
 * 返回值是HILINK_ERROR时，表示写入失败
 */
int HILINK_OtaAdapterFlashWrite(const unsigned char *buf, unsigned int bufLen)
{
	//printf("HILINK_OtaAdapterFlashWrite start\r\n");
	#if 1
	int ret;
	if (bufLen == 0) 
	{
		return HILINK_OK;
	}
	if (buf == NULL)
	{
		return HILINK_ERROR;
	}
	
	ret = IoTFlashWrite(DOWNLOAD_PATITION_ADDR + write_length, bufLen, buf, 0);

	write_length += bufLen;
	if(ret == 0)
		return HILINK_OK;
	else
		return HILINK_ERROR;
	#else
	return HILINK_OK;
	#endif
}


/*
 * 读取升级分区数据
 * offset表示读写偏移
 * buf表示输出数据的内存地址
 * bufLen表示输出数据的内存长度
 * 返回值是HILINK_OK时，表示读取成功
 * 返回值是HILINK_ERROR时，表示读取失败
 */
int HILINK_OtaAdapterFlashRead(unsigned int offset, unsigned char *buf, unsigned int bufLen)
{
	//printf("HILINK_OtaAdapterFlashRead start\r\n");
	#if 1
    int ret;
	if ( (buf == NULL) || (bufLen == 0)) 
	{
		return HILINK_ERROR;
	}
  ret = IoTFlashRead(DOWNLOAD_PATITION_ADDR + offset, bufLen, buf);

	if(ret == 0)
		return HILINK_OK;
	else
		return HILINK_ERROR;
	#else
	return HILINK_OK;
	#endif
}

/*
 * 分区升级结束
 * 返回值是true时，表示结束正常
 * 返回值是false时，表示结束异常
 */
bool HILINK_OtaAdapterFlashFinish(void)
{
	printf("HILINK_OtaAdapterFlashFinish start\r\n");
	IoTFlashDeinit();
    return true;
}

/* 获取升级区间最大长度 */
unsigned int HILINK_OtaAdapterFlashMaxSize(void)
{
	printf("HILINK_OtaAdapterFlashMaxSize start\r\n");
	
	return DOWNLOAD_PATITION_LEN;
}

/*
 * 根据标志重启模组
 * flag表示重启标志
 * 当flag是RESTART_FLAG_NOW时，表示只有MCU升级时立即重启
 * 当flag是RESTART_FLAG_LATER时，表示有模组时切换分区后再重启
 */
void HILINK_OtaAdapterRestart(int flag)
{
	printf("HILINK_OtaAdapterRestart start\r\n");
 reboot();
    return;
}

/*
 * 开始模组升级
 * type表示升级类型
 * 当type是UPDATE_TYPE_MANUAL时，表示本次升级流程是由用户主动发起的手动升级
 * 当type是UPDATE_TYPE_AUTO时，表示本次升级流程是经过用户同意的自动升级
 * 返回值是RETURN_OK时，表示处理成功，HiLink SDK将开始启动升级流程
 * 返回值是RETURN_ERROR时，表示处理不成功，HiLink SDK将终止本次升级流程
 * 注意：在手动场景场景下，HiLink SDK在接收到用户发出的升级指令后，将直接调用此接口；
 * 在自动升级场景下，当HiLink SDK在调用HilinkGetRebootFlag接口返回值是MODULE_CAN_REBOOT时，HiLink SDK将调用此接口。
 * 厂商可在此接口中完成和升级流程相关的处理。
 * 开机后10分钟到1小时内随机时间检测一次是否有新版本，之后以当前时间为起点，23小时加1小时内随机值周期性检测新版本。
 * 如果用户打开了自动升级开关，检测到有新版本并且是可以重启的情况下，就进行新版本的下载，下载完成后自动重启。
 * 自动升级流程可能在凌晨进行，因此厂商在实现升级流程相关功能时，确保在升级的下载安装固件和重启设备时避免对用户产生
 * 影响，比如发出声音，光亮等。
 */
int HILINK_OtaStartProcess(int type)
{
	printf("HILINK_OtaStartProcess start type =%d\r\n",type);
    return RETURN_OK;
}

/*
 * 模组升级结束
 * status表示升级结果
 * 当status是100时，表示升级成功
 * 当status不是100时，表示升级失败
 * 返回值是RETURN_OK时，表示处理成功，HiLink SDK将置升级标志或切换运行区标志
 * 返回值不是RETURN_OK时，表示处理不成功，HiLink SDK将终止本次升级流程
 * 注意：HiLink SDK在将固件写入到OTA升级区后，且完整性校验通过后，将调用厂商适配的此接口；
 * 厂商可在此接口中完成和升级流程相关的处理。
 * 开机后10分钟到1小时内随机时间检测一次是否有新版本，之后以当前时间为起点，23小时加1小时内随机值周期性检测新版本。
 * 如果用户打开了自动升级开关，检测到有新版本并且是可以重启的情况下，就进行新版本的下载，下载完成后自动重启。
 * 自动升级流程可能在凌晨进行，因此厂商在实现升级流程相关功能时，确保在升级的下载安装固件和重启设备时避免对用户产生
 * 影响，比如发出声音，光亮等；升级类型是否为自动升级可参考接口HilinkOtaStartProcess的参数type的描述。
 */
int HILINK_OtaEndProcess(int status)
{
	printf("HILINK_OtaEndProcess start status=%d\r\n",status);
    return RETURN_OK;
}

/*
 * 判断模组是否能立即升级并重启
 * 返回值是MODULE_CAN_REBOOT时，表示模组可以立即升级并重启，HiLink SDK将开始自动升级流程。
 * 返回值是MODULE_CANNOT_REBOOT时，表示模组不能立即升级并重启，HiLink SDK将不进行本次自动升级流程。
 * 注意：在用户同意设备可以自动升级的情况下，HiLink SDK调用此接口获取设备当前业务状态下，模组是否可以立即升级并重启的标志。
 * 只有当设备处于业务空闲状态时，接口才可以返回MODULE_CAN_REBOOT。
 * 当设备处于业务非空闲状态时，接口返回MODULE_CANNOT_REBOOT。
 */
int HILINK_GetRebootFlag(void)
{
	printf("HILINK_GetRebootFlag start\r\n");
	return MODULE_CAN_REBOOT;
}

/*
 * 触发设备的ota新版本升级
 * 检测到的版本，sdk将通过cb回调函数触发升级
 */
void HILINK_TrigOtaSelfUpdate(TrigSelfUpdateCb cb)
{
    return;
}