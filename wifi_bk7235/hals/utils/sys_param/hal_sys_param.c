// Copyright (C) 2022 Beken Corporation
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "hal_sys_param.h"


#define PRD_Lamp_2HSB 1 /* DALEN product  wifi+BLE +KIT */
#define PRD_Lamp_2IES 2 /* DALEN product  wifi+BLE +KIT */


#define USING_PRODUCT_ID PRD_Lamp_2HSB

#if(USING_PRODUCT_ID == PRD_Lamp_2HSB)
static const char OHOS_DEVICE_TYPE[] = {"Table Lamp"};
static const char OHOS_DISPLAY_VERSION[] = {"OpenHarmony 3.0 LTS"};
static const char OHOS_MANUFACTURE[] = {"BEKEN"};
static const char OHOS_BRAND[] = {"BEKEN"};
static const char OHOS_MARKET_NAME[] = {"BK7231M-LIGHT"};
static const char OHOS_PRODUCT_SERIES[] = {"light"};
static const char OHOS_PRODUCT_MODEL[] = {"light-01"};
static const char OHOS_SOFTWARE_MODEL[] = {"1.0.0"};
static const char OHOS_HARDWARE_MODEL[] = {"1.0.0"};
static const char OHOS_HARDWARE_PROFILE[] = {"wifi:true,ble:true"};
static const char OHOS_BOOTLOADER_VERSION[] = {"bootloader"};
static const char OHOS_ABI_LIST[] = {"riscv-liteos"};
//static const char OHOS_SERIAL[] = {"C86000000001"};
char OHOS_SERIAL[16] = {0};
#elif(USING_PRODUCT_ID == PRD_Lamp_2IES)
static const char OHOS_DEVICE_TYPE[] = {"Table Lamp"};
static const char OHOS_DISPLAY_VERSION[] = {"OpenHarmony 3.0 LTS"};
static const char OHOS_MANUFACTURE[] = {"BEKEN"};
static const char OHOS_BRAND[] = {"BEKEN"};
static const char OHOS_MARKET_NAME[] = {"BK7235-LIGHT"};
static const char OHOS_PRODUCT_SERIES[] = {"bk7235"};
static const char OHOS_PRODUCT_MODEL[] = {"bk7235-002"};
static const char OHOS_SOFTWARE_MODEL[] = {"1.0.0"};
static const char OHOS_HARDWARE_MODEL[] = {"1.0.0"};
static const char OHOS_HARDWARE_PROFILE[] = {"wifi:true,ble:true"};
static const char OHOS_BOOTLOADER_VERSION[] = {"bootloader"};
static const char OHOS_ABI_LIST[] = {"riscv-liteos"};

//static const char OHOS_SERIAL[] = {"123456789"};
char OHOS_SERIAL[16] = {0};

#endif
static const int OHOS_FIRST_API_VERSION = 1;

const char* HalGetDeviceType(void)
{
    return OHOS_DEVICE_TYPE;
}

const char* HalGetManufacture(void)
{
    return OHOS_MANUFACTURE;
}

const char* HalGetBrand(void)
{
    return OHOS_BRAND;
}

const char* HalGetMarketName(void)
{
    return OHOS_MARKET_NAME;
}

const char* HalGetProductSeries(void)
{
    return OHOS_PRODUCT_SERIES;
}

const char* HalGetProductModel(void)
{
    return OHOS_PRODUCT_MODEL;
}

const char* HalGetSoftwareModel(void)
{
    return OHOS_SOFTWARE_MODEL;
}

const char* HalGetHardwareModel(void)
{
    return OHOS_HARDWARE_MODEL;
}

const char* HalGetHardwareProfile(void)
{
    return OHOS_HARDWARE_PROFILE;
}
char make_ohos_serial[16] = {0};
#define in_range(c, lo, up)  ((unsigned char)c >= lo && (unsigned char)c <= up)
#define islower(c) 		in_range(c, 'a', 'z')
#include "string.h"
const char* HalGetSerial(void)
{
	#if 1
	typedef int 			bk_err_t;			 /**< Return error code */
	typedef unsigned char   uint8_t;
	typedef unsigned char 	mac_type_t;
	uint8_t mac_read[6];

	extern bk_err_t bk_get_mac(uint8_t *mac, mac_type_t type);
	bk_get_mac(mac_read,0);
	os_snprintf(&make_ohos_serial[0],sizeof(make_ohos_serial),"%02x%02x%02x%02x%02x%02x",mac_read[0],mac_read[1],mac_read[2],mac_read[3],mac_read[4],mac_read[5]);
	
	for(int i = 0; i<sizeof(make_ohos_serial);i++)
	{
		if(islower(make_ohos_serial[i]))
			make_ohos_serial[i] +='A' - 'a';
	}
	memcpy(OHOS_SERIAL,make_ohos_serial,strlen(make_ohos_serial)+1);
	#endif
	printf("OHOS_SERIAL=%s\r\n",OHOS_SERIAL);
	return OHOS_SERIAL;
}

const char* HalGetBootloaderVersion(void)
{
    return OHOS_BOOTLOADER_VERSION;
}

const char* HalGetAbiList(void)
{
    return OHOS_ABI_LIST;
}

const char* HalGetDisplayVersion(void)
{
    return OHOS_DISPLAY_VERSION;
}

const char* HalGetIncrementalVersion(void)
{
    return INCREMENTAL_VERSION;
}

const char* HalGetBuildType(void)
{
    return BUILD_TYPE;
}

const char* HalGetBuildUser(void)
{
    return BUILD_USER;
}

const char* HalGetBuildHost(void)
{
    return BUILD_HOST;
}

const char* HalGetBuildTime(void)
{
    return BUILD_TIME;
}

int HalGetFirstApiVersion(void)
{
    return OHOS_FIRST_API_VERSION;
}
