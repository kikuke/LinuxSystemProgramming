#ifndef SSU_BACKUP_UTIL_H
#define SSU_BACKUP_UTIL_H

//Comment: 인자로 백업 경로를 저장할 버퍼를 받습니다.
// 버퍼의 시작 주소값을 리턴합니다.
char* GetBackupPath(char* buf);
//Comment: 인자로 백업 경로를 저장할 버퍼를 받습니다.
//	버퍼의 시작 주소값을 리턴합니다.
//	만들어진 디렉토리는 사용자만 접근 가능합니다.
char* SetBackupPath(char* buf);

//Todo: 파일리스트 가져오는 함수 만들기

#endif
