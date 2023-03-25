#ifndef SSU_BACKUP_HASH_H
#define SSU_BACKUP_HASH_H

//Comment: 해쉬모드를 기준으로 두 해시값을 비교합니다.
//	일치하지 않으면 0 일치하면 1을 리턴합니다.
//  실패시 -1을 리턴합니다.
int CompareHashByPath(const char* path1, const char* path2, int hashMode);

//Comment: 해쉬모드를 기준으로 두 해시값을 비교합니다.
//	일치하지 않으면 0 일치하면 1을 리턴합니다.
int CompareHash(const char* hash1, const char* hash2, int hashMode);

//Comment: 성공시 0, 실패시 -1 리턴
//  hashBuf에 값을 채워서 줍니다.
int GetHashByPath(const char* path, char* hashBuf, int hashMode);

//Comment: 성공시 0, 실패시 -1리턴
//	hashBuf에 해시값을 채워서 줍니다.
int GetMd5HashByPath(const char* path, char* hashBuf);

//Comment: 성공시 0, 실패시 -1리턴
//	hashBuf에 해시값을 채워서 줍니다.
int GetSha1HashByPath(const char* path, char* hashBuf);

#endif