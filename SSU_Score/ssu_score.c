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
#include "util/ssu_util_path.h"

struct ssu_scoreTable score_table[QNUM];
struct ScoreTree scoreTree;
char id_table[SNUM][10];

char stuDir[BUFLEN];
char ansDir[BUFLEN];
char resDir[FILELEN];
char errorDir[BUFLEN];
char threadFiles[ARGNUM][FILELEN];
char cIDs[ARGNUM][FILELEN];

int nOption = false;
int eOption = false;
int tOption = false;
int mOption = false;
int pOption = false;
int cOption = false;
int sOption = false;
int isDesc;
int isSortBySum;

void ssu_score(int argc, char *argv[])
{
	struct ScoreTree* tempTree;
	char saved_path[BUFLEN];

	//인자 중 -h옵션이 들어온 것이 하나라도 있다면 뒤의 명령들 무시하고 무조건 사용법 출력
	for(int i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-h")){
			print_usage();
			exit(1);
		}
	}

	//메모리의 값을 0으로 채움
	//	stuDir, ansDir에 학생 폴더, 정답 폴더를 복사한다.
	memset(saved_path, 0, BUFLEN);
	strcpy(stuDir, argv[1]);
	strcpy(ansDir, argv[2]);

	//옵션 처리. 플래그를 키고 각 인자들을 복사한다.
	if(!check_option(argc, argv))
		exit(1);

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

	if(access(DEFSTDDIR, F_OK) < 0)
		mkdir(DEFSTDDIR, 0755);

	if(access(DEFANSDIR, F_OK) < 0)
		mkdir(DEFANSDIR, 0755);

	//scoreTable이 없다면 생성하고, 있다면 읽어들여서 스코어테이블을 값을 설정.
	set_scoreTable(ansDir);
	
	//stuDir경로의 디렉토리들을 읽어들여 파일이름을 가져와 idTable에 학번 리스트를 만든 후 정렬한다.
	set_idTable(stuDir);

	//배점 수정 옵션이 있는 경우 배점을 수정할 문제를 입력 받고 배점을 수정한 뒤 score_table.csv파일을 새로 쓰기함.
	if(mOption){
		if(do_mOption() == -1){
			fprintf(stderr, "do_mOption error()\n");
			exit(1);
		}
	}

	printf("grading student's test papers..\n");

	if(!nOption){
		strcpy(resDir, DEFANSDIR);
		ConcatPath(resDir, DEFRESNAME);
	}
	//score_student를 호출해 학생들의 채점을 한 뒤 채점 결과를 csv에 출력 하고 총점을 출력함
	//	실행 파일이 있는 경우 stdout, exe파일을 만들어 직접 실행시키고 실행 결과를 stdout파일에 출력후 두 파일을 비교해서 채점한다.

	if(score_students(&scoreTree) == -1){
		fprintf(stderr, "score_students error\n");
		exit(1);
	}

	if(sOption){
		//sum으로 정렬인 경우 sum의 위치 계산
		if(isSortBySum != false){
			int i=0;
			for(tempTree=scoreTree.next[SCORE_DOWN]; tempTree!=NULL; tempTree=tempTree->next[SCORE_AFT])
				i++;

			isSortBySum = i-1;
		}

		//sOption인 경우 정렬
		if(SortTreeByField(&scoreTree, isSortBySum, isDesc) == -1){
			fprintf(stderr, "SortTreeByField error\n");
			exit(1);
		}
	}

	if(write_result_file(resDir, &scoreTree) == -1){
		fprintf(stderr, "write_result_file error for %s\n", resDir);
		exit(1);
	}

	if(eOption){
		printf("error saved.. (%s/)\n", errorDir);
	}

	return;
}

int check_option(int argc, char *argv[])
{
	int i, j;
	int c;
	char *extension;
	char buf[BUFLEN];

	//옵션을 추출한다.
	while((c = getopt(argc, argv, "n:e:sthmpc")) != -1)
	{
		switch(c){
			//s옵션이 들어왔을 경우 정렬 설정
			case 's':
				sOption = true;
				i = optind;

				if(i < argc && !strcmp(argv[i], "stdid")){
					isSortBySum = false;
				} else if(i < argc && !strcmp(argv[i], "score")){
					isSortBySum = true;
				} else {
					fprintf(stderr, "-s use < stdid | score > < 1 | -1 >\n");
					return false;
				}
				i++;
				optind+=2;

				if(i < argc && !strcmp(argv[i], "1")){
					isDesc = false;
				} else if(i < argc && !strcmp(argv[i], "-1")){
					isDesc = true;
				} else {
					fprintf(stderr, "-s use < stdid | score > < 1 | -1 >\n");
					return false;
				}
				break;
			//n옵션이 들어왔을 경우 변경할 파일 명 복사
			case 'c':
				cOption = true;
				i = optind;
				j = 0;

				//이미 p옵션 등을 통해 인자가 들어왔을 경우 에러처리
				if(**cIDs != 0 && (i < argc && argv[i][0] != '-')){
					fprintf(stderr, "-c -p Option's Argument Ocurred Twice\n");
					return false;
				}
				//t옵션 사용형태와 비슷함
				while(i < argc && argv[i][0] != '-'){
					if(j >= ARGNUM){
						if(j == ARGNUM)
							printf("Maximum Number of Argument Exceeded. ::");
						printf(" %s", argv[i]);
						continue;
					}
					else
						strcpy(cIDs[j], argv[i]);

					if(find_file(stuDir, cIDs[j]) == -1){
						fprintf(stderr, "%s student are not exist\n", cIDs[j]);
						return false;
					}

					i++;
					j++;
				}
				if(j >= ARGNUM)
					printf("\n");
				break;
			case 'n':
				nOption = true;
				strcpy(resDir, optarg);

				//csv확장자인지 검사
				if((extension = get_file_extension(resDir)) == NULL || strcmp(extension, ".csv")){
					fprintf(stderr, "result file extension is not csv for %s\n", resDir);
					return false;
				}

				break;
			//e옵션의 경우 errorDir에 폴더경로를 복사 후
			//	무조건 디렉토리를 새로 만든다.
			case 'e':
				eOption = true;
				strcpy(buf, optarg);
				if(GetVirtualRealPath(buf, errorDir) == NULL){
					fprintf(stderr, "Get Virtual Real Path Failed: %s\n", buf);
					return false;
				}

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

					if(j >= ARGNUM){
						if(j == ARGNUM)
							printf("Maximum Number of Argument Exceeded. ::");
						printf(" %s", argv[i]);
						continue;
					}
					else{
						//해당 목록을 treadFiles에 복사함
						strcpy(threadFiles[j], argv[i]);
					}
					if((find_file(ansDir, strcat(strcpy(buf, threadFiles[j]), ".c")) == -1) && (find_file(ansDir, strcat(strcpy(buf, threadFiles[j]), ".txt")) == -1)){
						fprintf(stderr, "%s Problem's not exist\n", threadFiles[j]);
						return false;
					}

					i++; 
					j++;
				}
				if(j >= ARGNUM)
					printf("\n");
				break;
			case 'm':
				//특정 문제 번호 배점 수정 옵션을 킴
				mOption = true;
				break;

			case 'p':
				pOption = true;
				i = optind;
				j = 0;

				//이미 c옵션 등을 통해 인자가 들어왔을 경우 에러처리
				if(**cIDs != 0 && (i < argc && argv[i][0] != '-')){
					fprintf(stderr, "-c -p Option's Argument Ocurred Twice\n");
					return false;
				}
				//t옵션 사용형태와 비슷함
				while(i < argc && argv[i][0] != '-'){
					if(j >= ARGNUM){
						if(j == ARGNUM)
							printf("Maximum Number of Argument Exceeded. ::");
						printf(" %s", argv[i]);
					}
					else
						strcpy(cIDs[j], argv[i]);

					if(find_file(stuDir, cIDs[j]) == -1){
						fprintf(stderr, "%s student are not exist\n", cIDs[j]);
						return false;
					}
					i++;
					j++;
				}
				if(j >= ARGNUM)
					printf("\n");
				break;

			//에러 핸들링
			case '?':
				printf("Unkown option %c\n", optopt);
				return false;
		}
	}

	return true;
}

int do_mOption()
{
	double newScore;
	char modiName[FILELEN];
	char filename[BUFLEN + FILELEN];
	char *ptr;
	int i;

	//FILELEN만큼 동적할당
	ptr = malloc(sizeof(char) * FILELEN);

	while(1){

		printf("Input question's number to modify >> ");
		//배점을 변경할 문제 이름을 가져옴
		scanf("%s", modiName);

		//no를 입력했을 경우 종료
		if(strcmp(modiName, "no") == 0)
			break;

		//scoreTable 배열 크기만큼 순회
		for(i=0; i < sizeof(score_table) / sizeof(score_table[0]); i++){
			if(score_table[i].score == 0){
				fprintf(stderr, "%s question doesn't exist\n", modiName);
				break;
			}

			//파일 이름을 ptr로 복사
			strcpy(ptr, score_table[i].qname);
			//딱 한번만 사용했기에 .이 없지 않은 이상 ptr은 변경되지 않는다.
			//.이 없는 이름이었을 경우 메모리 누수
			ptr = strtok(ptr, ".");
			//배점 변경하려는 문제이름과 일치하다면
			if(!strcmp(ptr, modiName)){
				printf("Current score : %lg\n", score_table[i].score);
				printf("New score : ");
				//라인단위. 스코어 입력을 가져옴
				scanf("%lf", &newScore);
				//개행을 읽어들임
				getchar();
				//대입
				score_table[i].score = newScore;
				break;
			}
		}
	}

	//쓰기위한 파일이름을 생성함
	sprintf(filename, "%s/%s", DEFANSDIR, "score_table.csv");
	//해당 파일이름으로 스코어테이블을 쓰기함.
	write_scoreTable(filename);
	//동적할당 메모리 해제
	free(ptr);
	return 0;
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
	char filename[BUFLEN];

	//정답경로에 score_table.csv를 생성함.
	//	이 파일은 점수를 내는 기준이 됨.
	sprintf(filename, "%s/%s", DEFANSDIR, "score_table.csv");

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
	num = get_create_type(DEFANSDIR);

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

	//학생 답안 디렉토리를 오픈
	if((dp = opendir(stuDir)) == NULL){
		fprintf(stderr, "opendir error for %s\n", stuDir);
		exit(1);
	}

	//학생 답안 디렉토리에 있는 디렉토리 이름들을 읽어들임
	while((dirp = readdir(dp)) != NULL){
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		//해당 파일의 경로명을 이어 붙임
		sprintf(tmp, "%s/%s", stuDir, dirp->d_name);
		//stat구조체를 가져옴
		stat(tmp, &statbuf);

		//디렉토리인 경우에만 id_table에 이름을 저장함
		if(S_ISDIR(statbuf.st_mode))
			strcpy(id_table[num++], dirp->d_name);
		else
			continue;
	}
	closedir(dp);

	//오름차순으로 정렬함
	sort_idTable(num);
}

void sort_idTable(int size)
{
	int i, j;
	char tmp[10];

	//버블 정렬
	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 -i; j++){
			//id테이블을 오름차순으로 정렬함
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

int get_create_type(const char *ansDir)
{
	char tmp[BUFLEN];
	int num;
	
	if(GetVirtualRealPath(ansDir, tmp) == NULL){
		fprintf(stderr, "GetVirtualRealPath() Failed %s\n", ansDir);
		return -1;
	}
	while(1)
	{
		printf("score_table.csv file doesn't exist in \"%s\"!\n", tmp);
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

int score_students(struct ScoreTree* rootTree)
{
	double score = 0;
	int num;
	int size = sizeof(id_table) / sizeof(id_table[0]);
	struct ScoreTree* befTree;
	struct ScoreTree* idTree;

	befTree = rootTree;

	//id테이블 크기만큼 반복
	for(num = 0; num < size; num++)
	{
		//데이터가 없는 구간이 나왔다면 반복문 종료.
		if(!strcmp(id_table[num], ""))
			break;   

		//Todo: strtod써서 바꿔서 트리에 저장하기. 이후 로직들 쭉 작성
		idTree = CreateScoreTree(NULL, NULL, NULL, NULL, atof(id_table[num]));
		SetUpDownScoreTree(befTree, idTree);

		//해당 id의 학생을 채점하고 레코드를 작성해 파일에 씀
		//해당 학생의 점수를 계산한 뒤 총점에 더함
		score += score_student(idTree, id_table[num]);

		befTree = idTree;
	}

	//총점 출력
	if(cOption)
		printf("Total average : %.2f\n", score / num);
		
	return 0;
}

double score_student(struct ScoreTree* idTree, char *id)
{
	int type;
	double result;
	double score = 0;
	int i;
	char tmp[BUFLEN+EXTRABUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]);
	struct ScoreTree* befTree;
	struct ScoreTree* nowTree;
	int isCIDs = false;
	int isFirst = true;
	int num1, num2;

	befTree = idTree;
	//스코어테이블의 배열 크기만큼 순회
	for(i = 0; i < size ; i++)
	{
		//데이터가 없는 부분일 경우 데이터의 끝으로 판단하고 순회 종료
		if(score_table[i].score == 0)
			break;

		nowTree = CreateScoreTree(NULL, NULL, NULL, NULL, 0);
		SetBefAftScoreTree(befTree, nowTree);

		//학생 정답 디렉토리의 id폴더의 해당 문제 파일을 경로로 만듦
		sprintf(tmp, "%s/%s/%s", stuDir, id, score_table[i].qname);

		//해당 경로에 해당 파일이 없다면 오답으로 간주 0점 처리
		if(access(tmp, F_OK) < 0)
			result = false;
		//해당 파일이 있다면
		else
		{
			//타입이 txt나 csv가 아닌 경우 건너뜀
			if((type = get_file_type(score_table[i].qname)) < 0)
				continue;
			
			//txt파일인 경우 아래 함수를 통해 채점
			if(type == TEXTFILE)
				result = score_blank(id, score_table[i].qname);
			//.c파일인 경우 아래 함수를 통해 채점
			else if(type == CFILE)
				result = score_program(id, score_table[i].qname);
		}

		//0점인 경우 0점 기록
		if(result != false){
			//맞은 경우 해당문제의 설정한 배점으로 기록
			if(result == true){
				//총점에 더함
				nowTree->record = score_table[i].score;
				score += nowTree->record;
			}
			//부분 정답인 경우
			else if(result < 0){
				//해당 점수만큼 감점
				//총점에 더함
				nowTree->record = score_table[i].score + result;
				score = score + nowTree->record;
			}
		}

		befTree = nowTree;
	}

	//해당 학생의 채점 결과 출력
	nowTree = CreateScoreTree(NULL, NULL, NULL, NULL, score);
	SetBefAftScoreTree(befTree, nowTree);

	printf("%s is finished..", id);
	
	if(cOption || pOption){
		for(i=0; i<ARGNUM; i++){
			if(**cIDs == 0 || !strcmp(cIDs[i], id)){
				isCIDs = true;
				break;
			}
		}
	}

	if(cOption && isCIDs){
		printf(" score : %.2f", score);
	}
	if(cOption && pOption && isCIDs){
		printf(",");
	}
	//틀린 문제 출력
	if(pOption && isCIDs){
		int i=0;
		nowTree = FindFirstTree(nowTree);
		printf(" wrong problem : ");
		for(nowTree=nowTree->next[SCORE_AFT]; nowTree!=NULL; nowTree=nowTree->next[SCORE_AFT]) {
			//틀린 문제일 경우
			if(nowTree->record == 0 && nowTree->next[SCORE_AFT] != NULL){
				if(!isFirst){
					printf(", ");
				}
				get_qname_number(score_table[i].qname, &num1, &num2);
				printf("%d", num1);
				if(num2 != 0){
					printf("-%d", num2);
				}
				printf("(%.2lg)", score_table[i].score);

				isFirst = false;
			}

			i++;
		}
	}
	printf("\n");

	//총 점 리턴
	return score;
}

int write_result_file(const char *resDir, struct ScoreTree* rootTree)
{
	int fd;
	char savePath[BUFLEN];
	char tmp[BUFLEN];
	struct ScoreTree* downTree = rootTree;
	struct ScoreTree* aftTree;
	
	if(GetVirtualRealPath(resDir, savePath) == NULL){
		fprintf(stderr, "GetVirtualRealPath Failed for %s\n", resDir);
		return -1;
	}

	if((fd = creat(savePath, 0666)) < 0){
		fprintf(stderr, "creat error for %s\n", savePath);
		return -1;
	}

	//score테이블을 이용해 csv의 컬럼을 구성함.
	write_first_row(fd);
	for(downTree=downTree->next[SCORE_DOWN]; downTree!=NULL; downTree=downTree->next[SCORE_DOWN]){
		aftTree = downTree;
		//'id,' 꼴로 csv의 id 필드를 채움
		sprintf(tmp, "%.f", aftTree->record);
		write(fd, tmp, strlen(tmp)); 
		for(aftTree=aftTree->next[SCORE_AFT]; aftTree!=NULL; aftTree=aftTree->next[SCORE_AFT]){
			//',점수' 꼴로 csv의 점수 필드를 채움
			if(aftTree->record == 0){
				sprintf(tmp, ",%.f", aftTree->record);
			} else {
				sprintf(tmp, ",%.2f", aftTree->record);
			}
			write(fd, tmp, strlen(tmp)); 
		}
		sprintf(tmp, "\n");
		write(fd, tmp, strlen(tmp)); 
	}

	printf("result saved.. (%s)\n", savePath);
	close(fd);

	return 0;
}

void write_first_row(int fd)
{
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]);

	//컬럼을 의미함. 이 컴마의 앞은 id를 의미함 그 뒤는 문제 번호들.
	//첫 문자로 ,를 찍음
	write(fd, ",", 1);

	//테이블 길이만큼 반복
	for(i = 0; i < size; i++){
		//테이블에 데이터가 없는 구간이 됐다면 나감
		if(score_table[i].score == 0)
			break;
		
		//'문제파일이름,' 꼴로 쓰기를 함.
		sprintf(tmp, "%s,", score_table[i].qname);
		//해당 파일에 이어쓰기
		write(fd, tmp, strlen(tmp));
	}
	//마지막에 sum 컬럼을 만들고 개행. 이로써 컬럼 구성이 끝남.
	write(fd, "sum\n", 4);
}

char *get_answer(int fd, char *result)
{
	char c;
	int idx = 0;

	//받은 메모리를 0으로 초기화. 메모리의 크기가 BUFLEN이라고 가정. 아니면 위험함
	memset(result, 0, BUFLEN);
	//blank 문제 파일을 한글자 씩 읽어들임
	while(read(fd, &c, 1) > 0)
	{
		//:문자면 입력이 끝난 것으로 간주.
		if(c == ':')
			break;
		
		//읽은 문자열을 result에 채워나감
		result[idx++] = c;
	}
	//위에서 0으로 초기화했기에 다른 경우는 자동 문자열 끝 처리가 되지만 개행인 경우
	//\0으로 바꿔줌
	if(result[strlen(result) - 1] == '\n')
		result[strlen(result) - 1] = '\0';

	return result;
}

int score_blank(char *id, char *filename)
{
	char tokens[TOKEN_CNT][MINLEN];
	node *std_root = NULL, *ans_root = NULL;
	int idx;
	char tmp[BUFLEN+EXTRABUFLEN];
	char s_answer[BUFLEN], a_answer[BUFLEN];
	char qname[FILELEN];
	int fd_std, fd_ans;
	int result = true;
	int has_semicolon = false;

	//qname의 메모리를 0으로 초기화
	memset(qname, 0, sizeof(qname));
	//.을 제외한 길이만큼(문제이름) 파일 이름 복사. 확장자가 제외됨.
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	//문제파일이 있는 경로를 tmp에 입력
	sprintf(tmp, "%s/%s/%s", stuDir, id, filename);
	//읽기 전용으로 오픈
	fd_std = open(tmp, O_RDONLY);

	//학생이 쓴 답안을 s_answer에 옮겨줌 :이 있다면 뒤에는 무시함
	//Warning: 이미 안에서 썼는데 또 쓰는중. 자기 자신을 쓰는중임.
	strcpy(s_answer, get_answer(fd_std, s_answer));

	//못읽어들였다면 실패로 간주
	if(!strcmp(s_answer, "")){
		close(fd_std);
		return false;
	}

	//양쪽의 괄호개수가 맞는지 체크 아닌경우 실패
	if(!check_brackets(s_answer)){
		close(fd_std);
		return false;
	}

	//양 끝의 공백을 지워서 다시 씀
	strcpy(s_answer, ltrim(rtrim(s_answer)));

	//문자열 끝이 세미콜론이라면 세미콜론을 \0으로 바꿔줌
	//	또한 has_semicolon을 true로 바꿔줌
	if(s_answer[strlen(s_answer) - 1] == ';'){
		has_semicolon = true;
		s_answer[strlen(s_answer) - 1] = '\0';
	}

	//해당 문자열을 토큰단위로 파싱해서 tokens에 채워넣음
	//	여러방식으로 표현이 될수 있는 경우 일관된 표현으로 바꿔준다.
	//	문법에 문제가 있는 경우 종료
	if(!make_tokens(s_answer, tokens)){
		close(fd_std);
		return false;
	}

	idx = 0;
	//토큰을 연산자 우선순위가 적용된 트리로 만듦
	std_root = make_tree(std_root, tokens, &idx, 0);

	//정답 디렉토리의 파일경로를 tmp에 입력.
	sprintf(tmp, "%s/%s", ansDir, filename);
	//파일을 연다.
	fd_ans = open(tmp, O_RDONLY);

	//정답이 :으로 여러개 나올수 있으므로 :개수만큼 여러번 체크한다.
	while(1)
	{
		ans_root = NULL;
		result = true;

		//이전에 사용한 토큰 주소공간을 0으로 다시 초기화 한다.
		for(idx = 0; idx < TOKEN_CNT; idx++)
			memset(tokens[idx], 0, sizeof(tokens[idx]));

		//:이전의 답을 가져온다.
		//루프로 이후에 다시 읽을 경우 읽었던 : 이후부터 읽게됨.
		strcpy(a_answer, get_answer(fd_ans, a_answer));

		//여러답을 다 읽어들였다면 루프 종료
		if(!strcmp(a_answer, ""))
			break;

		//읽어들인 답의 양측 공백을 없앰.
		strcpy(a_answer, ltrim(rtrim(a_answer)));

		//학생답안에 세미콜론이 없고 답안에는 세미콜론이 있다면 틀린것으로 간주하고 루프를 넘김
		if(has_semicolon == false){
			if(a_answer[strlen(a_answer) -1] == ';')
				continue;
		}
		//학생답안에 세미콜론이 있고 답안에는 세미콜론이 없다면 틀린것으로 간주하고 루프를 넘김
		//둘다 있는 경우 정답의 세미콜른을 \0으로 바꿔줌
		else if(has_semicolon == true)
		{
			if(a_answer[strlen(a_answer) - 1] != ';')
				continue;
			else
				a_answer[strlen(a_answer) - 1] = '\0';
		}

		//토큰화 시킴. 실패했다면 오답 처리
		if(!make_tokens(a_answer, tokens))
			continue;

		idx = 0;
		//정답 토큰을 연산자 우선순위가 적용된 트리화 시킴
		ans_root = make_tree(ans_root, tokens, &idx, 0);

		//두 트리를 비교함. 앞뒤가 바뀌어도 되는경우 바꿔서도 비교해봄
		compare_tree(std_root, ans_root, &result);

		//결과가 맞았다면 트루를 리턴하고 종료함
		if(result == true){
			close(fd_std);
			close(fd_ans);

			//할당한 메모리들 free
			if(std_root != NULL)
				free_node(std_root);
			if(ans_root != NULL)
				free_node(ans_root);
			return true;

		}
	}
	
	close(fd_std);
	close(fd_ans);

	//할당한 메모리들 free
	if(std_root != NULL)
		free_node(std_root);
	if(ans_root != NULL)
		free_node(ans_root);

	//루프동안 답을 못찾았다면 실패로 간주하고 false리턴
	return false;
}

double score_program(char *id, char *filename)
{
	double compile;
	int result;

	//파일을 컴파일해 결과 값을 가져옴
	compile = compile_program(id, filename);

	//컴파일 에러거나 실패한 경우 false(0)리턴
	if(compile == ERROR || compile == false)
		return false;
	
	//프로그램을 실행시켜 두 답이 일치한 경우 true 실패한 경우 false리턴
	result = execute_program(id, filename);

	//실패한 경우 false리턴
	if(!result)
		return false;

	//컴파일 에러가 있다면 있었던 만큼 까진 점수를 리턴함
	if(compile < 0)
		return compile;

	//위에 해당 없다면 true(1)리턴
	return true;
}

int is_thread(char *qname)
{
	int i;
	int size = sizeof(threadFiles) / sizeof(threadFiles[0]);

	//threadFiles를 순회하며 해당 이름이 threadFile에 있다면 true 아니라면 false
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

	//문제 번호가 저장될 곳을 0으로 초기화
	memset(qname, 0, sizeof(qname));
	//파일이름에서 문제번호만 추출함.
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));
	
	//스레드 컴파일 되기로한 문제인지 체크
	if(tOption){
		if(**threadFiles == 0){
			isthread = true;
		} else {
			isthread = is_thread(qname);
		}
	}
	
	//정답파일이 있는 경로를 tmp_f에 저장
	sprintf(tmp_f, "%s/%s", ansDir, filename);
	//컴파일 된 정답 실행파일이 저장될 경로를 tmp_e에 저장
	sprintf(tmp_e, "%s/%s.exe", DEFANSDIR, qname);

	//tOption이 켜져있고 스레드 컴파일 되기로 한 프로그램이라면 lpthread 추가
	if(tOption && isthread)
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	//아니라면 그냥 컴파일
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	//에러 목록이 저장될 파일 경로를 tmp_e에 저장
	sprintf(tmp_e, "%s/%s_error.txt", ansDir, qname);
	//해당 파일 생성
	fd = creat(tmp_e, 0666);

	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);
	unlink(tmp_e);

	if(size > 0)
		return false;

	//./STD 폴더에 id 폴더가 없다면 생성
	strcpy(tmp_e, DEFSTDDIR);
	ConcatPath(tmp_e, id);
	if(access(tmp_e, F_OK) < 0)
		mkdir(tmp_e, 0755);

	sprintf(tmp_f, "%s/%s/%s", stuDir, id, filename);
	sprintf(tmp_e, "%s/%s/%s.stdexe", DEFSTDDIR, id, qname);

	if(tOption && isthread)
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	sprintf(tmp_f, "%s/%s/%s_error.txt", stuDir, id, qname);
	fd = creat(tmp_f, 0666);

	//해당 명령어로 system명령어로 실행
	//	에러출력 디스크립터를 에러 출력 파일로 바꿈.
	//	따라서 출력되는 에러메시지가 파일에 써짐
	redirection(command, fd, STDERR);
	//파일 크기를 size에 저장
	size = lseek(fd, 0, SEEK_END);
	close(fd);

	//size가 0이상이라면 에러가 있었음을 의미
	if(size > 0){
		//eOption이 있었다면
		if(eOption)
		{
			//error파일 저장 경로를 tmp_e에 입력
			sprintf(tmp_e, "%s/%s", errorDir, id);
			//만약 해당 경로가 없을경우 생성
			if(access(tmp_e, F_OK) < 0)
				mkdir(tmp_e, 0755);

			//에러파일을 옮길 경로를 tmp_e에 저장
			sprintf(tmp_e, "%s/%s/%s_error.txt", errorDir, id, qname);
			//파일을 tmp_f에서 tmp_e로 옮김
			rename(tmp_f, tmp_e);

			//에러가 있으면 0점
			//	Warning이 나온만큼 감점
			result = check_error_warning(tmp_e);
		}
		//eOption이 없엇다면
		else{
			//에러가 있으면 0점
			//	Warning이 나온만큼 감점
			result = check_error_warning(tmp_f);
			//에러파일 삭제
			unlink(tmp_f);
		}

		//까진 점수를 리턴 또는 에러가 발생했다면 0점
		return result;
	}

	//생성됐던에러 파일을 지움
	unlink(tmp_f);
	//에러가 없다면 true 리턴
	return true;
}

double check_error_warning(char *filename)
{
	FILE *fp;
	char tmp[BUFLEN];
	double warning = 0;

	//에러 파일을 열어봄. 없다면 에러 처리
	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "fopen error for %s\n", filename);
		return false;
	}

	//에러가 있는 경우 바로 ERROR값 리턴
	//	아닌경우 warning이 발생한 만큼 더한 점수를 warning에 누적.
	while(fscanf(fp, "%s", tmp) > 0){
		if(!strcmp(tmp, "error:"))
			return ERROR;
		else if(!strcmp(tmp, "warning:"))
			warning += WARNING;
	}

	//에러가 발생한만큼 누적된 감점을 리턴
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

	//qname의 메모리를 0으로 초기화
	memset(qname, 0, sizeof(qname));
	//뒤의 확장자를 빼고 문제 이름만 넣기
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	//정답 답안 문제의 실행 결과 파일 경로를 ans_fname에 넣음
	sprintf(ans_fname, "%s/%s.stdout", DEFANSDIR, qname);
	//해당 파일 생성
	fd = creat(ans_fname, 0666);

	//정답 답안 실행파일의 경로를 tmp에 넣음
	sprintf(tmp, "%s/%s.exe", DEFANSDIR, qname);
	//STDOUT을 파일 디스크립터로 바꿔서 출력이 나오게함.
	//	프로그램이 실행 되고 파일이 출력되는 결과가 .stdout에 찍히게 됨.
	redirection(tmp, fd, STDOUT);
	close(fd);

	//학생 정답 답안 문제의 실행 결과 파일 경로를 std_fname에 담음
	sprintf(std_fname, "%s/%s/%s.stdout", DEFSTDDIR, id, qname);
	//해당 파일 생성
	fd = creat(std_fname, 0666);

	//학생 정답 답안 실행파일의 경로를 tmp에 넣음
	//	학생 답안은 에러가 프린트되어 나오거나 무한루프 될 수 있으니 백그라운드로 실행되게끔 변환해서 tmp에 넣음
	sprintf(tmp, "%s/%s/%s.stdexe &", DEFSTDDIR, id, qname);

	//프로그램 실행시간을 측정
	start = time(NULL);

	//학생 답안을 실행.
	//	STDOUT을 파일 디스크립터로 바꿔서 출력이 나오게함.
	//	프로그램이 실행 되고 파일이 출력되는 결과가 .stdout에 찍히게 됨.
	redirection(tmp, fd, STDOUT);
	//문제이름.stdexe가 tmp에 저장됨
	sprintf(tmp, "%s.stdexe", qname);
	//inBackground를 통해 현재 프로세스가 실행중인지 체크함
	while((pid = inBackground(tmp)) > 0){
		//end를 갱신
		end = time(NULL);

		//OVER만큼 시간이 지나면 오답으로 판단하고 해당 프로세스를 죽임
		if(difftime(end, start) > OVER){
			kill(pid, SIGKILL);
			close(fd);
			//실패한걸로 간주해 false(0) 리턴
			return false;
		}
	}

	close(fd);

	//두 파일이 같은 지 비교 같다면 true 다르다면 false
	return compare_resultfile(std_fname, ans_fname);
}

pid_t inBackground(char *name)
{
	pid_t pid;
	char command[64];
	char tmp[64];
	int fd;
	
	//tmp메모리를 0으로 초기화
	memset(tmp, 0, sizeof(tmp));
	//background.txt를 새로 만듦
	fd = open("background.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);

	sprintf(command, "ps | grep %s", name);
	//ps명령어를 실행뒤 입력으로 들어온 프로세스 이름의 행만 뽑아서 출력.
	//위의 명령어를 실행하며 표준 출력으로 나올 데이터를 backgound.txt에 쓰게함
	redirection(command, fd, STDOUT);

	//파일포인터를 처음으로 되돌림
	lseek(fd, 0, SEEK_SET);
	//받아온 프로세스의 상태를 읽어옴
	read(fd, tmp, sizeof(tmp));

	//해당 프로세스가 없는 경우 텍스트파일을 삭제후 0을리턴함
	if(!strcmp(tmp, "")){
		unlink("background.txt");
		close(fd);
		return 0;
	}

	//해당 프로세스의 pid를 가져와 정수형대의 값을 pid에 넣음
	pid = atoi(strtok(tmp, " "));
	close(fd);

	//텍스트 파일 삭제
	unlink("background.txt");
	//위에서 가져온 pid를 리턴
	return pid;
}

int compare_resultfile(char *file1, char *file2)
{
	int fd1, fd2;
	char c1, c2;
	int len1, len2;

	//인자로 들어온 두 파일을 열어봄
	fd1 = open(file1, O_RDONLY);
	fd2 = open(file2, O_RDONLY);

	while(1)
	{
		//한글자 씩 읽음. 공백이 아닐때 까지 읽어들임
		while((len1 = read(fd1, &c1, 1)) > 0){
			if(c1 == ' ') 
				continue;
			else 
				break;
		}
		//한글자 씩 읽음. 공백이 아닐때 까지 읽어들임
		while((len2 = read(fd2, &c2, 1)) > 0){
			if(c2 == ' ') 
				continue;
			else 
				break;
		}
		
		//두개 다 다읽었을 때 까지 while을 돌았다면 정답으로 간주
		if(len1 == 0 && len2 == 0)
			break;

		//두 문자 전부 대문자인 경우 소문자로 변환한다.
		to_lower_case(&c1);
		to_lower_case(&c2);

		//두 문자가 다르다면 오답으로 간주 false리턴.
		if(c1 != c2){
			close(fd1);
			close(fd2);
			return false;
		}
	}
	//while문을 탈출했다면 정답으로 간주 true 리턴.
	close(fd1);
	close(fd2);
	return true;
}

void redirection(char *command, int new, int old)
{
	int saved;

	//old파일디스크립터를 saved에 복제
	saved = dup(old);
	//new디스크립터를 old에 복제
	dup2(new, old);

	//바뀐 디스크립터를 적용해 해당 명령어를 실행
	system(command);

	//다시 원래 디스크립터로 되돌림
	dup2(saved, old);
	//늘렸던거 다시 닫음
	close(saved);
}

char* get_file_extension(const char *filename)
{
	return strrchr(filename, '.');
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
	printf("Usage : ssu_score <STD_DIR> <ANS_DIR> [OPTION]\n");
	printf("Option : \n");
	printf(" -n <CSVFILENAME>\n");
	printf(" -m\n");
	printf(" -c [STUDENTIDS...]\n");
	printf(" -p [STUDENTIDS...]\n");
	printf(" -t [QNAMES ...]\n");
	printf(" -s <CATEGORY> <1|-1>\n");
	printf(" -e <DIRNAME>\n");
	printf(" -h\n");
}

int find_file(const char* dir, const char* file)
{
	char buf[BUFLEN];

	strcpy(buf, dir);
	ConcatPath(buf, file);
	return access(buf, F_OK);
}