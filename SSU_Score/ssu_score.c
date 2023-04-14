#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include "blank.h"
#include "ssu_score.h"

struct ssu_scoreTable score_table[QNUM];
char id_table[SNUM][10];

char stuDir[BUFLEN];
char ansDir[BUFLEN];
char errorDir[BUFLEN];
char threadFiles[ARGNUM][FILELEN];
char iIDs[ARGNUM][FILELEN];

int eOption = false;
int tOption = false;
int mOption = false;
int iOption = false;

void ssu_score(int argc, char *argv[])
{
	char saved_path[BUFLEN];

	//인자 중 -h옵션이 들어온 것이 하나라도 있다면 뒤의 명령들 무시하고 무조건 사용법 출력
	for(int i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-h")){
			print_usage();
			return;
		}
	}

	//Todo: i 옵션이란 것은 없다.
	//메모리의 값을 0으로 채움
	//	i옵션이 아닌 일반적인 상황의 경우 stuDir, ansDir에 학생 폴더, 정답 폴더를 복사한다.
	memset(saved_path, 0, BUFLEN);
	if(argc >= 3 && strcmp(argv[1], "-i") != 0){
		strcpy(stuDir, argv[1]);
		strcpy(ansDir, argv[2]);
	}

	//옵션 처리. 플래그를 키고 각 인자들을 복사한다.
	if(!check_option(argc, argv))
		exit(1);

	//아무 옵션도 없고, 디렉토리 설정 없이 i옵션만 켜져있다면 i옵션 실행 후 종료
	//	다른 옵션이 있어서 못했다면 밑에서 한번 더 실행하긴함.
	if(!mOption && !eOption && !tOption && iOption 
			&& !strcmp(stuDir, "") && !strcmp(ansDir, "")){
		//인자로 들어온 학생들의 틀린 문제를 출력
		do_iOption(iIDs);
		return;
	}

	//현재 작업 경로를 가져옴
	getcwd(saved_path, BUFLEN);

	//직접 경로 이동해서 학생 답안 경로가 있는지 체크
	if(chdir(stuDir) < 0){
		fprintf(stderr, "%s doesn't exist\n", stuDir);
		return;
	}
	//학생 답안 파일이 있는 폴더의 경로를 다시 stuDir로 복사
	getcwd(stuDir, BUFLEN);

	//다시 원래 위치로 이동
	chdir(saved_path);
	//직접 경로이동해서 답안 경로가 있는지 체크.
	if(chdir(ansDir) < 0){
		fprintf(stderr, "%s doesn't exist\n", ansDir);
		return;
	}
	//답안 파일이 있는 경로를 다시 ansDir로 복사
	getcwd(ansDir, BUFLEN);

	//다시 원래 위치로 이동
	chdir(saved_path);

	//스코어 테이블이 없다면 생성하고, 있다면 읽어들여서 스코어테이블을 값을 설정.
	set_scoreTable(ansDir);
	
	set_idTable(stuDir);

	if(mOption)
		do_mOption();

	printf("grading student's test papers..\n");
	score_students();

	if(iOption)
		do_iOption(iIDs);

	return;
}

int check_option(int argc, char *argv[])
{
	int i, j;
	int c;

	//옵션을 추출한다.
	while((c = getopt(argc, argv, "e:thmi")) != -1)
	{
		switch(c){
			//e옵션의 경우 errorDir에 폴더경로를 복사 후
			//	무조건 디렉토리를 새로 만든다.
			case 'e':
				eOption = true;
				strcpy(errorDir, optarg);

				if(access(errorDir, F_OK) < 0)
					mkdir(errorDir, 0755);
				else{
					//해당 폴더 있는경우 모든 경로 삭제
					rmdirs(errorDir);
					mkdir(errorDir, 0755);
				}
				break;
			//스레드 컴파일 옵션.
			//	인자가 5개 넘을 경우 초과했다고 표시한 후 
			case 't':
				tOption = true;
				//다음번에 처리될 옵션의 인덱스를 가져옴
				i = optind;
				j = 0;

				//옵션인 경우가 나오기 전까지 반복
				while(i < argc && argv[i][0] != '-'){

					if(j >= ARGNUM)
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else{
						//해당 목록을 treadFiles에 복사함
						strcpy(threadFiles[j], argv[i]);
					}
					i++; 
					j++;
				}
				break;
			case 'm':
				//특정 문제 번호 배점 수정 옵션을 킴
				mOption = true;
				break;

			case 'i':
				//Todo: i옵션이란 것은 없다.
				iOption = true;
				i = optind;
				j = 0;

				//t옵션 사용형태와 비슷함
				while(i < argc && argv[i][0] != '-'){
					if(j >= ARGNUM)
						printf("Maximum Number of Argument Exceeded. :: %s\n", argv[i]);
					else
						strcpy(iIDs[j], argv[i]);
					i++;
					j++;
				}
				break;

			//에러 핸들링
			case '?':
				printf("Unkown option %c\n", optopt);
				return false;
		}
	}

	return true;
}

void do_iOption(char (*ids)[FILELEN])
{
	FILE *fp;
	char tmp[BUFLEN];
	char qname[QNUM][FILELEN];
	char *p, *id;
	int i, j;
	char first, exist;

	//score.csv 파일을 오픈한다. 채점 결과 파일
	if((fp = fopen("./score.csv", "r")) == NULL){
		fprintf(stderr, "score.csv file doesn't exist\n");
		return;
	}

	// get qnames
	i = 0;
	//csv의 첫 줄을 가져온다.
	fscanf(fp, "%s\n", tmp);
	//,단위로 분리해서 문제번호를 추출한다.
	strcpy(qname[i++], strtok(tmp, ","));
	//,단위로 분리해서 문제번호를 추출한다.
	while((p = strtok(NULL, ",")) != NULL)
		strcpy(qname[i++], p);

	// print result
	i = 0;
	//Todo: 무조건 5번 반복됨?
	while(i++ <= ARGNUM - 1)
	{
		exist = 0;
		//파일 포인터를 초기 위치로 이동
		fseek(fp, 0, SEEK_SET);
		//첫 줄을 tmp로 복사
		fscanf(fp, "%s\n", tmp);

		//score파일의 학번과 점수를 한줄씩 가져옴.
		while(fscanf(fp, "%s\n", tmp) != EOF){
			//최초 토큰은 학번이 됨
			id = strtok(tmp, ",");

			//추출한 학번이 인자로 들어왔던 학번과 일치하다면
			if(!strcmp(ids[i - 1], id)){
				//일치 플래그 세팅
				exist = 1;
				j = 0;
				first = 0;
				//학번 다음은 문제 점수들임.
				//	문제 점수를 가져옴
				while((p = strtok(NULL, ",")) != NULL){
					//문자 스트링을 부동 소수점으로 변환
					if(atof(p) == 0){
						//최초일 때만 출력
						if(!first){
							printf("%s's wrong answer :\n", id);
							first = 1;
						}
						//해당 열이 sum이 아니라면 해당 점수 출력
						//	따라서 틀린문제의 리스트 들을 출력하게 됨.
						if(strcmp(qname[j], "sum"))
							printf("%s    ", qname[j]);
					}
					j++;
				}
				printf("\n");
			}
		}

		//해당 학번이 존재하지 않앗다면 에러 표시
		if(!exist)
			printf("%s doesn't exist!\n", ids[i - 1]);
	}

	fclose(fp);
}

void do_mOption()
{
	double newScore;
	char modiName[FILELEN];
	char filename[FILELEN];
	char *ptr;
	int i;

	ptr = malloc(sizeof(char) * FILELEN);

	while(1){

		printf("Input question's number to modify >> ");
		scanf("%s", modiName);

		if(strcmp(modiName, "no") == 0)
			break;

		for(i=0; i < sizeof(score_table) / sizeof(score_table[0]); i++){
			strcpy(ptr, score_table[i].qname);
			ptr = strtok(ptr, ".");
			if(!strcmp(ptr, modiName)){
				printf("Current score : %.2f\n", score_table[i].score);
				printf("New score : ");
				scanf("%lf", &newScore);
				getchar();
				score_table[i].score = newScore;
				break;
			}
		}
	}

	sprintf(filename, "./%s", "score_table.csv");
	write_scoreTable(filename);
	free(ptr);

}

int is_exist(char (*src)[FILELEN], char *target)
{
	int i = 0;

	while(1)
	{
		if(i >= ARGNUM)
			return false;
		else if(!strcmp(src[i], ""))
			return false;
		else if(!strcmp(src[i++], target))
			return true;
	}
	return false;
}

void set_scoreTable(char *ansDir)
{
	char filename[FILELEN];

	//현재 작업 경로에 score_table.csv를 생성함.
	//	이 파일은 점수를 내는 기준이 됨.
	sprintf(filename, "./%s", "score_table.csv");

	// check exist
	if(access(filename, F_OK) == 0)
		//파일이 있다면 이전에 설정한 채점 기준을 읽어들임
		read_scoreTable(filename);
	else{
		//없다면 답안 경로로 정답 테이블을 생성 및 쓰기
		//채점 기준을 설정
		make_scoreTable(ansDir);
		//스코어테이블 전역변수에 등록된 파일 이름과 점수를 가지고 filename 파일을 만듦
		write_scoreTable(filename);
	}
}

void read_scoreTable(char *path)
{
	FILE *fp;
	char qname[FILELEN];
	char score[BUFLEN];
	int idx = 0;

	//인자로 들어온 경로의 파일을 읽기 전용으로 오픈함
	if((fp = fopen(path, "r")) == NULL){
		fprintf(stderr, "file open error for %s\n", path);
		return ;
	}

	//개행 단위로 읽어들이되, ,를 기준으로 ,를 제외한 왼쪽 문자열를 qname에, 오른쪽에 위치한 문자열을 score에 넣는다.
	while(fscanf(fp, "%[^,],%s\n", qname, score) != EOF){
		//스코어테이블에 문제 파일 이름을 넣고
		strcpy(score_table[idx].qname, qname);
		//스코어에 문자열을 float형태로 변환해서 넣는다.
		score_table[idx++].score = atof(score);
	}

	fclose(fp);
}

void make_scoreTable(char *ansDir)
{
	int type, num;
	double score, bscore, pscore;
	struct dirent *dirp;
	DIR *dp;
	int idx = 0;
	int i;

	//안내창 출력 후 몇번을 선택했는지 가져옴
	num = get_create_type();

	//1번을 선택한 경우 자동으로 설정할 값들을 세팅.
	if(num == 1)
	{
		printf("Input value of blank question : ");
		scanf("%lf", &bscore);
		printf("Input value of program question : ");
		scanf("%lf", &pscore);
	}

	//인자로 들어온 정답 디렉토리가 있는지 체크
	if((dp = opendir(ansDir)) == NULL){
		fprintf(stderr, "open dir error for %s\n", ansDir);
		return;
	}

	//해당 디렉토리의 파일들을 순회한다.
	while((dirp = readdir(dp)) != NULL){

		//.과 .. 디렉토리는 제외
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		
		//c나 txt파일이 아닌 경우 건너뜀
		if((type = get_file_type(dirp->d_name)) < 0)
			continue;

		//c나 txt로 된 파일을 스코어테이블의 qname으로 복사
		//	c나 txt 확장자의 파일을 문제로 간주하겠다는 것.
		strcpy(score_table[idx].qname, dirp->d_name);

		idx++;
	}

	closedir(dp);
	//Todo: 학생 답안과 정답 폴더가 바뀌면 세그멘테이션 폴트 발생.
	//idx는 문제가 몇개인지 의미함.
	//스코어 테이블의 파일이름의 숫자를 기준으로 파일 이름들을 정렬함.
	sort_scoreTable(idx);

	//문제 개수만큼 반복
	for(i = 0; i < idx; i++)
	{
		//해당 파일의 파일타입을 가져옴
		type = get_file_type(score_table[i].qname);

		//1번 메뉴를 선택했다면
		if(num == 1)
		{
			//위에서 설정한 점수대로 자동으로 설정
			if(type == TEXTFILE)
				score = bscore;
			else if(type == CFILE)
				score = pscore;
		}
		else if(num == 2)
		{
			//2번 메뉴를 선택했다면 수동으로 점수 입력
			printf("Input of %s: ", score_table[i].qname);
			scanf("%lf", &score);
		}

		//해당 문제 채점 점수 저장
		score_table[i].score = score;
	}
}

void write_scoreTable(char *filename)
{
	int fd;
	char tmp[BUFLEN];
	int i;
	int num = sizeof(score_table) / sizeof(score_table[0]);

	//해당 파일을 생성
	if((fd = creat(filename, 0666)) < 0){
		fprintf(stderr, "creat error for %s\n", filename);
		return;
	}

	//스코어테이블의 배열 개수만큼 반복
	for(i = 0; i < num; i++)
	{
		//설정된 스코어가 0인 경우 설정되지 않은 파일로 간주, 점수테이블의 끝으로 생각해
		//반복 종료
		if(score_table[i].score == 0)
			break;

		//문제 이름,점수\n 형식으로 점수는 소수 둘째 까지 저장
		sprintf(tmp, "%s,%.2f\n", score_table[i].qname, score_table[i].score);
		//문자열 길이만큼 해당 파일로 출력
		write(fd, tmp, strlen(tmp));
	}

	close(fd);
}


void set_idTable(char *stuDir)
{
	struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;
	char tmp[BUFLEN];
	int num = 0;

	if((dp = opendir(stuDir)) == NULL){
		fprintf(stderr, "opendir error for %s\n", stuDir);
		exit(1);
	}

	while((dirp = readdir(dp)) != NULL){
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		sprintf(tmp, "%s/%s", stuDir, dirp->d_name);
		stat(tmp, &statbuf);

		if(S_ISDIR(statbuf.st_mode))
			strcpy(id_table[num++], dirp->d_name);
		else
			continue;
	}
	closedir(dp);

	sort_idTable(num);
}

void sort_idTable(int size)
{
	int i, j;
	char tmp[10];

	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 -i; j++){
			if(strcmp(id_table[j], id_table[j+1]) > 0){
				strcpy(tmp, id_table[j]);
				strcpy(id_table[j], id_table[j+1]);
				strcpy(id_table[j+1], tmp);
			}
		}
	}
}

void sort_scoreTable(int size)
{
	int i, j;
	struct ssu_scoreTable tmp;
	int num1_1, num1_2;
	int num2_1, num2_2;

	//정렬 횟수 버블 정렬
	for(i = 0; i < size - 1; i++){
		//비교 횟수
		for(j = 0; j < size - 1 - i; j++){

			//파일 이름에서 숫자를 빼옴.
			get_qname_number(score_table[j].qname, &num1_1, &num1_2);
			get_qname_number(score_table[j+1].qname, &num2_1, &num2_2);

			//번호순으로 정렬함.
			if((num1_1 > num2_1) || ((num1_1 == num2_1) && (num1_2 > num2_2))){

				memcpy(&tmp, &score_table[j], sizeof(score_table[0]));
				memcpy(&score_table[j], &score_table[j+1], sizeof(score_table[0]));
				memcpy(&score_table[j+1], &tmp, sizeof(score_table[0]));
			}
		}
	}
}

void get_qname_number(char *qname, int *num1, int *num2)
{
	char *p;
	char dup[FILELEN];

	//dup으로 들어온 문자열을 복사
	strncpy(dup, qname, strlen(qname));
	//num1에 -나 .이전에 오는 것을 숫자로 간주하고 num1 주소가 가리키는 위치에 저장
	*num1 = atoi(strtok(dup, "-."));
	
	//한번더 똑같은 것으로 파싱. 1-1같은 케이스 때문에.
	p = strtok(NULL, "-.");
	if(p == NULL)
		//-가 아니었다면 토큰화가 안되므로 숫자가 올 일이 없기에 0으로 저장
		*num2 = 0;
	else
		//하위 문제가 있는 것이었다면 다음에 숫자가 오므로 문자열을 정수로 변환후 num2가 가리키는 위치에 저장
		*num2 = atoi(p);
}

int get_create_type()
{
	int num;

	while(1)
	{
		printf("score_table.csv file doesn't exist in TREUDIR!\n");
		printf("1. input blank question and program question's score. ex) 0.5 1\n");
		printf("2. input all question's score. ex) Input value of 1-1: 0.1\n");
		printf("select type >> ");
		scanf("%d", &num);

		if(num != 1 && num != 2)
			printf("not correct number!\n");
		else
			break;
	}

	return num;
}

void score_students()
{
	double score = 0;
	int num;
	int fd;
	char tmp[BUFLEN];
	int size = sizeof(id_table) / sizeof(id_table[0]);

	if((fd = creat("score.csv", 0666)) < 0){
		fprintf(stderr, "creat error for score.csv");
		return;
	}
	write_first_row(fd);

	for(num = 0; num < size; num++)
	{
		if(!strcmp(id_table[num], ""))
			break;

		sprintf(tmp, "%s,", id_table[num]);
		write(fd, tmp, strlen(tmp)); 

		score += score_student(fd, id_table[num]);
	}

	printf("Total average : %.2f\n", score / num);

	close(fd);
}

double score_student(int fd, char *id)
{
	int type;
	double result;
	double score = 0;
	int i;
	char tmp[BUFLEN+EXTRABUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]);

	for(i = 0; i < size ; i++)
	{
		if(score_table[i].score == 0)
			break;

		sprintf(tmp, "%s/%s/%s", stuDir, id, score_table[i].qname);

		if(access(tmp, F_OK) < 0)
			result = false;
		else
		{
			if((type = get_file_type(score_table[i].qname)) < 0)
				continue;
			
			if(type == TEXTFILE)
				result = score_blank(id, score_table[i].qname);
			else if(type == CFILE)
				result = score_program(id, score_table[i].qname);
		}

		if(result == false)
			write(fd, "0,", 2);
		else{
			if(result == true){
				score += score_table[i].score;
				sprintf(tmp, "%.2f,", score_table[i].score);
			}
			else if(result < 0){
				score = score + score_table[i].score + result;
				sprintf(tmp, "%.2f,", score_table[i].score + result);
			}
			write(fd, tmp, strlen(tmp));
		}
	}

	printf("%s is finished. score : %.2f\n", id, score); 

	sprintf(tmp, "%.2f\n", score);
	write(fd, tmp, strlen(tmp));

	return score;
}

void write_first_row(int fd)
{
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]);

	write(fd, ",", 1);

	for(i = 0; i < size; i++){
		if(score_table[i].score == 0)
			break;
		
		sprintf(tmp, "%s,", score_table[i].qname);
		write(fd, tmp, strlen(tmp));
	}
	write(fd, "sum\n", 4);
}

char *get_answer(int fd, char *result)
{
	char c;
	int idx = 0;

	memset(result, 0, BUFLEN);
	while(read(fd, &c, 1) > 0)
	{
		if(c == ':')
			break;
		
		result[idx++] = c;
	}
	if(result[strlen(result) - 1] == '\n')
		result[strlen(result) - 1] = '\0';

	return result;
}

int score_blank(char *id, char *filename)
{
	char tokens[TOKEN_CNT][MINLEN];
	node *std_root = NULL, *ans_root = NULL;
	int idx, start;
	char tmp[BUFLEN+EXTRABUFLEN];
	char s_answer[BUFLEN], a_answer[BUFLEN];
	char qname[FILELEN];
	int fd_std, fd_ans;
	int result = true;
	int has_semicolon = false;

	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	sprintf(tmp, "%s/%s/%s", stuDir, id, filename);
	fd_std = open(tmp, O_RDONLY);
	strcpy(s_answer, get_answer(fd_std, s_answer));

	if(!strcmp(s_answer, "")){
		close(fd_std);
		return false;
	}

	if(!check_brackets(s_answer)){
		close(fd_std);
		return false;
	}

	strcpy(s_answer, ltrim(rtrim(s_answer)));

	if(s_answer[strlen(s_answer) - 1] == ';'){
		has_semicolon = true;
		s_answer[strlen(s_answer) - 1] = '\0';
	}

	if(!make_tokens(s_answer, tokens)){
		close(fd_std);
		return false;
	}

	idx = 0;
	std_root = make_tree(std_root, tokens, &idx, 0);

	sprintf(tmp, "%s/%s", ansDir, filename);
	fd_ans = open(tmp, O_RDONLY);

	while(1)
	{
		ans_root = NULL;
		result = true;

		for(idx = 0; idx < TOKEN_CNT; idx++)
			memset(tokens[idx], 0, sizeof(tokens[idx]));

		strcpy(a_answer, get_answer(fd_ans, a_answer));

		if(!strcmp(a_answer, ""))
			break;

		strcpy(a_answer, ltrim(rtrim(a_answer)));

		if(has_semicolon == false){
			if(a_answer[strlen(a_answer) -1] == ';')
				continue;
		}

		else if(has_semicolon == true)
		{
			if(a_answer[strlen(a_answer) - 1] != ';')
				continue;
			else
				a_answer[strlen(a_answer) - 1] = '\0';
		}

		if(!make_tokens(a_answer, tokens))
			continue;

		idx = 0;
		ans_root = make_tree(ans_root, tokens, &idx, 0);

		compare_tree(std_root, ans_root, &result);

		if(result == true){
			close(fd_std);
			close(fd_ans);

			if(std_root != NULL)
				free_node(std_root);
			if(ans_root != NULL)
				free_node(ans_root);
			return true;

		}
	}
	
	close(fd_std);
	close(fd_ans);

	if(std_root != NULL)
		free_node(std_root);
	if(ans_root != NULL)
		free_node(ans_root);

	return false;
}

double score_program(char *id, char *filename)
{
	double compile;
	int result;

	compile = compile_program(id, filename);

	if(compile == ERROR || compile == false)
		return false;
	
	result = execute_program(id, filename);

	if(!result)
		return false;

	if(compile < 0)
		return compile;

	return true;
}

int is_thread(char *qname)
{
	int i;
	int size = sizeof(threadFiles) / sizeof(threadFiles[0]);

	for(i = 0; i < size; i++){
		if(!strcmp(threadFiles[i], qname))
			return true;
	}
	return false;
}

double compile_program(char *id, char *filename)
{
	int fd;
	char tmp_f[BUFLEN+EXTRABUFLEN], tmp_e[BUFLEN+EXTRABUFLEN];
	char command[(BUFLEN+EXTRABUFLEN)*2+EXTRABUFLEN];
	char qname[FILELEN];
	int isthread;
	off_t size;
	double result;

	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));
	
	isthread = is_thread(qname);

	sprintf(tmp_f, "%s/%s", ansDir, filename);
	sprintf(tmp_e, "%s/%s.exe", ansDir, qname);

	if(tOption && isthread)
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	sprintf(tmp_e, "%s/%s_error.txt", ansDir, qname);
	fd = creat(tmp_e, 0666);

	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);
	unlink(tmp_e);

	if(size > 0)
		return false;

	sprintf(tmp_f, "%s/%s/%s", stuDir, id, filename);
	sprintf(tmp_e, "%s/%s/%s.stdexe", stuDir, id, qname);

	if(tOption && isthread)
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	sprintf(tmp_f, "%s/%s/%s_error.txt", stuDir, id, qname);
	fd = creat(tmp_f, 0666);

	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);

	if(size > 0){
		if(eOption)
		{
			sprintf(tmp_e, "%s/%s", errorDir, id);
			if(access(tmp_e, F_OK) < 0)
				mkdir(tmp_e, 0755);

			sprintf(tmp_e, "%s/%s/%s_error.txt", errorDir, id, qname);
			rename(tmp_f, tmp_e);

			result = check_error_warning(tmp_e);
		}
		else{ 
			result = check_error_warning(tmp_f);
			unlink(tmp_f);
		}

		return result;
	}

	unlink(tmp_f);
	return true;
}

double check_error_warning(char *filename)
{
	FILE *fp;
	char tmp[BUFLEN];
	double warning = 0;

	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "fopen error for %s\n", filename);
		return false;
	}

	while(fscanf(fp, "%s", tmp) > 0){
		if(!strcmp(tmp, "error:"))
			return ERROR;
		else if(!strcmp(tmp, "warning:"))
			warning += WARNING;
	}

	return warning;
}

int execute_program(char *id, char *filename)
{
	char std_fname[BUFLEN+EXTRABUFLEN], ans_fname[BUFLEN+EXTRABUFLEN];
	char tmp[BUFLEN+EXTRABUFLEN];
	char qname[FILELEN];
	time_t start, end;
	pid_t pid;
	int fd;

	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	sprintf(ans_fname, "%s/%s.stdout", ansDir, qname);
	fd = creat(ans_fname, 0666);

	sprintf(tmp, "%s/%s.exe", ansDir, qname);
	redirection(tmp, fd, STDOUT);
	close(fd);

	sprintf(std_fname, "%s/%s/%s.stdout", stuDir, id, qname);
	fd = creat(std_fname, 0666);

	sprintf(tmp, "%s/%s/%s.stdexe &", stuDir, id, qname);

	start = time(NULL);
	redirection(tmp, fd, STDOUT);
	
	sprintf(tmp, "%s.stdexe", qname);
	while((pid = inBackground(tmp)) > 0){
		end = time(NULL);

		if(difftime(end, start) > OVER){
			kill(pid, SIGKILL);
			close(fd);
			return false;
		}
	}

	close(fd);

	return compare_resultfile(std_fname, ans_fname);
}

pid_t inBackground(char *name)
{
	pid_t pid;
	char command[64];
	char tmp[64];
	int fd;
	off_t size;
	
	memset(tmp, 0, sizeof(tmp));
	fd = open("background.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);

	sprintf(command, "ps | grep %s", name);
	redirection(command, fd, STDOUT);

	lseek(fd, 0, SEEK_SET);
	read(fd, tmp, sizeof(tmp));

	if(!strcmp(tmp, "")){
		unlink("background.txt");
		close(fd);
		return 0;
	}

	pid = atoi(strtok(tmp, " "));
	close(fd);

	unlink("background.txt");
	return pid;
}

int compare_resultfile(char *file1, char *file2)
{
	int fd1, fd2;
	char c1, c2;
	int len1, len2;

	fd1 = open(file1, O_RDONLY);
	fd2 = open(file2, O_RDONLY);

	while(1)
	{
		while((len1 = read(fd1, &c1, 1)) > 0){
			if(c1 == ' ') 
				continue;
			else 
				break;
		}
		while((len2 = read(fd2, &c2, 1)) > 0){
			if(c2 == ' ') 
				continue;
			else 
				break;
		}
		
		if(len1 == 0 && len2 == 0)
			break;

		to_lower_case(&c1);
		to_lower_case(&c2);

		if(c1 != c2){
			close(fd1);
			close(fd2);
			return false;
		}
	}
	close(fd1);
	close(fd2);
	return true;
}

void redirection(char *command, int new, int old)
{
	int saved;

	saved = dup(old);
	dup2(new, old);

	system(command);

	dup2(saved, old);
	close(saved);
}

int get_file_type(char *filename)
{
	//가장 뒤에오는 . 문자를 찾는다.
	char *extension = strrchr(filename, '.');

	//값을 비교 해 분류된 값을 리턴
	if(!strcmp(extension, ".txt"))
		return TEXTFILE;
	else if (!strcmp(extension, ".c"))
		return CFILE;
	else
		return -1;
}

void rmdirs(const char *path)
{
	struct dirent *dirp;
	struct stat statbuf;
	DIR *dp;
	char tmp[BUFLEN];
	
	//해당 폴더가 없는 경우 건너뛰기
	if((dp = opendir(path)) == NULL)
		return;

	//해당 폴더 내용을 순차적으로 돌아본다
	while((dirp = readdir(dp)) != NULL)
	{
		//.과 ..디렉토리면 건너뛴다.
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		//경로를 붙인다.
		sprintf(tmp, "%s/%s", path, dirp->d_name);

		//해당 경로의 stat구조체를 가져온다
		if(lstat(tmp, &statbuf) == -1)
			continue;

		//폴더인 경우 재귀 호출, 아닌경우 해당 파일 지우기
		if(S_ISDIR(statbuf.st_mode))
			rmdirs(tmp);
		else
			unlink(tmp);
	}

	closedir(dp);
	//마지막으로 해당 폴더 지우기
	rmdir(path);
}

void to_lower_case(char *c)
{
	if(*c >= 'A' && *c <= 'Z')
		*c = *c + 32;
}

void print_usage()
{
	printf("Usage : ssu_score <STUDENTDIR> <TRUEDIR> [OPTION]\n");
	printf("Option : \n");
	printf(" -m                modify question's score\n");
	printf(" -e <DIRNAME>      print error on 'DIRNAME/ID/qname_error.txt' file \n");
	printf(" -t <QNAMES>       compile QNAME.C with -lpthread option\n");
	printf(" -t <IDS>          print ID's wrong questions\n");
	printf(" -h                print usage\n");
}
