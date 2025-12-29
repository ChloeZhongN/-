#pragma once
#include <graphics.h>

// 屏幕尺寸
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int WATER_LEVEL = 500;

// 雨滴状态枚举
typedef enum {
    STATE_FALLING = 0,  // 下落阶段
    STATE_SPLASH = 1,   // 入水溅起阶段（新增）
    STATE_RIPPLE = 2    // 水圈扩散阶段
} RainState;

// 雨滴结构体
typedef struct RainDrop {
    int x, y;           // 当前位置
    int endY;           // 入水位置
    int speed;          // 速度
    COLORREF color;     // 颜色

    // 状态控制
    RainState status;         // 当前状态
    int splashTimer;          // 溅起效果计时器
    int circleR;              // 水圈半径
    int maxCircleR;           // 最大半径
    int fade;                 // 淡出值

    // 断断续续效果
    int visible;              // 当前是否可见（0/1）
    int blinkTimer;         // 可见性计时器
    int soundType;            // 声音类型

    struct RainDrop* next;    // 链表指针
} RainDrop;

// 函数声明
RainDrop* CreateRainDrop(int windX, int rainSpeed);
void UpdateRainDrop(RainDrop* drop, int windX);
void DrawRainDrop(RainDrop* drop);
void PlaySplashSound(int type);