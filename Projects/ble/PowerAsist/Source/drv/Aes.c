
#include "Ll.h"

#include "Aes.h"

//len % 16 == 0
bool AesEncryptData(uint8 *key, uint8 *plainText, uint8 len, uint8 *cypherText)
{
	for (uint8 i = 0; i < len; i += 16)
	{
		if (LL_Encrypt(key, &plainText[i], &cypherText[i]) != LL_STATUS_SUCCESS)
		{
			return false;
		}
	}

	return true;
}

//len % 16 == 0
bool AesDecryptData(uint8 *key, uint8 *cypherText, uint8 len, uint8 *decypherText)
{
	for (uint8 i = 0; i < len; i += 16)
	{
		if (LL_EXT_Decrypt(key, &cypherText[i], &decypherText[i]) != LL_STATUS_SUCCESS)
		{
			return false;
		}
	}

	return true;
}

