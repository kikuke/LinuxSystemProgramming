#ifndef SSU_UTIL_SCORETREE_H
#define SSU_UTIL_SCORETREE_H

enum ScoreNext {
    SCORE_BEF,
    SCORE_AFT,
    SCORE_UP,
    SCORE_DOWN
};

typedef struct ScoreTree
{
    double record;

    //ScoreNext를 통해 접근하기
    struct ScoreTree *next[4];
} SCORETREE;

//해당 인자를 바탕으로 ScoreTree를 생성된 ScoreTree를 리턴
struct ScoreTree* CreateScoreTree(struct ScoreTree *bef, struct ScoreTree *aft, struct ScoreTree *up, struct ScoreTree *down, double record);

//해당 트리의 up, down관계를 설정함.
void SetUpDownScoreTree(struct ScoreTree *upTree, struct ScoreTree *downTree);

//해당 트리의 bef, aft관계를 설정함.
void SetBefAftScoreTree(struct ScoreTree *befTree, struct ScoreTree *aftTree);

//가장 이전에 있는 트리를 리턴함.
struct ScoreTree* FindFirstTree(struct ScoreTree *target);

//가장 이후에 있는 트리를 리턴함.
struct ScoreTree* FindLastTree(struct ScoreTree *target);

//해당 트리를 한단계 아래로 내립니다. target의 아래있던 트리는 단계가 상승합니다.
//  성공시 target리턴, 실패시 NULL
struct ScoreTree* DownTree(struct ScoreTree* target);

#endif