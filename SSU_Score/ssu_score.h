#ifndef SSU_SCORE_H_
#define SSU_SCORE_H_

#include <sys/types.h>

#include "util/ssu_util_scoretree.h"

#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif
#ifndef STDOUT
	#define STDOUT 1
#endif
#ifndef STDERR
	#define STDERR 2
#endif
#ifndef TEXTFILE
	#define TEXTFILE 3
#endif
#ifndef CFILE
	#define CFILE 4
#endif
//프로그래밍 문제 제한시간
#ifndef OVER
	#define OVER 5
#endif
//warning 발생시 감점
#ifndef WARNING
	#define WARNING -0.1
#endif
#ifndef ERROR
	#define ERROR 0
#endif

#define FILELEN 128
#define BUFLEN 1024
#define EXTRABUFLEN 256
#define SNUM 100
#define QNUM 100
#define ARGNUM 5

#define DEFRESNAME "score.csv"

struct ssu_scoreTable{
	char qname[FILELEN];
	double score;
};

void ssu_score(int argc, char *argv[]);

//옵션 처리. 플래그를 키고 각 인자들을 복사한다.
//	잘못된 인자가 들어오면 false리턴
int check_option(int argc, char *argv[]);

//사용법 출력
void print_usage();

//score_student를 호출해 학생들의 채점을 한 뒤 채점 결과를 csv에 출력 하고 총점을 출력함
//	학생 평균 점수 리턴
//	실행 파일이 있는 경우 stdout, exe파일을 만들어 직접 실행시키고 실행 결과를 stdout파일에 출력후 두 파일을 비교해서 채점한다.
//	성공시 0 실패시 -1
int score_students(struct ScoreTree* rootTree);

//인자로 들어론 id를 채점 후 stdin으로 출력 및 fd 파일에 이어쓰기
//	이때 쓰기는 id, 뒤부터 쓰게됨. id,는 score_students에서 쓰기 때문
//	부분 점수도 score_blank, score_program이 -로 나온경우 그만큼 깎아서 처리
//	score_students에서 내부적으로 사용하는 함수
double score_student(struct ScoreTree* idTree, char *id);

//채점 결과 파일을 해당 경로에 쓰기합니다.
//	성공 시 0, 실패시 -1을 리턴.
int write_result_file(const char *resDir, struct ScoreTree* rootTree);

//score테이블을 이용해 csv의 컬럼을 구성함.
//	,문제번호,...,sum 꼴임
void write_first_row(int fd);

//fd를 읽어들여 :이 나오기 전까지 또는 문자열 끝까지 문자를 result에 담아줌
//	score_blank에서 활용함
char *get_answer(int fd, char *result);

//정답 파일과 학생 파일을 토큰화 후 트리로 바꿔서 비교후 일치하면 true 일치하지 않으면 false리턴/
//	여러 답이 나올수 있는 경우까지 고려된다.
//	공백이거나 못 읽어 들인경우 실패
//	괄호개수가 맞지 않은 경우 실패
//	문법에 문제가 있는 경우 실패
int score_blank(char *id, char *filename);

//실패한경우 true 리턴, warning이 있다면 -된 점수를 리턴하고, 성공한 경우 true를 리턴한다.
double score_program(char *id, char *filename);

//해당 학생의 아이디로 해당 문제를 컴파일함
//	에러 발생시 ERROR(0)점 Warning발생시 발생한 만큼 점수를 차감하며 정답시 true(1)리턴
//	에러파일을 만들며, 만약 -e옵션이 있다면 error파일을 이동시키고 없다면 삭제
//	내부적으로 check_error_warning을 호출함
double compile_program(char *id, char *filename);

//정답 실행파일과 학생 답안 실행파일을 실행시킨 뒤 그 결과를 .stdout파일에 저장한다.
//	만들어진 .stdout파일을 비교해 두파일이 같은 경우 true, 다른경우 false를 리턴한다.
//	시간이 OVER보다 초과될 경우 실행을 강제 종료하고 false를 리턴한다.
int execute_program(char *id, char *filname);

//ps|grep을 이용해 인자로 들어온 name의 pid를 리턴함.
//	name으로 들어온 프로세스가 없는 경우 0 리턴
pid_t inBackground(char *name);

//만들어진 에러파일을 뒤져가며 error: 가 있다면 ERROR를 바로 리턴
//	Warning만 있다면 Waning이 발생한 만큼 점수를 까서 깐 점수의 총점을 리턴
double check_error_warning(char *filename);

//인자로 들어온 두 파일이 공백과 대소문자를 고려하지 않았을 때 같은지 판별.
//	성공시 true, 실패시 false 리턴
int compare_resultfile(char *file1, char *file2);

//배점을 수정할 문제를 입력 받고 배점을 수정한 뒤 score_table.csv파일을 새로 쓰기함.
//	실패시 -1 성공시 0
int do_mOption();

int is_exist(char (*src)[FILELEN], char *target);

//threadFiles를 순회하며 해당 이름이 threadFile에 있다면 true 아니라면 false
int is_thread(char *qname);

//oldfd를 newfd로 디스크립터를 임시로 바꿔서 command명령어를 system으로 실행한다.
void redirection(char *command, int newfd, int oldfd);

//파일 확장자를 .csv와 같은 스트링으로 리턴한다.
//	없다면 NULL 리턴
char* get_file_extension(const char *filename);

//파일의 .을 기준으로 .오른쪽에 오는 이름을 확장자로 판단하여 파일 타입을 분류한다.
//	txt = TESTFILE, c = CFILE 이외에 -1 리턴
int get_file_type(char *filename);

//디렉토리 내의 모든 파일을 삭제한다.
void rmdirs(const char *path);

//대문자인 경우 소문자로 변환
void to_lower_case(char *c);

//스코어 테이블이 없다면 생성하고, 있다면 읽어들여서 스코어테이블을 값을 설정.
void set_scoreTable(char *ansDir);

//해당 경로에 이전에 설정한 scoreTable을 읽어들인다.
void read_scoreTable(char *path);

//채점기준을 설정한다.
void make_scoreTable(char *ansDir);

//스코어 테이블을 해당 파일이름으로 쓰기함.
void write_scoreTable(char *filename);

//학생 디렉토리에 있는 파일을 순회하여 디렉토리 파일 이름을 읽어옴.
//	학생 디렉토리의 파일 이름은 학번으로 간주함.
void set_idTable(char *stuDir);

//점수 테이블을 생성할 때 안내창을 출력 후 몇 번을 선택했는지 가져옴
int get_create_type(const char *ansDir);

//Todo: 부등호만 잘 바꾸면 역 정렬 가능할듯
//idTable의 파일이름을 오름차순으로 정렬함
void sort_idTable(int size);

//Todo: 부등호만 잘 바꾸면 역 정렬 가능할듯
//스코어 테이블의 파일이름의 숫자를 기준으로 파일 이름들을 오름차순으로 정렬함.
void sort_scoreTable(int size);

//-과. 을 기준으로 구분해 1-1.txt 1.c와 같은 문자열에서 숫자만 빼냄.
void get_qname_number(char *qname, int *num1, int *num2);

//파일이 있다면 0리턴, 없다면 -1리턴
int find_file(const char* dir, const char* file);

#endif
