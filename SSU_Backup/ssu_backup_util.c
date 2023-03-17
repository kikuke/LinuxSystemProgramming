#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

#include "ssu_backup_define.h"
#include "ssu_backup_usage.h"
#include "ssu_backup_util.h"

char* GetParentPath(const char* path, char* buf)
{
	size_t endLen;

	strcpy(buf, path);
	endLen = strlen(path);
	if(path[endLen] == '/'){
		endLen--;
	}

	while((endLen > 0) && path[endLen] != '/')
		endLen--;
	buf[endLen] = '\0';

	return buf;
}

char* BackupPathToSourcePath(char* path)
{
	char* matchPtr;
	size_t sLen;
	char rootPathName[SSU_BACKUP_MAX_FILENAME];
	char tempPath[SSU_BACKUP_MAX_PATH_SZ];

	strcpy(rootPathName, "/");
	strcat(rootPathName, SSU_BACKUP_ROOT_DIR_NAME);
	strcat(rootPathName, "/");
	if((matchPtr = strstr(path, rootPathName)) == NULL)
		return path;

	strcpy(tempPath, matchPtr + strlen(rootPathName) - 1);
	strcpy(matchPtr, tempPath);
	return path;
}

int MakeDirPath(const char* path)
{
	int fileType;
	char* dirPtr;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	strcpy(pathBuf, path);
	pathBuf[SSU_BACKUP_MAX_PATH_SZ-1] = '\0';
	dirPtr = pathBuf; 

	if(*dirPtr == '/')
		dirPtr++;
	while(*dirPtr != '\0'){
		if(*dirPtr == '/'){
			*dirPtr = '\0';
			if((fileType = CheckFileTypeByPath(pathBuf)) != SSU_BACKUP_TYPE_DIR){
				if(mkdir(pathBuf, SSU_BACKUP_MKDIR_AUTH) == -1)
					return -1;
			}
			*dirPtr = '/';
		}
		dirPtr++;
	}
	if((fileType = CheckFileTypeByPath(pathBuf)) != SSU_BACKUP_TYPE_DIR){
		if(mkdir(pathBuf, SSU_BACKUP_MKDIR_AUTH) == -1)
			return -1;
	}

	return 0;
}

char* ExtractHomePath(char* path)
{
	char* homeDir = getenv("HOME");
	char* homePtr = strstr(path, homeDir);
	size_t homeLen;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	if(homePtr == NULL || homePtr != path){
		return path;
	}
	homeLen = strlen(homeDir);
	strcpy(pathBuf, path + homeLen + 1);
	strcpy(path, pathBuf);

	return path;
}

int GetNowTime(char* buf)
{
	struct timeval now;
	struct tm* tm_now;

	if(gettimeofday(&now, NULL) == -1)
		return -1;

	tm_now = localtime(&now.tv_sec);
	sprintf(buf, "_%02d%02d%02d%02d%02d%02d",
			tm_now->tm_year+1900, tm_now->tm_mon+1, tm_now->tm_mday,
			tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
	
	return 0;
}

int CopyFile(const char* destPath, const char* sourcePath)
{
	int srcFd, destFd;
	size_t readLen;
	unsigned char* fileBuf[SSU_BACKUP_FILE_BUF_SZ];

	if((srcFd = open(sourcePath, O_RDONLY)) == -1)
		return -1;
	if((destFd = open(destPath, O_WRONLY|O_CREAT|O_TRUNC, SSU_BACKUP_MKFILE_AUTH)) == -1)
		return -1;

	while((readLen = read(srcFd, fileBuf, SSU_BACKUP_FILE_BUF_SZ)) > 0){
		if(write(destFd, fileBuf, readLen) == -1)
			return -1;
	}
	if(readLen == -1)
		return -1;

	close(srcFd);
	close(destFd);
	return 0;
}

int CreateFileByFileTree(const char* addPath, const struct filetree* addTree, int isRecover)
{
	char srcFilePath[SSU_BACKUP_MAX_PATH_SZ];
	char destFilePath[SSU_BACKUP_MAX_PATH_SZ];
	size_t srcPathLen = 0;

	strcpy(destFilePath, addPath);
	ConcatPath(destFilePath, addTree->file);
	if(addTree->childNodeNum == 0){
		if(isRecover){
			GetBackupPath(srcFilePath);
			//Todo: 이거 틀릴수도 있음. 아마 중복되서 나올거 같은 느낌
			srcPathLen = strlen(srcFilePath);
		}
		strcpy(srcFilePath + srcPathLen, addPath);
		ConcatPath(srcFilePath, addTree->file);
		if(!isRecover){
			BackupPathToSourcePath(srcFilePath);
		}

		if(isRecover){
			destFilePath[strlen(destFilePath) - SSU_BACKUP_FILE_META_LEN] = '\0';
			if(CopyFile(destFilePath, srcFilePath) == -1)
				return -1;

			fprintf(stdout, "\"%s\" backup recover to \"%s\"\n", srcFilePath, destFilePath);
			return 0;
		}

		if(GetNowTime(destFilePath + strlen(destFilePath)) == -1)
			return -1;
		if(CopyFile(destFilePath, srcFilePath) == -1)
			return -1;

		fprintf(stdout, "\"%s\" backuped\n", destFilePath);
		return 0;
	}

	//Comment: 폴더 만들고 재귀 호출
	if(MakeDirPath(destFilePath) == -1)
		return -1;
	for(int i=0; i < addTree->childNodeNum; i++){
		if(CreateFileByFileTree(destFilePath, addTree->childNodes[i], isRecover) == -1)
			return -1;
	}

	return 0;
}

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

	return CreateFileTree(fileName, hashBuf, hash_sz);
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
