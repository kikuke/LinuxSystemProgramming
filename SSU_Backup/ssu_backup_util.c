#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

#include "ssu_backup_define.h"
#include "ssu_backup_usage.h"
#include "ssu_backup_util.h"

struct filetree* FindFileTreeInPath(const char* path, struct filetree* ftree, int isBackup)
{
	size_t nextStartPathLen;
	size_t ftreeNameLen;
	size_t pathFileLen;
	struct filetree* retTree = NULL;

	nextStartPathLen = 0;
	if(path[0] == '/'){
		nextStartPathLen++;
	}

	ftreeNameLen = strlen(ftree->file);
	if(isBackup && ftree->childNodeNum == 0){
		ftreeNameLen -= SSU_BACKUP_FILE_META_LEN;
	}
	pathFileLen = 0;
	while((path[nextStartPathLen] != '/') && (path[nextStartPathLen] != '\0')){
		if(ftree->file[pathFileLen] != path[nextStartPathLen]){
			return NULL;
		}
		nextStartPathLen++;
		pathFileLen++;
	}
	if(pathFileLen != ftreeNameLen){
		return NULL;
	}
	if(ftree->childNodeNum == 0){
		if(path[nextStartPathLen] != '\0')
			return NULL;

		return ftree;
	}

	for(int i=0; i<ftree->childNodeNum; i++){
		retTree = FindFileTreeInPath(&path[nextStartPathLen], ftree->childNodes[i], isBackup);
		if(retTree != NULL)
			return retTree;
	}
	return NULL;
}

char* GetRealNameByFileTree(char* buf, const struct filetree* ftree)
{
	size_t fileLen;

	if(ftree->childNodeNum > 0){
		strcpy(buf, ftree->file);
		return buf;
	}

	fileLen = strlen(ftree->file) - SSU_BACKUP_FILE_META_LEN;
	strncpy(buf, ftree->file, fileLen);

	return buf;
}

char* GetPathByFileTree(char* buf, struct filetree* ftree, int isBackup)
{
	struct filetree* ptree = ftree;
	size_t parentLen;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	if(isBackup){
		GetRealNameByFileTree(buf, ptree);
	} else {
		strcpy(buf, ptree->file);
	}
	while(ptree->parentNode != NULL){
		parentLen = strlen(ptree->parentNode->file);
		strcpy(pathBuf, buf);
		strcpy(buf, ptree->parentNode->file);
		buf[parentLen] = '/';
		strcpy(buf + parentLen + 1, pathBuf);
		ptree = ptree->parentNode;
	}

	return buf;
}

struct filetree* PathToFileTree(const char* path, int hashMode)
{
	struct filetree* ftree = NULL;
	int fileType;

	fileType = CheckFileTypeByPath(path);
	switch(fileType){
		case SSU_BACKUP_TYPE_ERROR:
			ftree = NULL;
			break;
		case SSU_BACKUP_TYPE_REG:
			ftree = FileToFileTree(path, hashMode);
			break;
		case SSU_BACKUP_TYPE_DIR:
			ftree = _PathToFileTreeDir(path, hashMode);
			break;
		default:
			ftree = NULL;
			break;
	}

	return ftree;
}

struct filetree* _PathToFileTreeDir(const char* path, int hashMode)
{
	struct filetree* ptree = NULL;
	struct filetree* ctree = NULL;
	struct dirent** childList;
	int childCount;
	int realChildCnt;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	if((childCount = scandir(path, &childList, filterParentInScanDir, alphasort)) < 1){
		return NULL;
	}
	ptree = FileToFileTree(path, hashMode);

	realChildCnt = 0;
	for(int i=0; i<childCount; i++){
		strcpy(pathBuf, path);
		ConcatPath(pathBuf, childList[i]->d_name);
		ctree = PathToFileTree(pathBuf, hashMode);
		free(childList[i]);
		if(ctree == NULL) continue;

		AddChildFileNode(ptree, ctree);	
		realChildCnt++;
	}
	free(childList);

	if(realChildCnt < 1)
		return NULL;
	return ptree;
}

int filterParentInScanDir(const struct dirent* target)
{
	if(!strcmp(target->d_name, ".") || !strcmp(target->d_name, "..")){
		return 0;
	}
	return 1;
}

struct filetree* FileToFileTree(const char* path, int hashMode)
{
	struct filetree* ftree = NULL;
	char* fileName = NULL;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];
	char hashBuf[SSU_BACKUP_HASH_MAX_LEN];
	int hashReturnVal;
	size_t hash_sz = 0;

	strcpy(pathBuf, path);
	if((fileName = GetFileNameByPath(pathBuf)) == NULL){
		return NULL;
	}

	switch(hashMode){
		case SSU_BACKUP_HASH_MD5:
			hashReturnVal = GetMd5HashByPath(path, hashBuf);
			hash_sz = MD5_DIGEST_LENGTH;
			break;
		case SSU_BACKUP_HASH_SHA1:
			hashReturnVal = GetSha1HashByPath(path, hashBuf);
			hash_sz = SHA_DIGEST_LENGTH;
			break;
		default:
			hashReturnVal = -1;
			break;
	}
	if(hashReturnVal == -1){
		perror("GetHash()");
		return NULL;
	}

	ftree = CreateFileTree(fileName, hashBuf, hash_sz);

	return ftree;
}

char* GetFileNameByPath(char* path)
{
	char* fileName = NULL;
	char* fptr = strtok(path, "/");
	
	while(fptr != NULL)
	{
		fileName = fptr;
		fptr = strtok(NULL, "/");
	}

	return fileName;
}

int CompareHash(const struct filetree* tree1, const struct filetree* tree2, int hashMode)
{
	size_t cmpLen = 0;
	if(hashMode == SSU_BACKUP_HASH_MD5){
		cmpLen = MD5_DIGEST_LENGTH;
	} else if(hashMode == SSU_BACKUP_HASH_SHA1){
		cmpLen = SHA_DIGEST_LENGTH;
	} else
		return 0;
	return !strncmp(tree1->hash, tree2->hash, cmpLen);
}

int GetMd5HashByPath(const char* path, char* hashBuf)
{
	MD5_CTX c;
	int fd;
	unsigned char buf[SSU_BACKUP_HASH_BUF_SZ];
	size_t readLen;

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
	size_t readLen;

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

char* ConcatPath(char* dest, const char* target)
{
	strcat(dest, "/");
	strcat(dest, target);
	return dest;
}

char* GetRealpathAndHandle(const char* path, char* resolved_path, SSU_BACKUP_IDX thisUsage)
{
	char* homeDir = getenv("HOME");
	char backup_path[SSU_BACKUP_MAX_PATH_SZ];

	if((realpath(path, resolved_path) == NULL)){
		switch(errno){
			case EACCES:
				fputs("권한이 없습니다.", stderr);
				break;

			case EINVAL:
				fputs("경로의 값이 NULL입니다.", stderr);
				break;
			
			case ENAMETOOLONG:
				fputs("경로의 길이가 큽니다.", stderr);
				break;

			case ENOENT:
				Usage(thisUsage);
				break;

			case ENOTDIR:
				Usage(thisUsage);
				break;

			default:
				fputs("Unhandled RealpathError!", stderr);
				break;
		}
		return NULL;
	}

	if(strncmp(homeDir, resolved_path, strlen(homeDir)) != 0){
		fprintf(stdout, "<%s> can't be backuped\n", resolved_path);
		return NULL;
	}

	if(strncmp(resolved_path, homeDir, strlen(resolved_path)) == 0){
		fprintf(stdout, "<%s> can't be backuped\n", resolved_path);
		return NULL;
	}

	GetBackupPath(backup_path);
	if(strncmp(backup_path, resolved_path, strlen(backup_path)) == 0){
		fprintf(stdout, "<%s> can't be backuped\n", resolved_path);
		return NULL;
	}

	if(CheckFileTypeByPath(resolved_path) == SSU_BACKUP_TYPE_OTHER){
		fputs("일반 파일이나 디렉토리가 아닙니다.", stderr);
		return NULL;
	}
	
	return resolved_path;
}

int CheckFileTypeByPath(const char* path)
{
	struct stat f_stat;
	if(stat(path, &f_stat) == -1){
		return SSU_BACKUP_TYPE_ERROR;
	}

	return CheckFileType(&f_stat);
}

int CheckFileType(const struct stat* p_stat)
{
	if(S_ISREG((*p_stat).st_mode)){
		return SSU_BACKUP_TYPE_REG;
	} else if(S_ISDIR((*p_stat).st_mode)){
		return SSU_BACKUP_TYPE_DIR;
	}

	return SSU_BACKUP_TYPE_OTHER;
}

char* GetBackupPath(char* buf)
{
	char* homeDir = getenv("HOME");
	strcpy(buf, homeDir);
	ConcatPath(buf, SSU_BACKUP_ROOT_DIR_NAME);

	return buf;
}

int SetBackupPath(char* buf)
{
	GetBackupPath(buf);
	return mkdir(buf, SSU_BACKUP_MKDIR_AUTH);
}

int SetHashMode(int mode)
{
	int fd;
	char path_buf[SSU_BACKUP_MAX_PATH_SZ];

	GetBackupPath(path_buf);
	ConcatPath(path_buf, SSU_BACKUP_HASH_SET_FILE);
	if((fd = open(path_buf, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1){
		return -1;
	}
	
	write(fd, (void*)&mode, sizeof(mode));
	close(fd);
	return 0;
}

int GetHashMode()
{
	int fd;
	int mode;
	char path_buf[SSU_BACKUP_MAX_PATH_SZ];

	GetBackupPath(path_buf);
	ConcatPath(path_buf, SSU_BACKUP_HASH_SET_FILE);
	if((fd = open(path_buf, O_RDONLY)) == -1){
		return -1;
	}
	read(fd, &mode, sizeof(mode));

	return mode;
}
