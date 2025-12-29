#pragma once
#include "raindrop.h"

// 链表管理
typedef struct {
    RainDrop* head;
    int count;
} RainList;

// 链表操作函数
void InitRainList(RainList* list);
void AddRainDrop(RainList* list, RainDrop* drop);
void CleanupRainDrops(RainList* list);
void ClearRainList(RainList* list);
int GetRaindropCount(RainList* list);
int CountRaindropsByState(RainList* list, RainState state);