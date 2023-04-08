#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#include "blank.h"

//35가지 자료형들 명칭 정리
char datatype[DATATYPE_SIZE][MINLEN] = {"int", "char", "double", "float", "long"
			, "short", "ushort", "FILE", "DIR","pid"
			,"key_t", "ssize_t", "mode_t", "ino_t", "dev_t"
			, "nlink_t", "uid_t", "gid_t", "time_t", "blksize_t"
			, "blkcnt_t", "pid_t", "pthread_mutex_t", "pthread_cond_t", "pthread_t"
			, "void", "size_t", "unsigned", "sigset_t", "sigjmp_buf"
			, "rlim_t", "jmp_buf", "sig_atomic_t", "clock_t", "struct"};

//24개의 연산자들에 대한 우선순위 정리
operator_precedence operators[OPERATOR_CNT] = {
	{"(", 0}, {")", 0}
	,{"->", 1}	
	,{"*", 4}	,{"/", 3}	,{"%", 2}	
	,{"+", 6}	,{"-", 5}	
	,{"<", 7}	,{"<=", 7}	,{">", 7}	,{">=", 7}
	,{"==", 8}	,{"!=", 8}
	,{"&", 9}
	,{"^", 10}
	,{"|", 11}
	,{"&&", 12}
	,{"||", 13}
	,{"=", 14}	,{"+=", 14}	,{"-=", 14}	,{"&=", 14}	,{"|=", 14}
};

void compare_tree(node *root1,  node *root2, int *result)
{
	node *tmp;

	//둘 중 하나라도 NULL이면 실패
	if(root1 == NULL || root2 == NULL){
		*result = false;
		return;
	}

	//root1과 root2 모두 비교연산자이고, 서로 다르다면 root2의 부등호를 반대로 한다. 이때 등호는 유지한다.
	//	root2의 자식순서는 root2가 비교연산자일 경우와 상관없이 root1이 비교연산자일 때 무조건 바꾼다.
	if(!strcmp(root1->name, "<") || !strcmp(root1->name, ">") || !strcmp(root1->name, "<=") || !strcmp(root1->name, ">=")){
		if(strcmp(root1->name, root2->name) != 0){

			if(!strncmp(root2->name, "<", 1))
				strncpy(root2->name, ">", 1);

			else if(!strncmp(root2->name, ">", 1))
				strncpy(root2->name, "<", 1);

			else if(!strncmp(root2->name, "<=", 2))
				strncpy(root2->name, ">=", 2);

			else if(!strncmp(root2->name, ">=", 2))
				strncpy(root2->name, "<=", 2);

			//root2의 자식노드 순서를 바꾼다.
			root2 = change_sibling(root2);
		}
	}

	//두 이름이 다를경우 실패 리턴
	if(strcmp(root1->name, root2->name) != 0){
		*result = false;
		return;
	}

	//둘 중 하나만 자식 노드가 없을 경우 실패 리턴
	if((root1->child_head != NULL && root2->child_head == NULL)
		|| (root1->child_head == NULL && root2->child_head != NULL)){
		*result = false;
		return;
	}
	//root1이 자식노드가 있는 경우
	else if(root1->child_head != NULL){
		//두 자식 노드의 개수가 다르다면 실패 리턴
		if(get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)){
			*result = false;
			return;
		}


		//앞뒤가 바뀌어도 상관없는 연산자들일 경우
		//	얘는 자식이 딱 두개임
		if(!strcmp(root1->name, "==") || !strcmp(root1->name, "!="))
		{
			//각 자식 헤드를 기준으로 재귀 호출
			//	둘 중 하나라도 NULL이면 실패가 리턴된다.
			compare_tree(root1->child_head, root2->child_head, result);

			if(*result == false)
			{
				*result = true;
				//실패 했다면 자식 순서를 바꿔서 다시 시도.
				//	함수가 한 쪽만 고려하고 있기에 이런 방식으로 양방향 체크함.
				root2 = change_sibling(root2);
				compare_tree(root1->child_head, root2->child_head, result);
			}
		}
		//앞뒤가 바뀌어도 상관없는 연산자들일 경우
		//	자식이 두개 이상?
		else if(!strcmp(root1->name, "+") || !strcmp(root1->name, "*")
				|| !strcmp(root1->name, "|") || !strcmp(root1->name, "&")
				|| !strcmp(root1->name, "||") || !strcmp(root1->name, "&&"))
		{
			//두 자식노드의 개수가 다르다면 실패 리턴
			if(get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)){
				*result = false;
				return;
			}
			//temp가 자식노드를 가리키고 있음
			tmp = root2->child_head;
			//맨 처음의 자식노드로 되감음
			while(tmp->prev != NULL)
				tmp = tmp->prev;
			//root1의 자식노드에 대해 root2의 모든 자식노드를 순회하며 result가 true인 것이 없는지 탐색함
			//	일치하는 것이 없다면 실패
			while(tmp != NULL)
			{
				compare_tree(root1->child_head, tmp, result);
			
				if(*result == true)
					break;
				else{
					if(tmp->next != NULL)
						*result = true;
					tmp = tmp->next;
				}
			}
		}
		else{
			//바뀌어도 되는 연산자가 아닐경우 바로 자식 헤드에 대해 재귀 호출
			compare_tree(root1->child_head, root2->child_head, result);
		}
	}

	//형제 노드가 있는 경우
	if(root1->next != NULL){
		//형제 노드 개수가 다르다면 실패 리턴
		if(get_sibling_cnt(root1) != get_sibling_cnt(root2)){
			*result = false;
			return;
		}
		//이전의 비교에서 성공했을 경우
		if(*result == true)
		{
			//부모 노드. 즉 연산자를 가져옴
			tmp = get_operator(root1);
			//앞뒤가 바뀌어도 상관없는 연산자들일 경우
			//	위에서 root2의 자식들을 순회하며 이미 true체크를 했음. 따라서 반대편 한쪽도 true이기만 하면됨.
			if(!strcmp(tmp->name, "+") || !strcmp(tmp->name, "*")
					|| !strcmp(tmp->name, "|") || !strcmp(tmp->name, "&")
					|| !strcmp(tmp->name, "||") || !strcmp(tmp->name, "&&"))
			{	
				tmp = root2;
				//root2를 맨 앞으로 되감음
				while(tmp->prev != NULL)
					tmp = tmp->prev;

				while(tmp != NULL)
				{
					//root1은 위에서 같은지 검사했으므로 순회하며 다른 형제도 일치하는게 있는지 검사함.
					//	근데 이렇게 하면 1 + 1 vs 1 + 2 의 검사에 문제가 있음
					compare_tree(root1->next, tmp, result);

					if(*result == true)
						break;
					else{
						if(tmp->next != NULL)
							*result = true;
						tmp = tmp->next;
					}
				}
			}
			//바뀌어도 상관없지 않은 경우 그냥 같은 건지 검사
			else
				compare_tree(root1->next, root2->next, result);
		}
	}
}

int make_tokens(char *str, char tokens[TOKEN_CNT][MINLEN])
{
	char *start, *end;
	char tmp[BUFLEN];
	char *op = "(),;><=!|&^/+-*\""; 
	int row = 0;
	int i;
 	int isPointer;
	int lcount, rcount;
	int p_str;
	
	//값을 모두 0으로 채움
	clear_tokens(tokens);

	//start가 str의 맨 처음을 가리키게 함.
	start = str;
	
	//타입 검사를 해서 오류인 경우 false리턴
	if(is_typeStatement(str) == 0) 
		return false;	
	
	while(1)
	{
		//start에서 op에 해당하는 문자가 있는지 검사함
		//	character가 아닌경우.
		if((end = strpbrk(start, op)) == NULL)
			break;

		//character가 아닌것이 바로 발견된 경우.
		if(start == end){

			//-- ++인 경우
			if(!strncmp(start, "--", 2) || !strncmp(start, "++", 2)){
				//잘못된 입력인 경우 false 리턴
				if(!strncmp(start, "++++", 4)||!strncmp(start,"----",4))
					return false;

				// ex) ++a
				if(is_character(*ltrim(start + 2))){
					if(row > 0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]))
						return false; //ex) ++a++

					end = strpbrk(start + 2, op);
					if(end == NULL)
						end = &str[strlen(str)];
					while(start < end) {
						if(*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
							return false;
						else if(*start != ' ')
							strncat(tokens[row], start, 1);
						start++;	
					}
				}
				// ex) a++
				else if(row>0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){
					if(strstr(tokens[row - 1], "++") != NULL || strstr(tokens[row - 1], "--") != NULL)	
						return false;

					memset(tmp, 0, sizeof(tmp));
					strncpy(tmp, start, 2);
					strcat(tokens[row - 1], tmp);
					start += 2;
					row--;
				}
				else{
					memset(tmp, 0, sizeof(tmp));
					strncpy(tmp, start, 2);
					strcat(tokens[row], tmp);
					start += 2;
				}
			}

			else if(!strncmp(start, "==", 2) || !strncmp(start, "!=", 2) || !strncmp(start, "<=", 2)
				|| !strncmp(start, ">=", 2) || !strncmp(start, "||", 2) || !strncmp(start, "&&", 2) 
				|| !strncmp(start, "&=", 2) || !strncmp(start, "^=", 2) || !strncmp(start, "!=", 2) 
				|| !strncmp(start, "|=", 2) || !strncmp(start, "+=", 2)	|| !strncmp(start, "-=", 2) 
				|| !strncmp(start, "*=", 2) || !strncmp(start, "/=", 2)){

				strncpy(tokens[row], start, 2);
				start += 2;
			}
			else if(!strncmp(start, "->", 2))
			{
				end = strpbrk(start + 2, op);

				if(end == NULL)
					end = &str[strlen(str)];

				while(start < end){
					if(*start != ' ')
						strncat(tokens[row - 1], start, 1);
					start++;
				}
				row--;
			}
			else if(*end == '&')
			{
				// ex) &a (address)
				if(row == 0 || (strpbrk(tokens[row - 1], op) != NULL)){
					end = strpbrk(start + 1, op);
					if(end == NULL)
						end = &str[strlen(str)];
					
					strncat(tokens[row], start, 1);
					start++;

					while(start < end){
						if(*(start - 1) == ' ' && tokens[row][strlen(tokens[row]) - 1] != '&')
							return false;
						else if(*start != ' ')
							strncat(tokens[row], start, 1);
						start++;
					}
				}
				// ex) a & b (bit)
				else{
					strncpy(tokens[row], start, 1);
					start += 1;
				}
				
			}
		  	else if(*end == '*')
			{
				isPointer=0;

				if(row > 0)
				{
					//ex) char** (pointer)
					for(i = 0; i < DATATYPE_SIZE; i++) {
						if(strstr(tokens[row - 1], datatype[i]) != NULL){
							strcat(tokens[row - 1], "*");
							start += 1;	
							isPointer = 1;
							break;
						}
					}
					if(isPointer == 1)
						continue;
					if(*(start+1) !=0)
						end = start + 1;

					// ex) a * **b (multiply then pointer)
					if(row>1 && !strcmp(tokens[row - 2], "*") && (all_star(tokens[row - 1]) == 1)){
						strncat(tokens[row - 1], start, end - start);
						row--;
					}
					
					// ex) a*b(multiply)
					else if(is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]) == 1){ 
						strncat(tokens[row], start, end - start);   
					}

					// ex) ,*b (pointer)
					else if(strpbrk(tokens[row - 1], op) != NULL){		
						strncat(tokens[row] , start, end - start); 
							
					}
					else
						strncat(tokens[row], start, end - start);

					start += (end - start);
				}

			 	else if(row == 0)
				{
					if((end = strpbrk(start + 1, op)) == NULL){
						strncat(tokens[row], start, 1);
						start += 1;
					}
					else{
						while(start < end){
							if(*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
								return false;
							else if(*start != ' ')
								strncat(tokens[row], start, 1);
							start++;	
						}
						if(all_star(tokens[row]))
							row--;
						
					}
				}
			}
			else if(*end == '(') 
			{
				lcount = 0;
				rcount = 0;
				if(row>0 && (strcmp(tokens[row - 1],"&") == 0 || strcmp(tokens[row - 1], "*") == 0)){
					while(*(end + lcount + 1) == '(')
						lcount++;
					start += lcount;

					end = strpbrk(start + 1, ")");

					if(end == NULL)
						return false;
					else{
						while(*(end + rcount +1) == ')')
							rcount++;
						end += rcount;

						if(lcount != rcount)
							return false;

						if( (row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1])) || row == 1){ 
							strncat(tokens[row - 1], start + 1, end - start - rcount - 1);
							row--;
							start = end + 1;
						}
						else{
							strncat(tokens[row], start, 1);
							start += 1;
						}
					}
						
				}
				else{
					strncat(tokens[row], start, 1);
					start += 1;
				}

			}
			else if(*end == '\"') 
			{
				end = strpbrk(start + 1, "\"");
				
				if(end == NULL)
					return false;

				else{
					strncat(tokens[row], start, end - start + 1);
					start = end + 1;
				}

			}

			else{
				// ex) a++ ++ +b
				if(row > 0 && !strcmp(tokens[row - 1], "++"))
					return false;

				// ex) a-- -- -b
				if(row > 0 && !strcmp(tokens[row - 1], "--"))
					return false;
	
				strncat(tokens[row], start, 1);
				start += 1;
				
				// ex) -a or a, -b
				if(!strcmp(tokens[row], "-") || !strcmp(tokens[row], "+") || !strcmp(tokens[row], "--") || !strcmp(tokens[row], "++")){


					// ex) -a or -a+b
					if(row == 0)
						row--;

					// ex) a+b = -c
					else if(!is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){
					
						if(strstr(tokens[row - 1], "++") == NULL && strstr(tokens[row - 1], "--") == NULL)
							row--;
					}
				}
			}
		}
		//character가 아닌것이 바로 처음으로 발견 되지 않은 경우. 사이에 뭐가 낀 경우
		else{ 
			if(all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1]))   
				row--;				

			if(all_star(tokens[row - 1]) && row == 1)   
				row--;	

			for(i = 0; i < end - start; i++){
				if(i > 0 && *(start + i) == '.'){
					strncat(tokens[row], start + i, 1);

					while( *(start + i +1) == ' ' && i< end - start )
						i++; 
				}
				else if(start[i] == ' '){
					while(start[i] == ' ')
						i++;
					break;
				}
				else
					strncat(tokens[row], start + i, 1);
			}

			if(start[0] == ' '){
				start += i;
				continue;
			}
			start += i;
		}
			
		strcpy(tokens[row], ltrim(rtrim(tokens[row])));

		 if(row > 0 && is_character(tokens[row][strlen(tokens[row]) - 1]) 
				&& (is_typeStatement(tokens[row - 1]) == 2 
					|| is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])
					|| tokens[row - 1][strlen(tokens[row - 1]) - 1] == '.' ) ){

			if(row > 1 && strcmp(tokens[row - 2],"(") == 0)
			{
				if(strcmp(tokens[row - 1], "struct") != 0 && strcmp(tokens[row - 1],"unsigned") != 0)
					return false;
			}
			else if(row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				if(strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)	
					return false;
			}
			else if(row > 1 && is_typeStatement(tokens[row - 1]) == 2){
				if(strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false;
			}
			
		}

		if((row == 0 && !strcmp(tokens[row], "gcc")) ){
			clear_tokens(tokens);
			strcpy(tokens[0], str);	
			return 1;
		} 

		row++;
	}

	if(all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1]))  
		row--;				
	if(all_star(tokens[row - 1]) && row == 1)   
		row--;	

	for(i = 0; i < strlen(start); i++)   
	{
		if(start[i] == ' ')  
		{
			while(start[i] == ' ')
				i++;
			if(start[0]==' ') {
				start += i;
				i = 0;
			}
			else
				row++;
			
			i--;
		} 
		else
		{
			strncat(tokens[row], start + i, 1);
			if( start[i] == '.' && i<strlen(start)){
				while(start[i + 1] == ' ' && i < strlen(start))
					i++;

			}
		}
		strcpy(tokens[row], ltrim(rtrim(tokens[row])));

		if(!strcmp(tokens[row], "lpthread") && row > 0 && !strcmp(tokens[row - 1], "-")){ 
			strcat(tokens[row - 1], tokens[row]);
			memset(tokens[row], 0, sizeof(tokens[row]));
			row--;
		}
	 	else if(row > 0 && is_character(tokens[row][strlen(tokens[row]) - 1]) 
				&& (is_typeStatement(tokens[row - 1]) == 2 
					|| is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])
					|| tokens[row - 1][strlen(tokens[row - 1]) - 1] == '.') ){
			
			if(row > 1 && strcmp(tokens[row-2],"(") == 0)
			{
				if(strcmp(tokens[row-1], "struct") != 0 && strcmp(tokens[row-1], "unsigned") != 0)
					return false;
			}
			else if(row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				if(strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)	
					return false;
			}
			else if(row > 1 && is_typeStatement(tokens[row - 1]) == 2){
				if(strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false;
			}
		} 
	}


	if(row > 0)
	{

		// ex) #include <sys/types.h>
		if(strcmp(tokens[0], "#include") == 0 || strcmp(tokens[0], "include") == 0 || strcmp(tokens[0], "struct") == 0){ 
			clear_tokens(tokens); 
			strcpy(tokens[0], remove_extraspace(str)); 
		}
	}

	if(is_typeStatement(tokens[0]) == 2 || strstr(tokens[0], "extern") != NULL){
		for(i = 1; i < TOKEN_CNT; i++){
			if(strcmp(tokens[i],"") == 0)  
				break;		       

			if(i != TOKEN_CNT -1 )
				strcat(tokens[0], " ");
			strcat(tokens[0], tokens[i]);
			memset(tokens[i], 0, sizeof(tokens[i]));
		}
	}
	
	//change ( ' char ' )' a  ->  (char)a
	while((p_str = find_typeSpecifier(tokens)) != -1){ 
		if(!reset_tokens(p_str, tokens))
			return false;
	}

	//change sizeof ' ( ' record ' ) '-> sizeof(record)
	while((p_str = find_typeSpecifier2(tokens)) != -1){  
		if(!reset_tokens(p_str, tokens))
			return false;
	}
	
	return true;
}

node *make_tree(node *root, char (*tokens)[MINLEN], int *idx, int parentheses)
{
	node *cur = root;
	node *new;
	node *saved_operator;
	node *operator;
	int fstart;
	int i;

	while(1)	
	{
		if(strcmp(tokens[*idx], "") == 0)
			break;
	
		if(!strcmp(tokens[*idx], ")"))
			return get_root(cur);

		else if(!strcmp(tokens[*idx], ","))
			return get_root(cur);

		else if(!strcmp(tokens[*idx], "("))
		{
			// function()
			if(*idx > 0 && !is_operator(tokens[*idx - 1]) && strcmp(tokens[*idx - 1], ",") != 0){
				fstart = true;

				while(1)
				{
					*idx += 1;

					if(!strcmp(tokens[*idx], ")"))
						break;
					
					new = make_tree(NULL, tokens, idx, parentheses + 1);
					
					if(new != NULL){
						if(fstart == true){
							cur->child_head = new;
							new->parent = cur;
	
							fstart = false;
						}
						else{
							cur->next = new;
							new->prev = cur;
						}

						cur = new;
					}

					if(!strcmp(tokens[*idx], ")"))
						break;
				}
			}
			else{
				*idx += 1;
	
				new = make_tree(NULL, tokens, idx, parentheses + 1);

				if(cur == NULL)
					cur = new;

				else if(!strcmp(new->name, cur->name)){
					if(!strcmp(new->name, "|") || !strcmp(new->name, "||") 
						|| !strcmp(new->name, "&") || !strcmp(new->name, "&&"))
					{
						cur = get_last_child(cur);

						if(new->child_head != NULL){
							new = new->child_head;

							new->parent->child_head = NULL;
							new->parent = NULL;
							new->prev = cur;
							cur->next = new;
						}
					}
					else if(!strcmp(new->name, "+") || !strcmp(new->name, "*"))
					{
						i = 0;

						while(1)
						{
							if(!strcmp(tokens[*idx + i], ""))
								break;

							if(is_operator(tokens[*idx + i]) && strcmp(tokens[*idx + i], ")") != 0)
								break;

							i++;
						}
						
						if(get_precedence(tokens[*idx + i]) < get_precedence(new->name))
						{
							cur = get_last_child(cur);
							cur->next = new;
							new->prev = cur;
							cur = new;
						}
						else
						{
							cur = get_last_child(cur);

							if(new->child_head != NULL){
								new = new->child_head;

								new->parent->child_head = NULL;
								new->parent = NULL;
								new->prev = cur;
								cur->next = new;
							}
						}
					}
					else{
						cur = get_last_child(cur);
						cur->next = new;
						new->prev = cur;
						cur = new;
					}
				}
	
				else
				{
					cur = get_last_child(cur);

					cur->next = new;
					new->prev = cur;
	
					cur = new;
				}
			}
		}
		else if(is_operator(tokens[*idx]))
		{
			if(!strcmp(tokens[*idx], "||") || !strcmp(tokens[*idx], "&&")
					|| !strcmp(tokens[*idx], "|") || !strcmp(tokens[*idx], "&") 
					|| !strcmp(tokens[*idx], "+") || !strcmp(tokens[*idx], "*"))
			{
				if(is_operator(cur->name) == true && !strcmp(cur->name, tokens[*idx]))
					operator = cur;
		
				else
				{
					new = create_node(tokens[*idx], parentheses);
					operator = get_most_high_precedence_node(cur, new);

					if(operator->parent == NULL && operator->prev == NULL){

						if(get_precedence(operator->name) < get_precedence(new->name)){
							cur = insert_node(operator, new);
						}

						else if(get_precedence(operator->name) > get_precedence(new->name))
						{
							if(operator->child_head != NULL){
								operator = get_last_child(operator);
								cur = insert_node(operator, new);
							}
						}
						else
						{
							operator = cur;
	
							while(1)
							{
								if(is_operator(operator->name) == true && !strcmp(operator->name, tokens[*idx]))
									break;
						
								if(operator->prev != NULL)
									operator = operator->prev;
								else
									break;
							}

							if(strcmp(operator->name, tokens[*idx]) != 0)
								operator = operator->parent;

							if(operator != NULL){
								if(!strcmp(operator->name, tokens[*idx]))
									cur = operator;
							}
						}
					}

					else
						cur = insert_node(operator, new);
				}

			}
			else
			{
				new = create_node(tokens[*idx], parentheses);

				if(cur == NULL)
					cur = new;

				else
				{
					operator = get_most_high_precedence_node(cur, new);

					if(operator->parentheses > new->parentheses)
						cur = insert_node(operator, new);

					else if(operator->parent == NULL && operator->prev ==  NULL){
					
						if(get_precedence(operator->name) > get_precedence(new->name))
						{
							if(operator->child_head != NULL){
	
								operator = get_last_child(operator);
								cur = insert_node(operator, new);
							}
						}
					
						else	
							cur = insert_node(operator, new);
					}
	
					else
						cur = insert_node(operator, new);
				}
			}
		}
		else 
		{
			new = create_node(tokens[*idx], parentheses);

			if(cur == NULL)
				cur = new;

			else if(cur->child_head == NULL){
				cur->child_head = new;
				new->parent = cur;

				cur = new;
			}
			else{

				cur = get_last_child(cur);

				cur->next = new;
				new->prev = cur;

				cur = new;
			}
		}

		*idx += 1;
	}

	return get_root(cur);
}

node *change_sibling(node *parent)
{
	node *tmp;
	
	//자식 헤드 노드를 가져옴
	tmp = parent->child_head;

	//부모노드의 자식 헤드를 자식 헤드의 형제노드로 바꿈
	parent->child_head = parent->child_head->next;
	//바뀐 자식헤드의 부모를 설정해줌
	parent->child_head->parent = parent;
	//이전 헤드를 지움. 헤드가 되었기 때문
	parent->child_head->prev = NULL;

	//바뀐 자식헤드의 형제로 재설정
	parent->child_head->next = tmp;
	//이전 형제 노드는 바뀐 자식헤드가 됨
	parent->child_head->next->prev = parent->child_head;
	//자식헤드의 옆 형제노드(원래 헤드노드)의 오른쪽 형제 노드는 NULL이 됨.
	//	이로 인해 이진트리 전제가 됨.
	parent->child_head->next->next = NULL;
	//원래 헤드노드의 부모가리키는것을 NULL로 바꿈
	parent->child_head->next->parent = NULL;

	return parent;
}

node *create_node(char *name, int parentheses)
{
	node *new;

	new = (node *)malloc(sizeof(node));
	new->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(new->name, name);

	new->parentheses = parentheses;
	new->parent = NULL;
	new->child_head = NULL;
	new->prev = NULL;
	new->next = NULL;

	return new;
}

int get_precedence(char *op)
{
	int i;

	for(i = 2; i < OPERATOR_CNT; i++){
		if(!strcmp(operators[i].operator, op))
			return operators[i].precedence;
	}
	return false;
}

int is_operator(char *op)
{
	int i;

	for(i = 0; i < OPERATOR_CNT; i++)
	{
		if(operators[i].operator == NULL)
			break;
		if(!strcmp(operators[i].operator, op)){
			return true;
		}
	}

	return false;
}

void print(node *cur)
{
	if(cur->child_head != NULL){
		print(cur->child_head);
		printf("\n");
	}

	if(cur->next != NULL){
		print(cur->next);
		printf("\t");
	}
	printf("%s", cur->name);
}

node *get_operator(node *cur)
{
	if(cur == NULL)
		return cur;

	if(cur->prev != NULL)
		while(cur->prev != NULL)
			cur = cur->prev;

	return cur->parent;
}

node *get_root(node *cur)
{
	if(cur == NULL)
		return cur;

	while(cur->prev != NULL)
		cur = cur->prev;

	if(cur->parent != NULL)
		cur = get_root(cur->parent);

	return cur;
}

node *get_high_precedence_node(node *cur, node *new)
{
	if(is_operator(cur->name))
		if(get_precedence(cur->name) < get_precedence(new->name))
			return cur;

	if(cur->prev != NULL){
		while(cur->prev != NULL){
			cur = cur->prev;
			
			return get_high_precedence_node(cur, new);
		}


		if(cur->parent != NULL)
			return get_high_precedence_node(cur->parent, new);
	}

	if(cur->parent == NULL)
		return cur;
}

node *get_most_high_precedence_node(node *cur, node *new)
{
	node *operator = get_high_precedence_node(cur, new);
	node *saved_operator = operator;

	while(1)
	{
		if(saved_operator->parent == NULL)
			break;

		if(saved_operator->prev != NULL)
			operator = get_high_precedence_node(saved_operator->prev, new);

		else if(saved_operator->parent != NULL)
			operator = get_high_precedence_node(saved_operator->parent, new);

		saved_operator = operator;
	}
	
	return saved_operator;
}

node *insert_node(node *old, node *new)
{
	if(old->prev != NULL){
		new->prev = old->prev;
		old->prev->next = new;
		old->prev = NULL;
	}

	new->child_head = old;
	old->parent = new;

	return new;
}

node *get_last_child(node *cur)
{
	if(cur->child_head != NULL)
		cur = cur->child_head;

	while(cur->next != NULL)
		cur = cur->next;

	return cur;
}

int get_sibling_cnt(node *cur)
{
	int i = 0;

	//맨 처음 형제로 되감음
	while(cur->prev != NULL)
		cur = cur->prev;

	//맨 끝 형제까지 루프하며 카운트함.
	//	여기선 cur이 바뀌지만 결과적으로 cur이 바뀌진 않음
	while(cur->next != NULL){
		cur = cur->next;
		i++;
	}

	return i;
}

void free_node(node *cur)
{
	if(cur->child_head != NULL)
		free_node(cur->child_head);

	if(cur->next != NULL)
		free_node(cur->next);

	if(cur != NULL){
		cur->prev = NULL;
		cur->next = NULL;
		cur->parent = NULL;
		cur->child_head = NULL;
		free(cur);
	}
}


int is_character(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_typeStatement(char *str)
{ 
	char *start;
	char str2[BUFLEN] = {0}; 
	char tmp[BUFLEN] = {0}; 
	char tmp2[BUFLEN] = {0}; 
	int i;	 
	
	//start가 str의 맨 처음을 가리키게 함
	start = str;
	//str2에 복사함
	strncpy(str2,str,strlen(str));
	//str2의 공백을 제거함
	remove_space(str2);

	//원본 파일의 시작이 공백일 경우 한칸 뒤를 참조
	while(start[0] == ' ')
		start += 1;

	//gcc라는 글자가 공백 제거한 스트링에 포함되어 있을 경우
	if(strstr(str2, "gcc") != NULL)
	{
		//start의 맨 앞을 gcc 글자만큼 가져옴
		strncpy(tmp2, start, strlen("gcc"));
		//gcc로 시작하는 경우 2 리턴
		//gcc가 아니라면 에러로 판단해 0 리턴
		if(strcmp(tmp2,"gcc") != 0)
			return 0;
		else
			return 2;
	}
	
	for(i = 0; i < DATATYPE_SIZE; i++)
	{
		//str2와 자료형들 중에서 일치하는 것이 있다면
		if(strstr(str2,datatype[i]) != NULL)
		{	
			//공백을 제거한 것과, 공백을 제거하지 않은 것을 맨앞에서 부터 데이터 타입 길이만큼 두 개다 복사
			strncpy(tmp, str2, strlen(datatype[i]));
			strncpy(tmp2, start, strlen(datatype[i]));
			
			//공백을 제거한 것에서 일치하다면
			if(strcmp(tmp, datatype[i]) == 0)
				//두개가 다르게 생겼다면 오류로 판단해 0 리턴
				//	아닌경우 2 리턴
				if(strcmp(tmp, tmp2) != 0)
					return 0;  
				else
					return 2;
		}

	}
	//자료형이나 gcc둘 다 아닌 경우 1 리턴
	return 1;

}

int find_typeSpecifier(char tokens[TOKEN_CNT][MINLEN]) 
{
	int i, j;

	for(i = 0; i < TOKEN_CNT; i++)
	{
		for(j = 0; j < DATATYPE_SIZE; j++)
		{
			if(strstr(tokens[i], datatype[j]) != NULL && i > 0)
			{
				if(!strcmp(tokens[i - 1], "(") && !strcmp(tokens[i + 1], ")") 
						&& (tokens[i + 2][0] == '&' || tokens[i + 2][0] == '*' 
							|| tokens[i + 2][0] == ')' || tokens[i + 2][0] == '(' 
							|| tokens[i + 2][0] == '-' || tokens[i + 2][0] == '+' 
							|| is_character(tokens[i + 2][0])))  
					return i;
			}
		}
	}
	return -1;
}

int find_typeSpecifier2(char tokens[TOKEN_CNT][MINLEN]) 
{
    int i, j;

   
    for(i = 0; i < TOKEN_CNT; i++)
    {
        for(j = 0; j < DATATYPE_SIZE; j++)
        {
            if(!strcmp(tokens[i], "struct") && (i+1) <= TOKEN_CNT && is_character(tokens[i + 1][strlen(tokens[i + 1]) - 1]))  
                    return i;
        }
    }
    return -1;
}

int all_star(char *str)
{
	int i;
	int length= strlen(str);
	
 	if(length == 0)	
		return 0;
	
	for(i = 0; i < length; i++)
		if(str[i] != '*')
			return 0;
	return 1;

}

int all_character(char *str)
{
	int i;

	for(i = 0; i < strlen(str); i++)
		if(is_character(str[i]))
			return 1;
	return 0;
	
}

int reset_tokens(int start, char tokens[TOKEN_CNT][MINLEN]) 
{
	int i;
	int j = start - 1;
	int lcount = 0, rcount = 0;
	int sub_lcount = 0, sub_rcount = 0;

	if(start > -1){
		if(!strcmp(tokens[start], "struct")) {		
			strcat(tokens[start], " ");
			strcat(tokens[start], tokens[start+1]);	     

			for(i = start + 1; i < TOKEN_CNT - 1; i++){
				strcpy(tokens[i], tokens[i + 1]);
				memset(tokens[i + 1], 0, sizeof(tokens[0]));
			}
		}

		else if(!strcmp(tokens[start], "unsigned") && strcmp(tokens[start+1], ")") != 0) {		
			strcat(tokens[start], " ");
			strcat(tokens[start], tokens[start + 1]);	     
			strcat(tokens[start], tokens[start + 2]);

			for(i = start + 1; i < TOKEN_CNT - 1; i++){
				strcpy(tokens[i], tokens[i + 1]);
				memset(tokens[i + 1], 0, sizeof(tokens[0]));
			}
		}

     		j = start + 1;           
        	while(!strcmp(tokens[j], ")")){
                	rcount ++;
                	if(j==TOKEN_CNT)
                        	break;
                	j++;
        	}
	
		j = start - 1;
		while(!strcmp(tokens[j], "(")){
        	        lcount ++;
                	if(j == 0)
                        	break;
               		j--;
		}
		if( (j!=0 && is_character(tokens[j][strlen(tokens[j])-1]) ) || j==0)
			lcount = rcount;

		if(lcount != rcount )
			return false;

		if( (start - lcount) >0 && !strcmp(tokens[start - lcount - 1], "sizeof")){
			return true; 
		}
		
		else if((!strcmp(tokens[start], "unsigned") || !strcmp(tokens[start], "struct")) && strcmp(tokens[start+1], ")")) {		
			strcat(tokens[start - lcount], tokens[start]);
			strcat(tokens[start - lcount], tokens[start + 1]);
			strcpy(tokens[start - lcount + 1], tokens[start + rcount]);
		 
			for(int i = start - lcount + 1; i < TOKEN_CNT - lcount -rcount; i++) {
				strcpy(tokens[i], tokens[i + lcount + rcount]);
				memset(tokens[i + lcount + rcount], 0, sizeof(tokens[0]));
			}


		}
 		else{
			if(tokens[start + 2][0] == '('){
				j = start + 2;
				while(!strcmp(tokens[j], "(")){
					sub_lcount++;
					j++;
				} 	
				if(!strcmp(tokens[j + 1],")")){
					j = j + 1;
					while(!strcmp(tokens[j], ")")){
						sub_rcount++;
						j++;
					}
				}
				else 
					return false;

				if(sub_lcount != sub_rcount)
					return false;
				
				strcpy(tokens[start + 2], tokens[start + 2 + sub_lcount]);	
				for(int i = start + 3; i<TOKEN_CNT; i++)
					memset(tokens[i], 0, sizeof(tokens[0]));

			}
			strcat(tokens[start - lcount], tokens[start]);
			strcat(tokens[start - lcount], tokens[start + 1]);
			strcat(tokens[start - lcount], tokens[start + rcount + 1]);
		 
			for(int i = start - lcount + 1; i < TOKEN_CNT - lcount -rcount -1; i++) {
				strcpy(tokens[i], tokens[i + lcount + rcount +1]);
				memset(tokens[i + lcount + rcount + 1], 0, sizeof(tokens[0]));

			}
		}
	}
	return true;
}

void clear_tokens(char tokens[TOKEN_CNT][MINLEN])
{
	int i;

	for(i = 0; i < TOKEN_CNT; i++)
		memset(tokens[i], 0, sizeof(tokens[i]));
}

char *rtrim(char *_str)
{
	char tmp[BUFLEN];
	char *end;

	//tmp로 문자열 복사
	strcpy(tmp, _str);

	//문자열의 맨 뒤 문자를 end로
	end = tmp + strlen(tmp) - 1;

	//문자열 시작위치가 아니고, 공백인 동안 계속 감소
	//	end와 _str은 다를수 밖에 없음. 서로 다른 메모리라.
	//	오류가 발생할 여지가 있음.
	while(end != _str && isspace(*end))
		--end;

	//맨 뒤에서부터 최초로 공백이 아닌 지점을 문자열 끝으로 재지정
	*(end + 1) = '\0';
	_str = tmp;
	return _str;
}

char *ltrim(char *_str)
{
	char *start = _str;

	//문자열 끝이 아니거나 공백인 동안 start의 값을 증가
	//	isspace는 개행을 확인하는 함수임.
	while(*start != '\0' && isspace(*start))
		++start;
	
	//공백이 아닌 최초 지점을 리턴
	_str = start;
	return _str;
}

char* remove_extraspace(char *str)
{
	int i;
	char *str2 = (char*)malloc(sizeof(char) * BUFLEN);
	char *start, *end;
	char temp[BUFLEN] = "";
	int position;

	if(strstr(str,"include<")!=NULL){
		start = str;
		end = strpbrk(str, "<");
		position = end - start;
	
		strncat(temp, str, position);
		strncat(temp, " ", 1);
		strncat(temp, str + position, strlen(str) - position + 1);

		str = temp;		
	}
	
	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] ==' ')
		{
			if(i == 0 && str[0] ==' ')
				while(str[i + 1] == ' ')
					i++;	
			else{
				if(i > 0 && str[i - 1] != ' ')
					str2[strlen(str2)] = str[i];
				while(str[i + 1] == ' ')
					i++;
			} 
		}
		else
			str2[strlen(str2)] = str[i];
	}

	return str2;
}



void remove_space(char *str)
{
	char* i = str;
	char* j = str;
	
	while(*j != 0)
	{
		*i = *j++;
		if(*i != ' ')
			i++;
	}
	*i = 0;
}

int check_brackets(char *str)
{
	char *start = str;
	int lcount = 0, rcount = 0;
	
	while(1){
		if((start = strpbrk(start, "()")) != NULL){
			if(*(start) == '(')
				lcount++;
			else
				rcount++;

			start += 1; 		
		}
		else
			break;
	}

	if(lcount != rcount)
		return 0;
	else 
		return 1;
}

int get_token_cnt(char tokens[TOKEN_CNT][MINLEN])
{
	int i;
	
	for(i = 0; i < TOKEN_CNT; i++)
		if(!strcmp(tokens[i], ""))
			break;

	return i;
}
