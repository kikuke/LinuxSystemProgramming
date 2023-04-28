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
    if(target->next[SCORE_UP] != NULL)
        target->next[SCORE_UP]->next[SCORE_DOWN] = down;
    if(down_down != NULL)
        down_down->next[SCORE_UP] = target;
    down->next[SCORE_UP] = target->next[SCORE_UP];
    down->next[SCORE_DOWN] = target;
    target->next[SCORE_UP] = down;
    target->next[SCORE_DOWN] = down_down;

    return target;
}

int SortTreeByField(struct ScoreTree* target, int fieldIdx, int isDesc)
{
    struct ScoreTree *downTree = target;
    struct ScoreTree *aftTree;
    //0은 해당 타겟의 제일 첫 트리, 1은 해당 타겟의 해당 idx 트리
    struct ScoreTree *sortTarget[QNUM][2];
    struct ScoreTree *tmpTree;
    int idxCnt;
    int sortCnt;
    int sign;

    sortCnt = 0;
	for(downTree=downTree->next[SCORE_DOWN]; downTree!=NULL; downTree=downTree->next[SCORE_DOWN]){
        idxCnt = 0;

		for(aftTree = downTree; aftTree!=NULL; aftTree=aftTree->next[SCORE_AFT]){
            if(idxCnt == fieldIdx){
                sortTarget[sortCnt][0] = downTree;
                sortTarget[sortCnt][1] = aftTree;
                sortCnt++;
            }
            idxCnt++;
		}
	}
    if(sortCnt == 0)
        return -1;
    
    //오름/내림차순 설정
    isDesc == 0 ? (sign = +1) : (sign = -1);

    for(int i=0; i<sortCnt-1; i++){
        for(int j=0; j<sortCnt-1-i; j++){
            if(sign * (sortTarget[j][1]->record - sortTarget[j+1][1]->record) > 0){
                if(DownTree(sortTarget[j][0]) == NULL)
                    return -1;

                tmpTree = sortTarget[j][0];
                sortTarget[j][0] = sortTarget[j+1][0];
                sortTarget[j+1][0] = tmpTree;
                tmpTree = sortTarget[j][1];
                sortTarget[j][1] = sortTarget[j+1][1];
                sortTarget[j+1][1] = tmpTree;
            }
        }
    }

    return 0;
}