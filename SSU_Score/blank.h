#ifndef BLANK_H_
#define BLANK_H_

#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif
#ifndef BUFLEN
	#define BUFLEN 4096
#endif

#define OPERATOR_CNT 24
#define DATATYPE_SIZE 35
#define MINLEN 64
#define TOKEN_CNT 50

typedef struct node{
	int parentheses;
	//토큰?
	char *name;
	//자신이 부모노드의 chld head인 경우만 값을 가지고 있다.
	struct node *parent;

	//child head는 prev가 NULL 이고, child_head만이 부모노드를 가질 수 있다.
	struct node *child_head;

	struct node *prev;
	struct node *next;
}node;

//연산자 우선순위 저장용
typedef struct operator_precedence{
	//어떤 오퍼레이터가
	char *operator;
	//몇 번째 우선순위인지
	int precedence;
}operator_precedence;

//앞 뒤가 바뀌어도 되는 연산자일 경우를 고려해 두 노드가 같은지를 체크
//	result에 성공시 1, 실패시 0이 담김
//Warning: 1+1과 1+2와 같은 케이스에 문제가 발생함
void compare_tree(node *root1,  node *root2, int *result);

//받은 토큰으로 연산자 우선순위에 근거해 트리를 만듦
//	idx는 토큰의 어디부터 트리를 만들지임.
node *make_tree(node *root, char (*tokens)[MINLEN], int *idx, int parentheses);

//자식 노드의 순서를 바꾼다.
//	자식이 2개일 경우를 전제로 한다.
node *change_sibling(node *parent);

node *create_node(char *name, int parentheses);
int get_precedence(char *op);
int is_operator(char *op);
void print(node *cur);

//부모노드를 리턴함. 연산자를 의미
//	cur이 NULL일 경우 NULL 리턴
node *get_operator(node *cur);

//root 노드를 리턴함. cur이 NULL일 경우 NULL 리턴
node *get_root(node *cur);

node *get_high_precedence_node(node *cur, node *new);
node *get_most_high_precedence_node(node *cur, node *new);
node *insert_node(node *old, node *new);
node *get_last_child(node *cur);
void free_node(node *cur);

//자신을 포함한 형제의 개수를 리턴함.
//	cur이 바뀌진 않음.
int get_sibling_cnt(node *cur);

//해당 문자열을 토큰 단위로 파싱해서 tokens에 채워 넣음
//	만약 토큰이 여러 방식으로 표현될 수 있는 경우 일관된 표현으로 바꾼다.
//	문법에 문제가 있는경우 false 정상인 경우 true 리턴
//	최초 사용시 ltrim을 사용해 공백을 제거한 뒤 사용해야됨.
//Warning: a** 같은 string을 집어넣을 경우 무한루프
//Warning: int a++ 같은 애도 통과함
int make_tokens(char *str, char tokens[TOKEN_CNT][MINLEN]);

//str에 대해 타입 검사를 함
//	공백을 제거한 뒤 str의 시작이 gcc나 data type인 경우 2리턴, 아닌경우 1 리턴, 띄어쓰기 등 오류가 있는 경우 0 리턴
//Warning: 우연히 중간에 gcc라는 글자가 들어가거나 공백을 제거했더니 gcc라는 글자가 만들어지면 오류로 판단됨
int is_typeStatement(char *str);

int find_typeSpecifier(char tokens[TOKEN_CNT][MINLEN]);
int find_typeSpecifier2(char tokens[TOKEN_CNT][MINLEN]);

//0 - 9, a - z, A - Z 사이인 경우 참, 아닌경우 false.
int is_character(char c);

int all_star(char *str);
int all_character(char *str);
int reset_tokens(int start, char tokens[TOKEN_CNT][MINLEN]);

//tokens의 값을 0으로 채운다.
void clear_tokens(char tokens[TOKEN_CNT][MINLEN]);

int get_token_cnt(char tokens[TOKEN_CNT][MINLEN]);

//문자열의 맨 뒤의 공백들이 지워진 새로운 주소의 문자열을 리턴한다.
//	공백이 없는 경우 문자열 길이는 0이 된다.
//	_str의 변경은 없다
//Warning: while문이 잘못되어 오류로 동작함. 공백이 없는 경우 문자열 길이가 0이 된다는 것도 사실 1이 되야함
char *rtrim(char *_str);

//문자열에 최초로 공백들이 올 경우 이를 지움.
//	실제로는 _str에서의 공백을 제외한 위치가 리턴됨
//	공백이 없는 경우 문자열 끝(\0)의 위치가 리턴됨. 문자열 길이는 0
//	_str의 변경은 없다.
char *ltrim(char *_str);

//str에서 공백을 제거함.
void remove_space(char *str);

//양쪽의 괄호 갯수가 맞는지 체크
int check_brackets(char *str);

char* remove_extraspace(char *str);

#endif
