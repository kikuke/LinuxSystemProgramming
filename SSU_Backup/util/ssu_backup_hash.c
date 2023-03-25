#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

#include "ssu_backup_define.h"
#include "ssu_backup_hash.h"

int CompareHashByPath(const char* path1, const char* path2, int hashMode)
{
	char hash1[SSU_BACKUP_HASH_MAX_LEN];
	char hash2[SSU_BACKUP_HASH_MAX_LEN];
	
	if(GetHashByPath(path1, hash1, hashMode) == -1){
		perror("GetHashByPath()");
		return -1;
	}
	if(GetHashByPath(path2, hash2, hashMode) == -1){
		perror("GetHashByPath()");
		return -1;
	}

	return CompareHash(hash1, hash2, hashMode);
}

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

int GetHashByPath(const char* path, char* hashBuf, int hashMode)
{
	switch(hashMode){
		case SSU_BACKUP_HASH_MD5:
			return GetMd5HashByPath(path, hashBuf);
			break;
		case SSU_BACKUP_HASH_SHA1:
			return GetSha1HashByPath(path, hashBuf);
			break;
		default:
			return -1;
			break;
	}

	return -1;
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

	close(fd);
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

	close(fd);
	return 0;
}