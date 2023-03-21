#ifndef SSU_BACKUP_PATH_H
#define SSU_BACKUP_PATH_H

//Comment: dest에 target문자열을 붙입니다. 사이에 /가 추가됩니다.
//	dest의 시작 주소값을 리턴합니다.
char* ConcatPath(char* dest, const char* target);

//Comment: 부모 경로를 리턴합니다.
char* GetParentPath(const char* path, char* buf);

//Comment: 경로의 마지막 파일 이름의 위치를 반환합니다.
//	실패시 NULL을 리턴합니다.
//	path의 문자열의 변경이 있습니다.
char* GetFileNameByPath(char* path);

//Comment: 인자로 백업 경로를 저장할 버퍼를 받습니다.
// 버퍼의 시작 주소값을 리턴합니다.
char* GetBackupPath(char* buf);

//Comment: Home경로를 지웁니다.
//	지우고 난 뒤의 경로를 리턴합니다.
char* ExtractHomePath(char* path);

//Comment: path가 원본의 경로라면 path를 백업이 있을 경로로 변환해줍니다.
//	경로사이의 $HOME이 있다면 이를 $HOME/backup로 치환하는 역할을 합니다.
//	바뀐 path를 리턴합니다.
char* SourcePathToBackupPath(char* path);

//Comment: path가 백업 경로라면 path를 원본이 있을 경로로 변환해줍니다.
//	경로사이의 /backup/이 있다면 이를 /로 치환하는 역할을 합니다.
//	바뀐 path를 리턴합니다.
char* BackupPathToSourcePath(char* path);

//Comment: path를 backupPath주소로 변환해줍니다.
//	상대경로, 절대경로 모두 변환 허용합니다.
//	변환된 주소를 리턴합니다.
//	백업 경로가 포함되어있거나, 실패했을경우NULL을 리턴합니다.
//	resolved_path는 최대 패스 길이 보다 1 커야합니다.
//	실패하거나 잘못된 입력일 경우 NULL을 리턴합니다.
char* GetVirtualRealPath(const char* path, char* resolved_path);

#endif
