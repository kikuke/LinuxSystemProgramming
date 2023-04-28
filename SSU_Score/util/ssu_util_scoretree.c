#include <stdlib.h>
#include <string.h>

#include "ssu_util_scoretree.h"

struct ScoreTree* CreateScoreTree(struct ScoreTree *bef, struct ScoreTree *aft, struct ScoreTree *up, struct ScoreTree *down, double record)
{
    struct ScoreTree* scoreTree;
    
    scoreTree = (struct ScoreTree*)malloc(sizeof(struct ScoreTree));
    scoreTree->next[SCORE_BEF] = bef;
    scoreTree->next[SCORE_AFT] = aft;
    scoreTree->next[SCORE_UP] = up;
    scoreTree->next[SCORE_DOWN] = down;
    scoreTree->record = record;

    return scoreTree;
}

void SetUpDownScoreTree(struct ScoreTree *upTree, struct ScoreTree *downTree)
{
    upTree->next[SCORE_DOWN] = downTree;
    downTree->next[SCORE_UP] = upTree;
}

void SetBefAftScoreTree(struct ScoreTree *befTree, struct ScoreTree *aftTree)
{
    befTree->next[SCORE_AFT] = aftTree;
    aftTree->next[SCORE_BEF] = befTree;
}

struct ScoreTree* FindFirstTree(struct ScoreTree *target)
{
    while(target->next[SCORE_BEF] != NULL)
        target = target->next[SCORE_BEF];
    return target;
}

struct ScoreTree* FindLastTree(struct ScoreTree *target)
{
    while(target->next[SCORE_AFT] != NULL)
        target = target->next[SCORE_AFT];
    return target;
}

struct ScoreTree* DownTree(struct ScoreTree* target)
{
    struct ScoreTree* down;
    struct ScoreTree* down_down;

    down = target->next[SCORE_DOWN];
    if(down == NULL)
        return NULL;

    down_down = down->next[SCORE_DOWN];
    down->next[SCORE_UP] = target->next[SCORE_UP];
    down->next[SCORE_DOWN] = target;
    target->next[SCORE_UP] = down;
    target->next[SCORE_DOWN] = down_down;

    return target;
}