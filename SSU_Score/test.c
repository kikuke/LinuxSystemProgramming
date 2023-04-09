#include <stdio.h>

#include "blank.c"

void is_typeStatement_test(char *str);
void make_tokens_test(char *str);
void rtrim_test(char* arr);
void ltrim_test(char* arr);
void compare_tree_Test(const char* n1_c1_s, const char* n1_c2_s, const char* n2_c1_s, const char* n2_c2_s);

int main(void)
{
    char test1 = ' ';
    char hello[] = "int gcc = 0";//11문자열

    is_typeStatement_test(hello);

    //make_tokens_test(hello);

    //compare_tree_Test("2", "2", "1", "2");
    //rtrim_test(hello);
    //ltrim_test(hello);

    return 0;
}

void is_typeStatement_test(char *str)
{
    printf("is_typeStatement result: %d\n", is_typeStatement(str));
}

void make_tokens_test(char *str)
{
    char test2 = 'a';
    char tokens[TOKEN_CNT][MINLEN];
    printf("make_token result: %d\n", make_tokens(str, tokens));
}

void rtrim_test(char* arr)
{   
    printf("%ld\n", strlen(rtrim(arr)));
}

void ltrim_test(char* arr)
{
    printf("%ld\n", strlen(ltrim(arr)));
}

void compare_tree_Test(const char* n1_c1_s, const char* n1_c2_s, const char* n2_c1_s, const char* n2_c2_s)
{
    int result = true;
    node n1_p = {0, "+", NULL, NULL, NULL, NULL};
    node n2_p = {0, "+", NULL, NULL, NULL, NULL};
    node n1_c1 = {0, n1_c1_s, &n1_p, NULL, NULL, NULL};
    node n1_c2 = {0, n1_c2_s, NULL, NULL, &n1_c1, NULL};
    node n2_c1 = {0, n2_c1_s, &n2_p, NULL, NULL, NULL};
    node n2_c2 = {0, n2_c2_s, NULL, NULL, &n2_c1, NULL};

    n1_p.child_head = &n1_c1;
    n2_p.child_head = &n2_c1;
    n1_c1.next = &n1_c2;
    n2_c1.next = &n2_c2;

    compare_tree(&n1_p, &n2_p, &result);
    printf("%s+%s cmp %s+%s: %d\n", n1_c1_s, n1_c2_s, n2_c1_s, n2_c2_s, result);
}