#include "rainlist.h"
#include <stdlib.h>

// 初始化链表
void InitRainList(RainList* list) {
    list->head = NULL;
    list->count = 0;
}

// 添加雨滴到链尾
void AddRainDrop(RainList* list, RainDrop* drop) {
    if (!drop) return;

    drop->next = NULL;

    if (!list->head) {
        list->head = drop;
    }
    else {
        RainDrop* temp = list->head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = drop;
    }
    list->count++;
}

// 清理已完成的雨滴
void CleanupRainDrops(RainList* list) {
    if (!list || !list->head) return;

    RainDrop* prev = NULL;
    RainDrop* curr = list->head;

    while (curr) {
        if (curr->fade <= 0) {
            // 删除
            if (prev == NULL) {//删除的是头节点
                list->head = curr->next;
            }
            else {
                prev->next = curr->next;
            }

            RainDrop* toDelete = curr;
            curr = curr->next;
            delete toDelete;
            list->count--;
        }
        else {
            prev = curr;
            curr = curr->next;
        }
    }
}

// 清空链表
void ClearRainList(RainList* list) {
    if (!list) return;//链表不存在

    RainDrop* curr = list->head;
    while (curr) {
        RainDrop* next = curr->next;
        delete curr;
        curr = next;
    }

    list->head = NULL;
    list->count = 0;
}

// 获取雨滴数量
int GetRaindropCount(RainList* list) {
    return list ? list->count : 0;
}

int CountRaindropsByState(RainList* list, RainState state) {
    int count = 0;
    RainDrop* current = list->head;
    while (current) {
        if (current->status == state) {
            count++;
        }
        current = current->next;
    }
    return count;
}