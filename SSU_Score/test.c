#include <stdio.h>

#include "blank.c"

void rtrim_test(char* arr);
void ltrim_test(char* arr);
void compare_tree_Test(const char* n1_c1_s, const char* n1_c2_s, const char* n2_c1_s, const char* n2_c2_s);

int main(void)
{
    char hello[] = "          ";//11문자열
    //compare_tree_Test("2", "2", "1", "2");
    rtrim_test(hello);
    ltrim_test(hello);

    return 0;
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