#include <fcntl.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

#include "define/ssu_backup_define.h"

int CompareHash(const char* hash1, const char* hash2, int hashMode)
{
	size_t cmpLen = 0;
	if(hashMode == SSU_BACKUP_HASH_MD5){
		cmpLen = MD5_DIGEST_LENGTH;
	} else if(hashMode == SSU_BACKUP_HASH_SHA1){
		cmpLen = SHA_DIGEST_LENGTH;
	} else
		return 0;
	return !strncmp(hash1, hash2, cmpLen);
}

int GetMd5HashByPath(const char* path, char* hashBuf)
{
	MD5_CTX c;
	int fd;
	unsigned char buf[SSU_BACKUP_HASH_BUF_SZ];
	int readLen;

	MD5_Init(&c);
	if((fd = open(path, O_RDONLY)) == -1){
		return -1;
	}
	while((readLen = read(fd, buf, SSU_BACKUP_HASH_BUF_SZ)) > 0){
		MD5_Update(&c, buf, readLen);
	}
	MD5_Final(hashBuf, &c);

	return 0;
}

int GetSha1HashByPath(const char* path, char* hashBuf)
{
	SHA_CTX c;
	int fd;
	unsigned char buf[SSU_BACKUP_HASH_BUF_SZ];
	int readLen;

	SHA1_Init(&c);
	if((fd = open(path, O_RDONLY)) == -1){
		return -1;
	}
	while((readLen = read(fd, buf, SSU_BACKUP_HASH_BUF_SZ)) > 0){
		SHA1_Update(&c, buf, readLen);
	}
	SHA1_Final(hashBuf, &c);

	return 0;
}