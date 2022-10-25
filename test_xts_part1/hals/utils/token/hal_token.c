#include "hal_token.h"
#include "bk_cli.h"
#include "ohos_errno.h"
#include "ohos_types.h"
#include "utils_file.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_file.h"
#include <securec.h>
#include "iot_device.h"


#define TokenPathA   "/usrdata/hilink/tokenA.cfg"
#define TokenPathB   "/usrdata/hilink/tokenB.cfg"
#define TOKEN_SIZE 151
#define BITS_PER_BYTE 8
// 4 Bytes for token flag
// if token's both area are available, when read token, always return area which flag is bigger;
// and recover area which flag is small while write token.
#define TOKEN_FLAG_SIZE 4
#define TOKEN_WITH_FLAG_SIZE (TOKEN_SIZE + TOKEN_FLAG_SIZE)

#define tokenstring "Y1DjGH2dzole/ZXcWPSBj1aysCNBqJ633C2AJPG0tHliEktcHys3WW15F/fSLEcP,kf+TmQyFh43H7loKz80Zz8Avog6xA+u3hM47Qcm2UicQ5GrHNRNBbnhxxYm8Q7Ve,n0yQTslNQFsnvrM3,0000"

static uint32_t GetTokenFlag(const char tokenWithFlag[])
{
    uint32_t result = 0;
    for (uint32_t i = 0; i < TOKEN_FLAG_SIZE; i++) {
        result |= ((uint8_t)tokenWithFlag[TOKEN_SIZE + i]) << ((TOKEN_FLAG_SIZE - 1 - i) * BITS_PER_BYTE);
    }
    return result;
}

static void SetTokenFlag(uint8_t flag[], uint32_t value)
{
    for (uint32_t i = 0; i < TOKEN_FLAG_SIZE; i++) {
        flag[i] = (value >> (BITS_PER_BYTE * (TOKEN_FLAG_SIZE - 1 - i))) & 0xFF;
    }
}


static int OEMReadToken(char *token, unsigned int len)
{
	int tokenAfd,tokenBfd;
	int tokenReadLen;
	(void) tokenReadLen;
	tokenAfd = HalFileOpen(TokenPathA  , O_RDWR_FS, 0);
	tokenBfd = HalFileOpen(TokenPathB, O_RDWR_FS, 0);
	if((tokenAfd < 0)&&(tokenBfd < 0)){
		memcpy((char *)token, (char *)tokenstring, len);
		return EC_SUCCESS;//-2;

	}else if((tokenAfd >= 0)&&(tokenBfd < 0)){
		HalFileSeek(tokenAfd, 0, SEEK_SET_FS);
		tokenReadLen = HalFileRead(tokenAfd,token,len);
		HalFileClose(tokenAfd);
		return EC_SUCCESS;

	}else if((tokenAfd < 0)&&(tokenBfd >= 0)){
		HalFileSeek(tokenBfd, 0, SEEK_SET_FS);
		tokenReadLen = HalFileRead(tokenBfd,token,len);
		HalFileClose(tokenBfd);
		return EC_SUCCESS;

	}else if((tokenAfd >= 0)&&(tokenBfd >= 0)){
		char *tokenA_withFlag = malloc(TOKEN_WITH_FLAG_SIZE);
		char *tokenB_withFlag = malloc(TOKEN_WITH_FLAG_SIZE);
		HalFileSeek(tokenAfd, 0, SEEK_SET_FS);
		HalFileSeek(tokenBfd, 0, SEEK_SET_FS);
		tokenReadLen = HalFileRead(tokenAfd,tokenA_withFlag,TOKEN_WITH_FLAG_SIZE);
		tokenReadLen = HalFileRead(tokenBfd,tokenB_withFlag,TOKEN_WITH_FLAG_SIZE);
        uint32_t flagA = GetTokenFlag(tokenA_withFlag);
        uint32_t flagB = GetTokenFlag(tokenB_withFlag);
		if(flagA >= flagB){
			(void)memcpy_s(token, len, tokenA_withFlag, len);
		}else{
			(void)memcpy_s(token, len, tokenB_withFlag, len);
		}
		free(tokenA_withFlag);
		free(tokenB_withFlag);
		HalFileClose(tokenAfd);
		HalFileClose(tokenBfd);
		return EC_SUCCESS;
	}

}

static int OEMWriteToken(const char *token, unsigned int len)
{
	int tokenAfd,tokenBfd;
	int tokenAfileSize,tokenBfileSize;
	int tokenWriteLen;
	(void )tokenWriteLen;
	uint32_t flagA,flagB;

	tokenAfileSize = FileSize(TokenPathA  );
	tokenBfileSize = FileSize(TokenPathB);
	
	if((tokenAfileSize <= 0)&&(tokenBfileSize <= 0)){
		//no A,no B. creat A
		tokenAfd = HalFileOpen(TokenPathA, O_CREAT_FS|O_RDWR_FS, 0);
		if(tokenAfd>=0){
			char *tokenA_withFlag = malloc(TOKEN_WITH_FLAG_SIZE);
			(void)memcpy_s(tokenA_withFlag, len, token, len);
			SetTokenFlag((uint8_t *)&tokenA_withFlag[TOKEN_SIZE],1);

			tokenWriteLen = HalFileWrite(tokenAfd,tokenA_withFlag,TOKEN_WITH_FLAG_SIZE);
			free(tokenA_withFlag);
			HalFileClose(tokenAfd);
		}

	}else if((tokenAfileSize > 0)&&(tokenBfileSize <= 0)){
		//only A. creat B
		tokenAfd = HalFileOpen(TokenPathA, O_RDWR_FS, 0);
		tokenBfd = HalFileOpen(TokenPathB, O_CREAT_FS|O_RDWR_FS, 0);
		if(tokenAfd>=0){
			char *tokenA_withFlag = malloc(TOKEN_WITH_FLAG_SIZE);
			HalFileSeek(tokenAfd, 0, SEEK_SET_FS);
			HalFileRead(tokenAfd,tokenA_withFlag,TOKEN_WITH_FLAG_SIZE);
			flagA = GetTokenFlag(tokenA_withFlag);
			free(tokenA_withFlag);
			HalFileClose(tokenAfd);
		}else{
			flagA = 0;
		}
	
		flagB = flagA + 1;

		if(tokenBfd>=0){
			char *tokenB_withFlag = malloc(TOKEN_WITH_FLAG_SIZE);
			(void)memcpy_s(tokenB_withFlag, len, token, len);
			SetTokenFlag((uint8_t *)&tokenB_withFlag[TOKEN_SIZE],flagB);
			tokenWriteLen = HalFileWrite(tokenBfd,tokenB_withFlag,TOKEN_WITH_FLAG_SIZE);
			free(tokenB_withFlag);
			HalFileClose(tokenBfd);
		}

	}else if((tokenAfileSize <= 0)&&(tokenBfileSize > 0)){
		//only B.creat A
		tokenBfd = HalFileOpen(TokenPathB, O_RDWR_FS, 0);
		tokenAfd = HalFileOpen(TokenPathA, O_CREAT_FS|O_RDWR_FS, 0);
		if(tokenBfd>=0){
			char *tokenB_withFlag = malloc(TOKEN_WITH_FLAG_SIZE);
			HalFileSeek(tokenBfd, 0, SEEK_SET_FS);
			HalFileRead(tokenBfd,tokenB_withFlag,TOKEN_WITH_FLAG_SIZE);
			flagB = GetTokenFlag(tokenB_withFlag);
			free(tokenB_withFlag);
			HalFileClose(tokenBfd);
		}else{
			flagB = 0;
		}
	
		flagA = flagB + 1;

		if(tokenAfd>=0){
			char *tokenA_withFlag = malloc(TOKEN_WITH_FLAG_SIZE);
			(void)memcpy_s(tokenA_withFlag, len, token, len);
			SetTokenFlag((uint8_t *)&tokenA_withFlag[TOKEN_SIZE],flagA);
			HalFileSeek(tokenAfd, 0, SEEK_SET_FS);
			tokenWriteLen = HalFileWrite(tokenAfd,tokenA_withFlag,TOKEN_WITH_FLAG_SIZE);
			free(tokenA_withFlag);
			HalFileClose(tokenAfd);
		}

	}else if((tokenAfileSize > 0)&&(tokenBfileSize > 0)){
		tokenAfd = HalFileOpen(TokenPathA, O_RDWR_FS, 0);
		tokenBfd = HalFileOpen(TokenPathB, O_RDWR_FS, 0);
		char *tokenA_withFlag = malloc(TOKEN_WITH_FLAG_SIZE);
		char *tokenB_withFlag = malloc(TOKEN_WITH_FLAG_SIZE);
		//get flagA,flagB
		if(tokenAfd>=0){
			HalFileSeek(tokenAfd, 0, SEEK_SET_FS);
			HalFileRead(tokenAfd,tokenA_withFlag,TOKEN_WITH_FLAG_SIZE);
			flagA = GetTokenFlag(tokenA_withFlag);
		}else{
			flagA = 0;
		}

		if(tokenBfd>=0){
			HalFileSeek(tokenBfd, 0, SEEK_SET_FS);
			HalFileRead(tokenBfd,tokenB_withFlag,TOKEN_WITH_FLAG_SIZE);
			flagB = GetTokenFlag(tokenB_withFlag);
		}else{
			flagB = 0;
		}
		//write file
		if(flagA <= flagB){
			if(tokenAfd>=0){
				flagA = flagB + 1;
				(void)memcpy_s(tokenA_withFlag, len, token, len);
				SetTokenFlag((uint8_t *)&tokenA_withFlag[TOKEN_SIZE],flagA);
				HalFileSeek(tokenAfd, 0, SEEK_SET_FS);
				tokenWriteLen = HalFileWrite(tokenAfd,tokenA_withFlag,TOKEN_WITH_FLAG_SIZE);
			}
		}else{
			if(tokenBfd>=0){
				flagB = flagA + 1;
				(void)memcpy_s(tokenB_withFlag, len, token, len);
				SetTokenFlag((uint8_t *)&tokenB_withFlag[TOKEN_SIZE],flagB);
				HalFileSeek(tokenBfd, 0, SEEK_SET_FS);
				tokenWriteLen = HalFileWrite(tokenBfd,tokenB_withFlag,TOKEN_WITH_FLAG_SIZE);
			}

		}
		//close file
		if(tokenAfd>=0) HalFileClose(tokenAfd);
		if(tokenBfd>=0) HalFileClose(tokenBfd);
		free(tokenA_withFlag);
		free(tokenB_withFlag);

	}
    return EC_SUCCESS;
}

static int OEMGetAcKey(char *acKey, unsigned int len)
{
	extern unsigned char A_C[48];
	memcpy(acKey,A_C,len);
    return EC_SUCCESS;
}

static int OEMGetProdId(char *productId, unsigned int len)
{
	memcpy(productId,PRODUCT_ID,len);
    return EC_SUCCESS;
}
//c35d6777954b439aaf2da4a1e3b1fe61
static int OEMGetProdKey(char *productKey, unsigned int len)
{
	memcpy(productKey,PRODUCT_KEY,len);
    return EC_SUCCESS;
}


int HalReadToken(char *token, unsigned int len)
{
    if (token == NULL) {
        return EC_FAILURE;
    }

    return OEMReadToken(token, len);
}

int HalWriteToken(const char *token, unsigned int len)
{
    if ((token == NULL) ||(len==0)){
        return EC_FAILURE;
    }

    return OEMWriteToken(token, len);
}

int HalGetAcKey(char *acKey, unsigned int len)
{
    if (acKey == NULL) {
        return EC_FAILURE;
    }

    return OEMGetAcKey(acKey, len);
}

int HalGetProdId(char *productId, unsigned int len)
{
    if (productId == NULL) {
        return EC_FAILURE;
    }

    return OEMGetProdId(productId, len);
}

int HalGetProdKey(char *productKey, unsigned int len)
{
    if (productKey == NULL) {
        return EC_FAILURE;
    }

    return OEMGetProdKey(productKey, len);
}

void token_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	static char *tokenbuf;
	static uint8 token_pos = 0;
		if (argc >= 1)
		{
			if (os_strcmp(argv[1], "w") == 0){
				if (os_strcmp(argv[2], "part1") == 0){
					if(strlen(argv[3]) <= TOKEN_SIZE){
						if(tokenbuf == NULL){
							tokenbuf = (char *)malloc(TOKEN_SIZE+1);
						}
						(void)memcpy_s(tokenbuf, TOKEN_SIZE, argv[3], strlen(argv[3]));
						token_pos = strlen(argv[3]);
					}else{
						printf("token part1 len valid.should <= 151\r\n");
					}
				}else if (os_strcmp(argv[2], "part2") == 0){
					if(tokenbuf == NULL){
						printf("token buff if NULL.should write part1 first!\r\n");
					}else{
						(void)memcpy_s(&tokenbuf[token_pos], TOKEN_SIZE, argv[3], strlen(argv[3]));
						printf("token len : %d = %d + %d. : %s\r\n",token_pos+strlen(argv[3]), token_pos,strlen(argv[3]),tokenbuf);
						if((token_pos+strlen(argv[3]))==TOKEN_SIZE){
							HalWriteToken(tokenbuf,TOKEN_SIZE);
						}else{
							printf("token len valid.part1+part2 should be 151\r\n");
						}
						free(tokenbuf);
					}

				}else if (os_strcmp(argv[2], "local") == 0){
					HalWriteToken(tokenstring,sizeof(tokenstring));
				}else{
					printf("str len:%d\r\n",strlen(argv[2]));
					if(strlen(argv[2])==TOKEN_SIZE){
						HalWriteToken(argv[2],TOKEN_SIZE);
					}
				}
			}else if(os_strcmp(argv[1], "r") == 0){
				char *tokenRbuf = (char *)malloc(TOKEN_SIZE+1);
				HalReadToken(tokenRbuf,TOKEN_SIZE);
				char tmp_c;
				tmp_c = tokenRbuf[80];
				tokenRbuf[80] = '\0';
				printf("tokenbuf =%s",tokenRbuf); //printf part1
				tokenRbuf[80] = tmp_c;
				tokenRbuf[151] = '\0';
				printf("%s\r\n",&tokenRbuf[80]); //printf part2
				free(tokenRbuf);

			}
		}
}

const struct cli_command token_clis[] = {

	{"token", "token w/r ", token_command},

};

void bk_token_cli_init(void)
{
	int ret;

	bk_printf("token cli int \r\n");
	ret = cli_register_commands(token_clis, sizeof(token_clis) / sizeof(struct cli_command));
	if (ret)
		bk_printf("ret: %d token commands fail.\r\n",ret);
}


