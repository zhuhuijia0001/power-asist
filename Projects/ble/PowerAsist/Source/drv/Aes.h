
#ifndef _AES_H_
#define _AES_H_

extern bool AesEncryptData(uint8 *key, uint8 *plainText, uint8 len, uint8 *cypherText);

extern bool AesDecryptData(uint8 *key, uint8 *cypherText, uint8 len, uint8 *decypherText);

#endif

