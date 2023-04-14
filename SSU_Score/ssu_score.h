#ifndef SSU_SCORE_H_
#define SSU_SCORE_H_

#include <sys/types.h>

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
#ifndef OVER
	#define OVER 5
#endif
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

void score_students();
double score_student(int fd, char *id);
void write_first_row(int fd);

char *get_answer(int fd, char *result);
int score_blank(char *id, char *filename);
double score_program(char *id, char *filename);
double compile_program(char *id, char *filename);
int execute_program(char *id, char *filname);
pid_t inBackground(char *name);
double check_error_warning(char *filename);
int compare_resultfile(char *file1, char *file2);

//인자로 받은 id의 틀린 문제를 출력한다.
void do_iOption(char (*ids)[FILELEN]);

void do_mOption();
int is_exist(char (*src)[FILELEN], char *target);

int is_thread(char *qname);
void redirection(char *command, int newfd, int oldfd);

//파일의 .을 기준으로 .오른쪽에 오는 이름을 확장자로 판단하여 파일 타입을 분류한다.
//	txt = TESTFILE, c = CFILE 이외에 -1 리턴
int get_file_type(char *filename);

//디렉토리 내의 모든 파일을 삭제한다.
void rmdirs(const char *path);

void to_lower_case(char *c);

//스코어 테이블이 없다면 생성하고, 있다면 읽어들여서 스코어테이블을 값을 설정.
void set_scoreTable(char *ansDir);

//해당 경로에 이전에 설정한 scoreTable을 읽어들인다.
void read_scoreTable(char *path);

//채점기준을 설정한다.
void make_scoreTable(char *ansDir);

void write_scoreTable(char *filename);
void set_idTable(char *stuDir);

//점수 테이블을 생성할 때 안내창을 출력 후 몇 번을 선택했는지 가져옴
int get_create_type();

void sort_idTable(int size);

//스코어 테이블의 파일이름의 숫자를 기준으로 파일 이름들을 정렬함.
void sort_scoreTable(int size);

//-과. 을 기준으로 구분해 1-1.txt 1.c와 같은 문자열에서 숫자만 빼냄.
void get_qname_number(char *qname, int *num1, int *num2);

#endif
